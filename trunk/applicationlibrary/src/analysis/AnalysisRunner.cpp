#include "AnalysisRunner.h"
#include "Globals.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QMutexLocker>

//Other includes
#include <iostream>
using namespace std;


/*! Constructor */
AnalysisRunner::AnalysisRunner(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo){
	//Store information about databases
	this->networkDBInfo = netDBInfo;
	this->archiveDBInfo = archDBInfo;
	this->analysisDBInfo = anaDBInfo;

	//Initialize variables
	stopThread = true;
}


/*! Destructor */
AnalysisRunner::~AnalysisRunner(){
}


/*-------------------------------------------------------------*/
/*-------                 PUBLIC METHODS                 ------*/
/*-------------------------------------------------------------*/

/*! Sets up the class ready to carry out the analysis */
void AnalysisRunner::prepareAnalysisTask(const AnalysisInfo& analysisInfo, int firstTimeStep, int lastTimeStep){
	if(subThreadsRunning())
		throw SpikeStreamException("Cannot prepare analysis task when sub threads are running.");
	if(stopThread == false)
		throw SpikeStreamException("Cannot prepare analysis task when analysis thread is running.");

	//Reset class
	this->reset();

	//Check variables
	if(analysisInfo.getID() == 0)
		throw SpikeStreamException("Analysis ID has not been set.");
	if(firstTimeStep > lastTimeStep)
		throw SpikeStreamException("First time step is greater than the last time step.");

	//Store variables
	this->analysisInfo = analysisInfo;
	this->firstTimeStep = firstTimeStep;
	this->lastTimeStep = lastTimeStep;
	nextTimeStep = firstTimeStep;
}


/*! Run method inherited from QThread.
	A parameter controls how many threads are launched at once.
	Each new thread is connected to appropriate signals and when it finishes a new thread is launched
	until all of the time steps have been analyzed. */
void AnalysisRunner::run(){
	stopThread = false;

	//Launch the first batch of threads
	unsigned int threadCount = 0;
	while(threadCount < analysisInfo.getNumberOfThreads()){
		int tmpTimeStep = getNextTimeStep();

		//No more time steps, break out of the launching loop
		if(tmpTimeStep == -1)
			break;

		//Start the thread running to analyze this time step
		try{
			startAnalysisTimeStepThread(tmpTimeStep);
			++threadCount;
		}
		catch(SpikeStreamException& ex){
			setError(ex.getMessage());
		}
	}

	/* Wait for the analysis to complete
	When all of the sub threads have finished, stopThread should be set to true. */
	while(!stopThread){
		usleep(500000);//Sleep for half a second
	}
}


/*! Stops the thread running */
void AnalysisRunner::stop(){
	//Halt all of the other threads and wait for them to finish
	for(QHash<int, AbstractAnalysisTimeStepThread*>::iterator iter = subThreadMap.begin(); iter != subThreadMap.end(); ++iter){
		iter.value()->stopThread();
		iter.value()->wait();
		delete iter.value();
	}

	subThreadMap.clear();
	stopThread = true;
}


/*! Sets the function used to create new time step threads */
void AnalysisRunner::setTimeStepThreadCreationFunction(AbstractAnalysisTimeStepThread* (*createAnalysisTimeStepThread)(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo)){
	this->createAnalysisTimeStepThread = createAnalysisTimeStepThread;
}


/*-------------------------------------------------------------*/
/*-------                  PRIVATE SLOTS                 ------*/
/*-------------------------------------------------------------*/

/*! Called when one of the sub threads emits a progress signal */
void AnalysisRunner::updateProgress(const QString& msg, unsigned int timeStep, unsigned int stepsCompleted, unsigned int totalSteps){
	emit progress(msg, timeStep, stepsCompleted, totalSteps);
}


/*! Called when one of the sub threads finishes.
	Another thread is launched if time steps sill need to be analyzed. */
void AnalysisRunner::threadFinished(){
	//Do nothing if thread is trying to stop
	if(stopThread)
		return;

	/* Multiple sub threads could call this method simultaneously and mess things up
	Lock mutex to make sure we only process one thread finished event at a time. */
	QMutexLocker(&this->mutex);

	//Get a reference to the thread that has stopped
	AbstractAnalysisTimeStepThread* tmpSubThread = (AbstractAnalysisTimeStepThread*) sender();

	//Remove it from the map of currently running threads
	int tmpTimeStep = tmpSubThread->getTimeStep();
	if(!subThreadMap.contains(tmpTimeStep)){
		setError("Thread with time step " + QString::number(tmpTimeStep) + " is not recorded as running.");
		return;
	}

	//Check for errors - set error will delete all of the threads
	if(tmpSubThread->isError()){
		setError(tmpSubThread->getErrorMessage());
		return;
	}

	//Delete the thread class and remove it from the map
	delete tmpSubThread;
	subThreadMap.remove(tmpTimeStep);

	//Inform listening classes that the analysis of this time step is now complete.
	emit timeStepComplete(tmpTimeStep);

	//Determine if any time steps need to be analyzed
	int tmpNextTimeStep = getNextTimeStep();
	if(tmpNextTimeStep >= 0){
		try{
			startAnalysisTimeStepThread(tmpNextTimeStep);
		}
		catch(SpikeStreamException& ex){
			setError(ex.getMessage());
		}
	}

	//Next time step is -1. All of the time step threads have been launched and may have all finished
	else if(!subThreadsRunning()){//See if any threads are still running
		//Stop this thread when no sub threads are running
		stop();
	}
}


/*! Emits signal to update results */
void AnalysisRunner::updateResults(){
	emit newResultsFound();
}


/*-------------------------------------------------------------*/
/*-------                PRIVATE METHODS                 ------*/
/*-------------------------------------------------------------*/

/*! Clears the error state */
void AnalysisRunner::clearError(){
	error = false;
	errorMessage = "";
}


/*! Returns the next time step to be analyzed or -1 if no time steps remain
	to be analyzed. */
int AnalysisRunner::getNextTimeStep(){
	if(nextTimeStep == -1)
		return -1;

	//Store current value of next time step. This is what will be returned
	int oldNextTimeStep = nextTimeStep;

	//Increase the next time step and check to see if it has exceeded the range
	++nextTimeStep;
	if(nextTimeStep > lastTimeStep)
		nextTimeStep = -1;//No more time steps left. -1 will be returned next time this method is called

	//Return the original value of the next time step before it was increased or invalidated
	return oldNextTimeStep;
}


/*! Resets this class ready for another analysis. All information should be cleared. */
void AnalysisRunner::reset(){
	subThreadMap.clear();
	stopThread = true;
	clearError();
	firstTimeStep = -1;
	lastTimeStep = -1;
	nextTimeStep = -1;
}


/*! Sets the thread into the error state */
void AnalysisRunner::setError(const QString& message){
	error = true;
	errorMessage = message;

	//exit this thread
	stop();
}


/*! Starts a thread to analyze a particular time step */
void AnalysisRunner::startAnalysisTimeStepThread(int timeStep){
	//Run some checks on the time step
	if(timeStep == -1){
		throw SpikeStreamException("Trying to start a new thread with an invalid time step.");
	}
	if(subThreadMap.contains(timeStep)){
		throw SpikeStreamException("Trying to launch new thread to analyze time step " + QString::number(timeStep) + " which is already running.");
	}

	//Lanch the new thread and store its address in the map
	AbstractAnalysisTimeStepThread* newThread = createAnalysisTimeStepThread(networkDBInfo, archiveDBInfo, analysisDBInfo);
	connect(newThread, SIGNAL(newResultsFound()), this, SLOT(updateResults()));
	connect(newThread, SIGNAL(finished()), this, SLOT(threadFinished()));
	connect(newThread, SIGNAL(progress(const QString&, unsigned int, unsigned int, unsigned int)), this, SLOT(updateProgress(const QString&, unsigned int, unsigned int, unsigned int)));
	newThread->prepareTimeStepAnalysis(analysisInfo, timeStep);
	subThreadMap[timeStep] = newThread;
	newThread->start();
}


/*! Returns true if threads are running carrying out the analysis of time steps. */
bool AnalysisRunner::subThreadsRunning(){
	if(subThreadMap.isEmpty())
		return false;
	return true;
}

