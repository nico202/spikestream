//SpikeStream includes
#include "AbstractDeviceManager.h"
using namespace spikestream;


/*! Constructor */
AbstractDeviceManager::AbstractDeviceManager() {
	fireNeuronMode = true;
	current = 10;
}

/*! Destructor */
AbstractDeviceManager::~AbstractDeviceManager(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

