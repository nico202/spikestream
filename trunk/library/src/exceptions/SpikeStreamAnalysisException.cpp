#include "SpikeStreamAnalysisException.h"
using namespace spikestream;


/*! Constructor */
SpikeStreamAnalysisException::SpikeStreamAnalysisException(QString details) : SpikeStreamException (details) {
       this->type = "SpikeStreamAnalysisException";
}


/*! Destructor */
SpikeStreamAnalysisException::~SpikeStreamAnalysisException(){
}

