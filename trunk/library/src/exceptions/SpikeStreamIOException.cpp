#include "SpikeStreamIOException.h"
using namespace spikestream;


/*! Constructor */
SpikeStreamIOException::SpikeStreamIOException(QString details) : SpikeStreamException (details) {
	   this->type = "SpikeStreamIOException";
}


/*! Destructor */
SpikeStreamIOException::~SpikeStreamIOException(){
}

