//SpikeStream includes
#include "GlobalVariables.h"
#include "NetworkDao.h"
#include "SpikeStreamDBException.h"
#include "XMLParameterParser.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;

/*! Constructor. DBInfo is stored by AbstractDao */
NetworkDao::NetworkDao(DBInfo& dbInfo) : AbstractDao(dbInfo){

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
    QSqlQuery query = getQuery("INSERT INTO Networks (Name, Description) VALUES ('" + netInfo.getName() + "', '" + netInfo.getDescription() + "')");
    executeQuery(query);

    //Check id is correct and add to network info if it is
    int lastInsertID = query.lastInsertId().toInt();
    if(lastInsertID >= START_NEURALNETWORK_ID)
	netInfo.setID(lastInsertID);
    else
	throw SpikeStreamDBException("Insert ID for Network is invalid: " + QString::number(lastInsertID));
}


void NetworkDao::deleteNetwork(unsigned int networkID){
    executeQuery("DELETE FROM Networks WHERE NetworkID = " + QString::number(networkID));
}


/*! Returns information about the connection groups associated with the specified network */
QList<ConnectionGroupInfo> NetworkDao::getConnectionGroupsInfo(unsigned int networkID){
    QSqlQuery query = getQuery("SELECT ConnectionGroupID, Description, FromNeuronGroupID, ToNeuronGroupID, Parameters, SynapseTypeID FROM ConnectionGroups WHERE NetworkID=" + QString::number(networkID));
    executeQuery(query);
    QList<ConnectionGroupInfo> tmpList;
    XMLParameterParser parameterParser;
    for(int i=0; i<query.size(); ++i){
	query.next();
	tmpList.append(
		ConnectionGroupInfo(
			query.value(0).toUInt(),//Connection group id
			query.value(1).toString(),//Description
			query.value(2).toUInt(),//From group id
			query.value(3).toUInt(),//To group id
			parameterParser.getParameterMap(query.value(4).toString()),//Parameters
			query.value(5).toUInt()//Synapse id
		)
	);
    }
    return tmpList;
}


/*! Returns a box enclosing a particular neuron group */
Box NetworkDao::getNeuronGroupBoundingBox(unsigned int neurGrpID){
    QSqlQuery query = getQuery("SELECT MIN(X), MIN(Y), MIN(Z), MAX(X), MAX(Y), MAX(Z) FROM Neurons WHERE NeuronGroupID = " + QString::number(neurGrpID));
    executeQuery(query);
    query.next();
    Box box(
	query.value(0).toInt(),//x1
	query.value(1).toInt(),//y1
	query.value(2).toInt(),//z1
	query.value(3).toInt(),//x2
	query.value(4).toInt(),//y2
	query.value(5).toInt()//z2
    );
    return box;
}


/*! Returns the number of connections in the specified connection group */
unsigned int NetworkDao::getConnectionGroupSize(unsigned int connGrpID){
    QSqlQuery query = getQuery("SELECT COUNT(*) FROM Connections WHERE ConnectionGroupID=" + QString::number(connGrpID));
    executeQuery(query);
    query.next();
    return query.value(0).toUInt();
}


/*! Returns a list of information about the available networks. */
QList<NetworkInfo> NetworkDao::getNetworksInfo(){
    QSqlQuery query = getQuery("SELECT NetworkID, Name, Description, Locked FROM Networks");
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
    QSqlQuery query = getQuery("SELECT NeuronGroupID, Name, Description, Parameters, NeuronTypeID FROM NeuronGroups WHERE NetworkID=" + QString::number(networkID));
    executeQuery(query);
    QList<NeuronGroupInfo> tmpList;
    XMLParameterParser parameterParser;
    for(int i=0; i<query.size(); ++i){
	query.next();
	tmpList.append(
		NeuronGroupInfo(
			query.value(0).toUInt(),
			query.value(1).toString(),
			query.value(2).toString(),
			parameterParser.getParameterMap(query.value(3).toString()),
			query.value(4).toUInt()
		)
	);
    }
    return tmpList;

}



