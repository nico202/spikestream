//SpikeStream includes
#include "GlobalVariables.h"
#include "NetworkDaoThread.h"
#include "Neuron.h"
#include "SpikeStreamException.h"
#include "SpikeStreamDBException.h"
using namespace spikestream;

#include <iostream>
using namespace std;

/*! Constructor  */
NetworkDaoThread::NetworkDaoThread(const DBInfo& dbInfo) : NetworkDao(dbInfo) {
    currentTask = NO_TASK_DEFINED;
    clearError();
    stopThread = true;
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
void NetworkDaoThread::prepareLoadConnections(QList<ConnectionGroup*>& connGrpList){
    //Store necessary variables
    this->connectionGroupList = connGrpList;

    //Set the taks that will run when the thread starts
    currentTask = LOAD_CONNECTIONS_TASK;

	//Set total number of steps to a value greater than 1 - this will change when task starts
	totalNumberOfSteps = 100;
}


/*! Prepares to load connections for a single connection group. This task is executed when the thread runs.  */
void NetworkDaoThread::prepareLoadConnections(ConnectionGroup* connGrp){
    //Store necessary variables
    connectionGroupList.clear();
    connectionGroupList.append(connGrp);

    //Set the task that will run when the thread starts
    currentTask = LOAD_CONNECTIONS_TASK;

	//Set total number of steps to a value greater than 1 - this will change when task starts
	totalNumberOfSteps = 100;
}


/*! Prepares to load a list of neuron groups. This task is executed when the thread runs.  */
void NetworkDaoThread::prepareLoadNeurons(const QList<NeuronGroup*>& neurGrpList){
    //Store necessary variables
    this->neuronGroupList = neurGrpList;

    //Set the task that will run when the thread starts
    currentTask = LOAD_NEURONS_TASK;

	//Set total number of steps to a value greater than 1 - this will change when task starts
	totalNumberOfSteps = 100;
}


/*! Prepares to load neurons for a single neuron group. This task is executed when the thread runs.  */
void NetworkDaoThread::prepareLoadNeurons(NeuronGroup* neurGrp){
    //Store necessary variables
    neuronGroupList.clear();
    neuronGroupList.append(neurGrp);

    //Set the task that will run when the thread starts
    currentTask = LOAD_NEURONS_TASK;

	//Set total number of steps to a value greater than 1 - this will change when task starts
	totalNumberOfSteps = 100;
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
			setError("Insert ID for ConnectionGroup is invalid.");
			return;
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

		//Add connections to database
		ConnectionList::const_iterator endConGrp = connectionGroup->end();
		for(ConnectionList::const_iterator iter = connectionGroup->begin(); iter != endConGrp; ++iter){
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
    for(QList<NeuronGroup*>::iterator iter = neuronGroupList.begin(); iter != neuronGroupList.end(); ++iter){
		//User friendly pointer to group
		NeuronGroup* neuronGroup = *iter;

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

		//Add neurons
		bool firstTime = true;
		NeuronMap* neurMap = neuronGroup->getNeuronMap();
		NeuronMap* newNeurMap = new NeuronMap();
		NeuronMap::iterator mapEnd = neurMap->end();//Saves accessing this function multiple times
		for(NeuronMap::iterator iter=neurMap->begin(); iter != mapEnd; ++iter){
			QString queryStr("INSERT INTO Neurons (NeuronGroupID, X, Y, Z) VALUES (");
			queryStr += QString::number(neuronGroup->getID()) + ", ";
			queryStr += QString::number(iter.value()->getXPos()) + ", ";
			queryStr += QString::number(iter.value()->getYPos()) + ", ";
			queryStr += QString::number(iter.value()->getZPos()) + ")";
			query = getQuery(queryStr);
			executeQuery(query);

			//Add neuron with correct id to the new map
			int lastInsertID = query.lastInsertId().toInt();
			if(lastInsertID < START_NEURON_ID){
				setError("Insert ID for Neuron is invalid.");
				return;
			}
			(*newNeurMap)[lastInsertID] = iter.value();

			//Store the id in the neuron
			iter.value()->setID(lastInsertID);

			/* Store the first neuron id in the group.
				NOTE: Most neuron groups are stored with continuously increasing IDs, but
				there may be exceptions, so take care! */
			if(firstTime){
				neuronGroup->setStartNeuronID(lastInsertID);
				firstTime = false;
			}

			if(stopThread)
				return;
		}
		//Add the correct map to the neuron group. This should also clean up the old map
		neuronGroup->setNeuronMap(newNeurMap);

		/* Clean up the dynamically allocated neuron map, but keep the dynamically allocated
			neurons, which are used in the new map */
		delete neurMap;

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
	totalNumberOfSteps = getConnectionCount(connectionGroupList);

    //Work through all the connections to be loaded
    for(QList<ConnectionGroup*>::iterator iter = connectionGroupList.begin(); iter != connectionGroupList.end(); ++iter){

		//Empty current connections in group
		(*iter)->clearConnections();

		//Load connections into group
		unsigned int tmpConGrpID = (*iter)->getID();
		QSqlQuery query = getQuery("SELECT ConnectionID, FromNeuronID, ToNeuronID, Delay, Weight, TempWeight FROM Connections WHERE ConnectionGroupID = " + QString::number(tmpConGrpID));
		executeQuery(query);
		while ( query.next() ) {
			Connection* tmpConn = new Connection(
					query.value(0).toUInt(),//ConnectionID
					tmpConGrpID,//Connection Group ID
					query.value(1).toUInt(),//FromNeuronID
					query.value(2).toUInt(),//ToNeuronID
					query.value(3).toString().toFloat(),//Delay
					query.value(4).toString().toFloat(),//Weight
					query.value(5).toString().toFloat()//tempWeight
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
	totalNumberOfSteps = getNeuronCount(neuronGroupList);

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
	qDebug()<<"ERROR FOUND: "<<msg<<" current task: "<<currentTask;
    errorMessage = msg;
    error = true;
    stopThread = true;
}


