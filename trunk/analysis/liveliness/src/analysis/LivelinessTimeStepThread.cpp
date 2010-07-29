//SpikeStream includes
#include "Globals.h"
#include "LivelinessTimeStepThread.h"
#include "PerformanceTimer.h"
#include "SpikeStreamException.h"
#include "WeightlessLivelinessAnalyzer.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

/*! Constructor */
LivelinessTimeStepThread::LivelinessTimeStepThread(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo) : AbstractAnalysisTimeStepThread(netDBInfo, archDBInfo, anaDBInfo) {
}


/*! Destructor */
LivelinessTimeStepThread::~LivelinessTimeStepThread(){
}


/*-------------------------------------------------------------*/
/*-------                 PUBLIC METHODS                 ------*/
/*-------------------------------------------------------------*/

/*! Run method inherited from QThread */
void LivelinessTimeStepThread::run(){
	stop = false;

	//Start timer
	PerformanceTimer timer;

	//Reset error state
	clearError();

	try{
		//Check that network is all weightless neurons
		NetworkDao netDao(networkDBInfo);
		if(netDao.isWeightlessNetwork(analysisInfo.getNetworkID())){
			WeightlessLivelinessAnalyzer* weiLivAna = new WeightlessLivelinessAnalyzer(networkDBInfo, archiveDBInfo, analysisDBInfo, analysisInfo, timeStep);
			connect(weiLivAna, SIGNAL(newResultsFound()), this, SLOT(updateResults()), Qt::DirectConnection);
			connect(weiLivAna, SIGNAL(progress(const QString&, unsigned int, unsigned int, unsigned int)), this, SLOT(updateProgress(const QString&, unsigned int, unsigned int, unsigned int)), Qt::DirectConnection);
			weiLivAna->runCalculation(&stop);
			delete weiLivAna;
		}
		else{
			setError("Network contains non-weightless neurons. Only weightless neurons are supported at present.");
		}
	}
	catch(SpikeStreamException& ex){
		setError(ex.getMessage());
	}

	//Show time taken for the analysis
	timer.printTime("Liveliness Analysis time step " + QString::number(timeStep));

	stop = true;
}

