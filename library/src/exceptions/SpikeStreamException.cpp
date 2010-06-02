#include "SpikeStreamException.h"
using namespace spikestream;

SpikeStreamException::SpikeStreamException(QString details){
       this->type = "SpikeStreamException";
       this->details = details;
}

SpikeStreamException::~SpikeStreamException(){
}

QString SpikeStreamException::getMessage(){
	return type + ": \'" + details + " \'";
}
