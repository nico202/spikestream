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

}


//Inherited from AbstractDeviceManager
void ISpikeManager::step(){

}

