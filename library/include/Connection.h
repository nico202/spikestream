#ifndef CONNECTION_H
#define CONNECTION_H

namespace spikestream {

    /*! Holds information about each connection. Should match the information stored
	in the Connections table of the SpikeStream database */
    class Connection{
	public:
	    Connection (unsigned int fromNeuronID, unsigned int toNeuronID, float delay, float weight, float tempWeight){
		this->fromNeuronID = fromNeuronID;
		this->toNeuronID = toNeuronID;
		this->delay = delay;
		this->weight = weight;
		this->tempWeight = tempWeight;
	    }

	    Connection (unsigned int id, unsigned int fromNeuronID, unsigned int toNeuronID, float delay, float weight, float tempWeight){
		this->id = id;
		this->fromNeuronID = fromNeuronID;
		this->toNeuronID = toNeuronID;
		this->delay = delay;
		this->weight = weight;
		this->tempWeight = tempWeight;
	    }

	    void setID(unsigned int id){ this->id = id; }

	    unsigned int id;
	    unsigned int fromNeuronID;
	    unsigned int toNeuronID;
	    float delay;
	    float weight;
	    float tempWeight;
    };

}

#endif//CONNECTION_H

