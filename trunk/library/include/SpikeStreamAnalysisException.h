#ifndef SPIKESTREAMANALYSISEXCEPTION_H
#define SPIKESTREAMANALYSISEXCEPTION_H

//SpikeStream includes
#include "SpikeStreamException.h"

namespace spikestream {

    class SpikeStreamAnalysisException : public SpikeStreamException {
	public:
	    SpikeStreamAnalysisException(QString details);
	    ~SpikeStreamAnalysisException();

    };

}

#endif//SPIKESTREAMANALYSISEXCEPTION_H


