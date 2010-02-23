//SpikeStream includes
#include "GlobalVariables.h"
#include "NetworkDao.h"
#include "SpikeStreamDBException.h"
#include "Util.h"
#include "XMLParameterParser.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;

/*! Constructor. DBInfo is stored by AbstractDao */
NetworkDao::NetworkDao(const DBInfo& dbInfo) : AbstractDao(dbInfo){

}


/*! Destructor */
NetworkDao::~NetworkDao(){
	closeDatabaseConnection();
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Adds the specified neural network to the database */
void NetworkDao::addNetwork(NetworkInfo& netInfo){
	QSqlQuery query = getQuery("INSERT INTO Networks (Name, Description) VALUES ('" + netInfo.getName() + "', '" + netInfo.getDescription() + "')");
	executeQuery(query);

	//Check id is correct and add to network info if it is
	int lastInsertID = query.lastInsertId().toInt();
	if(lastInsertID >= START_NEURALNETWORK_ID)
		netInfo.setID(lastInsertID);
	else
		throw SpikeStreamDBException("Insert ID for Network is invalid: " + QString::number(lastInsertID));
}


/*! Deletes a network from the database. Does nothing if a network with the specified
	id does not exist. */
void NetworkDao::deleteNetwork(unsigned int networkID){
	executeQuery("DELETE FROM Networks WHERE NetworkID = " + QString::number(networkID));
}


/*! Adds a weightless connection pattern index to the database */
void NetworkDao::addWeightlessConnection(unsigned int connectionID, unsigned int patternIndex){
	QSqlQuery query = getQuery("INSERT INTO WeightlessConnections (ConnectionID, PatternIndex) VALUES (" + QString::number(connectionID) + ", " + QString::number(patternIndex) + ")");
	executeQuery(query);
}


/*! Adds a weightless neuron training pattern to the database and returns the new pattern id. */
unsigned int NetworkDao::addWeightlessNeuronTrainingPattern(unsigned int neuronID, const unsigned char* patternArray, bool output, unsigned int patternArraySize){
	//Create a byte array that does not copy data
	QByteArray tmpByteArray = QByteArray::fromRawData((char*) patternArray, patternArraySize);

	//Add data to database
	QSqlQuery query = getQuery("INSERT INTO WeightlessNeuronTrainingPatterns (NeuronID, Pattern, Output) VALUES (" + QString::number(neuronID) + ", :PATTERNARRAY, " + QString::number(output) + ")");
	query.bindValue(":PATTERNARRAY", tmpByteArray);
	executeQuery(query);

	//Check id is correct and return it if it is
	int lastInsertID = query.lastInsertId().toInt();
	if(lastInsertID >= START_WEIGHTLESS_TRAINING_PATTERN_ID)
		return lastInsertID;

	//Throw exception if the last insert id is not in the valid range
	throw SpikeStreamDBException("Insert ID for WeightlessNeuronTrainingPatterns is invalid: " + QString::number(lastInsertID));
}


/*! Deletes all networks and associated data from the database. */
void NetworkDao::deleteAllNetworks(){
	executeQuery("DELETE FROM Networks");
}


/*! Returns all of the connections from the specified neuron to the specified neuron. */
QList<Connection> NetworkDao::getConnections(unsigned int fromNeuronID, unsigned int toNeuronID){
	QSqlQuery query = getQuery("SELECT ConnectionID, ConnectionGroupID, Delay, Weight, TempWeight FROM Connections WHERE FromNeuronID=" + QString::number(fromNeuronID) + " AND ToNeuronID="+ QString::number(toNeuronID));
	executeQuery(query);
	QList<Connection> conList;
	while ( query.next() ) {
		Connection tmpCon(
				query.value(0).toUInt(),//ConnectionID
				query.value(1).toUInt(),//Connection group id
				fromNeuronID,//FromNeuronID
				toNeuronID,//ToNeuronID
				query.value(2).toString().toFloat(),//Delay
				query.value(3).toString().toFloat(),//Weight
				query.value(4).toString().toFloat()//tempWeight
				);
		conList.append(tmpCon);
	}
	return conList;
}


/*! Returns information about the connection groups associated with the specified network */
QList<ConnectionGroupInfo> NetworkDao::getConnectionGroupsInfo(unsigned int networkID){
	QSqlQuery query = getQuery("SELECT ConnectionGroupID, Description, FromNeuronGroupID, ToNeuronGroupID, Parameters, SynapseTypeID FROM ConnectionGroups WHERE NetworkID=" + QString::number(networkID));
	executeQuery(query);
	QList<ConnectionGroupInfo> tmpList;
	XMLParameterParser parameterParser;
	for(int i=0; i<query.size(); ++i){
		query.next();
		tmpList.append(
				ConnectionGroupInfo(
						query.value(0).toUInt(),//Connection group id
						query.value(1).toString(),//Description
						query.value(2).toUInt(),//From group id
						query.value(3).toUInt(),//To group id
						parameterParser.getParameterMap(query.value(4).toString()),//Parameters
						query.value(5).toUInt()//Synapse id
						)
				);
	}
	return tmpList;
}


/*! Fills the supplied nested hash map with all the connections made FROM a neuron to other neurons.
	The key is the FROM neuron id, the value is a second map whose key is the TO neuron id and whose
	value is a boolean always set to true. */
void NetworkDao::getAllFromConnections(unsigned int networkID, QHash<unsigned int, QHash<unsigned int, bool> >& connMap){
	//Reset the map
	connMap.clear();

	//Get a list of all the neuron ids in the network
	QList<unsigned int> neuronIDList = getNeuronIDs(networkID);

	//Work through all of the neurons in the network
	foreach(unsigned int neuronID, neuronIDList){
		//Add the connections FROM this neuron id TO other neurons
		QHash<unsigned int, bool> tmpFromConMap;
		QList<unsigned int> fromConList = getFromConnections(neuronID);
		foreach(unsigned int fromConNeurID, fromConList)
			tmpFromConMap[fromConNeurID] = true;
		connMap.insert(neuronID, tmpFromConMap);
	}
}


/*! Fills the supplied nested  hash map with all the connections made TO each neuron from other neurons.
	The key is the TO neuron id, the value is a second map whose key is the FROM neuron id and whose
	value is a boolean always set to true.  */
void NetworkDao::getAllToConnections(unsigned int networkID, QHash<unsigned int, QHash<unsigned int, bool> >& connMap){
	//Reset the map
	connMap.clear();

	//Get a list of all the neuron ids in the network
	QList<unsigned int> neuronIDList = getNeuronIDs(networkID);

	//Work through all of the neurons in th network
	foreach(unsigned int neuronID, neuronIDList){
		//Add the connections FROM this neuron id TO other neurons
		QHash<unsigned int, bool> tmpToConMap;
		QList<unsigned int> toConList = getToConnections(neuronID);
		foreach(unsigned int toConNeurID, toConList)
			tmpToConMap[toConNeurID] = true;
		connMap.insert(neuronID, tmpToConMap);
	}
}


/*! Returns a list of all the connections FROM the specified neuron */
QList<unsigned int> NetworkDao::getFromConnections(unsigned int fromNeuronID){
	QSqlQuery query = getQuery("SELECT ToNeuronID FROM Connections WHERE FromNeuronID=" + QString::number(fromNeuronID));
	executeQuery(query);

	//Add neuron ids to list
	QList<unsigned int> conList;
	while(query.next()){
		unsigned int neurID = Util::getUInt(query.value(0).toString());
		conList.append(neurID);
	}

	//Return list
	return conList;
}


/*! Returns a list of all the connections TO the specified neuron */
QList<unsigned int> NetworkDao::getToConnections(unsigned int toNeuronID){
	QSqlQuery query = getQuery("SELECT FromNeuronID FROM Connections WHERE ToNeuronID=" + QString::number(toNeuronID));
	executeQuery(query);

	//Add neuron ids to list
	QList<unsigned int> conList;
	while(query.next()){
		unsigned int neurID = Util::getUInt(query.value(0).toString());
		conList.append(neurID);
	}

	//Return list
	return conList;
}


/*! Returns a list of connections filtered according to the connection mode */
QList<Connection*> NetworkDao::getConnections(unsigned int connectionMode, unsigned int singleNeuronID, unsigned int toNeuronID){
	QList<Connection*> conList;
	//Return empty list if connection mode is disabled
	if( !(connectionMode & CONNECTION_MODE_ENABLED) )
		return conList;

	QString queryStr = "SELECT ConnectionID, ConnectionGroupID, FromNeuronID, ToNeuronID, Delay, Weight, TempWeight FROM Connections WHERE ";

	//Filter by weight
	if(connectionMode & SHOW_POSITIVE_CONNECTIONS)
		queryStr += "Weight >= 0 AND ";
	else if(connectionMode & SHOW_NEGATIVE_CONNECTIONS)
		queryStr += "Weight < 0 AND ";

	//Filter by from or to in single neuron connection mode
	if( !(connectionMode & SHOW_BETWEEN_CONNECTIONS) ){
		if(connectionMode & SHOW_FROM_CONNECTIONS){
			queryStr += "FromNeuronID=" + QString::number(singleNeuronID) + " ";
		}
		else if(connectionMode & SHOW_TO_CONNECTIONS){
			queryStr += "ToNeuronID=" + QString::number(singleNeuronID) + " ";
		}
		else{
			queryStr += "(FromNeuronID=" + QString::number(singleNeuronID) + " OR ToNeuronID=" + QString::number(singleNeuronID) + ") ";
		}
	}
	//Showing connections between two neurons
	else{
		queryStr += "FromNeuronID=" + QString::number(singleNeuronID) + " AND ToNeuronID=" + QString::number(toNeuronID);
	}

	//Execute query
	QSqlQuery query = getQuery(queryStr);
	executeQuery(query);

	//Add returned connections to the list
	while ( query.next() ) {
		Connection* tmpConn = new Connection(
				query.value(0).toUInt(),//ConnectionID
				query.value(1).toUInt(),//Connection Group ID
				query.value(2).toUInt(),//FromNeuronID
				query.value(3).toUInt(),//ToNeuronID
				query.value(4).toString().toFloat(),//Delay
				query.value(5).toString().toFloat(),//Weight
				query.value(6).toString().toFloat()//tempWeight
				);
		conList.append(tmpConn);
	}

	//Return the list
	return conList;
}


/*! Returns a box enclosing a particular neuron group */
Box NetworkDao::getNeuronGroupBoundingBox(unsigned int neurGrpID){
	QSqlQuery query = getQuery("SELECT MIN(X), MIN(Y), MIN(Z), MAX(X), MAX(Y), MAX(Z) FROM Neurons WHERE NeuronGroupID = " + QString::number(neurGrpID));
	executeQuery(query);
	query.next();
	Box box(
			query.value(0).toInt(),//x1
			query.value(1).toInt(),//y1
			query.value(2).toInt(),//z1
			query.value(3).toInt(),//x2
			query.value(4).toInt(),//y2
			query.value(5).toInt()//z2
			);
	return box;
}


/*! Returns the number of connections in the specified connection group */
unsigned int NetworkDao::getConnectionGroupSize(unsigned int connGrpID){
	QSqlQuery query = getQuery("SELECT COUNT(*) FROM Connections WHERE ConnectionGroupID=" + QString::number(connGrpID));
	executeQuery(query);
	query.next();
	return query.value(0).toUInt();
}


/*! Returns a list of information about the available networks. */
QList<NetworkInfo> NetworkDao::getNetworksInfo(){
	QSqlQuery query = getQuery("SELECT NetworkID, Name, Description FROM Networks");
	executeQuery(query);
	QList<NetworkInfo> tmpList;
	for(int i=0; i<query.size(); ++i){
		query.next();
		tmpList.append( NetworkInfo(query.value(0).toUInt(), query.value(1).toString(), query.value(2).toString()));
	}
	return tmpList;
}


/*! Returns information about the neuron groups associated with the specified network */
QList<NeuronGroupInfo> NetworkDao::getNeuronGroupsInfo(unsigned int networkID){
	QSqlQuery query = getQuery("SELECT NeuronGroupID, Name, Description, Parameters, NeuronTypeID FROM NeuronGroups WHERE NetworkID=" + QString::number(networkID));
	executeQuery(query);
	QList<NeuronGroupInfo> tmpList;
	XMLParameterParser parameterParser;
	for(int i=0; i<query.size(); ++i){
		query.next();
		tmpList.append(
				NeuronGroupInfo(
						query.value(0).toUInt(),
						query.value(1).toString(),
						query.value(2).toString(),
						parameterParser.getParameterMap(query.value(3).toString()),
						query.value(4).toUInt()
						)
				);
	}
	return tmpList;

}


/*! Returns a list of the neuron ids in the specified network */
QList<unsigned int> NetworkDao::getNeuronIDs(unsigned int networkID){
	QList<unsigned int> neurIDList;
	QSqlQuery query = getQuery("SELECT NeuronID FROM Neurons WHERE NeuronGroupID IN (SELECT NeuronGroupID FROM NeuronGroups WHERE NetworkID=" + QString::number(networkID) + ")");
	executeQuery(query);
	for(int i=0; i<query.size(); ++i){
		query.next();
		neurIDList.append(Util::getUInt(query.value(0).toString()));
	}
	return neurIDList;
}


WeightlessNeuron* NetworkDao::getWeightlessNeuron(unsigned int neuronID){
	//Query to select neurons connected to weightless neuron and the pattern index of each neuron
	QSqlQuery query = getQuery("SELECT cons.FromNeuronID, weiCons.PatternIndex FROM Connections cons INNER JOIN WeightlessConnections weiCons ON cons.ConnectionID=weiCons.ConnectionID WHERE cons.ToNeuronID=" + QString::number(neuronID));
	executeQuery(query);

	/* Store the pattern index of each neuron that connects to
	There may be several connections between neurons, so connection map contains a list of connections for each neuron
	instead of a single entry */
	QHash<unsigned int, QList<unsigned int> > tmpConMap;
	unsigned int tmpFromNeurID, tmpPatternIndex;
	for(int i=0; i<query.size(); ++i){
		query.next();
		tmpFromNeurID = Util::getUInt(query.value(0).toString());
		tmpPatternIndex = Util::getUInt(query.value(1).toString());
		tmpConMap[tmpFromNeurID].append(tmpPatternIndex);
	}

	//Create the weightless neuron
	WeightlessNeuron* tmpWeightlessNeuron = new WeightlessNeuron(tmpConMap, neuronID);

	//Query to get the training patterns
	query = getQuery("SELECT Pattern, Output FROM WeightlessNeuronTrainingPatterns WHERE NeuronID = " + QString::number(neuronID));
	executeQuery(query);

	//Add training patterns to neuron
	for(int i=0; i<query.size(); ++i){
		query.next();
		QByteArray tmpByteArray = query.value(0).toByteArray();
		tmpWeightlessNeuron->addTraining(tmpByteArray, Util::getUInt(query.value(1).toString()));
	}

	//Create and return the weigthless neuron
	return tmpWeightlessNeuron;
}


/*! Returns the neuron group containing the specified neuron */
unsigned int NetworkDao::getNeuronGroupID(unsigned int neuronID){
	QSqlQuery query = getQuery("SELECT NeuronGroupID FROM Neurons WHERE NeuronID = " + QString::number(neuronID));
	executeQuery(query);
	query.next();
	return Util::getUInt(query.value(0).toString());
}


/*! Returns true if the neuron is weightless.
	Uses the string "Weightless Neuron" rather than the ID, which may change. */
bool NetworkDao::isWeightlessNeuron(unsigned int neuronID){
	unsigned int tmpNeurGrpID = getNeuronGroupID(neuronID);

	//Query to select neuron type description
	QSqlQuery query = getQuery("SELECT neurType.Description FROM NeuronTypes neurType INNER JOIN NeuronGroups neurGrps ON neurType.NeuronTypeID=neurGrps.NeuronTypeID WHERE neurGrps.NeuronGroupID=" + QString::number(tmpNeurGrpID));
	executeQuery(query);

	//Check to see if it is weightless
	query.next();
	if(query.value(0).toString().toUpper() == "WEIGHTLESS NEURON")
		return true;
	return false;
}


/*! Returns true if all the neurons in the network are weightless.
	Uses the string "Weightless Neuron" rather than the ID, which may change. */
bool NetworkDao::isWeightlessNetwork(unsigned int networkID){

	//Query to select neuron type description
	QSqlQuery query = getQuery("SELECT neurType.Description FROM NeuronTypes neurType INNER JOIN NeuronGroups neurGrps ON neurType.NeuronTypeID=neurGrps.NeuronTypeID WHERE neurGrps.NetworkID=" + QString::number(networkID));
	executeQuery(query);

	//Check to see if it is weightless
	while(query.next()){
		if(query.value(0).toString().toUpper() != "WEIGHTLESS NEURON")
			return false;
	}
	return true;
}


/*! Sets the temporary weight between two neurons. In the event of multiple connections between two neurons
	all of the temporary weights will be updated */
void NetworkDao::setTempWeight(unsigned int fromNeurID, unsigned int toNeurID, double tempWeight){
	QSqlQuery query = getQuery("UPDATE Connections SET TempWeight=" + QString::number(tempWeight) + " WHERE FromNeuronID=" + QString::number(fromNeurID) + " AND ToNeuronID=" + QString::number(toNeurID));
	executeQuery(query);
}




