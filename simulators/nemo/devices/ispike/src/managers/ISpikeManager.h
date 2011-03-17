#ifndef ISPIKEMANAGER_H
#define ISPIKEMANAGER_H

//SpikeStream includes
#include "AbstractDeviceManager.h"
#include "NeuronGroup.h"
#include "SpikeStreamTypes.h"

//iSpike includes
#include "iSpike/Channel/InputChannel/InputChannel.hpp"
#include "iSpike/Channel/OutputChannel/OutputChannel.hpp"

//Qt includes
#include <QList>
#include <QPair>

namespace spikestream {

	class ISpikeManager : public AbstractDeviceManager{
		public:
			ISpikeManager();
			~ISpikeManager();
			void addChannel(InputChannel* inputChannel, NeuronGroup* neuronGroup);
			void addChannel(OutputChannel* outputChannel, NeuronGroup* neuronGroup);
			QList<neurid_t>::iterator outputNeuronsBegin();
			QList<neurid_t>::iterator outputNeuronsEnd();
			void setInputNeurons(timestep_t timeStep, QList<neurid_t>& firingNeuronIDs);
			void step();


		private:
			//=====================  VARIABLES  ======================
			/*! List of output neuron ids */
			QList<neurid_t> outputNeuronIDs;

			//STORE START NEURON ID AND END FOR EACH CHANNEL

			QList< QPair<InputChannel*, NeuronGroup*> > inputChannels;

			QList< QPair<OutputChannel*, NeuronGroup*> > outputChannels;
	};

}

#endif//ISPIKEMANAGER_H
