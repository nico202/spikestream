//SpikeStream includes
#include "AnalysisTimeStepThread.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>


/*! Constructor */
AnalysisTimeStepThread::AnalysisTimeStepThread(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo){
    //Create the data access objects
    networkDao = new NetworkDao(netDBInfo);
    archiveDao = new ArchiveDao(archDBInfo);
    analysisDao = new StateBasedPhiAnalysisDao(anaDBInfo);

    //Initialize variables
    timeStep = -1;
}


/*! Destructor */
AnalysisTimeStepThread::~AnalysisTimeStepThread(){
}

/*-------------------------------------------------------------*/
/*-------                 PUBLIC METHODS                 ------*/
/*-------------------------------------------------------------*/

/*! Clears the error state of the class */
void AnalysisTimeStepThread::clearError(){
    error = false;
    errorMessage = "";
}


/*! Prepares for the analysis by storing the time step to be analyzed. */
void AnalysisTimeStepThread::prepareTimeStepAnalysis(int timeStep){
    this->timeStep = timeStep;
	Util::seedRandom(timeStep);
}


/*! Run method inherited from QThread */
void AnalysisTimeStepThread::run(){
    threadRunning = true;
    clearError();

    int numberOfSteps = Util::getRandom(0, 50);

    for(int i=0; i<numberOfSteps; ++i){
	usleep(100000);//Sleep for 100 milliseconds
	emit progress(timeStep, i, numberOfSteps-1);
	qDebug()<<"Making progress, time step: "<<timeStep;
	if(!threadRunning)
	    return;
    }
    emit progress(timeStep, numberOfSteps, numberOfSteps);
qDebug()<<"THREAD FINISHED: "<<timeStep;
    threadRunning = false;
}


/*! Stops the analysis */
void AnalysisTimeStepThread::stop(){
    threadRunning = false;
}
