#ifndef SPIKESTREAMSIMULATIONEXCEPTION_H
#define SPIKESTREAMSIMULATIONEXCEPTION_H

//SpikeStream includes
#include "SpikeStreamException.h"

namespace spikestream {

	/*! Exception that relates to simulation functions. */
	class SpikeStreamSimulationException : public SpikeStreamException {
		public:
			SpikeStreamSimulationException(QString details);
			~SpikeStreamSimulationException();

	};

}

#endif//SPIKESTREAMSIMULATIONEXCEPTION_H


