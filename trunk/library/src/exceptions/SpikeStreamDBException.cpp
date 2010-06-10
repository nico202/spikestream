#include "SpikeStreamDBException.h"
using namespace spikestream;


/*! Constructor */
SpikeStreamDBException::SpikeStreamDBException(QString details) : SpikeStreamException (details) {
       this->type = "SpikeStreamDBException";
}


/*! Destructor */
SpikeStreamDBException::~SpikeStreamDBException(){
}

