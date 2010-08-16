#include "SpikeStreamThread.h"
using namespace spikestream;

/*! Constructor */
SpikeStreamThread::SpikeStreamThread() : QThread(){
}


/*! Destructor */
SpikeStreamThread::~SpikeStreamThread(){
}


/*----------------------------------------------------------*/
/*-----                PUBLIC  METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Clears error state and message */
void SpikeStreamThread::clearError(){
	error = false;
	errorMessage = "";
}


/*! Returns the error message */
QString SpikeStreamThread::getErrorMessage(){
	return errorMessage;
}

/*! Returns true if an error has occurred */
bool SpikeStreamThread::isError() {
	return error;
}


/*! Stops the thread running  */
void SpikeStreamThread::stop() {
	stopThread = true;
}


/*----------------------------------------------------------*/
/*-----              PROTECTED METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Puts the thread into the error state with the provided error message */
void SpikeStreamThread::setError(const QString& errorMessage){
	error = true;
	this->errorMessage = errorMessage;
	stopThread = true;
}

