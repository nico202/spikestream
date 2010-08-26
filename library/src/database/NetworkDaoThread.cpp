//SpikeStream includes
#include "ConfigLoader.h"
#include "Connection.h"
#include "GlobalVariables.h"
#include "NetworkDaoThread.h"
#include "Neuron.h"
#include "PerformanceTimer.h"
#include "SpikeStreamException.h"
#include "SpikeStreamDBException.h"
#include "Util.h"
using namespace spikestream;

#include <iostream>
using namespace std;

/*! Controls whether performance measurements are made for this class */
#define TIME_PERFORMANCE


/*! Constructor  */
NetworkDaoThread::NetworkDaoThread(const DBInfo& dbInfo) : NetworkDao(dbInfo) {
    currentTask = NO_TASK_DEFINED;
    clearError();
    stopThread = true;

	//Load up configuration parameters
	ConfigLoader configLoader;
	numConBuffers = Util::getInt(configLoader.getParameter("number_insert_connection_buffers"));
	numNeurBuffers = Util::getInt(configLoader.getParameter("number_insert_neuron_buffers"));
}


/*! Destructor */
NetworkDaoThread::~NetworkDaoThread(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Prepares to add a connection group to the database. This task is executed when the thread runs. */
void NetworkDaoThread::prepareAddConnectionGroup(unsigned int networkID, ConnectionGroup* connGrp){
    //Copy connection group into a list and call the mai nconnection group adding method
    QList<ConnectionGroup*> tmpConGrpList;
    tmpConGrpList.append(connGrp);
    prepareAddConnectionGroups(networkID, tmpConGrpList);
}


/*! Prepares to add a list of connection groups to the database. This task is executed when the thread runs. */
void NetworkDaoThread::prepareAddConnectionGroups(unsigned int networkID, QList<ConnectionGroup*>& connGrpList){
    //Store necessary variables
    this->networkID = networkID;
    this->connectionGroupList = connGrpList;

    //Set the task that will run when the thread starts
    currentTask = ADD_CONNECTION_GROUPS_TASK;

    //Record the total number of steps that the task involves
    totalNumberOfSteps = 1;
}


/*! Prepares to add a neuron group to the database. This task is executed when the thread runs. */
void NetworkDaoThread::prepareAddNeuronGroup(unsigned int networkID, NeuronGroup* neurGrp){
    //Copy neuron group into a list and call the main method to add neuron groups
    QList<NeuronGroup*> tmpNeurGrpList;
    tmpNeurGrpList.append(neurGrp);
    prepareAddNeuronGroups(networkID, tmpNeurGrpList);
}


/*! Prepares to add a list of neuron groups */
void NetworkDaoThread::prepareAddNeuronGroups(unsigned int networkID, QList<NeuronGroup*>& neurGrpList){
	//Store necessary variables
    this->networkID = networkID;
    this->neuronGroupList = neurGrpList;

    //Set the task that will run when the thread starts
    currentTask = ADD_NEURON_GROUPS_TASK;

    //Record the total number of steps that the task involves
    totalNumberOfSteps = 1;
}


/*! Prepares to delete connection groups with the specified ids */
void NetworkDaoThread::prepareDeleteConnectionGroups(unsigned int networkID, QList<unsigned int>& conGrpList){
	//Store necessary variables
	this->networkID = networkID;
	this->connectionGroupIDList = conGrpList;

	//Set the task that will run when the thread starts
	currentTask = DELETE_CONNECTION_GROUPS_TASK;

	//Record the total number of steps that the task involves
	totalNumberOfSteps = 1;
}


/*! Prepares to delete neuron groups with the specified ids */
void NetworkDaoThread::prepareDeleteNeuronGroups(unsigned int networkID, QList<unsigned int>& neurGrpList){
	//Store necessary variables
	this->networkID = networkID;
	this->neuronGroupIDList = neurGrpList;

	//Set the task that will run when the thread starts
	currentTask = DELETE_NEURON_GROUPS_TASK;

	//Record the total number of steps that the task involves
	totalNumberOfSteps = 1;
}


/*! Prepares to load connections for a list of connection groups. This task is executed when the thread runs.  */
void NetworkDaoThread::prepareLoadConnections(const QList<ConnectionGroup*>& connGrpList){
    //Store necessary variables
    this->connectionGroupList = connGrpList;

    //Set the taks that will run when the thread starts
    currentTask = LOAD_CONNECTIONS_TASK;

	//Calculate the total number of steps
	totalNumberOfSteps = getConnectionCount(connectionGroupList);
}


/*! Prepares to load connections for a single connection group. This task is executed when the thread runs.  */
void NetworkDaoThread::prepareLoadConnections(ConnectionGroup* connGrp){
    //Store necessary variables
    connectionGroupList.clear();
    connectionGroupList.append(connGrp);

    //Set the task that will run when the thread starts
    currentTask = LOAD_CONNECTIONS_TASK;

	//Calculate the total number of steps
	totalNumberOfSteps = getConnectionCount(connectionGroupList);
}


/*! Prepares to load a list of neuron groups. This task is executed when the thread runs.  */
void NetworkDaoThread::prepareLoadNeurons(const QList<NeuronGroup*>& neurGrpList){
    //Store necessary variables
    this->neuronGroupList = neurGrpList;

    //Set the task that will run when the thread starts
    currentTask = LOAD_NEURONS_TASK;

	//Calculate the total number of steps
	totalNumberOfSteps = getNeuronCount(neuronGroupList);
}


/*! Prepares to load neurons for a single neuron group. This task is executed when the thread runs.  */
void NetworkDaoThread::prepareLoadNeurons(NeuronGroup* neurGrp){
    //Store necessary variables
    neuronGroupList.clear();
    neuronGroupList.append(neurGrp);

    //Set the task that will run when the thread starts
    currentTask = LOAD_NEURONS_TASK;

	//Calculate the total number of steps
	totalNumberOfSteps = getNeuronCount(neuronGroupList);
}


/*! Resets the error state */
void NetworkDaoThread::clearError(){
    errorMessage = "";
    error = false;
}


/*! Run method inherited from QThread.
    Carries out the task that has been prepared. */
void NetworkDaoThread::run(){
    stopThread = false;
    clearError();
    numberOfCompletedSteps = 0;
    try{
    	//Connect to the database now that we are in a separate thread
		connectToDatabase();

		switch(currentTask){
			case ADD_NEURON_GROUPS_TASK:
				addNeuronGroups();
			break;
			case ADD_CONNECTION_GROUPS_TASK:
				addConnectionGroups();
			break;
			case DELETE_CONNECTION_GROUPS_TASK:
				deleteConnectionGroups();
			break;
			case DELETE_NEURON_GROUPS_TASK:
				deleteNeuronGroups();
			break;
			case LOAD_CONNECTIONS_TASK:
				loadConnections();
			break;
			case LOAD_NEURONS_TASK:
				loadNeurons();
			break;
			default:
				setError("NetworkDaoThread started without defined task.");
		}

		//Database connection can be closed
		closeDatabaseConnection();
    }
    catch(SpikeStreamException& ex){
		setError("NetworkDaoThread: SpikeStreamException thrown carrying out current task: " + ex.getMessage());
    }
    catch(...){
		setError("Unrecognized exception thrown carrying out current task.");
    }

    //Current task is complete
    currentTask = NO_TASK_DEFINED;
    stopThread = true;
}


/*! Stops the current task */
void NetworkDaoThread::stop(){
    stopThread = true;
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Adds a connection group to the network with the specified id.
    Should only work if neuron groups with the specified ids already exist in the database.
    FIXME: MAKE THIS ALL ONE TRANSACTION */
void NetworkDaoThread::addConnectionGroups(){
    //Work through the list of connection groups
    for(QList<ConnectionGroup*>::iterator iter = connectionGroupList.begin(); iter != connectionGroupList.end(); ++iter){
		//Get a pointer to the connection group
		ConnectionGroup* connectionGroup = *iter;

		//Get a copy of the information about the connection group
		ConnectionGroupInfo connGrpInfo = connectionGroup->getInfo();

		//Build query string
		QString queryStr = "INSERT INTO ConnectionGroups (NetworkID, Description, FromNeuronGroupID, ToNeuronGroupID, Parameters, SynapseTypeID ) VALUES (";
		queryStr += QString::number(networkID) + ", ";
		queryStr += "'" + connGrpInfo.getDescription() + "', ";
		queryStr += QString::number(connGrpInfo.getFromNeuronGroupID()) + ", ";
		queryStr += QString::number(connGrpInfo.getToNeuronGroupID()) + ", ";
		queryStr += "'" + connGrpInfo.getParameterXML() + "', ";
		queryStr += QString::number(connGrpInfo.getSynapseTypeID()) + ")";
		QSqlQuery query = getQuery(queryStr);
		executeQuery(query);

		//Check id is correct and add to connection group info if it is
		int lastInsertID = query.lastInsertId().toInt();
		if(lastInsertID >= START_CONNECTIONGROUP_ID)
			connectionGroup->setID(lastInsertID);
		else{
			throw SpikeStreamException("Insert ID for ConnectionGroup is invalid.");
		}

		//Add entry for connection group parameters
		//Get name of table
		SynapseType synapseType = getSynapseType(connGrpInfo.getSynapseTypeID());

		//Add parameter table entry for this connection group
		executeQuery( "INSERT INTO " + synapseType.getParameterTableName() + "(ConnectionGroupID) VALUES (" + QString::number(connectionGroup->getID()) + ")" );

		//Set parameters in connection group
		QHash<QString, double> tmpParamMap = getSynapseParameters(connectionGroup->getInfo());
		connectionGroup->setParameters(tmpParamMap);

		//Check for cancellation of task
		if(stopThread)
			return;

		#ifdef TIME_PERFORMANCE
			PerformanceTimer timer;
		#endif//TIME_PERFORMANCE

		//Build query
		query = getQuery();
		queryStr = "INSERT INTO Connections ( ConnectionGroupID, FromNeuronID, ToNeuronID, Delay, Weight) VALUES ";
		for(int i=0; i<numConBuffers-1; ++i)
			queryStr += "(?, ?, ?, ?, ?),";
		queryStr += "(?, ?, ?, ?, ?)";
		query.prepare(queryStr);

		//Add connections to database
		int conCntr = 0, offset = 0, conAddedCntr = 0;
		QList<Connection*> tmpConList;
		ConnectionList::const_iterator endConGrp = connectionGroup->end();
		for(ConnectionList::const_iterator iter = connectionGroup->begin(); iter != endConGrp; ++iter){
			offset = 5 * (conCntr % numConBuffers);

			//Bind values to query
			tmpConList.append(*iter);
			query.bindValue(0 + offset, connectionGroup->getID());
			query.bindValue(1 + offset, (*iter)->fromNeuronID);
			query.bindValue(2 + offset, (*iter)->toNeuronID);
			query.bindValue(3 + offset, (*iter)->delay);
			query.bindValue(4 + offset, (*iter)->weight);

			//Execute query
			if(conCntr % numConBuffers == numConBuffers-1){
				executeQuery(query);

				//Add connection id to connection - last insert id is the id of the first connection in the list of value entries
				int lastInsertID = query.lastInsertId().toInt();
				if(lastInsertID < START_CONNECTION_ID)
					throw SpikeStreamException("Insert ID for Connection is invalid.");
				if(tmpConList.size() != numConBuffers)
					throw SpikeStreamException("Temporary connection list size " + QString::number(tmpConList.size()) + " does not match number of buffers: " + QString::number(numConBuffers));

				//Set connection ID in connection groups
				for(int i=0; i<tmpConList.size(); ++i){
					tmpConList.at(i)->setID(lastInsertID + i);
				}

				//Count number of connections that have been added
				conAddedCntr += numConBuffers;

				//Clear up list
				tmpConList.clear();
			}

			//Keep track of the number of connections
			++conCntr;

			if(stopThread)
				return;
		}

		//Add remaining connections individually
		if(!tmpConList.isEmpty()){
			query = getQuery();
			query.prepare("INSERT INTO Connections ( ConnectionGroupID, FromNeuronID, ToNeuronID, Delay, Weight) VALUES (?, ?, ?, ?, ?)");
			for(QList<Connection*>::iterator iter = tmpConList.begin(); iter != tmpConList.end(); ++iter){
				query.bindValue(0, connectionGroup->getID());
				query.bindValue(1, (*iter)->fromNeuronID);
				query.bindValue(2, (*iter)->toNeuronID);
				query.bindValue(3, (*iter)->delay);
				query.bindValue(4, (*iter)->weight);

				//Execute query
				executeQuery(query);

				//Add connection id to connection
				int lastInsertID = query.lastInsertId().toInt();
				if(lastInsertID < START_CONNECTION_ID)
					throw SpikeStreamException("Insert ID for Connection is invalid.");
				(*iter)->setID(lastInsertID);

				//Count number of connections that have been added
				++conAddedCntr;
			}
		}

		//Check that we have added all the connections
		if(connectionGroup->size() != conAddedCntr)
			throw SpikeStreamException("Number of connections added to database: " + QString::number(conAddedCntr) + " does not match size of connection group: " + QString::number(connectionGroup->size()));

		#ifdef TIME_PERFORMANCE
			timer.printTime("Number of buffers: " + QString::number(numConBuffers) + ". Number of connections remaining: " + QString::number(tmpConList.size()) + ". Adding " + QString::number(conCntr) + " connections");
		#endif//TIME_PERFORMANCE

		//ConnectionGroup should now match information in database
		connectionGroup->setLoaded(true);
    }
}


/*! Adds a neuron group to the SpikeStreamNetwork database.
    FIXME: MAKE THIS ALL ONE TRANSACTION. */
void NetworkDaoThread::addNeuronGroups(){
    //Check that parameters have been set correctly
    if(networkID == 0){
		setError("Network ID has not been set.");
		return;
    }
    //Work through the list of neuron groups
	for(QList<NeuronGroup*>::iterator neurGrpIter = neuronGroupList.begin(); neurGrpIter != neuronGroupList.end(); ++neurGrpIter){
		//User friendly pointer to group
		NeuronGroup* neuronGroup = *neurGrpIter;

		//Get information about the neuron group
		NeuronGroupInfo neurGrpInfo = neuronGroup->getInfo();

		//Add the neuron group first
		QString queryStr = "INSERT INTO NeuronGroups (NetworkID, Name, Description, Parameters, NeuronTypeID ) VALUES (";
		queryStr += QString::number(networkID) + ", ";
		queryStr += "'" + neurGrpInfo.getName() + "', '" + neurGrpInfo.getDescription() + "', '" + neurGrpInfo.getParameterXML() + "', ";
		queryStr += QString::number(neurGrpInfo.getNeuronTypeID()) + ")";
		QSqlQuery query = getQuery(queryStr);
		executeQuery(query);

		//Check id is correct and add to neuron group info if it is
		int lastInsertID = query.lastInsertId().toInt();
		if(lastInsertID >= START_NEURONGROUP_ID)
			neuronGroup->setID(lastInsertID);//Set this on neuron group, not on the copied info
		else{
			setError("Insert ID for NeuronGroup is invalid.");
			return;
		}

		//Add entry for neuron group parameters
		//Get name of table
		NeuronType neurType = getNeuronType(neurGrpInfo.getNeuronTypeID());

		//Add parameter table entry for this neuron group
		executeQuery( "INSERT INTO " + neurType.getParameterTableName() + "(NeuronGroupID) VALUES (" + QString::number(neuronGroup->getID()) + ")" );

		//Set parameters in neuron group
		QHash<QString, double> tmpParamMap = getNeuronParameters(neuronGroup->getInfo());
		neuronGroup->setParameters(tmpParamMap);

		//Check for cancellation of task
		if(stopThread)
			return;

		#ifdef TIME_PERFORMANCE
			PerformanceTimer timer;
		#endif//TIME_PERFORMANCE

		//Build query
		query = getQuery();
		queryStr = "INSERT INTO Neurons (NeuronGroupID, X, Y, Z) VALUES ";
		for(int i=0; i<numNeurBuffers-1; ++i)
			queryStr += "(?, ?, ?, ?),";
		queryStr += "(?, ?, ?, ?)";
		query.prepare(queryStr);

		//Add neurons to database
		int neurCntr = 0, offset = 0, neurAddedCntr = 0, tmpNeurGrpID = neuronGroup->getID();
		NeuronMap* newNeurMap = new NeuronMap();
		QList<Neuron*> tmpNeurList;
		NeuronIterator endNeurGrp = neuronGroup->end();
		for(NeuronIterator neurIter = neuronGroup->begin(); neurIter != endNeurGrp; ++neurIter){
			offset = 4 * (neurCntr % numNeurBuffers);

			//Bind values to query
			tmpNeurList.append(neurIter.value());
			query.bindValue(0 + offset, tmpNeurGrpID);
			query.bindValue(1 + offset, neurIter.value()->getXPos());
			query.bindValue(2 + offset, neurIter.value()->getYPos());
			query.bindValue(3 + offset, neurIter.value()->getZPos());

			//Execute query if we have added a whole number of buffers
			if(neurCntr % numNeurBuffers == numNeurBuffers-1){
				executeQuery(query);

				//Add neuron id to neuron - last insert id is the id of the first neuron in the list of value entries
				int lastInsertID = query.lastInsertId().toInt();
				if(lastInsertID < START_NEURON_ID)
					throw SpikeStreamException("Insert ID for Neuron is invalid.");
				if(tmpNeurList.size() != numNeurBuffers)
					throw SpikeStreamException("Temporary neuron list size " + QString::number(tmpNeurList.size()) + " does not match number of buffers: " + QString::number(numNeurBuffers));

				//Set neuron IDs and add neurons to new map with the new ID
				for(int i=0; i<tmpNeurList.size(); ++i){
					tmpNeurList.at(i)->setID(lastInsertID + i);
					(*newNeurMap)[lastInsertID + i] = tmpNeurList.at(i);
				}

				//Count number of neurons that have been added
				neurAddedCntr += numNeurBuffers;

				//Clear up list
				tmpNeurList.clear();
			}

			//Keep track of the number of neurons
			++neurCntr;

			if(stopThread)
				return;
		}

		//Add remaining neurons individually
		if(!tmpNeurList.isEmpty()){
			query = getQuery();
			query.prepare("INSERT INTO Neurons ( NeuronGroupID, X, Y, Z) VALUES (?, ?, ?, ?)");
			for(QList<Neuron*>::iterator neurListIter = tmpNeurList.begin(); neurListIter != tmpNeurList.end(); ++neurListIter){
				query.bindValue(0, tmpNeurGrpID);
				query.bindValue(1, (*neurListIter)->getXPos());
				query.bindValue(2, (*neurListIter)->getYPos());
				query.bindValue(3, (*neurListIter)->getZPos());

				//Execute query
				executeQuery(query);

				//Add neuron id to neuron
				int lastInsertID = query.lastInsertId().toInt();
				if(lastInsertID < START_NEURON_ID)
					throw SpikeStreamException("Insert ID for Neuron is invalid.");
				(*neurListIter)->setID(lastInsertID);
				(*newNeurMap)[lastInsertID] = *neurListIter;

				//Count number of neurons that have been added
				++neurAddedCntr;
			}
		}

		//Check that we have added all the neurons
		if(neuronGroup->size() != neurAddedCntr)
			throw SpikeStreamException("Number of neurons added to database: " + QString::number(neurAddedCntr) + " does not match size of neuron group: " + QString::number(neuronGroup->size()));


		//Set the start ID of the neuron group
		NetworkDao networkDao(this->getDBInfo());
		neuronGroup->setStartNeuronID( networkDao.getStartNeuronID(tmpNeurGrpID) );

		#ifdef TIME_PERFORMANCE
			timer.printTime("Adding neurons. Number of buffers: " + QString::number(numNeurBuffers) + ". Number of neurons remaining: " + QString::number(tmpNeurList.size()) + ". Added " + QString::number(neurAddedCntr) + " neurons");
		#endif//TIME_PERFORMANCE

		//Add the new map to the neuron group. This should also clean up the old map
		neuronGroup->setNeuronMap(newNeurMap);

		//Neuron group now reflects state of table in database, so set loaded to true
		neuronGroup->setLoaded(true);
    }
}


/*! Deletes connection groups from the SpikeStreamNetwork database. */
void NetworkDaoThread::deleteConnectionGroups(){
	//Check that parameters have been set correctly
	if(networkID == 0){
		setError("Network ID has not been set.");
		return;
	}

	//Work through the list of neuron groups
	foreach(unsigned int conGroupID, connectionGroupIDList){
		executeQuery("DELETE FROM ConnectionGroups WHERE NetworkID="+ QString::number(networkID) + " AND ConnectionGroupID=" + QString::number(conGroupID) );
	}

	//Reset list
	connectionGroupIDList.clear();
}


/*! Deletes neuron groups from the SpikeStreamNetwork database. */
void NetworkDaoThread::deleteNeuronGroups(){
	//Check that parameters have been set correctly
	if(networkID == 0){
		setError("Network ID has not been set.");
		return;
	}

	//Work through the list of neuron groups
	foreach(unsigned int neuronGroupID, neuronGroupIDList){
		executeQuery("DELETE FROM NeuronGroups WHERE NetworkID="+ QString::number(networkID) + " AND NeuronGroupID=" + QString::number(neuronGroupID) );
	}

	//Reset list
	neuronGroupIDList.clear();
}


/*! Loads the prepared list of connection groups from the database. */
void NetworkDaoThread::loadConnections(){
	//Reset progress
	numberOfCompletedSteps = 0;

    //Work through all the connections to be loaded
    for(QList<ConnectionGroup*>::iterator iter = connectionGroupList.begin(); iter != connectionGroupList.end(); ++iter){

		//Empty current connections in group
		(*iter)->clearConnections();

		//Load connections into group
		unsigned int tmpConGrpID = (*iter)->getID();
		QSqlQuery query = getQuery("SELECT ConnectionID, FromNeuronID, ToNeuronID, Delay, Weight FROM Connections WHERE ConnectionGroupID = " + QString::number(tmpConGrpID));
		executeQuery(query);
		while ( query.next() ) {
			Connection* tmpConn = new Connection(
					query.value(0).toUInt(),//ConnectionID
					tmpConGrpID,//Connection Group ID
					query.value(1).toUInt(),//FromNeuronID
					query.value(2).toUInt(),//ToNeuronID
					query.value(3).toString().toFloat(),//Delay
					query.value(4).toString().toFloat()//Weight
			);
			(*iter)->addConnection(tmpConn);

			//Track progress
			++numberOfCompletedSteps;

			//Quit if user cancels
			if(stopThread)
				return;
		}

		//Load parameters in connection group
		QHash<QString, double> tmpParamMap = getSynapseParameters( (*iter)->getInfo());
		(*iter)->setParameters(tmpParamMap);

		//Connection group now matches the database
		(*iter)->setLoaded(true);
    }
}


/*! Loads the prepared list of neuron groups from the database */
void NetworkDaoThread::loadNeurons(){
	//Reset progress measure
	numberOfCompletedSteps = 0;

    //Work through all the neurons to be loaded
    for(QList<NeuronGroup*>::iterator iter = neuronGroupList.begin(); iter != neuronGroupList.end(); ++iter){

		//Empty current connections in group
		(*iter)->clearNeurons();

		//Get pointer to neuron map
		NeuronMap* tmpNeurMap = (*iter)->getNeuronMap();

		//Load neurons into group
		bool firstTime = true;
		QSqlQuery query = getQuery("SELECT NeuronID, X, Y, Z FROM Neurons WHERE NeuronGroupID = " + QString::number((*iter)->getID()) + " ORDER BY NeuronID");
		executeQuery(query);
		while ( query.next() ) {
			Neuron* tmpNeuron = new Neuron(
					query.value(0).toUInt(),//ID
					query.value(1).toString().toFloat(),//X
					query.value(2).toString().toFloat(),//Y
					query.value(3).toString().toFloat()//Z
					);
			(*tmpNeurMap)[query.value(0).toUInt()] = tmpNeuron;

			//Store the start neuron id - useful when neurons are stored continuously, which is usually but not always the case
			if(firstTime){
				(*iter)->setStartNeuronID(query.value(0).toUInt());
				firstTime = false;
			}

			//Track progress
			++numberOfCompletedSteps;

			//Quit if user cancels
			if(stopThread)
				return;
		}

		//Load parameters in neuron group
		QHash<QString, double> tmpParamMap = getNeuronParameters( (*iter)->getInfo());
		(*iter)->setParameters(tmpParamMap);

		//Neuron group now matches the database
		(*iter)->setLoaded(true);
    }
}



/*! Exceptions do not work across threads, so errors are flagged by calling this method.
    The invoking method is responsible for checking whether an error occurred and throwing
    an exeption if necessary.*/
void NetworkDaoThread::setError(const QString& msg){
    errorMessage = msg;
    error = true;
    stopThread = true;
}


