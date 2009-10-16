#include "AnalysisTimeStepThread.h"
using namespace spikestream;


/*! Constructor */
AnalysisTimeStepThread::AnalysisTimeStepThread(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo){
    //Create the data access objects
    networkDao = new NetworkDao(netDBInfo);
    archiveDao = new ArchiveDao(archDBInfo);
    analysisDao = new StateBasedPhiAnalysisDao(anaDBInfo);

    //Initialize variables
    timeStep = -1;

    Util::seedRandom();
}


/*! Destructor */
AnalysisTimeStepThread::~AnalysisTimeStepThread(){
}

/*! Prepares for the analysis by storing the time step to be analyzed. */
void AnalysisTimeStepThread::prepareTimeStepAnalysis(int timeStep){
    this->timeStep = timeStep;
}


/*! Run method inherited from QThread */
void AnalysisTimeStepThread::run(){
    threadRunning = true;

    int numberOfSteps = Util::getRandom(0, 50);

    for(int i=0; i<numberOfSteps; ++i){
	usleep(100000);//Sleep for 100 milliseconds
	emit progress(i, numberOfSteps-1);
	if(!threadRunning)
	    return;
    }

    emit progress(numberOfSteps, numberOfSteps);

    threadRunning = false;
}


/*! Stops the analysis */
void AnalysisTimeStepThread::stop(){
    threadRunning = false;
}
