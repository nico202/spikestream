#include "NRMException.h"
using namespace spikestream;


/*! Constructor */
NRMException::NRMException(QString details) : SpikeStreamException (details) {
	   this->type = "NRMException";
}


/*! Constructor */
NRMException::NRMException(string details) : SpikeStreamException (details.data()) {
	this->type = "NRMException";
}


/*! Constructor */
NRMException::NRMException(const char* details) : SpikeStreamException (details) {
	this->type = "NRMException";
}


/*! Constructor */
NRMException::NRMException(const char* details, int data1) : SpikeStreamException (QString(details) + ": " + QString::number(data1)) {
	this->type = "NRMException";
}


/*! Destructor */
NRMException::~NRMException(){
}
