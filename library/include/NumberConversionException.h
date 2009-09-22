#ifndef NUMBERCONVERSIONEXCEPTION_H
#define NUMBERCONVERSIONEXCEPTION_H

//SpikeStream includes
#include "SpikeStreamException.h"

class NumberConversionException : public SpikeStreamException {
    public:
	NumberConversionException(QString details);
	~NumberConversionException();

};

#endif//NUMBERCONVERSIONEXCEPTION_H



