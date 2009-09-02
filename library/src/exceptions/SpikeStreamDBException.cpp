#include "SpikeStreamDBException.h"


/*! Constructor */
SpikeStreamDBException::SpikeStreamDBException(QString details) : SpikeStreamException (details) {
       this->type = "SpikeStreamDBException";
}


/*! Destructor */
SpikeStreamDBException::~SpikeStreamDBException(){
}

