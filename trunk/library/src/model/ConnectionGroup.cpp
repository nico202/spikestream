#include "ConnectionGroup.h"
using namespace spikestream;

/*! Constructor */
ConnectionGroup::ConnectionGroup(const ConnectionGroupInfo& connGrpInfo){
    this->info = connGrpInfo;
    connectionArray = NULL;
    connectionArrayLength = 0;
}


/*! Destructor */
ConnectionGroup::~ConnectionGroup(){
    if(connectionArray != NULL)
	delete [] connectionArray;
}


/*! Returns true if the connections have been loaded */
bool ConnectionGroup::connectionsLoaded(){
    if(connectionArray == NULL)
	return false;
    return true;
}
