//SpikeStream includes
#include "Globals.h"
#include "IzhiAccuracyManager.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>


/*! Constructor */
IzhiAccuracyManager::IzhiAccuracyManager() : SpikeStreamThread(){
}


/*! Destructor */
IzhiAccuracyManager::~IzhiAccuracyManager(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

//Inherited from QThread
void IzhiAccuracyManager::run(){
	clearError();
	stopThread = false;
	unsigned origWaitInterval = nemoWrapper->getWaitInterval_ms();
	nemoWrapper->setWaitInterval(0);//Minimal wait between steps

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
void IzhiAccuracyManager::startExperiment(NemoWrapper* nemoWrapper, QHash<QString, double>& parameterMap){
	this->nemoWrapper = nemoWrapper;
	storeNeuronGroups();
	storeParameters(parameterMap);
	start();
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Runs the experiment. */
void IzhiAccuracyManager::runExperiment(){
	//Seed the random number generator
	Util::seedRandom(randomSeed);

	//Train network on numPatterns patterns
	emit statusUpdate("Starting Experiment " + QString::number(experimentNumber + 1));

	emit statusUpdate("First second with monitoring unchanged.");
	stepNemo(1000);

	emit statusUpdate("99 seconds without monitoring.");
	nemoWrapper->setMonitor(false);
	int origPauseInterval = pauseInterval_ms;
	pauseInterval_ms = 0;
	stepNemo(100000);
	pauseInterval_ms = origPauseInterval;

	emit statusUpdate("1 second with monitoring.");
	nemoWrapper->setMonitor(true);
	stepNemo(1000);

	//Output final result
	emit statusUpdate("Experiment complete.");
}


/*! Advances the simulation by the specified number of time steps */
void IzhiAccuracyManager::stepNemo(unsigned numTimeSteps){
	for(unsigned i=0; i<numTimeSteps && !stopThread; ++i){
		//Inject current into a randomly selected neuron
		//setInjectCurrentNeuronIDs(QList<neurid_t>& neurIDList, double current)


		//Step simulation
		nemoWrapper->stepSimulation();
		while((nemoWrapper->isWaitForGraphics() || nemoWrapper->getCurrentTask() == NemoWrapper::STEP_SIMULATION_TASK) && !stopThread)
			msleep(pauseInterval_ms);
	}
	msleep(pauseInterval_ms);
}


/*! Stores pointers to the excitatory and inhibitory neuron groups */
void IzhiAccuracyManager::storeNeuronGroups(){
	QList<NeuronGroup*> neurGrpList = Globals::getNetwork()->getNeuronGroups();

	//Get pointers to excitatory and inhibitory neuron groups
	excitatoryNeuronGroup = NULL;
	inhibitoryNeuronGroup = NULL;
	foreach(NeuronGroup* tmpNeurGrp, neurGrpList){
		if(tmpNeurGrp->getInfo().getName().toUpper() == "EXCITATORY NEURON GROUP"){
			excitatoryNeuronGroup = tmpNeurGrp;
		}
		if(tmpNeurGrp->getInfo().getName().toUpper() == "INHIBITORY NEURON GROUP"){
			inhibitoryNeuronGroup = tmpNeurGrp;
		}
		if(excitatoryNeuronGroup != NULL && inhibitoryNeuronGroup != NULL)
			break;
	}
}


/*! Stores the parameters for the experiment */
void IzhiAccuracyManager::storeParameters(QHash<QString, double> &parameterMap){
	if(!parameterMap.contains("experiment_number"))
		throw SpikeStreamException("IzhiAccuracyManager: experiment_number parameter missing");
	experimentNumber = (int)parameterMap["experiment_number"];

	if(!parameterMap.contains("random_seed"))
		throw SpikeStreamException("IzhiAccuracyManager: random_seed parameter missing");
	randomSeed = (int)parameterMap["random_seed"];

	if(!parameterMap.contains("pause_interval_ms"))
		throw SpikeStreamException("IzhiAccuracyManager: pause_interval_ms parameter missing");
	pauseInterval_ms = (int)parameterMap["pause_interval_ms"];
}




