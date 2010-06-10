#include "SpikeStreamXMLException.h"
using namespace spikestream;


/*! Constructor */
SpikeStreamXMLException::SpikeStreamXMLException(QString details) : SpikeStreamException (details) {
       this->type = "SpikeStreamXMLException";
}


/*! Destructor */
SpikeStreamXMLException::~SpikeStreamXMLException(){
}

