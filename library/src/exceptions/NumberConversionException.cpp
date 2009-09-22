#include "NumberConversionException.h"


/*! Constructor */
NumberConversionException::NumberConversionException(QString details) : SpikeStreamException (details) {
       this->type = "NumberConversionException";
}


/*! Destructor */
NumberConversionException::~NumberConversionException(){
}

