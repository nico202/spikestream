#ifndef ALEKSANDERNETWORKSBUILDER_H
#define ALEKSANDERNETWORKSBUILDER_H

//SpikeStream includes
#include "NetworksBuilder.h"
#include "NeuronGroup.h"
using namespace spikestream;

//Qt includes
#include <QString>

namespace spikestream {

	class AleksanderNetworksBuilder : public NetworksBuilder {
		public:
			AleksanderNetworksBuilder();
			~AleksanderNetworksBuilder();
			void add4NeuronNetwork1(const QString& networkName, const QString& networkDescription);
			void add4NeuronNetwork2_AND(const QString& networkName, const QString& networkDescription);
			void add4NeuronNetwork2_XOR(const QString& networkName, const QString& networkDescription);
			void add4NeuronNetwork3_AND(const QString& networkName, const QString& networkDescription);
			void add4NeuronNetwork3_XOR(const QString& networkName, const QString& networkDescription);
			void add4NeuronNetwork4_AND(const QString& networkName, const QString& networkDescription);
			void add4NeuronNetwork4_XOR(const QString& networkName, const QString& networkDescription);
			void add4NeuronNetwork5_AND(const QString& networkName, const QString& networkDescription);
			void add4NeuronNetwork5_XOR(const QString& networkName, const QString& networkDescription);

		private:
			//====================  VARIABLES  ========================
			/*! Map holding neurons in the network */
			QHash<unsigned int, Neuron*> neuronMap;

			/*! Aleksander networks consist of a single neuron group */
			NeuronGroup* neuronGroup;

			/*! ID of the network being added */
			unsigned int networkID;

			//======================  METHODS  ========================
			void add4NeuronBasicNetwork(const QString& networkName, const QString& networkDescription);
			void add4NeuronFiringPatterns(unsigned int networkID, NeuronMap& neuronMap);
			void reset();
	};

}

#endif//ALEKSANDERNETWORKSBUILDER_H








