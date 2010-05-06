//SpikeStream includes
#include "GlobalVariables.h"
#include "Network.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;

/*! Constructor that creates a new network and adds it to the database */
Network::Network(NetworkDao* networkDao, ArchiveDao* archiveDao, const QString& name, const QString& description){
    //Store information
    this->networkDao = networkDao;
    this->archiveDao = archiveDao;
    this->info.setName(name);
    this->info.setDescription(description);

    //Create network dao threads for heavy operations
    neuronNetworkDaoThread = new NetworkDaoThread(networkDao->getDBInfo());
    connect (neuronNetworkDaoThread, SIGNAL(finished()), this, SLOT(neuronThreadFinished()));
    connectionNetworkDaoThread = new NetworkDaoThread(networkDao->getDBInfo());
    connect (connectionNetworkDaoThread, SIGNAL(finished()), this, SLOT(connectionThreadFinished()));

    //Initialize variables
    currentNeuronTask = -1;
    currentConnectionTask = -1;
    clearError();

    //Create new network in database. ID will be stored in the network info
    networkDao->addNetwork(info);
}


/*! Constructor when using an existing network */
Network::Network(const NetworkInfo& networkInfo, NetworkDao* networkDao, ArchiveDao* archiveDao){
    //Store information
    this->info = networkInfo;
    this->networkDao = networkDao;
    this->archiveDao = archiveDao;

    //Check that network ID is valid
    if(info.getID() == INVALID_NETWORK_ID){
		throw SpikeStreamException ("Attempting to load an invalid network");
    }

    //Create network dao threads for heavy operations
    neuronNetworkDaoThread = new NetworkDaoThread(networkDao->getDBInfo());
    connect (neuronNetworkDaoThread, SIGNAL(finished()), this, SLOT(neuronThreadFinished()));
    connectionNetworkDaoThread = new NetworkDaoThread(networkDao->getDBInfo());
    connect (connectionNetworkDaoThread, SIGNAL(finished()), this, SLOT(connectionThreadFinished()));

    //Initialize variables
    currentNeuronTask = -1;
    currentConnectionTask = -1;
    clearError();

    //Load up basic information about the neuron and connection groups
    loadNeuronGroupsInfo();
    loadConnectionGroupsInfo();
}


/*! Destructor */
Network::~Network(){
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
    deleteConnectionGroups();
    deleteNeuronGroups();
}



/*--------------------------------------------------------- */
/*-----                PUBLIC METHODS                 ----- */
/*--------------------------------------------------------- */

/*! Adds a connection group to the network without saving it to the database.
    #FIXME#: MAKE THIS WORK IN THE SAME WAY AS A NEURON GROUP. WILL HAVE TO CHANGE NRM IMPORTER WHEN THIS IS DONE. */
void Network::addConnectionGroups(QList<ConnectionGroup*>& connectionGroupList, bool checkNetworkLocked){
    if(checkNetworkLocked && isLocked())//Check if network is editable or not
		throw SpikeStreamException("Cannot add connection groups to a locked network.");

    for(QList<ConnectionGroup*>::iterator iter = connectionGroupList.begin(); iter != connectionGroupList.end(); ++iter){
		if(connGrpMap.contains((*iter)->getID()))
			throw SpikeStreamException("Connection group with ID " + QString::number((*iter)->getID()) + " is already present in the network.");

		connGrpMap[(*iter)->getID()] = *iter;
    }
}


/*! Adds neuron groups to the network */
void Network::addNeuronGroups(QList<NeuronGroup*>& neuronGroupList){
    if(isLocked())//Check if network is editable or not
		throw SpikeStreamException("Cannot add neuron groups to a locked network.");

    //Store the list of neuron groups to be added later when the thread has finished and we have the correct ID
    newNeuronGroups = neuronGroupList;

    //Start thread that adds neuron groups to database
    clearError();
    neuronNetworkDaoThread->prepareAddNeuronGroups(getID(), neuronGroupList);
    currentNeuronTask = ADD_NEURONS_TASK;
    neuronNetworkDaoThread->start();
}


/*! Cancels thread-based operations that are in progress */
void Network::cancel(){
    neuronNetworkDaoThread->stop();
    connectionNetworkDaoThread->stop();
    currentNeuronTask = -1;
    currentConnectionTask = -1;
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


/*! Returns a complete list of connection group infos */
QList<ConnectionGroupInfo> Network::getConnectionGroupsInfo(){
    QList<ConnectionGroupInfo> tmpList;
    for(QHash<unsigned int, ConnectionGroup*>::iterator iter = connGrpMap.begin(); iter != connGrpMap.end(); ++iter)
		tmpList.append(iter.value()->getInfo());
    return tmpList;
}


/*! Returns a complete list of neuron group infos */
QList<NeuronGroupInfo> Network::getNeuronGroupsInfo(){
    QList<NeuronGroupInfo> tmpList;
    for(QHash<unsigned int, NeuronGroup*>::iterator iter = neurGrpMap.begin(); iter != neurGrpMap.end(); ++iter)
		tmpList.append(iter.value()->getInfo());
    return tmpList;
}


/*! Returns the number of neurons that connect to the specified neuron */
int Network::getNumberOfToConnections(unsigned int neuronID){
    //Check neuron id is in the network
    if(!containsNeuron(neuronID))
		throw SpikeStreamException("Request for number of connections to a neuron that is not in the network.");

    //Count up the number of connections to this neuron in each connection group
    int toConCount = 0;
    for(QHash<unsigned int, ConnectionGroup*>::iterator iter = connGrpMap.begin(); iter != connGrpMap.end(); ++iter){
		//Get the number of connections to the neuron in this connection group
		toConCount += iter.value()->getToConnections(neuronID).size();
    }

    //Return final count
    return toConCount;
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
			networkBox = networkDao->getNeuronGroupBoundingBox((*iter)->getID());
			firstTime = false;
		}
		else{//Expand box to include subsequent neuron groups
			Box neurGrpBox = networkDao->getNeuronGroupBoundingBox((*iter)->getID());
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


/*! Returns a box that encloses the specified neuron group */
Box Network::getNeuronGroupBoundingBox(unsigned int neurGrpID){
    Box box = networkDao->getNeuronGroupBoundingBox(neurGrpID);
    return box;
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

bool Network::connectionGroupIsLoaded(unsigned int id){
    checkConnectionGroupID(id);
    return connGrpMap[id]->isLoaded();
}

bool Network::neuronGroupIsLoaded(unsigned int id){
    checkNeuronGroupID(id);
    return neurGrpMap[id]->isLoaded();
}


/*! Returns information about the connection group with the specified id */
ConnectionGroupInfo Network::getConnectionGroupInfo(unsigned int id){
    checkConnectionGroupID(id);
    return connGrpMap[id]->getInfo();
}


/*! Returns the number of steps that have been completed so far during a load operation. */
int Network::getNumberOfCompletedSteps(){
    return connectionNetworkDaoThread->getNumberOfCompletedSteps() + neuronNetworkDaoThread->getNumberOfCompletedSteps();
}


/*! Returns true if the network is not editable because it is associated with archives */
bool Network::isLocked(){
    return archiveDao->networkIsLocked(getID());
}


/*! Loads up the network from the database */
void Network::load(){
    clearError();

    //Load up all neurons
    neuronNetworkDaoThread->prepareLoadNeurons(neurGrpMap.values());
    currentNeuronTask = LOAD_NEURONS_TASK;
    neuronNetworkDaoThread->start();

    //Load all connection groups that are above a certain size
    QList<ConnectionGroup*> loadList;
    QList<ConnectionGroup*> connGrpMapList = connGrpMap.values();
    for(QList<ConnectionGroup*>::iterator iter = connGrpMapList.begin(); iter != connGrpMapList.end(); ++iter){
		unsigned int connGrpSize = networkDao->getConnectionGroupSize((*iter)->getID());
		if(connGrpSize <= LAZY_LOADING_THRESHOLD)
			loadList.append(*iter);
    }
    connectionNetworkDaoThread->prepareLoadConnections(loadList);
    currentConnectionTask = LOAD_CONNECTIONS_TASK;
    connectionNetworkDaoThread->start();

    //Record the total number of steps and initialise the number of completed steps
    totalNumberOfSteps = neuronNetworkDaoThread->getTotalNumberOfSteps() + neuronNetworkDaoThread->getTotalNumberOfSteps();

    /* Both threads will now be running to load connections and neurons.
       This class will return true to the isBusy() method until the threads finish.
       When the threads finish they should call the appropriat slot in this class. */
}



/*--------------------------------------------------------- */
/*-----                  PRIVATE SLOTS                ----- */
/*--------------------------------------------------------- */

/*! Slot called when thread processing connections has finished running. */
void Network::connectionThreadFinished(){
    if(connectionNetworkDaoThread->isError()){
		errorMessage += "Connection Loading Error: '" + connectionNetworkDaoThread->getErrorMessage() + "'. ";
		error = true;
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
		errorMessage += "Neuron Loading Error: '" + neuronNetworkDaoThread->getErrorMessage() + "'. ";
		error = true;
    }

    //Handle any task-specific stuff
    if(!error){
		try{
			switch(currentNeuronTask){
				case ADD_NEURONS_TASK:
					//Add neuron groups to the network now that they have the correct ID
					for(QList<NeuronGroup*>::iterator iter = newNeuronGroups.begin(); iter != newNeuronGroups.end(); ++iter){
						//Check to see if ID already exists - error in this case
						if( neurGrpMap.contains( (*iter)->getID() ) ){
							cout<<"NEUR GRP MAP SIZE: "<<neurGrpMap.size()<<" incorrect id: "<<(*iter)->getID()<<endl;
							throw SpikeStreamException("Adding neurons task - trying to add a neuron group with ID " + QString::number((*iter)->getID()) + " that already exists in the network.");
						}

						//Store neuron group
						neurGrpMap[ (*iter)->getID() ] = *iter;
					}
				break;
				case LOAD_NEURONS_TASK:
					;//Nothing to do at present
				break;
				default:
					throw SpikeStreamException("The current task ID has not been recognized.");
			}
		}
		catch(SpikeStreamException& ex){
			errorMessage = ex.getMessage();
			error = true;
		}
    }
	qDebug()<<"NETWORK: "<<neurGrpMap.size();
    //Reset task
    currentNeuronTask = -1;

    //Emit signal that task has finished if no other threads are carrying out operations
    if(!isBusy())
		emit taskFinished();
}

/*! Returns the size of the network.
    An exception is thrown if not all neuron groups have been loaded.
    FIXME: USE DATABASE LOOKUP IN THIS CASE INSTEAD. */
int Network::size(){
    int neuronCount = 0;
    for(QHash<unsigned int, NeuronGroup*>::iterator iter = neurGrpMap.begin(); iter != neurGrpMap.end(); ++iter){
		neuronCount += iter.value()->size();
    }
    return neuronCount;
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


/*! Checks that the specified connection group id is present in the network
    and throws an exception if not. */
void Network::checkNeuronGroupID(unsigned int id){
    if(!neurGrpMap.contains(id))
		throw SpikeStreamException(QString("Neuron group with id ") + QString::number(id) + " is not in network with id " + QString::number(getID()));
}


/*! Uses network dao to obtain list of connection groups and load them into hash map.
    Individual connections are loaded separately to enable lazy loading. */
void Network::loadConnectionGroupsInfo(){
    //Clear hash map
    deleteConnectionGroups();

    //Get list of neuron groups from database
    QList<ConnectionGroupInfo> connGrpInfoList = networkDao->getConnectionGroupsInfo(getID());

    //Copy list into hash map for faster access
    for(int i=0; i<connGrpInfoList.size(); ++i)
		connGrpMap[ connGrpInfoList[i].getID() ] = new ConnectionGroup(connGrpInfoList[i]);
}


/*! Uses network dao to obtain list of neuron groups and load them into hash map.
    Individual neurons are loaded separately to enable lazy loading if required.  */
void Network::loadNeuronGroupsInfo(){
    //Clear hash map
    deleteNeuronGroups();

    //Get list of neuron groups from database
    QList<NeuronGroupInfo> neurGrpInfoList = networkDao->getNeuronGroupsInfo(getID());

    //Copy list into hash map for faster access
    for(int i=0; i<neurGrpInfoList.size(); ++i)
		neurGrpMap[ neurGrpInfoList[i].getID() ] = new NeuronGroup(neurGrpInfoList[i]);
}



void Network::deleteConnectionGroups(){
    //Delete all connection groups
    for(QHash<unsigned int, ConnectionGroup*>::iterator iter = connGrpMap.begin(); iter != connGrpMap.end(); ++iter)
		delete iter.value();
    connGrpMap.clear();
}


void Network::deleteNeuronGroups(){
    //Delete all neuron groups
    for(QHash<unsigned int, NeuronGroup*>::iterator iter = neurGrpMap.begin(); iter != neurGrpMap.end(); ++iter)
		delete iter.value();
    neurGrpMap.clear();
}
