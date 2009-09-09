//SpikeStream includes
#include "GlobalVariables.h"
#include "NetworkDaoThread.h"
#include "SpikeStreamDBException.h"

#include <iostream>
using namespace std;

/*! Constructor that prepares thread for adding a connection group */
NetworkDaoThread::NetworkDaoThread(const DBInfo& dbInfo, unsigned int networkID, ConnectionGroup* connGrp) : AbstractDao(dbInfo) {
    this->networkID = networkID;
    this->connectionGroup = connGrp;
    currentTask = ADD_CONNECTION_GROUP_TASK;
}

/*! Constructor that prepares thread for adding a neuron group */
NetworkDaoThread::NetworkDaoThread(const DBInfo& dbInfo, unsigned int networkID, NeuronGroup* neurGrp) : AbstractDao(dbInfo) {
    this->networkID = networkID;
    this->neuronGroup = neurGrp;
    currentTask = ADD_NEURON_GROUP_TASK;
}


/*! Destructor */
NetworkDaoThread::~NetworkDaoThread(){
}


/*! Run method inherited from QThread.
    Carries out the task that has been prepared. */
void NetworkDaoThread::run(){
    stopThread = false;

    //Connect to the database now that we are in a separate thread
    connectToDatabase();

    switch(currentTask){
	case ADD_NEURON_GROUP_TASK:
	    addNeuronGroup();
	break;
	case ADD_CONNECTION_GROUP_TASK:
	    addConnectionGroup();
	break;
	default:
	    throw SpikeStreamException("NetworkDaoThread started without defined task.");
    }

    //Database connection can be closed
    closeDatabaseConnection();
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
    else
	throw SpikeStreamDBException("Insert ID for ConnectionGroup is invalid.");

    //Check for cancellation of task
    if(stopThread)
	return;

    //Add connections to database
    for(unsigned int i=0; i<connectionGroup->size(); ++i){

    }
}


/*! Adds a neuron group to the SpikeStreamNetwork database.
    FIXME: MAKE THIS ALL ONE TRANSACTION. */
void NetworkDaoThread::addNeuronGroup(){
    //Check that parameters have been set correctly
    if(networkID == 0){
	throw SpikeStreamDBException("NeuralNetwork ID has not been set.");
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
    else
	throw SpikeStreamDBException("Insert ID for NeuronGroup is invalid.");

    //Check for cancellation of task
    if(stopThread)
	return;

    //Add neurons
    NeuronMap* neurMap = neuronGroup->getNeuronMap();
    NeuronMap::iterator mapEnd = neurMap->end();//Saves accessing this function multiple times
    for(NeuronMap::iterator iter=neurMap->begin(); iter != mapEnd; ++iter){
	QString queryStr("INSERT INTO Neurons (NeuronGroupID, X, Y, Z) VALUES (");
	queryStr += QString::number(neuronGroup->getID()) + ", ";
	queryStr += QString::number(iter.value()->x) + ", ";
	queryStr += QString::number(iter.value()->y) + ", ";
	queryStr += QString::number(iter.value()->z) + ")";
	executeQuery(queryStr);

	if(stopThread)
	    return;
    }
}
