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
	ConnectionList::iterator endConnList = connectionList->end();
	for(ConnectionList::iterator iter = connectionList->begin(); iter != endConnList; ++iter){
	    delete *iter;
	}
	delete connectionList;
    }
}



void ConnectionGroup::addConnection(Connection* newConn){
    connectionList->append(newConn);
}
