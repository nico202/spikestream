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

//	    void setFromNeuronPosition(float xPos, float yPos, float zPos){
//		fromXPos = xPos;
//		fromYPos = yPos;
//		fromZPos = zPos;
//	    }
//
//	    void setToNeuronPosition(float xPos, float yPos, float zPos){
//		toXPos = xPos;
//		toYPos = yPos;
//		toZPos = zPos;
//	    }


	    unsigned int id;
	    unsigned int fromNeuronID;
	    unsigned int toNeuronID;
	    float delay;
	    float weight;
	    float tempWeight;

//	    float fromXPos;
//	    float fromYPos;
//	    float fromZPos;
//	    float toXPos;
//	    float toYPos;
//	    float toZPos;
    };

}

#endif//CONNECTION_H

