#include "SpikeStreamSimulationException.h"
using namespace spikestream;


/*! Constructor */
SpikeStreamSimulationException::SpikeStreamSimulationException(QString details) : SpikeStreamException (details) {
	   this->type = "SpikeStreamSimulationException";
}


/*! Destructor */
SpikeStreamSimulationException::~SpikeStreamSimulationException(){
}

