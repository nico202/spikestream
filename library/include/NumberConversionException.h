#ifndef NUMBERCONVERSIONEXCEPTION_H
#define NUMBERCONVERSIONEXCEPTION_H

//SpikeStream includes
#include "SpikeStreamException.h"

namespace spikestream{

	/*! Exception thrown when an error is encountered converting numbers */
	class NumberConversionException : public SpikeStreamException {
		public:
			NumberConversionException(QString details);
			~NumberConversionException();

	};

}

#endif//NUMBERCONVERSIONEXCEPTION_H



