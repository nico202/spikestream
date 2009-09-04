//SpikeStream includes
#include "NetworkDao.h"
using namespace spikestream;


/*! Constructor. DBInfo is stored by AbstractDao */
NetworkDao::NetworkDao(DBInfo& dbInfo) : AbstractDao(dbInfo){
}


/*! Destructor */
NetworkDao::~NetworkDao(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Adds the specified neural network to the database */
void NetworkDao::addNetwork(NetworkInfo& netInfo){
    QSqlQuery query = getQuery();
    query.prepare("
}

/*! Returns information about the connection groups associated with the specified network */
QList<ConnectionGroupInfo> NetworkDao::getConnectionGroupsInfo(unsigned int networkID){
}


/*! Fills the supplied connection group class with detailed information about the connections */
void NetworkDao::getConnections(ConnectionGroup* connGrp){
}


/*! Returns a list of information about the available networks. */
QList<NetworkInfo> NetworkDao::getNetworksInfo(){
    QSqlQuery query = getQuery();
    query.prepare("SELECT * FROM NeuralNetworks");
    executeQuery(query);

    return QList<NetworkInfo>();
}


/*! Returns information about the neuron groups associated with the specified network */
QList<NeuronGroupInfo> NetworkDao::getNeuronGroupsInfo(unsigned int networkID){

}


/*! Fills the supplied neuron group class with the detailed information about the neurons */
void NetworkDao::getNeurons(NeuronGroup* neurGrp){
}


