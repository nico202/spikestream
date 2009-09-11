#include "ConnectionGroup.h"
using namespace spikestream;

/*! Constructor */
ConnectionGroup::ConnectionGroup(const ConnectionGroupInfo& connGrpInfo){
    this->info = connGrpInfo;
    connectionList = new ConnectionList();
}


/*! Destructor */
ConnectionGroup::~ConnectionGroup(){
    if(connectionList != NULL){
	clearConnections();
	delete connectionList;
    }
}


/*! Adds a connection to the group */
void ConnectionGroup::addConnection(Connection* newConn){
    connectionList->append(newConn);

    //Store connection data in easy to access format
    fromConnectionMap[newConn->fromNeuronID].append(newConn);
    toConnectionMap[newConn->toNeuronID].append(newConn);
}


/*! Removes all connections from this group */
void ConnectionGroup::clearConnections(){
    ConnectionList::iterator endConnList = connectionList->end();
    for(ConnectionList::iterator iter = connectionList->begin(); iter != endConnList; ++iter){
	delete *iter;
    }
    connectionList->clear();
}
