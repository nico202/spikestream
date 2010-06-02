#ifndef CONNECTION_H
#define CONNECTION_H

namespace spikestream {

    /*! Holds information about each connection. Should match the information stored
	in the Connections table of the SpikeStream database */
    class Connection{
		public:
			Connection (unsigned int fromNeuronID, unsigned int toNeuronID, float delay, float weight, float tempWeight);
			Connection (unsigned int id, unsigned int conGrpID, unsigned int fromNeuronID, unsigned int toNeuronID, float delay, float weight, float tempWeight);
			Connection(const Connection& conn);
			Connection& operator=(const Connection& rhs);
			void print();

			float getDelay() { return delay; }
			unsigned getFromNeuronID(){ return fromNeuronID; }
			unsigned getToNeuronID(){ return toNeuronID; }
			float getTempWeight() { return tempWeight; }
			float getWeight() { return weight; }
			unsigned int getID() { return id; }
			unsigned int getConnectionGroupID() { return connectionGroupID; }
			void setID(unsigned int id){ this->id = id; }

			unsigned int id;
			unsigned int connectionGroupID;
			unsigned int fromNeuronID;
			unsigned int toNeuronID;
			float delay;
			float weight;
			float tempWeight;

    };

}

#endif//CONNECTION_H

