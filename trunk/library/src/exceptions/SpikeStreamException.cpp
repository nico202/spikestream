#include "SpikeStreamException.h"

SpikeStreamException::SpikeStreamException(QString details){
       this->type = "SpikeStreamException";
       this->details = details;
}

SpikeStreamException::~SpikeStreamException(){
}

QString SpikeStreamException::getMessage(){
	return type + ": \'" + details + " \'";
}
