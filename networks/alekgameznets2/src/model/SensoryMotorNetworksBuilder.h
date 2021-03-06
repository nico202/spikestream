#ifndef SENSORYMOTORNETWORKSBUILDER_H
#define SENSORYMOTORNETWORKSBUILDER_H

//SpikeStream includes
#include "NetworksBuilder.h"

//Qt includes
#include <QWidget>
#include <QHash>
#include <QList>
#include <QThread>

namespace spikestream {

	class SensoryMotorNetworksBuilder : public QThread, public NetworksBuilder {
		Q_OBJECT

		public:
			SensoryMotorNetworksBuilder();
			~SensoryMotorNetworksBuilder();

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

			QString networkName;
			QString networkDescription;

			/*! Pointer to bool controlling stop in the invoking thread */
			const bool* stop;

			/*! Holds all of the connections to neurons */
			QList<unsigned>* conListArray;


			//==========================  METHODS  =============================
			using NetworksBuilder::addTraining;
			void addBasicNetwork(const QString& networkName, const QString& networkDescription);
			void addConnections();
			void addFiringPatterns();
			void addTraining();
			void reset();
	};
}

#endif//SENSORYMOTORNETWORKSBUILDER_H
