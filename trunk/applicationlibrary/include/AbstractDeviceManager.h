#ifndef ABSTRACTDEVICEMANAGER_H
#define ABSTRACTDEVICEMANAGER_H

//SpikeStream includes
#include "SpikeStreamTypes.h"

//Qt includes
#include <QList>

namespace spikestream {

	/*! Defines a generic interface used by all device managers */
	class AbstractDeviceManager{

		public:
			AbstractDeviceManager();
			virtual ~AbstractDeviceManager();

			/*! Returns iterator pointing to start of list of firing neurons output from the device */
			virtual QList<neurid_t>::iterator outputNeuronsBegin() = 0;

			/*! Returns iterator pointing to end of list of firing neurons output from the device */
			virtual QList<neurid_t>::iterator outputNeuronsEnd() = 0;

			/*! Sets firing neuron ids entering the device. */
			virtual void setInputNeurons(timestep_t timeStep, QList<neurid_t>& firingNeuronIDs) = 0;

			/*! Steps the device forward one time step */
			virtual void step() = 0;

	};

}

#endif//ABSTRACTDEVICEMANAGER_H
