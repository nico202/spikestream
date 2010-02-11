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
PhiAnalysisTimeStepThread::PhiAnalysisTimeStepThread(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo) : AbstractAnalysisTimeStepThread(netDBInfo, archDBInfo, anaDBInfo) {
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
		//Check that network is all weightless neurons
		NetworkDao netDao(networkDBInfo);
		if(netDao.isWeightlessNetwork(analysisInfo.getNetworkID())){
			//Create the calculator to run the phi calculation on the time step
			SubsetManager* subsetManager = new SubsetManager(networkDBInfo, archiveDBInfo, analysisDBInfo, analysisInfo, timeStep);
			connect(subsetManager, SIGNAL(complexFound()), this, SLOT(updateResults()), Qt::DirectConnection);
			connect(subsetManager, SIGNAL(progress(const QString&, unsigned int, unsigned int, unsigned int)), this, SLOT(updateProgress(const QString&, unsigned int, unsigned int, unsigned int)), Qt::DirectConnection);
			subsetManager->runCalculation(&stop);
			delete subsetManager;
		}
		else{
			setError("Network contains non-weightless neurons. Only weightless neurons are supported at present.");
		}
	}
	catch(SpikeStreamException& ex){
		setError(ex.getMessage());
	}

	stop = true;
}

