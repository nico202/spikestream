#ifndef CONNECTION_H
#define CONNECTION_H

namespace spikestream {

    /*! Holds information about each connection. Should match the information stored
		in the Connections table of the SpikeStreamNetwork database. */
    class Connection{
		public:
			Connection (unsigned int fromNeuronID, unsigned int toNeuronID, float delay, float weight);
			Connection (unsigned int id, unsigned int conGrpID, unsigned int fromNeuronID, unsigned int toNeuronID, float delay, float weight);
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
			void setTempWeight(float newTempWeight) { this->tempWeight = newTempWeight; }

			//=======================  VARIABLES  ========================
			/*! ID of the connection in the database */
			unsigned int id;

			/*! ID of the connection group that this connection is part of */
			unsigned int connectionGroupID;

			/*! Connection is from neuron with this ID. */
			unsigned int fromNeuronID;

			/*! Connection is to neuron with this ID. */
			unsigned int toNeuronID;

			/*! Delay of connection */
			float delay;

			/*! Weight of connection */
			float weight;

			/*! Temporary weight of connection */
			float tempWeight;

    };

}

#endif//CONNECTION_H

