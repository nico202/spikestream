#ifndef NEURON_H
#define NEURON_H

//SpikeStream includes
#include "Point3D.h"
using namespace spikestream;

namespace spikestream {

	/*! Class holding details about a neuron. */
    class Neuron {
		public:
			Neuron(float xPos, float yPos, float zPos);
			Neuron(unsigned int id, float xPos, float yPos, float zPos);
			virtual ~Neuron();
			float getXPos() { return location.getXPos(); }
			float getYPos() { return location.getYPos(); }
			float getZPos() { return location.getZPos(); }
			unsigned int getID() { return id; }
			Point3D& getLocation() { return location; }
			void print();
			void setID(unsigned int id) { this->id = id; }

		private:
			//=================  VARIABLES  ====================
			/*! ID of the neuron */
			unsigned int id;

			/*! Location of the neuron in three dimensions */
			Point3D location;
		};
}

#endif//NEURON_H
