//SpikeStream includes
#include "AbstractAnalysisTimeStepThread.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

//Other includes
#include <iostream>
using namespace std;


/*! Constructor */
AbstractAnalysisTimeStepThread::AbstractAnalysisTimeStepThread(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo){
	//Store the database information
	this->networkDBInfo = netDBInfo;
	this->archiveDBInfo = archDBInfo;
	this->analysisDBInfo = anaDBInfo;

	//Initialize variables
	timeStep = -1;
}


/*! Destructor */
AbstractAnalysisTimeStepThread::~AbstractAnalysisTimeStepThread(){
}


/*-------------------------------------------------------------*/
/*-------                 PUBLIC METHODS                 ------*/
/*-------------------------------------------------------------*/

/*! Clears the error state of the class */
void AbstractAnalysisTimeStepThread::clearError(){
	error = false;
	errorMessage = "";
}


/*! Prepares for the analysis by storing the analysis information and time step to be analyzed. */
void AbstractAnalysisTimeStepThread::prepareTimeStepAnalysis(const AnalysisInfo& anaInfo, int timeStep){
	this->analysisInfo = anaInfo;
	this->timeStep = timeStep;
}


/*! Stops the analysis */
void AbstractAnalysisTimeStepThread::stopThread(){
	stop = true;
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                -----*/
/*----------------------------------------------------------*/

/*! Emits a signal indicating that the list of complexes should be updated */
void AbstractAnalysisTimeStepThread::updateResults(){
	emit newResultsFound();
}


/*! Emits a signal indicating that progress has been made */
void AbstractAnalysisTimeStepThread::updateProgress(const QString& msg, unsigned int timeStep, unsigned int stepsCompleted, unsigned int totalSteps){
	emit progress(msg, timeStep, stepsCompleted, totalSteps);
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Sets the thread into the error state */
void AbstractAnalysisTimeStepThread::setError(const QString& message){
	error = true;
	errorMessage = message;

	//exit this thread
	stopThread();
}

