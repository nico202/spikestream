//SpikeStream includes
#include "Globals.h"
#include "IzhiAccuracyManager.h"
#include "SpikeStreamIOException.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QFile>

/*! Izhikevich's random function */
#define getrandom(max1) ((rand()%(int)((max1))))

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
	timeStep = 0;

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
	emit statusUpdate("Starting Experiment " + QString::number(experimentNumber + 1));

	QTextStream* logTextStream = NULL;
	QFile* logFile = new QFile(Globals::getSpikeStreamRoot() + "/log/nemo_spikes_1.dat");
	if(logFile->open(QFile::WriteOnly | QFile::Truncate))
		logTextStream = new QTextStream(logFile);
	else{
		throw SpikeStreamIOException("Cannot open log file for NemoLoader.");
	}

	emit statusUpdate("First 5 seconds with monitoring unchanged.");
	stepNemo(5000, true, logTextStream);

	if(!stopThread){
		emit statusUpdate("95 seconds without monitoring.");
		nemoWrapper->setMonitor(false);
		int origPauseInterval = pauseInterval_ms;
		pauseInterval_ms = 0;
		for(int i=1; i<=19;++i){
			stepNemo(5000, true);
			if(i%2 == 0)
				emit statusUpdate(QString::number(i*5000) + " steps complete without monitoring.");
		}
		pauseInterval_ms = origPauseInterval;
	}

	if(!stopThread){
		emit statusUpdate("5 seconds with monitoring.");
		nemoWrapper->setMonitor(true);
		stepNemo(5000, true);
	}

	//Clean up
	logFile->close();
	delete logTextStream;

	//Output final result
	emit statusUpdate("Experiment complete.");
}


/*! Advances the simulation by the specified number of time steps */
void IzhiAccuracyManager::stepNemo(unsigned numTimeSteps, bool injectCurrent, QTextStream* textStream){
	for(unsigned i=0; i<numTimeSteps && !stopThread; ++i){
		if(injectCurrent){
			//Inject current into a randomly selected neuron
			QList<neurid_t> injectCurrentIDs;
			injectCurrentIDs.append(neuronIDList.at(getrandom(1000)));
			nemoWrapper->setInjectCurrentNeuronIDs(injectCurrentIDs, 20.0);
		}

		//Step simulation
		nemoWrapper->stepSimulation();

		//Wait for NeMo and SpikeStream to finish
		while((nemoWrapper->isWaitForGraphics() || nemoWrapper->getCurrentTask() == NemoWrapper::STEP_SIMULATION_TASK) && !stopThread)
			msleep(pauseInterval_ms);

		//Output firing neuron ids
		if(timeStep < 1000 && textStream != NULL){
			QList<neurid_t> tmpFiringIDs = nemoWrapper->getFiringNeuronIDs();
			qDebug()<<"TIME STEP: "<<timeStep<<"; NUMBER OF FIRING NEURONS: "<<tmpFiringIDs.size();
			for(int i=0; i<tmpFiringIDs.size(); ++i)
				(*textStream)<<timeStep<<" "<<tmpFiringIDs.at(i)<<endl;
		}

		//Increase time step
		++timeStep;

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

	//Store list of neuron IDs
	neuronIDList.clear();
	NeuronIterator excitatoryNeuronGroupEnd = excitatoryNeuronGroup->end();
	for(NeuronIterator iter = excitatoryNeuronGroup->begin(); iter != excitatoryNeuronGroupEnd; ++iter){
		neuronIDList.append(iter.key());
	}
	NeuronIterator inhibitoryNeuronGroupEnd = inhibitoryNeuronGroup->end();
	for(NeuronIterator iter = inhibitoryNeuronGroup->begin(); iter != inhibitoryNeuronGroupEnd; ++iter){
		neuronIDList.append(iter.key());
	}

	//Run a check
	if(neuronIDList.size() != 1000)
		throw SpikeStreamException("There should be a total of 1000 neurons in the network. Actual number of neurons: " + QString::number(neuronIDList.size()));
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




