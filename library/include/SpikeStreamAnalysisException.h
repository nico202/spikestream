#ifndef SPIKESTREAMANALYSISEXCEPTION_H
#define SPIKESTREAMANALYSISEXCEPTION_H

//SpikeStream includes
#include "SpikeStreamException.h"

namespace spikestream {

	/*! Exception specific to analysis-related functionality. */
    class SpikeStreamAnalysisException : public SpikeStreamException {
		public:
			SpikeStreamAnalysisException(QString details);
			~SpikeStreamAnalysisException();

		};

}

#endif//SPIKESTREAMANALYSISEXCEPTION_H


