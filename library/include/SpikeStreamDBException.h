#ifndef SPIKESTREAMDBEXCEPTION_H
#define SPIKESTREAMDBEXCEPTION_H

//SpikeStream includes
#include "SpikeStreamException.h"

namespace spikestream {

	/*! Exception specific to database-related functionality. */
	class SpikeStreamDBException : public SpikeStreamException {
		public:
		SpikeStreamDBException(QString details);
		~SpikeStreamDBException();

	};
}

#endif//SPIKESTREAMDBEXCEPTION_H


