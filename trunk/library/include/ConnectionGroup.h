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
	    unsigned int getID() { return info.getID(); }
	    bool isLoaded() { return loaded; }
	    void setLoaded(bool loaded) { this->loaded = loaded; }

	    const QList<Connection*>& getFromConnections(unsigned int neurID);
	    const QList<Connection*>& getToConnections(unsigned int neurID);

	    ConnectionGroupInfo getInfo() { return info; }
	    ConnectionList* getConnections() { return connectionList; }
	    void setID(unsigned int id) { info.setID(id); }
	    int size() { return connectionList->size(); }

	private:
	    /*! Holds information about the connection group.
		Should match ConnectionGroup table in SpikeStreamNetwork database */
	    ConnectionGroupInfo info;

	    /*! List of connections between neurons.
		Used for fast access to complete list */
	    ConnectionList* connectionList;

	    /*! This empty connection list is returned whenever there are no from or to connections
		This avoids filling map with empty lists and avoids returning a reference to a temporary object. */
	    ConnectionList emptyConnectionList;

	    /*! Map enabling rapid access to the connections from any neuron */
	    QHash<unsigned int, QList<Connection*> > fromConnectionMap;

	    /*! Map enabling rapid access to connections to any neuron */
	    QHash<unsigned int, QList<Connection*> > toConnectionMap;

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

