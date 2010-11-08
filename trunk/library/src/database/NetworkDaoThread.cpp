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

/*! Switches on output of more debugging information */
#define DEBUG


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
			case DELETE_NETWORK_TASK:
				deleteConnectionGroups();
				deleteNeuronGroups();
				deleteNetwork();
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
			case SAVE_NETWORK_TASK:
				saveNetwork();
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
	neuronGroupIDList.clear();
	neuronGroupList.clear();
	connectionGroupIDList.clear();
	connectionGroupList.clear();
	networkID = 0;
    currentTask = NO_TASK_DEFINED;
    stopThread = true;
}

/*! Starts a thread to delete the specified network */
void NetworkDaoThread::startDeleteNetwork(unsigned networkID){
	this->networkID = networkID;
	//Store list of connection group IDs
	connectionGroupIDList.clear();
	QList<ConnectionGroupInfo> conGrpInfoList = getConnectionGroupsInfo(networkID);
	foreach(ConnectionGroupInfo conGrpInfo, conGrpInfoList)
		connectionGroupIDList.append(conGrpInfo.getID());

	//Store list of neuron group IDs
	neuronGroupIDList.clear();
	QList<NeuronGroupInfo> neurGrpInfoList = getNeuronGroupsInfo(networkID);
	foreach(NeuronGroupInfo neurGrpInfo, neurGrpInfoList)
		neuronGroupIDList.append(neurGrpInfo.getID());

	//Prepare and start task
	currentTask = DELETE_NETWORK_TASK;
	start();
}


/*! Saves network using supplied lists of neuron and connection groups to add and delete */
void NetworkDaoThread::startSaveNetwork(unsigned networkID, QList<NeuronGroup*> newNeuronGroups, QList<ConnectionGroup*> newConnectionGroups, QList<unsigned> deleteNeuronGroupIDs, QList<unsigned> deleteConnectionGroupIDs){
	//Store data
	this->networkID = networkID;
	this->neuronGroupList = newNeuronGroups;
	this->connectionGroupList = newConnectionGroups;
	this->neuronGroupIDList = deleteNeuronGroupIDs;
	this->connectionGroupIDList = deleteConnectionGroupIDs;

	//Start thread running
	currentTask = SAVE_NETWORK_TASK;
	start();
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
	for(QList<ConnectionGroup*>::iterator iter = connectionGroupList.begin(); iter != connectionGroupList.end() && !stopThread; ++iter){
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

		//Set synapse parameters to default values if they have not been set
		if(!connectionGroup->parametersSet()){
			QHash<QString, double> tmpParamMap = getDefaultSynapseParameters(connectionGroup->getSynapseTypeID());
			connectionGroup->setParameters(tmpParamMap);
		}

		//Copy parameters from connection group into parameter table
		QHash<QString, double> tmpParamMap = connectionGroup->getParameters();
		setSynapseParameters(connGrpInfo, tmpParamMap);

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
		ConnectionIterator endConGrp = connectionGroup->end();
		for(ConnectionIterator iter = connectionGroup->begin(); iter != endConGrp && !stopThread; ++iter){
			offset = 5 * (conCntr % numConBuffers);

			//Bind values to query
			tmpConList.append(&(*iter));
			query.bindValue(0 + offset, connectionGroup->getID());
			query.bindValue(1 + offset, iter->getFromNeuronID());
			query.bindValue(2 + offset, iter->getToNeuronID());
			query.bindValue(3 + offset, iter->getDelay());
			query.bindValue(4 + offset, iter->getWeight());

			//Execute query
			if(conCntr % numConBuffers == numConBuffers-1){
				executeQuery(query);

				//Add connection id to connection - last insert id is the id of the first connection in the list of value entries
				int lastInsertID = query.lastInsertId().toInt();
				if( (lastInsertID + tmpConList.size()) > LAST_CONNECTION_ID )
					throw SpikeStreamException("Database generated connection ID is out of range: " + QString::number(lastInsertID + tmpConList.size()) + ". It must be less than or equal to " + QString::number(LAST_CONNECTION_ID));
				if(lastInsertID < START_CONNECTION_ID)
					throw SpikeStreamException("Insert ID for Connection is invalid.");
				if(tmpConList.size() != numConBuffers)
					throw SpikeStreamException("Temporary connection list size " + QString::number(tmpConList.size()) + " does not match number of buffers: " + QString::number(numConBuffers));

				//Set connection ID in connection groups
				for(int i=0; i<tmpConList.size(); ++i){
					tmpConList.at(i)->setID(lastInsertID + i);
					//(*newConMap)[] = tmpConList.at(i);
				}

				//Count number of connections that have been added
				conAddedCntr += numConBuffers;

				//Clear up list
				tmpConList.clear();
			}

			//Keep track of the number of connections
			++conCntr;
		}

		//Add remaining connections individually
		if(!tmpConList.isEmpty() && !stopThread){
			query = getQuery();
			query.prepare("INSERT INTO Connections ( ConnectionGroupID, FromNeuronID, ToNeuronID, Delay, Weight) VALUES (?, ?, ?, ?, ?)");
			for(QList<Connection*>::iterator iter = tmpConList.begin(); iter != tmpConList.end(); ++iter){
				query.bindValue(0, connectionGroup->getID());
				query.bindValue(1, (*iter)->getFromNeuronID());
				query.bindValue(2, (*iter)->getToNeuronID());
				query.bindValue(3, (*iter)->getDelay());
				query.bindValue(4, (*iter)->getWeight());

				//Execute query
				executeQuery(query);

				//Add connection id to connection
				int lastInsertID = query.lastInsertId().toInt();
				if( lastInsertID > LAST_CONNECTION_ID )
					throw SpikeStreamException("Database generated connection ID is out of range: " + QString::number(lastInsertID) + ". It must be less than or equal to " + QString::number(LAST_CONNECTION_ID));
				if(lastInsertID < START_CONNECTION_ID)
					throw SpikeStreamException("Insert ID for Connection is invalid.");
				//(*newConMap)[lastInsertID] = *iter;
				(*iter)->setID(lastInsertID);

				//Count number of connections that have been added
				++conAddedCntr;
			}
		}

		//Check that we have added all the connections
		if(!stopThread && (connectionGroup->size() != conAddedCntr) )
			throw SpikeStreamException("Number of connections added to database: " + QString::number(conAddedCntr) + " does not match size of connection group: " + QString::number(connectionGroup->size()));

		#ifdef TIME_PERFORMANCE
			timer.printTime("Number of buffers: " + QString::number(numConBuffers) + ". Number of connections remaining: " + QString::number(tmpConList.size()) + ". Adding " + QString::number(conCntr) + " connections");
		#endif//TIME_PERFORMANCE
    }

	//Clean up connection groups if task was cancelled.
	if(stopThread){
		connectionGroupIDList.clear();
		foreach(ConnectionGroup* conGrp, connectionGroupList)
			connectionGroupIDList.append(conGrp->getID());
		deleteConnectionGroups();
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
	for(QList<NeuronGroup*>::iterator neurGrpIter = neuronGroupList.begin(); neurGrpIter != neuronGroupList.end() && !stopThread; ++neurGrpIter){
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

		//Check neuron group has parameters set and add default parameters if not
		if(!neuronGroup->parametersSet()){
			QHash<QString, double> tmpParamMap = getDefaultNeuronParameters(neuronGroup->getNeuronTypeID());
			neuronGroup->setParameters(tmpParamMap);
		}

		//Copy parameters from neuron group into parameter table
		QHash<QString, double> tmpParamMap = neuronGroup->getParameters();
		setNeuronParameters(neurGrpInfo, tmpParamMap);

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
		for(NeuronIterator neurIter = neuronGroup->begin(); neurIter != endNeurGrp && !stopThread; ++neurIter){
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
				if( (lastInsertID + tmpNeurList.size()) > LAST_NEURON_ID )
					throw SpikeStreamException("Database generated neuron ID is out of range: " + QString::number(lastInsertID + tmpNeurList.size()) + ". It must be less than or equal to " + QString::number(LAST_NEURON_ID));
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
				if( (lastInsertID) > LAST_NEURON_ID )
					throw SpikeStreamException("Database generated neuron ID is out of range: " + QString::number(lastInsertID) + ". It must be less than or equal to " + QString::number(LAST_NEURON_ID));
				if(lastInsertID < START_NEURON_ID)
					throw SpikeStreamException("Insert ID for Neuron is invalid.");
				(*neurListIter)->setID(lastInsertID);
				(*newNeurMap)[lastInsertID] = *neurListIter;

				//Count number of neurons that have been added
				++neurAddedCntr;
			}
		}

		//Check that we have added all the neurons
		if(!stopThread && (neuronGroup->size() != neurAddedCntr))
			throw SpikeStreamException("Number of neurons added to database: " + QString::number(neurAddedCntr) + " does not match size of neuron group: " + QString::number(neuronGroup->size()));


		//Set the start ID of the neuron group
		NetworkDao networkDao(this->getDBInfo());
		neuronGroup->setStartNeuronID( networkDao.getStartNeuronID(tmpNeurGrpID) );

		#ifdef TIME_PERFORMANCE
			timer.printTime("Adding neurons. Number of buffers: " + QString::number(numNeurBuffers) + ". Number of neurons remaining: " + QString::number(tmpNeurList.size()) + ". Added " + QString::number(neurAddedCntr) + " neurons");
		#endif//TIME_PERFORMANCE

		//Add the new map to the neuron group. This should also clean up the old map
		neuronGroup->setNeuronMap(newNeurMap);
    }

	//Clean up neuron groups if task was cancelled.
	if(stopThread){
		neuronGroupIDList.clear();
		foreach(NeuronGroup* neurGrp, neuronGroupList)
			neuronGroupIDList.append(neurGrp->getID());
		deleteNeuronGroups();
	}
}


/*! Deletes connection groups from the SpikeStreamNetwork database. */
void NetworkDaoThread::deleteConnectionGroups(){
	//Check that parameters have been set correctly
	if(networkID == 0){
		setError("Network ID has not been set.");
		return;
	}

	//Work through the list of connection groups
	foreach(unsigned int conGroupID, connectionGroupIDList){
		executeQuery("DELETE FROM Connections WHERE ConnectionGroupID=" + QString::number(conGroupID) );
		executeQuery("DELETE FROM ConnectionGroups WHERE NetworkID="+ QString::number(networkID) + " AND ConnectionGroupID=" + QString::number(conGroupID) );
	}

	//Reset list
	connectionGroupIDList.clear();
}


/*! Deletes a network from the database. Does nothing if a network with the specified
	id does not exist. */
void NetworkDaoThread::deleteNetwork(){
	executeQuery("DELETE FROM Networks WHERE NetworkID = " + QString::number(networkID));
	networkID = 0;
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
		executeQuery("DELETE FROM Neurons WHERE NeuronGroupID=" + QString::number(neuronGroupID) );
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
			(*iter)->addConnection(
					query.value(0).toUInt(),//ID
					query.value(1).toUInt(),//FromNeuronID
					query.value(2).toUInt(),//ToNeuronID
					query.value(3).toString().toFloat(),//Delay
					query.value(4).toString().toFloat()//Weight
			);

			//Track progress
			++numberOfCompletedSteps;

			//Quit if user cancels
			if(stopThread)
				return;
		}

		//Load parameters in connection group
		QHash<QString, double> tmpParamMap = getSynapseParameters( (*iter)->getInfo());
		(*iter)->setParameters(tmpParamMap);
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


/*! Saves a network that has been created or edited in prototype mode to the database. */
void NetworkDaoThread::saveNetwork(){
	#ifdef DEBUG
		cout<<"Saving network."<<endl;
		if(!neuronGroupIDList.isEmpty()){
			cout<<"Deleting neuron groups with IDs: ";
			foreach(unsigned int neuronGroupID, neuronGroupIDList){
				cout<<neuronGroupID<<" ";
			}
			cout<<endl;
		}
		if(!connectionGroupIDList.isEmpty()){
			cout<<"Deleting connection groups with IDs: ";
			foreach(unsigned int neuronGroupID, connectionGroupIDList){
				cout<<neuronGroupID<<" ";
			}
			cout<<endl;
		}
		if(!neuronGroupList.isEmpty()){
			cout<<"Adding neuron groups with temporary IDs: ";
			foreach(NeuronGroup* neuronGroup, neuronGroupList){
				cout<<neuronGroup->getID()<<" ";
			}
			cout<<endl;
		}
		if(!connectionGroupList.isEmpty()){
			cout<<"Adding connection groups with temporary IDs: ";
			foreach(ConnectionGroup* conGroup, connectionGroupList){
				cout<<conGroup->getID()<<" ";
			}
			cout<<endl;
		}
	#endif//DEBUG

	//Delete neuron and connection groups that have been deleted from the prototype
	deleteNeuronGroups();
	deleteConnectionGroups();

	/* Store link between old IDs and neurons in the groups that we are going to add.
	   Also store link between old neuron group IDs and neuron group IDs */
	QHash<unsigned, Neuron*> oldIDNeurMap;
	QHash<unsigned, NeuronGroup*> oldIDNeurGrpMap;
	foreach(NeuronGroup* tmpNeurGrp, neuronGroupList){
		//Store old neuron group ID
		oldIDNeurGrpMap[tmpNeurGrp->getID()] = tmpNeurGrp;

		//Store old neuron IDS
		QHash<unsigned, Neuron*>::iterator endNeurGrp = tmpNeurGrp->end();
		for(QHash<unsigned, Neuron*>::iterator neurIter = tmpNeurGrp->begin(); neurIter != endNeurGrp; ++neurIter){
			oldIDNeurMap[neurIter.key()] = neurIter.value();
		}
	}

	//Add neuron groups to database, the new IDs wil be stored in the neurons and neuron groups
	addNeuronGroups();

	//Update connections with the new IDs
	foreach(ConnectionGroup* tmpConGrp, connectionGroupList){
		//Update FROM neuron group ID and FROM neuron IDs
		if(oldIDNeurGrpMap.contains(tmpConGrp->getFromNeuronGroupID())){
			tmpConGrp->setFromNeuronGroupID(oldIDNeurGrpMap[tmpConGrp->getFromNeuronGroupID()]->getID());
			ConnectionIterator endConGrp = tmpConGrp->end();
			for(ConnectionIterator conIter = tmpConGrp->begin(); conIter!= endConGrp; ++conIter){
				//Check FROM neuron ID exists
				if(oldIDNeurMap.contains(conIter->getFromNeuronID()))
					conIter->setFromNeuronID(oldIDNeurMap[conIter->getFromNeuronID()]->getID());
				else
					throw SpikeStreamException("FROM neuron ID missing from old ID neuron map: " + QString::number(conIter->getFromNeuronID()));
			}
		}
		//Update TO neuron group ID and TO neuron IDs
		if(oldIDNeurGrpMap.contains(tmpConGrp->getToNeuronGroupID())){
			tmpConGrp->setToNeuronGroupID(oldIDNeurGrpMap[tmpConGrp->getToNeuronGroupID()]->getID());
			ConnectionIterator endConGrp = tmpConGrp->end();
			for(ConnectionIterator conIter = tmpConGrp->begin(); conIter!= endConGrp; ++conIter){
				//Check TO neuron ID exists
				if(oldIDNeurMap.contains(conIter->getToNeuronID()))
					conIter->setToNeuronID(oldIDNeurMap[conIter->getToNeuronID()]->getID());
				else
					throw SpikeStreamException("TO neuron ID missing from old ID neuron map: " + QString::number(conIter->getToNeuronID()));
			}
		}
	}

	//Add connection groups to the database, the new IDs will be stored in the connection groups
	addConnectionGroups();
}

