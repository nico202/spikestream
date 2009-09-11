#include "SpikeStreamXMLException.h"


/*! Constructor */
SpikeStreamXMLException::SpikeStreamXMLException(QString details) : SpikeStreamException (details) {
       this->type = "SpikeStreamXMLException";
}


/*! Destructor */
SpikeStreamXMLException::~SpikeStreamXMLException(){
}

