//SpikeStream includes
#include "AnalysisTimeStepThread.h"
#include "SpikeStreamException.h"
#include "SubsetManager.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>


/*! Constructor */
AnalysisTimeStepThread::AnalysisTimeStepThread(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo){
    //Store the database information
    this->networkDBInfo = netDBInfo;
    this->archiveDBInfo = archDBInfo;
    this->analysisDBInfo = anaDBInfo;

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


/*! Prepares for the analysis by storing the analysis information and time step to be analyzed. */
void AnalysisTimeStepThread::prepareTimeStepAnalysis(const AnalysisInfo& anaInfo, int timeStep){
    this->analysisInfo = anaInfo;
    this->timeStep = timeStep;
}


/*! Run method inherited from QThread */
void AnalysisTimeStepThread::run(){
    qDebug()<<"Analysis Time Step Thread Started";

    stop = false;
    clearError();

    try{
        //Create the calculator to run the phi calculation on the time step
	SubsetManager* subsetManager = new SubsetManager(networkDBInfo, archiveDBInfo, analysisDBInfo, analysisInfo, timeStep);
	connect(subsetManager, SIGNAL(complexFound()), this, SLOT(updateComplexes()));
	connect(subsetManager, SIGNAL(progress(const QString&, unsigned int, unsigned int, unsigned int)), this, SLOT(updateProgress(const QString&, unsigned int, unsigned int, unsigned int)));
	connect(subsetManager, SIGNAL(test()), this, SLOT(test()));
	subsetManager->runCalculation(&stop);
	delete subsetManager;
    }
    catch(SpikeStreamException& ex){
	setError(ex.getMessage());
    }

    stop = true;

    qDebug()<<"Analysis time step thread stopped.";
}

void AnalysisTimeStepThread::test(){
  qDebug()<<"TEST SLOT CALLED";
}

/*! Stops the analysis */
void AnalysisTimeStepThread::stopThread(){
    stop = true;
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                -----*/
/*----------------------------------------------------------*/

/*! Emits a signal indicating that the list of complexes should be updated */
void AnalysisTimeStepThread::updateComplexes(){
    emit complexFound();
}


/*! Emits a signal indicating that progress has been made */
void AnalysisTimeStepThread::updateProgress(const QString& msg, unsigned int timeStep, unsigned int stepsCompleted, unsigned int totalSteps){
   qDebug()<<"ANALYSIS TIME STEP THREAD PROGRESS: "<<msg;
    emit progress(msg, timeStep, stepsCompleted, totalSteps);
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Sets the thread into the error state */
void AnalysisTimeStepThread::setError(const QString& message){
    error = true;
    errorMessage = message;

    //exit this thread
    stopThread();
}

