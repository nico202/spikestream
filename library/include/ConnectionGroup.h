#ifndef CONNECTIONGROUP_H
#define CONNECTIONGROUP_H

//SpikeStream includes
#include "Connection.h"
#include "ConnectionGroupInfo.h"
using namespace spikestream;

typedef QList<Connection*> ConnectionList;

namespace spikestream {

    class ConnectionGroup {
	public:
	    ConnectionGroup(const ConnectionGroupInfo& connGrpInfo);
	    ~ConnectionGroup();
	    void clearConnections();

	    void addConnection(Connection* newConn);
	    ConnectionList::const_iterator begin();
	    ConnectionList::const_iterator end();
	    ConnectionList getConnections() { return connectionList; }
	    unsigned int getID() { return info.getID(); }
	    const ConnectionList getFromConnections(unsigned int neurID);
	    unsigned int getFromNeuronGroupID() { return info.getFromNeuronGroupID(); }
	    ConnectionGroupInfo getInfo() { return info; }
	    const ConnectionList getToConnections(unsigned int neurID);
	    unsigned int getToNeuronGroupID() { return info.getToNeuronGroupID(); }
	    bool isLoaded() { return loaded; }
	    void setID(unsigned int id) { info.setID(id); }
	    void setLoaded(bool loaded) { this->loaded = loaded; }
	    int size() { return connectionList.size(); }

	private:
	    /*! Holds information about the connection group.
		Should match ConnectionGroup table in SpikeStreamNetwork database */
	    ConnectionGroupInfo info;

	    /*! List of connections between neurons.
		Used for fast access to complete list */
	    ConnectionList connectionList;

	    /*! Map enabling rapid access to the connections from any neuron */
	    QHash<unsigned int, ConnectionList > fromConnectionMap;

	    /*! Map enabling rapid access to connections to any neuron */
	    QHash<unsigned int, ConnectionList > toConnectionMap;

	   /*! Returns true if the state of the connection array matches the database.
		This should be false if no connections have been loaded and false
		if the connection array is full of connections that are not in the database */
	   bool loaded;

	   //====================  METHODS  ==========================
	   ConnectionGroup(const ConnectionGroup& connGrp);
	   ConnectionGroup& operator=(const ConnectionGroup& rhs);
    };

}

#endif//CONNECTIONGROUP_H

