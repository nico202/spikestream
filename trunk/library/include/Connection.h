#ifndef CONNECTION_H
#define CONNECTION_H

namespace spikestream {

    /*! Holds information about each connection. Should match the information stored
	in the Connections table of the SpikeStream database */
    struct Connection{
    	float fromNeuronID;
	float toNeuronID;
	float delay;
	float weight;
	float tempWeight;
    };

}

#endif//CONNECTION_H

