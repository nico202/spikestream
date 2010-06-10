#include "SpikeStreamException.h"
using namespace spikestream;

/*! Constructor */
SpikeStreamException::SpikeStreamException(QString details){
       this->type = "SpikeStreamException";
       this->details = details;
}


/*! Destructor */
SpikeStreamException::~SpikeStreamException(){
}


/*! Returns a message describing the exception */
QString SpikeStreamException::getMessage(){
	return type + ": \'" + details + " \'";
}
