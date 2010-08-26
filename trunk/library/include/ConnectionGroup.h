#ifndef CONNECTIONGROUP_H
#define CONNECTIONGROUP_H

//SpikeStream includes
#include "Connection.h"
#include "ConnectionGroupInfo.h"
using namespace spikestream;

/*! A list of conneections */
typedef QList<Connection*> ConnectionList;

namespace spikestream {

	/*! Holds all of the connections in a connection group along with their parameters. */
    class ConnectionGroup {
		public:
			ConnectionGroup(const ConnectionGroupInfo& connGrpInfo);
			~ConnectionGroup();
			Connection* addConnection(Connection* newConn);
			ConnectionList::const_iterator begin();
			void clearConnections();
			bool contains(unsigned neuronID);
			QHash<unsigned int, ConnectionList >::const_iterator fromMapBegin() { return fromConnectionMap.begin(); }
			QHash<unsigned int, ConnectionList >::const_iterator fromMapEnd() { return fromConnectionMap.end(); }
			ConnectionList::const_iterator end();
			ConnectionList getConnections() { return connectionList; }
			unsigned int getID() { return info.getID(); }
			ConnectionList getFromConnections(unsigned int neurID);
			unsigned int getFromNeuronGroupID() { return info.getFromNeuronGroupID(); }
			ConnectionGroupInfo getInfo() { return info; }
			double getParameter(const QString& paramName);
			QHash<QString, double> getParameters() { return parameterMap; }
			ConnectionList getToConnections(unsigned int neurID);
			unsigned int getToNeuronGroupID() { return info.getToNeuronGroupID(); }
			bool isLoaded() { return loaded; }
			void setID(unsigned int id) { info.setID(id); }
			void setLoaded(bool loaded) { this->loaded = loaded; }
			void setParameters(QHash<QString, double>& paramMap) { this->parameterMap = paramMap; }
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

		   /*! Map of parameters for the synapses in the connection group */
		   QHash<QString, double> parameterMap;

		   //====================  METHODS  ==========================
		   ConnectionGroup(const ConnectionGroup& connGrp);
		   ConnectionGroup& operator=(const ConnectionGroup& rhs);
		};

}

#endif//CONNECTIONGROUP_H

