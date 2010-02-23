#ifndef SPIKESTREAMIOEXCEPTION_H
#define SPIKESTREAMIOEXCEPTION_H

//SpikeStream includes
#include "SpikeStreamException.h"

namespace spikestream {
	class SpikeStreamIOException : public SpikeStreamException {
		public:
			SpikeStreamIOException(QString details);
			~SpikeStreamIOException();

	};
}

#endif//SPIKESTREAMIOEXCEPTION_H


