//SpikeStream includes
#include "TemporalCodingExptManager.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>


/*! Constructor */
TemporalCodingExptManager::TemporalCodingExptManager() : SpikeStreamThread(){
}


/*! Destructor */
TemporalCodingExptManager::~TemporalCodingExptManager(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

//Inherited from QThread
void TemporalCodingExptManager::run(){
	clearError();
	stopThread = false;
	unsigned origWaitInterval = nemoWrapper->getWaitInterval_ms();
	nemoWrapper->setWaitInterval(1);//Minimal wait between steps

	try{
		runExperiment();
	}
	catch(SpikeStreamException& ex){
		setError(ex.getMessage());
	}
	catch(...){
		setError("Pop1ExperimentManager has thrown an unknown exception.");
	}

	nemoWrapper->setWaitInterval(origWaitInterval);//Restore wrapper to original state
	experimentNumber = NO_EXPERIMENT;
	stopThread = true;
}


/*! Sets up the experiment and starts thread running. */
void TemporalCodingExptManager::startExperiment(NemoWrapper* nemoWrapper, QHash<QString, double>& parameterMap){
	this->nemoWrapper = nemoWrapper;
	storeParameters(parameterMap);
	start();
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Runs the experiment. */
void TemporalCodingExptManager::runExperiment(){
	//Seed the random number generator
	Util::seedRandom(randomSeed);

	//Train network on numPatterns patterns
	emit statusUpdate("Starting Experiment " + QString::number(experimentNumber + 1));

	stepNemo(10);

	//Output final result
	emit statusUpdate("Experiment complete.");
}


/*! Advances the simulation by the specified number of time steps */
void TemporalCodingExptManager::stepNemo(unsigned numTimeSteps){
	for(unsigned i=0; i<numTimeSteps && !stopThread; ++i){
		nemoWrapper->stepSimulation();
		while((nemoWrapper->isWaitForGraphics() || nemoWrapper->getCurrentTask() == NemoWrapper::STEP_SIMULATION_TASK) && !stopThread)
			msleep(pauseInterval_ms);
	}
	msleep(pauseInterval_ms);
}


/*! Stores the parameters for the experiment */
void TemporalCodingExptManager::storeParameters(QHash<QString, double> &parameterMap){
	if(!parameterMap.contains("experiment_number"))
		throw SpikeStreamException("TemporalCodingExptManager: experiment_number parameter missing");
	experimentNumber = (int)parameterMap["experiment_number"];

	if(!parameterMap.contains("random_seed"))
		throw SpikeStreamException("TemporalCodingExptManager: random_seed parameter missing");
	randomSeed = (int)parameterMap["random_seed"];

	if(!parameterMap.contains("pause_interval_ms"))
		throw SpikeStreamException("TemporalCodingExptManager: pause_interval_ms parameter missing");
	pauseInterval_ms = (int)parameterMap["pause_interval_ms"];
}




