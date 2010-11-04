#ifndef CONNECTIONGROUP_H
#define CONNECTIONGROUP_H

//SpikeStream includes
#include "Connection.h"
#include "ConnectionGroupInfo.h"
using namespace spikestream;

namespace spikestream {

	/*! Holds all of the connections in a connection group along with their parameters. */
    class ConnectionGroup {
		public:
			ConnectionGroup(const ConnectionGroupInfo& connGrpInfo);
			~ConnectionGroup();
			Connection* addConnection(Connection* newConn);
			Connection* addConnection(unsigned int fromNeuronID, unsigned int toNeuronID, float delay, float weight);//UNTESTED
			QHash<unsigned, Connection*>::const_iterator begin();
			void clearConnections();
			QHash<unsigned, Connection*>::const_iterator end();
			QList<Connection*> getConnections();
			unsigned int getID() { return info.getID(); }
			unsigned int getFromNeuronGroupID() { return info.getFromNeuronGroupID(); }
			ConnectionGroupInfo getInfo() { return info; }
			double getParameter(const QString& paramName);
			QHash<QString, double> getParameters() { return parameterMap; }
			unsigned getSynapseTypeID();
			unsigned int getToNeuronGroupID() { return info.getToNeuronGroupID(); }
			bool parametersSet();
			void setConnectionMap(QHash<unsigned, Connection*>* newConnectionMap);
			void setDescription(const QString& description);
			void setFromNeuronGroupID(unsigned id);
			void setID(unsigned int id) { info.setID(id); }
			void setParameters(QHash<QString, double>& paramMap);
			void setTempWeight(unsigned connectionID, float tempWeight);
			void setToNeuronGroupID(unsigned id);
			void setWeight(unsigned connectionID, float weight);
			int size() { return connectionMap->size(); }

		private:
			/*! Holds information about the connection group.
				Should match ConnectionGroup table in SpikeStreamNetwork database */
			ConnectionGroupInfo info;

			/*! Map linking connection ID to connections. */
			QHash<unsigned, Connection*>* connectionMap;

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

