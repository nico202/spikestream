#ifndef CONNECTIONGROUP_H
#define CONNECTIONGROUP_H

//SpikeStream includes
#include "Connection.h"
#include "ConnectionGroupInfo.h"
using namespace spikestream;

//Other includes
#include <deque>
using namespace std;

/*! An iterator for working through all of the connections in the group. */
typedef deque<Connection>::iterator ConnectionIterator;

namespace spikestream {

	/*! Holds all of the connections in a connection group along with their parameters. */
    class ConnectionGroup {
		public:
			ConnectionGroup();
			ConnectionGroup(const ConnectionGroupInfo& connGrpInfo);
			~ConnectionGroup();
			unsigned addConnection(unsigned id, unsigned fromNeuronID, unsigned toNeuronID, float delay, float weight);
			unsigned addConnection(unsigned int fromNeuronID, unsigned int toNeuronID, float delay, float weight);
			ConnectionIterator begin();
			void clearConnections();
			ConnectionIterator end();
			unsigned int getID() { return info.getID(); }
			unsigned int getFromNeuronGroupID() { return info.getFromNeuronGroupID(); }
			ConnectionGroupInfo getInfo() { return info; }
			double getParameter(const QString& paramName);
			QHash<QString, double> getParameters() { return parameterMap; }
			unsigned getSynapseTypeID();
			unsigned int getToNeuronGroupID() { return info.getToNeuronGroupID(); }
			Connection& operator[] (unsigned index);
			bool parametersSet();
			void setDescription(const QString& description);
			void setFromNeuronGroupID(unsigned id);
			void setID(unsigned int id) { info.setID(id); }
			void setParameters(QHash<QString, double>& paramMap);
			void setToNeuronGroupID(unsigned id);
			int size() { return connectionDeque->size(); }

		private:
			/*! Holds information about the connection group.
				Should match ConnectionGroup table in SpikeStreamNetwork database */
			ConnectionGroupInfo info;

			/*! Vector of connections. */
			deque<Connection>* connectionDeque;

			/*! Map of parameters for the synapses in the connection group */
			QHash<QString, double> parameterMap;

			/*! Static counter used to generate connection IDs that are unique for an instance of the application.
				Used for prototyping networks. */
			static unsigned connectionIDCounter;


		   //====================  METHODS  ==========================
		   static unsigned getTemporaryID();
		   ConnectionGroup(const ConnectionGroup& connGrp);
		   ConnectionGroup& operator=(const ConnectionGroup& rhs);
		};

}

#endif//CONNECTIONGROUP_H

