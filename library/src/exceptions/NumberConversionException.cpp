#include "NumberConversionException.h"
using namespace spikestream;


/*! Constructor */
NumberConversionException::NumberConversionException(QString details) : SpikeStreamException (details) {
       this->type = "NumberConversionException";
}


/*! Destructor */
NumberConversionException::~NumberConversionException(){
}

