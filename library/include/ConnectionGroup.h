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

	    bool isLoaded() { return loaded; }
	    void setLoaded(bool loaded) { this->loaded = loaded; }
	    ConnectionGroupInfo getInfo() { return info; }
	    Connection* getConnectionArray() { return connectionArray; }
	    void setID(unsigned int id) { info.setID(id); }
	    unsigned int size() { return connectionArrayLength; }

	private:
	    /*! Holds information about the connection group.
		Should match ConnectionGroup table in SpikeStreamNetwork database */
	    ConnectionGroupInfo info;

	    /*! Array of connections between neurons */
	    Connection* connectionArray;

	    /*! Length of the array */
	    unsigned int connectionArrayLength;

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

