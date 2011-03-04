//SpikeStream includes
#include "ISpikeManager.h"
using namespace spikestream;

/*! Constructor */
ISpikeManager::ISpikeManager() {

}


/*! Destructor */
ISpikeManager::~ISpikeManager(){
}


/*----------------------------------------------------------*/
/*------                PUBLIC METHODS                ------*/
/*----------------------------------------------------------*/
/*! Adds a channel, which will be a source of spikes to pass to the network
	or the destination of spikes that will be passed back to iSpike. */
void ISpikeManager::addChannel(/* CHANNEL CLASS, NEURON GROUP */ ){

}


//Inherited from AbstractDeviceManager
QList<neurid_t>::iterator ISpikeManager::outputNeuronsBegin(){
	return outputNeuronIDs.begin();
}

//Inherited from AbstractDeviceManager
QList<neurid_t>::iterator ISpikeManager::outputNeuronsEnd(){
	return outputNeuronIDs.end();
}


//Inherited from AbstractDeviceManager
void ISpikeManager::setInputNeurons(timestep_t timeStep, QList<neurid_t>& firingNeuronIDs){
	//Remap firing neuron ids to correct range


	//Pass firing neuron ids to appropriate channels
}


//Inherited from AbstractDeviceManager
void ISpikeManager::step(){
	//Work through channels

	//Get firing neurons from iSpike library

	//Convert to range and copy to outputNeuronIDs

	//Step the iSpike library

}

