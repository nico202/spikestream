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
#include <QHash>
#include <QList>
#include <QPair>

//Other includes
#include <map>
using namespace std;

namespace spikestream {

	class ISpikeManager : public AbstractDeviceManager{
		public:
			ISpikeManager();
			~ISpikeManager();
			void addChannel(InputChannel* inputChannel, NeuronGroup* neuronGroup);
			void addChannel(OutputChannel* outputChannel, NeuronGroup* neuronGroup);
			void deleteAllChannels();
			void deleteInputChannel(int index);
			void deleteOutputChannel(int index);
			QList< QPair<InputChannel*, NeuronGroup*> > getInputChannels(){ return inputChannels; }
			int getInputChannelCount() { return inputChannels.size(); }
			map<string, Property*> getInputParameters(int index);
			QList< QPair<OutputChannel*, NeuronGroup*> >  getOutputChannels(){  return outputChannels; }
			int getOutputChannelCount() { return outputChannels.size(); }
			map<string, Property*> getOutputParameters(int index);
			QList<neurid_t>::iterator outputNeuronsBegin();
			QList<neurid_t>::iterator outputNeuronsEnd();
			void setInputNeurons(timestep_t timeStep, QList<neurid_t>& firingNeuronIDs);
			void step();


		private:
			//=====================  VARIABLES  ======================
			/*! List of output neuron ids */
			QList<neurid_t> outputNeuronIDs;

			/*! Map linking a SpikeStream ID to an ID within iSpike */
			QHash<neurid_t, QPair<neurid_t, vector<int>* >* > neurIDMap;

			/*! Vectors holding input neurons for each channel */
			vector< vector<int> > firingNeurIDVectors;

			/*! List of input channels and their associated neuron groups */
			QList< QPair<InputChannel*, NeuronGroup*> > inputChannels;

			/*! List of output channels and their associated neuron groups */
			QList< QPair<OutputChannel*, NeuronGroup*> > outputChannels;
	};

}

#endif//ISPIKEMANAGER_H