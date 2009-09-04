//SpikeStream includes
#include "Network.h"
#include "SpikeStreamException.h"
using namespace spikestream;

/*! Constructor */
Network::Network(const NetworkInfo& networkInfo, NetworkDao* networkDao){
    //Store information
    this->networkInfo = networkInfo;
    this->networkDao = networkDao;
}


/*! Destructor */
Network::~Network(){
    //Delete all neuron groups
    for(QHash<unsigned int, NeuronGroup*>::iterator iter = neurGrpMap.begin(); iter != neurGrpMap.end(); ++iter)
	delete iter.value();
    neurGrpMap.clear();

    //Delete all connection groups
    for(QHash<unsigned int, ConnectionGroup*>::iterator iter = connGrpMap.begin(); iter != connGrpMap.end(); ++iter)
	delete iter.value();
    connGrpMap.clear();
}



/*--------------------------------------------------------- */
/*-----                PUBLIC METHODS                 ----- */
/*--------------------------------------------------------- */

/*! Returns a list of the neuron group ids in the network */
QList<unsigned int> Network::getNeuronGroupIDs(){
    return neurGrpMap.keys();
}


/*! Returns a list of the connection group ids in the network */
QList<unsigned int> Network::getConnectionGroupIDs(){
    return connGrpMap.keys();
}


/*! Returns the neuron group with the specified id.
    An exception is thrown if the neuron group id is not in the network.
    Lazy loading used, so that neuron groups are loaded only if requested. */
NeuronGroup* Network::getNeuronGroup(unsigned int id){
    checkNeuronGroupID(id);

    if(!neurGrpMap[id]->neuronsLoaded())
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

    if(!connGrpMap[id]->connectionsLoaded())
	loadConnections(id);

    return connGrpMap[id];
}


/*! Returns information about the connection group with the specified id */
ConnectionGroupInfo Network::getConnectionGroupInfo(unsigned int id){
    checkConnectionGroupID(id);
    return connGrpMap[id]->getInfo();
}


/*! Loads up the network from the database */
void Network::loadNetwork(){
    //Load up information about the neuron and connection groups
    loadNeuronGroupsInfo();
    loadConnectionGroupsInfo();
}


/*--------------------------------------------------------- */
/*-----                PRIVATE METHODS                ----- */
/*--------------------------------------------------------- */

/*! Checks that the specified neurong group id is present in the network
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


/*! Uses network dao to obtain list of connection groups and load them into hash map */
void Network::loadConnectionGroupsInfo(){
    //Clear hash map
    connGrpMap.clear();

    //Get list of neuron groups from database
    QList<ConnectionGroupInfo> connGrpInfoList = networkDao->getConnectionGroupsInfo(networkID);

    //Copy list into hash map for faster access
    for(int i=0; i<connGrpInfoList.size(); ++i)
	connGrpMap[ connGrpInfoList[i].getID() ] = new ConnectionGroup(connGrpInfoList[i]);
}


/*! Passes a connection group to the network dao, which populates it with
    all the necessary data from the database. */
void Network::loadConnections(unsigned int connGrpId){
    networkDao->getConnections(connGrpMap[connGrpId]);
}


/*! Uses network dao to obtain list of neuron groups and load them into hash map */
void Network::loadNeuronGroupsInfo(){
    //Clear hash map
    neurGrpMap.clear();

    //Get list of neuron groups from database
    QList<NeuronGroupInfo> neurGrpInfoList = networkDao->getNeuronGroupsInfo(networkID);

    //Copy list into hash map for faster access
    for(int i=0; i<neurGrpInfoList.size(); ++i)
	neurGrpMap[ neurGrpInfoList[i].getID() ] = new NeuronGroup(neurGrpInfoList[i]);
}


/*! Loads up the positions of all of the neurons in the group */
void Network::loadNeurons(unsigned int neurGrpID){
    networkDao->getNeurons(neurGrpMap[neurGrpID]);
}




