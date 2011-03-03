#ifndef ISPIKEMANAGER_H
#define ISPIKEMANAGER_H

//SpikeStream includes
#include "AbstractDeviceManager.h"


namespace spikestream {

	class ISpikeManager : public AbstractDeviceManager{
		public:
			ISpikeManager();
			~ISpikeManager();
			QList<neurid_t>::iterator outputNeuronsBegin();
			QList<neurid_t>::iterator outputNeuronsEnd();
			void setInputNeurons(timestep_t timeStep, QList<neurid_t>& firingNeuronIDs);
			void step();


		private:
			//=====================  VARIABLES  ======================
			/*! List of output neuron ids */
			QList<neurid_t> outputNeuronIDs;

	};

}

#endif//ISPIKEMANAGER_H
