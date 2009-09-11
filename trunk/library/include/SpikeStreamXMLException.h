#ifndef SPIKESTREAMXMLEXCEPTION_H
#define SPIKESTREAMXMLEXCEPTION_H

//SpikeStream includes
#include "SpikeStreamException.h"

class SpikeStreamXMLException : public SpikeStreamException {
    public:
	SpikeStreamXMLException(QString details);
	~SpikeStreamXMLException();

};

#endif//SPIKESTREAMXMLEXCEPTION_H


