#ifndef CONNECTIONGROUPBUILDER_H
#define CONNECTIONGROUPBUILDER_H

//SpikeStream includes
#include "Network.h"

//Qt includes
#include <QObject>

namespace spikestream {


	/*! Adds connections to the supplied network using the loaded data about connection weights and delays. */
	class ConnectionGroupBuilder : public QObject {
		Q_OBJECT

		public:
			ConnectionGroupBuilder();
			~ConnectionGroupBuilder();
			void addConnectionGroups(Network* network, const QString& weightsFilePath, const QString& delaysFilePath, QHash<QString, double> parameterMap);


		signals:
			void progress(int stepsCompleted, int totalSteps, QString message);


		private:
			//======================  VARIABLES  =========================
			/*! List of nodes names in the order in which they appear in the connectivity matrix. */
			QList<QString> nodeNamesList;

			/*! Map linking the index of the neuron group to the neuron group. Indexes range from 0-65 for a 66 node network. */
			QHash<unsigned, NeuronGroup*> neurGrpIdxMap;

			/*! Map linking neuron group indexes with set of connections. Connection class holds to neuron group index and weight and delay. */
			QHash<unsigned, QList<Connection*> > neurGrpConMap;

			/*! Number of nodes in the network. */
			int numberOfNodes;

			/*! The synapse type of all the connections */
			SynapseType synapseType;

			//======================  METHODS  ===========================

	};

}

#endif//CONNECTIONGROUPBUILDER_H
