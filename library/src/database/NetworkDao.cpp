//SpikeStream includes
#include "GlobalVariables.h"
#include "NetworkDao.h"
#include "SpikeStreamDBException.h"
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
    QSqlQuery query = getQuery("INSERT INTO NeuralNetworks (Name) VALUES ('" + netInfo.getName() + "')");
    executeQuery(query);

    //Check id is correct and add to network info if it is
    int lastInsertID = query.lastInsertId().toInt();
    if(lastInsertID >= START_NEURALNETWORK_ID)
	netInfo.setID(lastInsertID);
    else
	throw SpikeStreamDBException("Insert ID for NeuralNetwork is invalid.");
}


/*! Adds a neuron group to the network with the specified id */
void NetworkDao::addNeuronGroup(unsigned int networkID, NeuronGroupInfo& neurGrpInfo){ 
    //Build query string
    QString queryStr = "INSERT INTO NeuronGroups (NeuralNetworkID, Name, Description, Parameters, NeuronTypeID ) VALUES (";
    queryStr += QString::number(networkID) + ", ";
    queryStr += neurGrpInfo.getName() + ", " + neurGrpInfo.getDescription() + ", '" + neurGrpInfo.getParameterXML() + "', ";
    queryStr += QString::number(neurGrpInfo.getNeuronType()) + ")";
    QSqlQuery query = getQuery(queryStr);
    executeQuery(query);

    //Check id is correct and add to network info if it is
    int lastInsertID = query.lastInsertId().toInt();
    if(lastInsertID >= START_NEURONGROUP_ID)
	neurGrpInfo.setID(lastInsertID);
    else
	throw SpikeStreamDBException("Insert ID for NeuronGroup is invalid.");
}


/*! Adds a connection group to the network with the specified id */
void NetworkDao::addConnectionGroup(unsigned int networkID, ConnectionGroupInfo& connGrpInfo){
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


