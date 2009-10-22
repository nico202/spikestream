//SpikeStream includes
#include "AnalysisTimeStepThread.h"
#include "SpikeStreamException.h"
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
    threadRunning = true;
    clearError();

    try{
	//Create the data access objects
	networkDao = new NetworkDao(networkDBInfo);
	archiveDao = new ArchiveDao(archiveDBInfo);
	analysisDao = new StateBasedPhiAnalysisDao(analysisDBInfo);

	Util::seedRandom(timeStep * 100);
	int numberOfSteps = Util::getRandom(0, 50);

	for(int i=0; i<numberOfSteps; ++i){
	    usleep(1000000 * timeStep);//Sleep for 100 milliseconds
	    QList<unsigned int> tmpNeuronList;
	    for(int j=0; j< 10; ++j){
		tmpNeuronList.append(Util::getRandom(0, 1000));
	    }
	    analysisDao->addComplex(analysisInfo.getID(), timeStep, 0.66, tmpNeuronList);
	    emit complexFound();
	    emit progress(timeStep, i, numberOfSteps);
	    if(!threadRunning)
		return;
	}
    }
    catch(SpikeStreamException& ex){
	setError(ex.getMessage());
    }

    threadRunning = false;
}


/*! Sets the thread into the error state */
void AnalysisTimeStepThread::setError(const QString& message){
    error = true;
    errorMessage = message;

    //exit this thread
    stop();
}


/*! Stops the analysis */
void AnalysisTimeStepThread::stop(){
    threadRunning = false;
}
