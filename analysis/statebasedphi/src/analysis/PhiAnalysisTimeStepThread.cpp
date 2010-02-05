//SpikeStream includes
#include "PhiAnalysisTimeStepThread.h"
#include "SpikeStreamException.h"
#include "SubsetManager.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

//Other includes
#include <iostream>
using namespace std;


/*! Constructor */
PhiAnalysisTimeStepThread::PhiAnalysisTimeStepThread(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo) : AbstractAnalysisTimeStepThread(netDBinfo, archDBInfo, anaDBInfo) {
}


/*! Destructor */
PhiAnalysisTimeStepThread::~PhiAnalysisTimeStepThread(){
}

/*-------------------------------------------------------------*/
/*-------                 PUBLIC METHODS                 ------*/
/*-------------------------------------------------------------*/

/*! Run method inherited from QThread */
void PhiAnalysisTimeStepThread::run(){
	stop = false;
	clearError();

	try{
		//Create the calculator to run the phi calculation on the time step
		SubsetManager* subsetManager = new SubsetManager(networkDBInfo, archiveDBInfo, analysisDBInfo, analysisInfo, timeStep);
		connect(subsetManager, SIGNAL(complexFound()), this, SLOT(updateResults()), Qt::DirectConnection);
		connect(subsetManager, SIGNAL(progress(const QString&, unsigned int, unsigned int, unsigned int)), this, SLOT(updateProgress(const QString&, unsigned int, unsigned int, unsigned int)), Qt::DirectConnection);
		subsetManager->runCalculation(&stop);
		delete subsetManager;
	}
	catch(SpikeStreamException& ex){
		setError(ex.getMessage());
	}

	stop = true;
}

