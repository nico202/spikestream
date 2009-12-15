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
	    void add4NeuronNetworks1(const QString& networkName);

	private:
	    //======================  METHODS  ========================
	    void add4NeuronFiringPatterns(unsigned int networkID, NeuronMap& neuronMap);
    };

}

#endif//ALEKSANDERNETWORKSBUILDER_H
