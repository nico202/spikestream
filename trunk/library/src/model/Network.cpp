//SpikeStream includes
#include "GlobalVariables.h"
#include "Network.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;

//Outputs memory debugging information
//#define MEMORY_DEBUG

/*! Constructor that creates a new network and adds it to the database */
Network::Network(const QString& name, const QString& description, const DBInfo& networkDBInfo, const DBInfo& archiveDBInfo){
    //Store information
	this->setObjectName(name);
    this->info.setName(name);
    this->info.setDescription(description);
	this->networkDBInfo = networkDBInfo;
	this->archiveDBInfo = archiveDBInfo;

    //Create network dao threads for heavy operations
	neuronNetworkDaoThread = new NetworkDaoThread(networkDBInfo, "neurNetDao1");
    connect (neuronNetworkDaoThread, SIGNAL(finished()), this, SLOT(neuronThreadFinished()));
	connectionNetworkDaoThread = new NetworkDaoThread(networkDBInfo, "conNetDao1");
    connect (connectionNetworkDaoThread, SIGNAL(finished()), this, SLOT(connectionThreadFinished()));

    //Initialize variables
	initializeVariables();

    //Create new network in database. ID will be stored in the network info
	NetworkDao networkDao(networkDBInfo);
	networkDao.addNetwork(info);
}


/*! Constructor when using an existing network */
Network::Network(const NetworkInfo& networkInfo, const DBInfo& networkDBInfo, const DBInfo& archiveDBInfo){
    //Store information
	this->setObjectName(info.getName());
    this->info = networkInfo;
	this->networkDBInfo = networkDBInfo;
	this->archiveDBInfo = archiveDBInfo;

    //Check that network ID is valid
    if(info.getID() == INVALID_NETWORK_ID){
		throw SpikeStreamException ("Attempting to load an invalid network");
    }

    //Create network dao threads for heavy operations
	neuronNetworkDaoThread = new NetworkDaoThread(networkDBInfo, "neurNetDao2");
    connect (neuronNetworkDaoThread, SIGNAL(finished()), this, SLOT(neuronThreadFinished()));
	connectionNetworkDaoThread = new NetworkDaoThread(networkDBInfo, "conNetDao2");
    connect (connectionNetworkDaoThread, SIGNAL(finished()), this, SLOT(connectionThreadFinished()));

    //Initialize variables
	initializeVariables();

    //Load up basic information about the neuron and connection groups
    loadNeuronGroupsInfo();
    loadConnectionGroupsInfo();
}


/*! Destructor */
Network::~Network(){
	#ifdef MEMORY_DEBUG
		cout<<"Deleting network"<<endl;
	#endif//MEMORY_DEBUG

    if(neuronNetworkDaoThread != NULL){
		neuronNetworkDaoThread->stop();
		neuronNetworkDaoThread->wait();
		delete neuronNetworkDaoThread;
    }
    if(connectionNetworkDaoThread != NULL){
		connectionNetworkDaoThread->stop();
		connectionNetworkDaoThread->wait();
		delete connectionNetworkDaoThread;
    }

    //Empties all data stored in the class
	clearConnectionGroups();
	clearNeuronGroups();
}



/*--------------------------------------------------------- */
/*-----                PUBLIC METHODS                 ----- */
/*--------------------------------------------------------- */

/*! Adds connection groups to the network. */
void Network::addConnectionGroups(QList<ConnectionGroup*>& connectionGroupList){
	//Clear variables
	connectionTaskCancelled = false;

	//Check if network is editable or not
	if(!prototypeMode && hasArchives())
		throw SpikeStreamException("Cannot add connection groups to a locked network.");

	//Set default parameters
	NetworkDao netDao(networkDBInfo);
	foreach(ConnectionGroup* conGrp, connectionGroupList){
		if(!conGrp->parametersSet()){
			QHash<QString, double> tmpParamMap = netDao.getDefaultSynapseParameters(conGrp->getSynapseTypeID());
			conGrp->setParameters(tmpParamMap);
		}
	}

	//In prototype mode, we add connection groups to network and store them in a list for later
	if(prototypeMode){
		foreach(ConnectionGroup* conGrp, connectionGroupList){
			unsigned tmpID = getTemporaryConGrpID();//Get an ID for the connection groups
			conGrp->setID(tmpID);//Set ID in connection group
			connGrpMap[tmpID] = conGrp;//Add connection group to network
			newConnectionGroupMap[tmpID] = conGrp;//Store connection group in map for adding to database later
			#ifdef MEMORY_DEBUG
				cout<<"Network size: "<<sizeof(*this)<<"; adding connection group with memory size: "<<sizeof(*conGrp);
				cout<<"; size of 1 connection: "<<sizeof(*conGrp->begin())<<endl;
			#endif//MEMORY_DEBUG
		}
	}
	//In normal mode, connection groups are saved immediately to the database
	else{
		//Store the connection groups to be added in the map with ids increasing from 1
		newConnectionGroupMap.clear();
		for(int i=0; i<connectionGroupList.size(); ++i)
			newConnectionGroupMap[i+1] = connectionGroupList.at(i);

		//Start thread that adds connection groups to database
		startAddConnectionGroups();
	}
}


/*! Adds neuron groups to the network */
void Network::addNeuronGroups(QList<NeuronGroup*>& neuronGroupList){
	//Reset variables
	neuronTaskCancelled = false;

	//Check if network is editable or not
	if(!prototypeMode && hasArchives())
		throw SpikeStreamException("Cannot add neuron groups to a locked network.");

	//Set default parameters
	NetworkDao netDao(networkDBInfo);
	foreach(NeuronGroup* neurGrp, neuronGroupList){
		if(!neurGrp->parametersSet()){
			QHash<QString, double> tmpParamMap = netDao.getDefaultNeuronParameters(neurGrp->getNeuronTypeID());
			neurGrp->setParameters(tmpParamMap);
		}
	}

	//In prototype mode, we add connection groups to network and store them in a list for later
	if(prototypeMode){
		foreach(NeuronGroup* neurGrp, neuronGroupList){
			unsigned tmpID = getTemporaryNeurGrpID();//Get an ID for the neuron group
			neurGrp->setID(tmpID);//Set ID in neuron group
			neurGrpMap[tmpID] = neurGrp;//Add neuron group to network
			newNeuronGroupMap[tmpID] = neurGrp;//Store neuron group in map for adding to database later
		}
	}
	//In normal mode, neuron groups are saved immediately to the database
	else{
		//Store the neuron groups to be added in the map with ids increasing from 1
		newNeuronGroupMap.clear();
		for(int i=0; i<neuronGroupList.size(); ++i)
			newNeuronGroupMap[i+1] = neuronGroupList.at(i);

		//Start thread to add neuron groups to database
		startAddNeuronGroups();
	}
}


/*! Cancels connection thread-based operations that are in progress.
	It is the responsibility of the dao thread to clean up when task has been stopped. */
void Network::cancel(){
	if(connectionNetworkDaoThread->isRunning()){
		connectionNetworkDaoThread->stop();
		connectionTaskCancelled = true;
	}
	if(neuronNetworkDaoThread->isRunning()){
		neuronNetworkDaoThread->stop();
		neuronTaskCancelled = true;
	}
}


/*! Clears the error state */
void Network::clearError(){
	error = false;
	errorMessage = "";
}


/*! Returns true if a neuron with the specified ID is in the network */
bool Network::containsNeuron(unsigned int neurID){
    //Need to check each neuron group to see if it contains the neuron.
    for(QHash<unsigned int, NeuronGroup*>::iterator iter = neurGrpMap.begin(); iter != neurGrpMap.end(); ++iter){
		if(iter.value()->contains(neurID)){
			return true;
		}
    }
    return false;
}


/*! Returns true if the network contains the neuron group with the specified ID. */
bool  Network::containsNeuronGroup(unsigned int neuronGroupID){
	if(neurGrpMap.contains(neuronGroupID))
		return true;
	return false;
}


/*! Removes the specified connection groups from the network and database.
	Throws an exception if the connection groups cannot be found */
void Network::deleteConnectionGroups(QList<unsigned>& deleteConGrpIDList){
	//Check if network is editable or not
	if(!prototypeMode && hasArchives())
		throw SpikeStreamException("Cannot delete connection groups from a locked network.");

	//Check that network is not currently busy with some other task
	if(connectionNetworkDaoThread->isRunning())
		throw SpikeStreamException("Network is busy with another connection-related task.");

	//Check that connection group ids exist in network
	foreach(unsigned int conGrpID, deleteConGrpIDList){
		if(!connGrpMap.contains(conGrpID))
			throw SpikeStreamException("Connection group ID " + QString::number(conGrpID) + " cannot be found in the current network.");
	}

	//Remove connection groups from memory
	foreach(unsigned conGrpID, deleteConGrpIDList){
		deleteConnectionGroupFromMemory(conGrpID);
	}

	if(prototypeMode){
		if(!isBusy())
			emit taskFinished();//Inform listening classes that deletion is complete
		else
			throw SpikeStreamException("Network is busy when deleting connection groups - it should not be engaged in other tasks.");
	}
	else{
		startDeleteConnectionGroups(deleteConGrpIDList);
	}
}


/*! Removes the specified neuron groups from the network and database.
	Throws an exception if the neuron groups cannot be found */
void Network::deleteNeuronGroups(QList<unsigned int>& deleteNeurGrpIDList){
	//Check if network is editable or not
	if(hasArchives())
		throw SpikeStreamException("Cannot delete neuron groups from a locked network.");

	//Check that network is not currently busy with some other task
	if(neuronNetworkDaoThread->isRunning())
		throw SpikeStreamException("Network is busy with another neuron-related task.");

	//Check that neuron group ids exist in network
	foreach(unsigned int neurGrpID, deleteNeurGrpIDList){
		if(!neurGrpMap.contains(neurGrpID))
			throw SpikeStreamException("Neuron group ID " + QString::number(neurGrpID) + " cannot be found in the current network.");
	}

	foreach(unsigned neurGrpID, deleteNeurGrpIDList){
		//Remove neuron groups from memory along with their associated connections
		deleteNeuronGroupFromMemory(neurGrpID);
	}

	if(prototypeMode){
		if(!isBusy())
			emit taskFinished();//Inform listening classes that deletion is complete
		else
			throw SpikeStreamException("Network is busy when deleting neuron groups - it should not be engaged in other tasks.");
	}
	else{
		startDeleteNeuronGroups(deleteNeurGrpIDList);
	}
}


/*! Returns the number of connections in the group with the specified ID.
	Throws an exception if a connection group with the specified ID is not present in the network. */
int Network::getConnectionCount(unsigned conGrpID){
	if(!connGrpMap.contains(conGrpID))
		throw SpikeStreamException("Error getting connection count. Connection group with the specified ID does not exist in this network.");
	return connGrpMap[conGrpID]->size();
}


/*! Returns a complete list of connection groups */
QList<ConnectionGroup*> Network::getConnectionGroups(){
	QList<ConnectionGroup*> tmpList;
	for(QHash<unsigned int, ConnectionGroup*>::iterator iter = connGrpMap.begin(); iter != connGrpMap.end(); ++iter)
		tmpList.append(iter.value());
	return tmpList;
}


/*! Returns a complete list of connection group infos */
QList<ConnectionGroupInfo> Network::getConnectionGroupsInfo(){
    QList<ConnectionGroupInfo> tmpList;
    for(QHash<unsigned int, ConnectionGroup*>::iterator iter = connGrpMap.begin(); iter != connGrpMap.end(); ++iter)
		tmpList.append(iter.value()->getInfo());
    return tmpList;
}


/*! Returns a list of connection group infos filtered by the specified synapse type ID */
QList<ConnectionGroupInfo> Network::getConnectionGroupsInfo(unsigned int synapseTypeID){
	QList<ConnectionGroupInfo> tmpList;
	for(QHash<unsigned int, ConnectionGroup*>::iterator iter = connGrpMap.begin(); iter != connGrpMap.end(); ++iter){
		if(iter.value()->getInfo().getSynapseTypeID() == synapseTypeID)
			tmpList.append(iter.value()->getInfo());
	}
	return tmpList;
}


/*! Returns the neuron group containing the specified neuron ID.
	Throws an exception if this cannot be found.  */
NeuronGroup* Network::getNeuronGroupFromNeuronID(unsigned neuronID){
	//Need to check each neuron group to see if it contains the neuron.
	for(QHash<unsigned int, NeuronGroup*>::iterator iter = neurGrpMap.begin(); iter != neurGrpMap.end(); ++iter){
		if(iter.value()->contains(neuronID)){
			return iter.value();
		}
	}
	throw SpikeStreamException("Neuron group containing neuron with id " + QString::number(neuronID) + " cannot be found.");
}


/*! Returns a list of the neuron groups in the network. */
QList<NeuronGroup*> Network::getNeuronGroups(){
	QList<NeuronGroup*> tmpList;
	for(QHash<unsigned int, NeuronGroup*>::iterator iter = neurGrpMap.begin(); iter != neurGrpMap.end(); ++iter)
		tmpList.append(iter.value());
	return tmpList;
}


/*! Returns a complete list of neuron group infos */
QList<NeuronGroupInfo> Network::getNeuronGroupsInfo(){
    QList<NeuronGroupInfo> tmpList;
    for(QHash<unsigned int, NeuronGroup*>::iterator iter = neurGrpMap.begin(); iter != neurGrpMap.end(); ++iter)
		tmpList.append(iter.value()->getInfo());
    return tmpList;
}


/*! Returns a list of neuron group infos filtered by the specified neuron type ID */
QList<NeuronGroupInfo> Network::getNeuronGroupsInfo(unsigned int neuronTypeID){
	QList<NeuronGroupInfo> tmpList;
	for(QHash<unsigned int, NeuronGroup*>::iterator iter = neurGrpMap.begin(); iter != neurGrpMap.end(); ++iter){
		if(iter.value()->getInfo().getNeuronTypeID() == neuronTypeID)
			tmpList.append(iter.value()->getInfo());
	}
	return tmpList;
}


/*! Returns true if heavy thread-based operations are in progress */
bool Network::isBusy() {
    if(neuronNetworkDaoThread->isRunning() || connectionNetworkDaoThread->isRunning())
		return true;
    return false;
}


/*! Returns a list of the neuron group ids in the network */
QList<unsigned int> Network::getNeuronGroupIDs(){
    return neurGrpMap.keys();
}


/*! Returns a list of the connection group ids in the network */
QList<unsigned int> Network::getConnectionGroupIDs(){
    return connGrpMap.keys();
}


/*! Returns a box that encloses the network */
Box Network::getBoundingBox(){
    /* Neurons are not directly linked with a neuron id, so need to work through
       each neuron group and create a box that includes all the other boxes. */
    Box networkBox;
    bool firstTime = true;
    QList<NeuronGroup*> tmpNeurGrpList = neurGrpMap.values();
    for(QList<NeuronGroup*>::iterator iter = tmpNeurGrpList.begin(); iter != tmpNeurGrpList.end(); ++iter){
		if(firstTime){//Take box enclosing first neuron group as a starting point
			networkBox = (*iter)->getBoundingBox();
			firstTime = false;
		}
		else{//Expand box to include subsequent neuron groups
			Box neurGrpBox = (*iter)->getBoundingBox();
			if(neurGrpBox.x1 < networkBox.x1)
				networkBox.x1 = neurGrpBox.x1;
			if(neurGrpBox.y1 < networkBox.y1)
				networkBox.y1 = neurGrpBox.y1;
			if(neurGrpBox.z1 < networkBox.z1)
				networkBox.z1 = neurGrpBox.z1;

			if(neurGrpBox.x2 > networkBox.x2)
				networkBox.x2 = neurGrpBox.x2;
			if(neurGrpBox.y2 > networkBox.y2)
				networkBox.y2 = neurGrpBox.y2;
			if(neurGrpBox.z2 > networkBox.z2)
				networkBox.z2 = neurGrpBox.z2;
		}
    }
    return networkBox;
}


/*! Returns true if a neuron group in the network overlaps with the specified box. */
bool Network::overlaps(const Box& box){
	QList<NeuronGroup*> tmpNeurGrpList = neurGrpMap.values();
	for(QList<NeuronGroup*>::iterator iter = tmpNeurGrpList.begin(); iter != tmpNeurGrpList.end(); ++iter){
		if( (*iter)->getBoundingBox().intersects(box) )
			return true;
	}
	return false;
}


/*! Returns a box that encloses the specified neuron group */
Box Network::getNeuronGroupBoundingBox(unsigned int neurGrpID){
	return getNeuronGroup(neurGrpID)->getBoundingBox();
}


/*! Returns the neuron group with the specified id.
    An exception is thrown if the neuron group id is not in the network.
    Lazy loading used, so that neuron groups are loaded only if requested. */
NeuronGroup* Network::getNeuronGroup(unsigned int id){
    checkNeuronGroupID(id);
    return neurGrpMap[id];
}


/*! Returns information about the neuron group with the specified id */
NeuronGroupInfo Network::getNeuronGroupInfo(unsigned int id){
    checkNeuronGroupID(id);
    return neurGrpMap[id]->getInfo();
}


/*! Returns the connection group with the specified id.
    An exception is thrown if the connection group id is not in the network.*/
ConnectionGroup* Network::getConnectionGroup(unsigned int id){
    checkConnectionGroupID(id);
    return connGrpMap[id];
}


/*! Returns information about the connection group with the specified id */
ConnectionGroupInfo Network::getConnectionGroupInfo(unsigned int id){
    checkConnectionGroupID(id);
    return connGrpMap[id]->getInfo();
}


/*! Returns the message associated with an error */
QString Network::getErrorMessage(){
	return errorMessage;
}


/*! Returns the number of steps that have been completed so far during a heavy operation. */
int Network::getNumberOfCompletedSteps(){
	int numSteps = 0;
	if(neuronNetworkDaoThread->isRunning())
		numSteps += neuronNetworkDaoThread->getNumberOfCompletedSteps();
	if(connectionNetworkDaoThread->isRunning())
		numSteps += connectionNetworkDaoThread->getNumberOfCompletedSteps();
	return numSteps;
}


/*! Returns the number of steps involved in the current tasks */
int Network::getTotalNumberOfSteps(){
	int total = 0;
	if(neuronNetworkDaoThread->isRunning())
		total += neuronNetworkDaoThread->getTotalNumberOfSteps();
	if(connectionNetworkDaoThread->isRunning())
		total += connectionNetworkDaoThread->getTotalNumberOfSteps();
	return total;
}


/*! Returns true if the network is not editable because it is associated with archives */
bool Network::hasArchives(){
	ArchiveDao archiveDao(archiveDBInfo);
	return archiveDao.networkHasArchives(getID());
}


/*! Returns true if the network matches the database */
bool Network::isSaved(){
	if(prototypeMode){
		if(!newConnectionGroupMap.isEmpty())
			return false;
		if(!newNeuronGroupMap.isEmpty())
			return false;
		if(!deleteNeuronGroupIDs.isEmpty())
			return false;
		if(!deleteConnectionGroupIDs.isEmpty())
			return false;
		if(neuronGroupParametersChanged || connectionGroupParametersChanged)
			return false;
	}
	return true;
}


/*! Loads up the network from the database using separate threads */
void Network::load(){
    clearError();

    //Load up all neurons
    neuronNetworkDaoThread->prepareLoadNeurons(neurGrpMap.values());
    currentNeuronTask = LOAD_NEURONS_TASK;
    neuronNetworkDaoThread->start();

	//Load all connection groups
	connectionNetworkDaoThread->prepareLoadConnections(connGrpMap.values());
    currentConnectionTask = LOAD_CONNECTIONS_TASK;
    connectionNetworkDaoThread->start();
}



/*! Loads up the network from the database without using separate threads.
	Only returns when load is complete. Mainly used for testing */
void Network::loadWait(){
	clearError();

	//Load up all neurons
	neuronNetworkDaoThread->prepareLoadNeurons(neurGrpMap.values());
	currentNeuronTask = LOAD_NEURONS_TASK;
	neuronNetworkDaoThread->start();
	neuronNetworkDaoThread->wait();
	neuronThreadFinished();

	//Load all connection groups
	connectionNetworkDaoThread->prepareLoadConnections(connGrpMap.values());
	currentConnectionTask = LOAD_CONNECTIONS_TASK;
	connectionNetworkDaoThread->start();
	connectionNetworkDaoThread->wait();
	connectionThreadFinished();
}


/*! Saves the network.
	Throws an exception if network is not in prototyping mode. */
void Network::save(){
	if(!prototypeMode)
		throw SpikeStreamException("Network should not be saved unless it is in prototype mode.");

	//Remove connection and neuron groups from network - they will be added later with the correct IDs.
	for(QHash<unsigned, ConnectionGroup*>::iterator iter = newConnectionGroupMap.begin(); iter != newConnectionGroupMap.end(); ++iter)
		connGrpMap.remove(iter.value()->getID());
	for(QHash<unsigned, NeuronGroup*>::iterator iter = newNeuronGroupMap.begin(); iter != newNeuronGroupMap.end(); ++iter)
		neurGrpMap.remove(iter.value()->getID());

	//Start thread to add/delete from database and get appropriate IDs inside the classes.
	currentNeuronTask = SAVE_NETWORK_TASK;
	neuronNetworkDaoThread->startSaveNetwork(getID(),
			newNeuronGroupMap.values(), newConnectionGroupMap.values(),
			deleteNeuronGroupIDs, deleteConnectionGroupIDs
	);

	//Store parameters in already saved neuron groups
	if(neuronGroupParametersChanged){
		NetworkDao netDao(networkDBInfo);
		for(QHash<unsigned int, NeuronGroup*>::iterator iter = neurGrpMap.begin(); iter != neurGrpMap.end(); ++iter){
			QHash<QString, double> tmpParamMap = iter.value()->getParameters();
			netDao.setNeuronParameters(iter.value()->getInfo(), tmpParamMap);
		}
		neuronGroupParametersChanged = false;
	}

	//Store parameters in already saved connection groups
	if(connectionGroupParametersChanged){
		NetworkDao netDao(networkDBInfo);
		for(QHash<unsigned int, ConnectionGroup*>::iterator iter = connGrpMap.begin(); iter != connGrpMap.end(); ++iter){
			QHash<QString, double> tmpParamMap = iter.value()->getParameters();
			netDao.setSynapseParameters(iter.value()->getInfo(), tmpParamMap);
		}
		connectionGroupParametersChanged = false;
	}
}


/*! Sets the description of a connection group.
	Throws an exception if the connection group cannot be found. */
void Network::setConnectionGroupProperties(unsigned conGrpID, const QString& description){
	if(!connGrpMap.contains(conGrpID))
		throw SpikeStreamException("Connection group cannot be found: " + QString::number(conGrpID));
	connGrpMap[conGrpID]->setDescription(description);

	//Store in database if connection group is in database
	if(!newConnectionGroupMap.contains(conGrpID)){
		NetworkDao netDao(networkDBInfo);
		netDao.setConnectionGroupProperties(conGrpID, description);
	}
}


/*! Sets the parameters of a particular neuron group */
void Network::setNeuronGroupParameters(unsigned neurGrpID, QHash<QString, double> paramMap){
	//Set the parameters in memory
	NeuronGroup* tmpNeurGrp = getNeuronGroup(neurGrpID);
	tmpNeurGrp->setParameters(paramMap);

	//Store parameters in the database if we are not in prototype mode
	if(!prototypeMode){
		NetworkDao netDao(networkDBInfo);
		netDao.setNeuronParameters(tmpNeurGrp->getInfo(), paramMap);
	}
	else{
		//Record fact that parameters have been changed if the neuron group is on disk
		if(!newNeuronGroupMap.contains(neurGrpID))
			neuronGroupParametersChanged = true;
	}
}


/*! Sets the name and description of a neuron group.
	Throws an exception of the neuron group cannot be found. */
void Network::setNeuronGroupProperties(unsigned neurGrpID, const QString& name, const QString& description){
	if(!neurGrpMap.contains(neurGrpID))
		throw SpikeStreamException("Neuron group cannot be found: " + QString::number(neurGrpID));
	neurGrpMap[neurGrpID]->setDescription(description);
	neurGrpMap[neurGrpID]->setName(name);

	//Store in database if we are not in prototype mode
	if(!newNeuronGroupMap.contains(neurGrpID)){
		NetworkDao netDao(networkDBInfo);
		netDao.setNeuronGroupProperties(neurGrpID, name, description);
	}
}


/*! Puts the network into prototype mode */
void Network::setPrototypeMode(bool mode){
	prototypeMode = mode;
}


/*--------------------------------------------------------- */
/*-----                  PRIVATE SLOTS                ----- */
/*--------------------------------------------------------- */

/*! Slot called when thread processing connections has finished running. */
void Network::connectionThreadFinished(){
    if(connectionNetworkDaoThread->isError()){
		setError("Connection Loading Error: '" + connectionNetworkDaoThread->getErrorMessage() + "'. ");
    }
	if(!error){
		try{
			switch(currentConnectionTask){
				case ADD_CONNECTIONS_TASK:
					updateConnectionGroupsAfterSave();
				break;
				case DELETE_CONNECTIONS_TASK:
					;
				break;
				case LOAD_CONNECTIONS_TASK:
					;//Nothing to do at present
				break;
				default:
					throw SpikeStreamException("The current task ID has not been recognized.");
			}
		}
		catch(SpikeStreamException& ex){
			setError("End connection thread error " + ex.getMessage());
		}
	}

    //Reset task
    currentConnectionTask = -1;

    if(!isBusy())
		emit taskFinished();
}


/*! Slot called when thread processing neurons has finished running. */
void Network::neuronThreadFinished(){
    //Check for errors
    if(neuronNetworkDaoThread->isError()){
		setError(neuronNetworkDaoThread->getErrorMessage() + "'. ");
    }

    //Handle any task-specific stuff
    if(!error){
		try{
			switch(currentNeuronTask){
				case ADD_NEURONS_TASK:
					updateNeuronGroupsAfterSave();
				break;
				case DELETE_NEURONS_TASK:
					;
				break;
				case LOAD_NEURONS_TASK:
					;//Nothing to do at present
				break;
				case SAVE_NETWORK_TASK:
					//Make IDs in memory match IDs in database
					updateNeuronGroupsAfterSave();
					updateConnectionGroupsAfterSave();

					//Remove IDs of neuron and connetion groups scheduled for deletion
					deleteNeuronGroupIDs.clear();
					deleteConnectionGroupIDs.clear();
				break;
				default:
					throw SpikeStreamException("The current task ID has not been recognized.");
			}
		}
		catch(SpikeStreamException& ex){
			setError("End neuron thread error: " + ex.getMessage());
		}
    }

    //Reset task
    currentNeuronTask = -1;

    //Emit signal that task has finished if no other threads are carrying out operations
    if(!isBusy())
		emit taskFinished();
}


/*! Returns the number of neurons in the network.
	Throws exception if this is called when network is not loaded. */
int Network::size(){
	if(this->isBusy())
		throw SpikeStreamException("Size of network cannot be determined while network is busy.");

	int neurCnt = 0;
	for(QHash<unsigned int, NeuronGroup*>::iterator iter = neurGrpMap.begin(); iter != neurGrpMap.end(); ++iter)
		neurCnt += iter.value()->size();
	return neurCnt;
}


/*--------------------------------------------------------- */
/*-----                PRIVATE METHODS                ----- */
/*--------------------------------------------------------- */

/*! Checks that the specified neuron group id is present in the network
    and throws an exception if not. */
void Network::checkConnectionGroupID(unsigned int id){
    if(!connGrpMap.contains(id))
		throw SpikeStreamException(QString("Connection group with id ") + QString::number(id) + " is not in network with id " + QString::number(getID()));
}


/*! Checks that the specified neuron group id is present in the network
    and throws an exception if not. */
void Network::checkNeuronGroupID(unsigned int id){
    if(!neurGrpMap.contains(id))
		throw SpikeStreamException(QString("Neuron group with id ") + QString::number(id) + " is not in network with id " + QString::number(getID()));
}


/*! Removes the specified connection group from memory */
void Network::deleteConnectionGroupFromMemory(unsigned conGrpID){
	//Remove connection group from map of new groups that only exist in memory
	if(prototypeMode){
		if(newConnectionGroupMap.contains(conGrpID)){//Deleting a new connection group
			newConnectionGroupMap.remove(conGrpID);
		}
		else{//Schedule connection group for deletion when network is saved
			deleteConnectionGroupIDs.append(conGrpID);
		}
	}

	//Remove connection group from memory
	delete connGrpMap[conGrpID];
	connGrpMap.remove(conGrpID);
}


/*! Removes the specified neuron group from memory along with all of its associated connections */
void Network::deleteNeuronGroupFromMemory(unsigned neurGrpID){
	//Remove neuron group from map of new groups that only exist in memory
	if(prototypeMode){
		if(newNeuronGroupMap.contains(neurGrpID)){//Deleting a new neuron group
			newNeuronGroupMap.remove(neurGrpID);
		}
		else{//Schedule neuron group for deletion when network is saved
			deleteNeuronGroupIDs.append(neurGrpID);
		}
	}

	//Identify connection groups involving this neuron group
	QHash<unsigned, bool> tmpDeleteConMap;
	for(QHash<unsigned int, ConnectionGroup*>::iterator iter = connGrpMap.begin(); iter != connGrpMap.end(); ++iter){
		if(iter.value()->getFromNeuronGroupID() == neurGrpID || iter.value()->getToNeuronGroupID() == neurGrpID){
			tmpDeleteConMap[iter.value()->getID()] = true;
		}
	}

	//Remove neuron group from memory
	delete neurGrpMap[neurGrpID];
	neurGrpMap.remove(neurGrpID);

	//Delete connection groups from memory and schedule their later deletion from the database
	for(QHash<unsigned, bool>::iterator iter = tmpDeleteConMap.begin(); iter != tmpDeleteConMap.end(); ++iter)
		deleteConnectionGroupFromMemory(iter.key());
}


/*! Applies connection mode filters to the specified connection and returns
	true if it should not be displayed. */
bool Network::filterConnection(Connection *connection, unsigned connectionMode){
	//Filter by weight
	if(connectionMode & SHOW_POSITIVE_CONNECTIONS){
		if(connection->getWeight() < 0 || connection->getTempWeight() < 0)
			return true;
	}
	else if(connectionMode & SHOW_NEGATIVE_CONNECTIONS){
		if(connection->getWeight() >= 0 || connection->getTempWeight() >= 0)
			return true;
	}
	return false;
}


/*! Returns an ID that is highly unlikey to conflict with database IDs
	for use as a temporary connection group ID. */
unsigned Network::getTemporaryConGrpID(){
	unsigned tmpID = 0xffffffff;
	while (connGrpMap.contains(tmpID)){
		--tmpID;
		if(tmpID == 0)
			throw SpikeStreamException("Cannot find a temporary connection ID");
	}
	return tmpID;
}


/*! Returns an ID that is highly unlikey to conflict with database IDs
	for use as a temporary neuron group ID. */
unsigned Network::getTemporaryNeurGrpID(){
	unsigned tmpID = 0xffffffff;
	while (neurGrpMap.contains(tmpID)){
		--tmpID;
		if(tmpID == 0)
			throw SpikeStreamException("Cannot find a temporary neuron group ID");
	}
	return tmpID;
}


/*! Initializes variables with default values */
void Network::initializeVariables(){
	currentNeuronTask = -1;
	currentConnectionTask = -1;
	prototypeMode = false;
	clearError();
	neuronGroupParametersChanged = false;
	connectionGroupParametersChanged = false;
}


/*! Uses network dao to obtain list of connection groups and load them into hash map. */
void Network::loadConnectionGroupsInfo(){
    //Clear hash map
	clearConnectionGroups();

    //Get list of neuron groups from database
	NetworkDao networkDao(networkDBInfo);
	QList<ConnectionGroupInfo> connGrpInfoList = networkDao.getConnectionGroupsInfo(getID());

    //Copy list into hash map for faster access
    for(int i=0; i<connGrpInfoList.size(); ++i)
		connGrpMap[ connGrpInfoList[i].getID() ] = new ConnectionGroup(connGrpInfoList[i]);
}


/*! Uses network dao to obtain list of neuron groups and load them into hash map.
    Individual neurons are loaded separately to enable lazy loading if required.  */
void Network::loadNeuronGroupsInfo(){
    //Clear hash map
	clearNeuronGroups();

    //Get list of neuron groups from database
	NetworkDao networkDao(networkDBInfo);
	QList<NeuronGroupInfo> neurGrpInfoList = networkDao.getNeuronGroupsInfo(getID());

    //Copy list into hash map for faster access
    for(int i=0; i<neurGrpInfoList.size(); ++i)
		neurGrpMap[ neurGrpInfoList[i].getID() ] = new NeuronGroup(neurGrpInfoList[i]);
}


/*! Deletes all connection groups */
void Network::clearConnectionGroups(){
    for(QHash<unsigned int, ConnectionGroup*>::iterator iter = connGrpMap.begin(); iter != connGrpMap.end(); ++iter)
		delete iter.value();
    connGrpMap.clear();
}


/*! Deletes all neuron groups */
void Network::clearNeuronGroups(){
    //Delete all neuron groups
    for(QHash<unsigned int, NeuronGroup*>::iterator iter = neurGrpMap.begin(); iter != neurGrpMap.end(); ++iter)
		delete iter.value();
    neurGrpMap.clear();
}


/*! Sets the class into error state and adds error message */
void Network::setError(const QString& errorMsg){
	this->errorMessage += " " + errorMsg;
	error = true;
}


/*! Starts thread that adds the new connection groups to the database. */
void Network::startAddConnectionGroups(){
	clearError();
	QList<ConnectionGroup*> tmpConGrpList = newConnectionGroupMap.values();
	connectionNetworkDaoThread->prepareAddConnectionGroups(getID(), tmpConGrpList);
	currentConnectionTask = ADD_CONNECTIONS_TASK;
	connectionNetworkDaoThread->start();
}


/*! Starts thread that adds the new neuron groups to the database. */
void Network::startAddNeuronGroups(){
	clearError();
	QList<NeuronGroup*> tmpNeurGrpList = newNeuronGroupMap.values();
	neuronNetworkDaoThread->prepareAddNeuronGroups(getID(), tmpNeurGrpList);
	currentNeuronTask = ADD_NEURONS_TASK;
	neuronNetworkDaoThread->start();
}


/*! Starts the thread that deletes connection groups from the database. */
void Network::startDeleteConnectionGroups(QList<unsigned>& deleteConGrpIDList){
	clearError();
	connectionNetworkDaoThread->prepareDeleteConnectionGroups(getID(), deleteConGrpIDList);
	currentConnectionTask = DELETE_CONNECTIONS_TASK;
	connectionNetworkDaoThread->start();
}


/*! Starts the thread that deletes connection groups from the database. */
void Network::startDeleteNeuronGroups(QList<unsigned>& deleteNeurGrpIDList){
	clearError();
	neuronNetworkDaoThread->prepareDeleteNeuronGroups(getID(), deleteNeurGrpIDList);
	currentNeuronTask = DELETE_NEURONS_TASK;
	neuronNetworkDaoThread->start();
}


/*! When connection groups have been added to the database their internal ID reflects the database state
	but the ID in the map held in network does not. This method fixes this and clears the map of
	new neuron groups. */
void Network::updateConnectionGroupsAfterSave(){
	//Add connection groups to network now that they have the correct ID or delete them if task has been cancelled
	for(QHash<unsigned, ConnectionGroup*>::iterator iter = newConnectionGroupMap.begin(); iter != newConnectionGroupMap.end(); ++iter){
		if(connGrpMap.contains(iter.value()->getID()))
			throw SpikeStreamException("Connection group with ID " + QString::number(iter.value()->getID()) + " is already present in the network.");
		if(connectionTaskCancelled)
			delete iter.value();
		else
			connGrpMap[iter.value()->getID()] = iter.value();
	}
	newConnectionGroupMap.clear();
}


/*! When neuron groups have been added to the database their internal ID reflects the database state
	but the ID in the map held in network does not. This method fixes this and clears the map of
	new neuron groups. */
void Network::updateNeuronGroupsAfterSave(){
	//Add neuron groups to the network now that they have the correct ID, or delete if task has not been cancelled
	for(QHash<unsigned, NeuronGroup*>::iterator iter = newNeuronGroupMap.begin(); iter != newNeuronGroupMap.end(); ++iter){
		//Check to see if ID already exists - error in this case
		if( neurGrpMap.contains( iter.value()->getID() ) ){
			throw SpikeStreamException("Adding neurons task - trying to add a neuron group with ID " + QString::number(iter.value()->getID()) + " that already exists in the network.");
		}
		if(neuronTaskCancelled)
			delete iter.value();
		else
			neurGrpMap[ iter.value()->getID() ] = iter.value();
	}
	newNeuronGroupMap.clear();
}


