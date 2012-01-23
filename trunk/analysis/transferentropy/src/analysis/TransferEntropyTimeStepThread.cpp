//SpikeStream includes
#include "Globals.h"
#include "TransferEntropyTimeStepThread.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

/*! Constructor */
TransferEntropyTimeStepThread::TransferEntropyTimeStepThread(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo) : AbstractAnalysisTimeStepThread(netDBInfo, archDBInfo, anaDBInfo) {
}


/*! Destructor */
TransferEntropyTimeStepThread::~TransferEntropyTimeStepThread(){
}


/*-------------------------------------------------------------*/
/*-------                 PUBLIC METHODS                 ------*/
/*-------------------------------------------------------------*/

/*! Run method inherited from QThread */
void TransferEntropyTimeStepThread::run(){
	stop = false;

	//Reset error state
	clearError();

	stop = true;
}

