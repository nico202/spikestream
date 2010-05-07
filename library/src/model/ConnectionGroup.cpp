#include "ConnectionGroup.h"
using namespace spikestream;

#include <iostream>
using namespace std;

/*! Constructor */
ConnectionGroup::ConnectionGroup(const ConnectionGroupInfo& connGrpInfo){
    this->info = connGrpInfo;
    loaded = false;
}


/*! Destructor */
ConnectionGroup::~ConnectionGroup(){
    clearConnections();
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Adds a connection to the group */
Connection* ConnectionGroup::addConnection(Connection* newConn){
    connectionList.append(newConn);

    //Store connection data in easy to access format
    fromConnectionMap[newConn->fromNeuronID].append(newConn);
    toConnectionMap[newConn->toNeuronID].append(newConn);

    //Return pointer to connection
    return newConn;
}


/*! Returns iterator pointing to beginning of connection group */
ConnectionList::const_iterator ConnectionGroup::begin(){
    return connectionList.begin();
}


/*! Returns iterator pointing to end of connection group */
ConnectionList::const_iterator ConnectionGroup::end(){
    return connectionList.end();
}


/*! Removes all connections from this group */
void ConnectionGroup::clearConnections(){
    ConnectionList::iterator endConnList = connectionList.end();
    for(ConnectionList::iterator iter = connectionList.begin(); iter != endConnList; ++iter){
		delete *iter;
    }
    connectionList.clear();
    fromConnectionMap.clear();
    toConnectionMap.clear();
    loaded = false;
}


/*! Returns a list of connections from the neuron with this ID.
    Empty list is returned if neuron id cannot be found. */
const ConnectionList ConnectionGroup::getFromConnections(unsigned int neurID){
    if(!fromConnectionMap.contains(neurID))
		return ConnectionList();//Returns an empty connection list without filling map with invalid from and to neurons
    return fromConnectionMap[neurID];
}


/*! Returns a list of connections to the neuron with the specified id.
    Exception is thrown if the neuron cannot be found. */
const ConnectionList ConnectionGroup::getToConnections(unsigned int neurID){
	if(!toConnectionMap.contains(neurID))
		return ConnectionList();//Returns an empty connection list without filling map with invalid from and to neurons
	return toConnectionMap[neurID];
}


