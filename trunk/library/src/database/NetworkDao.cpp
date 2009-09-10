//SpikeStream includes
#include "GlobalVariables.h"
#include "NetworkDao.h"
#include "SpikeStreamDBException.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;

/*! Constructor. DBInfo is stored by AbstractDao */
NetworkDao::NetworkDao(DBInfo& dbInfo) : AbstractDao(dbInfo){
    //Connect to the database straight away since this will be running in the same thread.
    connectToDatabase();
}


/*! Destructor */
NetworkDao::~NetworkDao(){
    closeDatabaseConnection();
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Adds the specified neural network to the database */
void NetworkDao::addNetwork(NetworkInfo& netInfo){
    QSqlQuery query = getQuery("INSERT INTO NeuralNetworks (Name, Description) VALUES ('" + netInfo.getName() + "', '" + netInfo.getDescription() + "')");
    executeQuery(query);

    //Check id is correct and add to network info if it is
    int lastInsertID = query.lastInsertId().toInt();
    if(lastInsertID >= START_NEURALNETWORK_ID)
	netInfo.setID(lastInsertID);
    else
	throw SpikeStreamDBException("Insert ID for NeuralNetwork is invalid: " + QString::number(lastInsertID));
}


/*! Returns information about the connection groups associated with the specified network */
QList<ConnectionGroupInfo> NetworkDao::getConnectionGroupsInfo(unsigned int networkID){
}


/*! Returns a list of information about the available networks. */
QList<NetworkInfo> NetworkDao::getNetworksInfo(){
    QSqlQuery query = getQuery("SELECT NeuralNetworkID, Name, Description, Locked FROM NeuralNetworks");
    executeQuery(query);
    QList<NetworkInfo> tmpList;
    for(int i=0; i<query.size(); ++i){
	query.next();
	tmpList.append( NetworkInfo(query.value(0).toUInt(), query.value(1).toString(), query.value(2).toString(), query.value(3).toBool()));
    }
    return tmpList;
}


/*! Returns information about the neuron groups associated with the specified network */
QList<NeuronGroupInfo> NetworkDao::getNeuronGroupsInfo(unsigned int networkID){
    QSqlQuery query = ("SELECT NeuronGroupID, Name, Description, Parameters, NeuronTypeID FROM NeuronGroups WHERE NeuralNetworkID=" + QString::number(networkID));
    executeQuery(query);
    QList<NeuronGroupInfo> tmpList;
    for(int i=0; i<query.size(); ++i){
	query.next();
	tmpList.append(
		NeuronGroupInfo(
			query.value(0).toUInt(),
			query.value(1).toString(),
			query.value(2).toString,
			    HERE********* WILL PROBABLY HAVE TO BUILD AN XML PARAMETER PARSER
			query.value(3).toString(),
			query.value(4).toUInt()
		)
	);
    }
    return tmpList;

}



