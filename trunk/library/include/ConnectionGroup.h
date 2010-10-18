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
			bool contains(unsigned neuronID);
			QHash<unsigned int, QList<Connection*> >::const_iterator fromMapBegin() { return fromConnectionMap.begin(); }
			QHash<unsigned int, QList<Connection*> >::const_iterator fromMapEnd() { return fromConnectionMap.end(); }
			QHash<unsigned, Connection*>::const_iterator end();
			QList<Connection*> getConnections();
			unsigned int getID() { return info.getID(); }
			QList<Connection*> getFromConnections(unsigned int neurID);
			unsigned int getFromNeuronGroupID() { return info.getFromNeuronGroupID(); }
			ConnectionGroupInfo getInfo() { return info; }
			double getParameter(const QString& paramName);
			QHash<QString, double> getParameters() { return parameterMap; }
			QList<Connection*> getToConnections(unsigned int neurID);
			unsigned int getToNeuronGroupID() { return info.getToNeuronGroupID(); }
			bool isLoaded() { return loaded; }
			void setConnectionMap(QHash<unsigned, Connection*>* newConnectionMap);
			void setDescription(const QString& description);
			void setFromNeuronGroupID(unsigned id);
			void setID(unsigned int id) { info.setID(id); }
			void setLoaded(bool loaded) { this->loaded = loaded; }
			void setParameters(QHash<QString, double>& paramMap) { this->parameterMap = paramMap; }
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

			/*! Map enabling rapid access to the connections from any neuron */
			QHash<unsigned int, QList<Connection*> > fromConnectionMap;

			/*! Map enabling rapid access to connections to any neuron */
			QHash<unsigned int, QList<Connection*> > toConnectionMap;

		   /*! Returns true if the state of the connection array matches the database.
				This should be false if no connections have been loaded and false
				if the connection array is full of connections that are not in the database */
		   bool loaded;

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

