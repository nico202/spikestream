#ifndef TESTPARTITIONEDNETWORKSBUILDER_H
#define TESTPARTITIONEDNETWORKSBUILDER_H

//SpikeStream includes
#include "NetworksBuilder.h"

//Qt includes
#include <QWidget>
#include <QHash>
#include <QList>
#include <QThread>

namespace spikestream {

	class TestPartitionedNetworksBuilder : public QThread, public NetworksBuilder {
		Q_OBJECT

		public:
			TestPartitionedNetworksBuilder();
			~TestPartitionedNetworksBuilder();

			void prepareAddNetworks(const QString& networkName, const QString& networkDescription, const bool* stop);
			void run();


		signals:
			void progress(int stepsComplete, int totalSteps);


		private:
			//==========================  VARIABLES  ============================
			/*! Map holding neurons in the network. The key is the neuron number used by this class
				which is in the range 1-12. The value is the neuron id. */
			QHash<unsigned int, unsigned int> neuronMap;

			/*! ID of the neuron group being added. */
			unsigned int neuronGroupID;

			/*! ID of the network being added */
			unsigned int networkID;

			static const unsigned int numberOfNeurons = 8;

			QString networkName;
			QString networkDescription;

			/*! Pointer to bool controlling stop in the invoking thread */
			const bool* stop;

			QString firingNeuronStr25;
			QString firingNeuronStr50;
			QString firingNeuronStr75;
			QString firingNeuronStr100;

			//==========================  METHODS  =============================
			using NetworksBuilder::addTraining;
			void addBasicNetwork(const QString& networkName, const QString& networkDescription);
			void addConnections();
			void addFiringPatterns();
			void addTraining(unsigned int percentInputs);
			QString getFiringNeuronStr(unsigned int percentNeurons);
			QList<unsigned int> getRandomFromNeuronIDs(unsigned int toNeuronID, int numFromCons);
			void reset();
	};
}

#endif//TESTPARTITIONEDNETWORKSBUILDER_H
