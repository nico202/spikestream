//SpikeStream includes
#include "GlobalVariables.h"
#include "Network.h"
#include "SpikeStreamException.h"
using namespace spikestream;


/*! Standard constructor */
Network::Network(const NetworkInfo& networkInfo, NetworkDao* networkDao){
    //Store information
    this->networkInfo = networkInfo;
    this->networkDao = networkDao;

    //Create network dao threads for heavy operations
    neuronNetworkDaoThread = new NetworkDaoThread(networkDao->getDBInfo());
    connect (thread, SIGNAL(finished()), this, SLOT(neuronThreadFinished()));
    connectionNetworkDaoThread = new NetworkDaoThread(networkDao->getDBInfo());
    connect (thread, SIGNAL(finished()), this, SLOT(connectionThreadFinished()));
}


/*! Destructor */
Network::~Network(){
    //Empties all data stored in the class
    reset();
}



/*--------------------------------------------------------- */
/*-----                PUBLIC METHODS                 ----- */
/*--------------------------------------------------------- */

void Network::cancel(){
    neuronNetworkDaoThread->stop();
    connectionNetworkDaoThread->stop();
}

bool Network::isBusy() {
    if(neuronNetworkDaoThread->isRunning() || connectionNetworkDaoThread->isRunning())
	return true;
    return false;
}

/*! Deletes all data stored in the class */
void Network::reset(){
    deleteConnectionGroups();
    deleteNeuronGroups();
}


/*! Returns a list of the neuron group ids in the network */
QList<unsigned int> Network::getNeuronGroupIDs(){
    return neurGrpMap.keys();
}


/*! Returns a list of the connection group ids in the network */
QList<unsigned int> Network::getConnectionGroupIDs(){
    return connGrpMap.keys();
}


Box Network::getMinimumBoundingBox(){
}


/*! Returns the neuron group with the specified id.
    An exception is thrown if the neuron group id is not in the network.
    Lazy loading used, so that neuron groups are loaded only if requested. */
NeuronGroup* Network::getNeuronGroup(unsigned int id){
    checkNeuronGroupID(id);

    if(!neurGrpMap[id]->isLoaded())
	loadNeurons(id);

    return neurGrpMap[id];
}


/*! Returns information about the neuron group with the specified id */
NeuronGroupInfo Network::getNeuronGroupInfo(unsigned int id){
    checkNeuronGroupID(id);
    return neurGrpMap[id]->getInfo();
}


/*! Returns the connection group with the specified id.
    An exception is thrown if the connection group id is not in the network.
    Lazy loading used, so that connection groups are loaded only if requested. */
ConnectionGroup* Network::getConnectionGroup(unsigned int id){
    checkConnectionGroupID(id);

    if(!connGrpMap[id]->isLoaded())
	loadConnections(id);

    return connGrpMap[id];
}


/*! Returns information about the connection group with the specified id */
ConnectionGroupInfo Network::getConnectionGroupInfo(unsigned int id){
    checkConnectionGroupID(id);
    return connGrpMap[id]->getInfo();
}


/*! Loads up the network from the database */
void Network::load(){
    //Clear current information
    reset();

    //Load up information about the neuron and connection groups
    loadNeuronGroupsInfo();
    loadConnectionGroupsInfo();

    //Load up all neurons
    neuronNetworkDaoThread->prepareLoadNeurons(this->networkID, neurGrpMap.keys());
    neuronNetworkDaoThread->start();

    //Load all connection groups that are above a certain size
    QList<ConnectionGroup*> loadList;
    for(QList<ConnectionGroup*>::iterator iter = connGrpMap.keys().begin(); iter != connGrpMap.keys().end(); ++iter){
	unsigned int connGrpSize = networkDao->getConnectionCount((*iter)->getID());
	if(connGrpSize <= LAZY_LOADING_THRESHOLD)
	    loadList.append(*iter);
    }
    connectionNetworkDaoThread->prepareLoadConnections(this->networkID, loadList);
    connectionNetworkDaoThread->start();
}

/*--------------------------------------------------------- */
/*-----                  PRIVATE SLOTS                ----- */
/*--------------------------------------------------------- */

/*! Slot called when thread processing connections has finished running. */
void Network::connectionThreadFinished(){
    if(connetionNetworkDaoThread->isError()){
	QString errMsg = connectionNetworkDaoThread->getErrorMessage();
	throw SpikeStreamException(errMsg);
    }
}


/*! Slot called when thread processing neurons has finished running. */
void Network::neuronThreadFinished(){
    if(neuronNetworkDaoThread->isError()){
	QString errMsg = neuronNetworkDaoThread->getErrorMessage();
	throw SpikeStreamException(errMsg);
    }
}


/*--------------------------------------------------------- */
/*-----                PRIVATE METHODS                ----- */
/*--------------------------------------------------------- */

/*! Checks that the specified neuron group id is present in the network
    and throws an exception if not. */
void Network::checkConnectionGroupID(unsigned int id){
    if(!connGrpMap.contains(id))
	throw SpikeStreamException(QString("Connection group with id ") + QString::number(id) + " is not in network with id " + QString::number(networkID));
}


/*! Checks that the specified connection group id is present in the network
    and throws an exception if not. */
void Network::checkNeuronGroupID(unsigned int id){
    if(!neurGrpMap.contains(id))
	throw SpikeStreamException(QString("Neuron group with id ") + QString::number(id) + " is not in network with id " + QString::number(networkID));
}


/*! Uses network dao to obtain list of connection groups and load them into hash map.
    Individual connections are loaded separately to enable lazy loading. */
void Network::loadConnectionGroupsInfo(){
    //Clear hash map
    deleteConnectionGroups();

    //Get list of neuron groups from database
    QList<ConnectionGroupInfo> connGrpInfoList = networkDao->getConnectionGroupsInfo(networkID);

    //Copy list into hash map for faster access
    for(int i=0; i<connGrpInfoList.size(); ++i)
	connGrpMap[ connGrpInfoList[i].getID() ] = new ConnectionGroup(connGrpInfoList[i]);
}


/*! Passes a connection group to the network dao, which populates it with
    all the necessary data from the database. */
void Network::loadConnections(unsigned int connGrpId){
    connectionNetworkDaoThread->prepareLoadConnections(connGrpMap[connGrpId]);
    connectionNeworkDaoThread->start();
}


/*! Uses network dao to obtain list of neuron groups and load them into hash map.
    Individual neurons are loaded separately to enable lazy loading if required.  */
void Network::loadNeuronGroupsInfo(){
    //Clear hash map
    deleteNeuronGroups();

    //Get list of neuron groups from database
    QList<NeuronGroupInfo> neurGrpInfoList = networkDao->getNeuronGroupsInfo(networkID);

    //Copy list into hash map for faster access
    for(int i=0; i<neurGrpInfoList.size(); ++i)
	neurGrpMap[ neurGrpInfoList[i].getID() ] = new NeuronGroup(neurGrpInfoList[i]);
}


/*! Loads up the positions of all of the neurons in the group */
void Network::loadNeurons(unsigned int neurGrpID){
    networkDaoThread->prepareLoadNeurons(this->networkID, neurGrpMap[neurGrpID]);
    runNetworkDaoThread();
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
}S
