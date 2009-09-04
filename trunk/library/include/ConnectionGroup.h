#ifndef CONNECTIONGROUP_H
#define CONNECTIONGROUP_H

//SpikeStream includes
#include "Connection.h"
#include "ConnectionGroupInfo.h"

namespace spikestream {

    class ConnectionGroup {
	public:
	    ConnectionGroup(const ConnectionGroupInfo& connGrpInfo);
	    ~ConnectionGroup();

	    bool connectionsLoaded();
	    ConnectionGroupInfo getInfo() { return info; }
	    Connection* getConnectionArray() { return connectionArray; }
	    unsigned int size() { return info.getNumberOfConnections(); }

	private:
	    /*! Holds information about the connection group.
		Should match ConnectionGroup table in SpikeStreamNetwork database */
	    ConnectionGroupInfo info;

	    /*! Array of connections between neurons */
	    Connection* connectionArray;

    };

}

#endif//CONNECTIONGROUP_H

