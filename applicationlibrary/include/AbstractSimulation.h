#ifndef ABSTRACTSIMULATION_H
#define ABSTRACTSIMULATION_H

//Qt includes
#include <QHash>

namespace spikestream {

	/*! Defines an interface common to all simulation plugins that enables
		generic functions, such as setting of neuron and synapse parameters. */
	class AbstractSimulation {
		public:
			AbstractSimulation();
			virtual ~AbstractSimulation();
			virtual void setNeuronParameters(unsigned neuronGroupID, QHash<QString, double> parameterMap) = 0;
			virtual void setSynapseParameters(unsigned connectionGroupID, QHash<QString, double> parameterMap) = 0;

	};

}


#endif//ABSTRACTSIMULATION_H
