#include "Network.h"
using namespace SpikeStream;

/*! Constructor */
Network::Network(unsigned int id, NetworkDao* networkDao){
    //Store information
    this->networkID = id;
    this->networkDao = networkDao;

    //Load up the general information about the network
    loadNeuronGroupsInfo();
    loadConnectionGroupsInfo();
}


/*! Destructor */
Network::~Network(){
    //Delete all neuron groups
    for(QHash<unsigned int, NeuronGroup*>::iterator iter = neurGrpHash.begin(); iter != neurGrpHash.end(); ++iter)
	delete iter.value();
    neurGrpHash.clear();

    //Delete all connection groups
    for(QHash<unsigned int, ConnectionGroup*>::iterator iter = connGrpHash.begin(); iter != connGrpHash.end(); ++iter)
	delete iter.value();
    connGrpHash.clear();
}



/*--------------------------------------------------------- */
/*-----                PUBLIC METHODS                ----- */
/*--------------------------------------------------------- */

/*! Returns a list of the neuron group ids in the network */
QList<unsigned int> Network::getNeuronGroupIDs(){
    return neurGrpHash.keys();
}


/*! Returns a list of the connection group ids in the network */
QList<unsigned int> Network::getConnectionGroupIDs(){
    return connGrpHash.keys();
}



/*! Returns the neuron group with the specified id.
    An exception is thrown if the neuron group id is not in the network.
    Lazy loading used, so that neuron groups are loaded only if requested. */
NeuronGroup* Network::getNeuronGroup(unsigned int id){
    checkNeuronGroupID(id);

    if(!neurGrpHash[id]->isLoaded())
	loadNeuronGroup(id);

    return neurGrpHash[id];
}


/*! Returns information about the neuron group with the specified id */
NeuronGroupInfo* Network::getNeuronGroupInfo(unsigned int id){
    checkNeuronGroupID(id);
    return neurGrpHash[id]->getInfo();
}


/*! Returns the connection group with the specified id.
    An exception is thrown if the connection group id is not in the network.
    Lazy loading used, so that connection groups are loaded only if requested. */
ConnectionGroup* Network::getConnectionGroup(unsigned int id){
    checkConnectionGroupID(id);

    if(!connGrpHash[id]->isLoaded())
	loadConnectionGroup(id);

    return connGrpHash[id];
}


/*! Returns information about the connection group with the specified id */
ConnectionGroupInfo* Network::getConnectionGroupInfo(unsigned int id){
    checkConnectionGroupID(id);
    return connGrpHash[id]->getInfo();
}


/*--------------------------------------------------------- */
/*-----                PRIVATE METHODS                ----- */
/*--------------------------------------------------------- */

/*! Checks that the specified neurong group id is present in the network
    and throws an exception if not. */
void Network::checkConnectionGroupID(unsigned int id){
    if(!connGrpHash.contains(id))
	throw SpikeStreamException(QString("Connection group with id ") + QString::number(id) + " is not in network with id " + QString::number(networkID));
}


/*! Checks that the specified connection group id is present in the network
    and throws an exception if not. */
void Network::checkNeuronGroupID(unsigned int id){
    if(!neurGrpHash.contains(id))
	throw SpikeStreamException(QString("Neuron group with id ") + QString::number(id) + " is not in network with id " + QString::number(networkID));
}


/*! Uses network dao to obtain list of connection groups and load them into hash map */
void Network::loadConnectionGroupsInfo(){
    //Clear hash map
    connGrpHash.clear();

    //Get list of neuron groups from database
    QList<ConnectionGroupInfo*> connGrpList = networkDao->getConnectionGroupsInfo();

    //Copy list into hash map for faster access
    for(int i=0; i<connGrpList.size(); ++i)
	connGrpHash[ connGrpList[i]->getID() ] = new ConnectionGroup(connGrpList[i]);
}


/*! Passes a connection group to the network dao, which populates it with
    all the necessary data from the database. */
void Network::loadConnections(unsigned int connGrpId){
    networkDao->getConnections(connGrpHash[connGrpId]);
}


/*! Uses network dao to obtain list of neuron groups and load them into hash map */
void Network::loadNeuronGroupsInfo(){
    //Clear hash map
    neurGrpHash.clear();

    //Get list of neuron groups from database
    QList<NeuronGroupInfo*> neurGrpList = networkDao->getNeuronGroupsInfo();

    //Copy list into hash map for faster access
    for(int i=0; i<neurGrpList.size(); ++i)
	neurGrpHash[ neurGrpList[i]->getID() ] = new NeuronGroup(neurGrpList[i]);
}


/*! Loads up the positions of all of the neurons in the group */
void Network::loadNeurons(unsigned int neurGrpID){
    networkDao->getNeurons(neurGrpHash[neurGrpId]);
}




