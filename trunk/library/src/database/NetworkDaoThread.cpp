//SpikeStream includes
#include "GlobalVariables.h"
#include "NetworkDaoThread.h"

#include <iostream>
using namespace std;

/*! Constructor  */
NetworkDaoThread::NetworkDaoThread(const DBInfo& dbInfo) : AbstractDao(dbInfo) {
    currentTask = NO_TASK_DEFINED;
    clearError();
}


/*! Destructor */
NetworkDaoThread::~NetworkDaoThread(){
}


void NetworkDaoThread::prepareAddConnectionGroup(unsigned int networkID, ConnectionGroup* connGrp){
    this->networkID = networkID;
    this->connectionGroup = connGrp;
    currentTask = ADD_CONNECTION_GROUP_TASK;
}


void NetworkDaoThread::prepareAddNeuronGroup(unsigned int networkID, NeuronGroup* neurGrp){
    this->networkID = networkID;
    this->neuronGroup = neurGrp;
    currentTask = ADD_NEURON_GROUP_TASK;
}

/*! Prepares to load connections for a list of connection groups */
void NetworkDaoThread::prepareLoadConnections(QList<ConnectionGroup*>& connGrpList){
    this->networkID = networkID;
    this->connectionGroupList = connGrpList;
    currentTask = LOAD_CONNECTIONS_TASK;
}

/*! Prepares to load connections for a single connection group */
void NetworkDaoThread::prepareLoadConnections(ConnectionGroup* connGrp){
    this->networkID = networkID;
    connectionGroupList.clear();
    connectionGroupList.append(connGrp);
    currentTask = LOAD_CONNECTIONS_TASK;
}


/*! Prepares to load neurons for a single neuron group */
void NetworkDaoThread::prepareLoadNeurons(QList<NeuronGroup*>& neurGrpList){
    this->networkID = networkID;
    this->neuronGroupList = neurGrpList;
    currentTask = LOAD_NEURONS_TASK;
}


/*! Resets the error state */
void NetworkDaoThread::clearError(){
    errorMessage = "";
    error = false;
}


/*! Exceptions do not work across threads, so errors are flagged by calling this method.
    The invoking method is responsible for checking whether an error occurred and throwing
    an exeption if necessary.*/
void NetworkDaoThread::setError(QString msg){
    errorMessage = msg;
    error = true;
    stopThread = true;
}


/*! Run method inherited from QThread.
    Carries out the task that has been prepared. */
void NetworkDaoThread::run(){
    stopThread = false;
    clearError();

    //Connect to the database now that we are in a separate thread
    connectToDatabase();

    switch(currentTask){
	case ADD_NEURON_GROUP_TASK:
	    addNeuronGroup();
	break;
	case ADD_CONNECTION_GROUP_TASK:
	    addConnectionGroup();
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

    //Current task is complete
    currentTask = NO_TASK_DEFINED;
    stopThread = true;
}


/*! Stops the current task */
void NetworkDaoThread::stop(){
    stopThread = true;
}


/*! Adds a connection group to the network with the specified id.
    Should only work if neuron groups with the specified ids already exist in the database.
    FIXME: MAKE THIS ALL ONE TRANSACTION */
void NetworkDaoThread::addConnectionGroup(){
    //Get a copy of the information about the connection group
    ConnectionGroupInfo connGrpInfo = connectionGroup->getInfo();

    //Build query string
    QString queryStr = "INSERT INTO ConnectionGroups (NeuralNetworkID, Description, FromNeuronGroupID, ToNeuronGroupID, Parameters, SynapseTypeID ) VALUES (";
    queryStr += QString::number(networkID) + ", ";
    queryStr += "'" + connGrpInfo.getDescription() + "', ";
    queryStr += QString::number(connGrpInfo.getFromNeuronGroupID()) + ", ";
    queryStr += QString::number(connGrpInfo.getToNeuronGroupID()) + ", ";
    queryStr += "'" + connGrpInfo.getParameterXML() + "', ";
    queryStr += QString::number(connGrpInfo.getSynapseType()) + ")";
    QSqlQuery query = getQuery(queryStr);
    executeQuery(query);

    //Check id is correct and add to connection group info if it is
    int lastInsertID = query.lastInsertId().toInt();
    if(lastInsertID >= START_CONNECTIONGROUP_ID)
	connectionGroup->setID(lastInsertID);
    else{
	setError("Insert ID for ConnectionGroup is invalid.");
	return;
    }

    //Check for cancellation of task
    if(stopThread)
	return;

    //Add connections to database
    QList<Connection*>* connList = connectionGroup->getConnections();
    ConnectionList::iterator endConnList = connList->end();
    for(ConnectionList::iterator iter = connList->begin(); iter != endConnList; ++iter){
	//Build query string
	queryStr = "INSERT INTO Connections ( ConnectionGroupID, FromNeuronID, ToNeuronID, Delay, Weight) VALUES (";
	queryStr += QString::number(connectionGroup->getID()) + ", ";
	queryStr += QString::number((*iter)->fromNeuronID) + ", ";
	queryStr += QString::number((*iter)->toNeuronID) + ", ";
	queryStr += QString::number((*iter)->delay) + ", ";
	queryStr += QString::number((*iter)->weight) + ")";

	//Execute query
    	query = getQuery(queryStr);
	executeQuery(query);

	//Add connection id to connection
	int lastInsertID = query.lastInsertId().toInt();
	if(lastInsertID < START_CONNECTION_ID){
	    setError("Insert ID for Connection is invalid.");
	    return;
	}

	(*iter)->setID(lastInsertID);

	if(stopThread)
	    return;
    }

    //ConnectionGroup should now match information in database
    connectionGroup->setLoaded(true);
}


/*! Adds a neuron group to the SpikeStreamNetwork database.
    FIXME: MAKE THIS ALL ONE TRANSACTION. */
void NetworkDaoThread::addNeuronGroup(){
    //Check that parameters have been set correctly
    if(networkID == 0){
	setError("NeuralNetwork ID has not been set.");
	return;
    }

    //Get information about the neuron group
    NeuronGroupInfo neurGrpInfo = neuronGroup->getInfo();

    //Add the neuron group first
    QString queryStr = "INSERT INTO NeuronGroups (NeuralNetworkID, Name, Description, Parameters, NeuronTypeID ) VALUES (";
    queryStr += QString::number(networkID) + ", ";
    queryStr += "'" + neurGrpInfo.getName() + "', '" + neurGrpInfo.getDescription() + "', '" + neurGrpInfo.getParameterXML() + "', ";
    queryStr += QString::number(neurGrpInfo.getNeuronType()) + ")";
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

    //Check for cancellation of task
    if(stopThread)
	return;

    //Add neurons
    NeuronMap* neurMap = neuronGroup->getNeuronMap();
    NeuronMap* newNeurMap = new NeuronMap();
    NeuronMap::iterator mapEnd = neurMap->end();//Saves accessing this function multiple times
    for(NeuronMap::iterator iter=neurMap->begin(); iter != mapEnd; ++iter){
	QString queryStr("INSERT INTO Neurons (NeuronGroupID, X, Y, Z) VALUES (");
	queryStr += QString::number(neuronGroup->getID()) + ", ";
	queryStr += QString::number(iter.value()->x) + ", ";
	queryStr += QString::number(iter.value()->y) + ", ";
	queryStr += QString::number(iter.value()->z) + ")";
	query = getQuery(queryStr);
	executeQuery(query);

	//Add neuron with correct id to the new map
	int lastInsertID = query.lastInsertId().toInt();
	if(lastInsertID < START_NEURON_ID){
	    setError("Insert ID for Neuron is invalid.");
	    return;
	}
	(*newNeurMap)[lastInsertID] = iter.value();

	if(stopThread)
	    return;
    }
    //Add the correct map to the neuron group. This should also clean up the old map
    neuronGroup->setNeuronMap(newNeurMap);

    /* Clean up the dynamically allocated neuron map, but keep the dynamically allocated
	Point3Ds, which are used in the new map */
    delete neurMap;

    //Neuron group now reflects state of table in database, so set loaded to true
    neuronGroup->setLoaded(true);
}


void NetworkDaoThread::loadConnections(){
    //Work through all the connections to be loaded
    for(QList<ConnectionGroup*>::iterator iter = connectionGroupList.begin(); iter != connectionGroupList.end(); ++iter){

	//Empty current connections in group
	(*iter)->clearConnections();

	//Load connections into group
	QSqlQuery query = getQuery("SELECT ConnectionID, FromNeuronID, ToNeuronID, Delay, Weight FROM Connections WHERE ConnectionGroupID = " + QString::number((*iter)->getID());    }
	while ( query.next() ) {
	    Connection* tmpConn = new Connection(
			query.value(0).toUInt(),//ConnectionID
			query.value(1).toUInt(),//FromNeuronID
			query.value(2).toUInt(),//ToNeuronID
			query.value(3).toString().toFloat(),//Delay
			query.value(4).toString().toFloat()//Weight
	    );
	    (*iter)->addConnection(tmpConn);
	}

	//Connection group now matches the database
	(*iter)->setLoaded(true);
}


void NetworkDaoThread::loadNeurons(){
}



