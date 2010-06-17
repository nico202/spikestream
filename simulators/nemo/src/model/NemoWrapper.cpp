//SpikeStream includes
#include "Globals.h"
#include "NemoLoader.h"
#include "NemoWrapper.h"
#include "SpikeStreamException.h"
#include "SpikeStreamSimulationException.h"
using namespace spikestream;


//Default parameter values
#define DEFAULT_LOGGING 0
#define DEFAULT_CUDA_PARTITION_SIZE 0
#define DEFAULT_CUDA_FIRING_BUFFER_LENGTH 0
#define DEFAULT_CUDA_DEVICE 0
#define DEFAULT_STDP_FUNCTION 0


/*! Constructor */
NemoWrapper::NemoWrapper(){
	//Initialise variables
	currentTaskID = NO_TASK_DEFINED;
	simulationLoaded = false;
	stopThread = true;
	archiveMode = false;
	monitorMode = false;
	updateInterval_ms = 500;

	//Construct the information about the parameters and their default values
	buildParameters();
}


/*! Destructor */
NemoWrapper::~NemoWrapper(){
	if(simulationLoaded)
		unloadSimulation();

}


void testFunction(){
	//Set up the configuration with the default parameters
	nemo_configuration_t nemoConfig = nemo_new_configuration();
qDebug()<<"PROBE1";
	//Add neurons
	nemo_network_t nemoNet = nemo_new_network();

	nemo_status_t result = nemo_add_neuron(nemoNet, 1, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7);
	if(result != NEMO_OK)
		throw SpikeStreamException("Error code returned from Nemo when adding neuron." + QString(nemo_strerror()));

	result = nemo_add_neuron(nemoNet, 2, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7);
	if(result != NEMO_OK)
		throw SpikeStreamException("Error code returned from Nemo when adding neuron." + QString(nemo_strerror()));

	result = nemo_add_neuron(nemoNet, 3, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7);
	if(result != NEMO_OK)
		throw SpikeStreamException("Error code returned from Nemo when adding neuron." + QString(nemo_strerror()));

	result = nemo_add_neuron(nemoNet, 4, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7);
	if(result != NEMO_OK)
		throw SpikeStreamException("Error code returned from Nemo when adding neuron." + QString(nemo_strerror()));
qDebug()<<"PROBE2";
	//Load the network into the simulator
	nemo_simulation_t nemoSim ;
	try{
		nemoSim = nemo_new_simulation(nemoNet, nemoConfig);
	}
	catch(...){
		qDebug()<<"ERROR LOADING NEMO SIMULATION."<<endl;
	}

	qDebug()<<"PROBE2A"<<nemoSim;
	if(nemoSim == NULL) {
		throw SpikeStreamSimulationException(QString("Failed to create Nemo simulation: ") + nemo_strerror());
	}
	qDebug()<<"PROBE3";
}



/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Prepares the wrapper for the loading task */
void NemoWrapper::prepareLoadSimulation(){
	if(!Globals::networkLoaded())
		throw SpikeStreamException("Cannot load simulation - no network loaded.");
	if(simulationLoaded)
		throw SpikeStreamException("Simulation is already loaded - you must unload the current simulation before loading another.");

	//Set the task to load
	currentTaskID = LOAD_SIMULATION_TASK;
}


/*! Prepares the wrapper for the playing task */
void NemoWrapper::prepareRunSimulation(){
	if(!simulationLoaded)
		throw SpikeStreamException("Cannot run simulation - no simulation loaded.");
	currentTaskID = RUN_SIMULATION_TASK;
}


/*! Run method inherited from QThread */
void NemoWrapper::run(){
	stopThread = false;
	clearError();

	try{
		//Create thread specific network and arhive daos
		networkDao = new NetworkDao(Globals::getNetworkDao()->getDBInfo());
		archiveDao = new ArchiveDao(Globals::getArchiveDao()->getDBInfo());

		//Execute the appropriate task
		switch(currentTaskID){
			case LOAD_SIMULATION_TASK:
				loadSimulation();
			break;
			case RUN_SIMULATION_TASK:
				Globals::setSimulationRunning(true);
				runSimulation();
				Globals::setSimulationRunning(false);
			break;
			default:
				throw SpikeStreamException("Task ID not recognized.");
		}

		//Clean up the thread specific network and archive daos
		delete networkDao;
		delete archiveDao;
	}
	catch(SpikeStreamException& ex){
		setError(ex.getMessage());
	}
	catch(...){
		setError("An unknown error occurred while NemoWrapper thread was running.");
	}

	stopThread = true;
}


/*! Sets the number of frames per second.
	This is converted into the update interval.
	If the frame rate is 0 the simulation runs at maximum speed. */
void NemoWrapper::setFrameRate(unsigned int frameRate){
	QMutexLocker locker(&mutex);
	if(frameRate == 0)
		this->updateInterval_ms = 0;
	else
		this->updateInterval_ms = 1000 / frameRate;
}


/*! Sets the archive mode.
	An archive is created the first time this method is called after the simulation has loaded. */
void NemoWrapper::setArchiveMode(bool mode){
	if(mode && !simulationLoaded)
		throw SpikeStreamSimulationException("Cannot switch archive mode on unless simulation is loaded.");

	/* Create archive if this is the first time the mode has been set
		Use globals archive dao because this method is called from a separate thread */
	if(archiveInfo.getID() == 0){
		Globals::getArchiveDao()->addArchive(archiveInfo);
		Globals::getEventRouter()->archiveListChangedSlot();
	}

	this->archiveMode = mode;
}


/*! Sets the monitor mode, which controls whether data is extracted from the simulation at each time step */
void NemoWrapper::setMonitorMode(bool mode){
	 this->monitorMode = mode;
}


/*! Sets the parameters. Checks that each one is correct and throws an exception if an unexpected
	parameter is present or a parameter is missing */
void NemoWrapper::setParameters(const QHash<QString, double>& parameterMap){
	//Check that the correct number of parameters is being set
	if(parameterMap.size() != parameterInfoList.size())
		throw SpikeStreamException("Parameter info list does not match size of parameter map");

	//Check all parameters are present
	foreach(ParameterInfo paramInfo, parameterInfoList){
		if(!parameterMap.contains(paramInfo.getName()))
			throw SpikeStreamException("Parameter " + paramInfo.getName() + " is missing from parameter map.");
	}

	//Copy parameters into map
	this->parameterMap = parameterMap;
}


/*! Advances the simulation by one step */
void NemoWrapper::stepSimulation(){
	firingNeuronList.clear();

	//Advance the simulation one time step
	checkNemoOutput( nemo_step(nemoSimulation, 0, 0), "Nemo error on step" );

	//Retrieve list of firing neurons
	if(archiveMode || monitorMode){
		unsigned** cycles;
		unsigned** nidx;
		unsigned* nfired;
		unsigned* ncycles;
		checkNemoOutput( nemo_read_firing(nemoSimulation, cycles, nidx, nfired, ncycles), "Nemo error reading firing neurons" );

		qDebug()<<"NUMBER FIRED="<<nfired<<" NUM CYCLES="<<ncycles;

		checkNemoOutput( nemo_flush_firing_buffer(nemoSimulation), "Nemo error flushing firing buffer" );

	}
	//Retrieve list of firing neurons
//	if(monitorMode || archiveMode)
//		getRandomFiringNeurons(firingNeuronList, 50);

	//Store firing neurons in database
	if(archiveMode){
		;//FIXME: archiveDao->addArchiveData(archiveInfo.getID(), timeStepCounter, firingNeuronList);
	}

	/* Set flag to cause thread to wait for graphics to update.
		This is needed even if we are just running a time step counter */
	waitForGraphics = true;

	//Update time step counter
	++timeStepCounter;
	emit timeStepChanged(timeStepCounter, firingNeuronList);
}


/*! Stops the thread running */
void NemoWrapper::stop(){
	stopThread = true;
}


/*! Unloads the current simulation */
void NemoWrapper::unloadSimulation(){
//	if(nemoSimulation != NULL){
//		delete nemoSimulation;
//		nemoSimulation = NULL;
//		//FIXME: DELETE NETWORK??
//	}
	simulationLoaded = false;
	Globals::setSimulationLoaded(false);
}


/*----------------------------------------------------------*/
/*-----                  PRIVATE SLOTS                 -----*/
/*----------------------------------------------------------*/

/*! Called by other classes being executed by this class to inform this
	class about progress with an operation */
void NemoWrapper::updateProgress(int stepsComplete, int totalSteps){
	emit progress(stepsComplete, totalSteps);
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Constructs the set of parameters associated with Nemo and sets their default values */
void NemoWrapper::buildParameters(){
	parameterMap.clear();
	defaultParameterMap.clear();
	parameterInfoList.clear();

	//Add simple parameters
	parameterInfoList.append(ParameterInfo("Logging", "Enable or disable logging.", ParameterInfo::BOOLEAN));
	parameterInfoList.append(ParameterInfo("CudaPartitionSize", "Size of the CUDA partition.", ParameterInfo::UNSIGNED_INTEGER));
	parameterInfoList.append(ParameterInfo("CudaFiringBufferLength", "Set the size of the firing buffer such that it can contain a \nfixed number of cycles worth of firing data before overflowing.", ParameterInfo::UNSIGNED_INTEGER));
	parameterInfoList.append(ParameterInfo("CudaDevice", "Set the cuda device to \a dev. The CUDA library allows the device to be set only once per thread, so this function may fail if called multiple times.", ParameterInfo::INTEGER));

	//Add list of STDP functions
	QList<QString> stdpNames;
	stdpNames.append("Standard STDP function");
	ParameterInfo tmpInfo("STDPFunction", "The type of STDP function. These are pre-defined in the Nemo Wrapper.", ParameterInfo::OPTION);
	tmpInfo.setOptionNames(stdpNames);
	parameterInfoList.append(tmpInfo);

	//Set the default parameters and initialize parameters to default values
	defaultParameterMap["Logging"] = DEFAULT_LOGGING;
	parameterMap["Logging"] = defaultParameterMap["Logging"];

	defaultParameterMap["CudaPartitionSize"] = DEFAULT_CUDA_PARTITION_SIZE;
	parameterMap["CudaPartitionSize"] = defaultParameterMap["CudaPartitionSize"];

	defaultParameterMap["CudaFiringBufferLength"] = DEFAULT_CUDA_FIRING_BUFFER_LENGTH;
	parameterMap["CudaFiringBufferLength"] = defaultParameterMap["CudaFiringBufferLength"];

	defaultParameterMap["CudaDevice"] = DEFAULT_CUDA_DEVICE;
	parameterMap["CudaDevice"] = defaultParameterMap["CudaDevice"];

	defaultParameterMap["STDPFunction"] = DEFAULT_STDP_FUNCTION;
	parameterMap["STDPFunction"] = defaultParameterMap["STDPFunction"];
}


/*! Checks the output from a nemo function call and throws exception if there is an error */
void NemoWrapper::checkNemoOutput(nemo_status_t result, const QString& message){
	if(result != NEMO_OK)
		throw SpikeStreamException(message + ": " + nemo_strerror());
}


/*! Clears the error state */
void NemoWrapper::clearError(){
	error = false;
	errorMessage = "";
}


/*! Loads the simulation into the CUDA hardware */
void NemoWrapper::loadSimulation(){
	simulationLoaded = false;
	nemoSimulation = NULL;
	timeStepCounter = 0;
	waitForGraphics = false;
	archiveMode = false;
	monitorMode = false;

	//Get the network
	if(!Globals::networkLoaded())
		throw SpikeStreamSimulationException("Cannot load simulation: no network loaded.");
	Network* currentNetwork = Globals::getNetwork();

	//Create network and archive daos and set them in the network
	currentNetwork->setNetworkDao(networkDao);
	currentNetwork->setArchiveDao(archiveDao);

	//Set up the archive info
	archiveInfo.reset();
	archiveInfo.setNetworkID(currentNetwork->getID());

	//Set up the configuration with the appropriate parameters
//	nemo_configuration_t nemoConfig = nemo_new_configuration();
//
//	//Build the Nemo network
//	NemoLoader* nemoLoader = new NemoLoader();
//	connect(nemoLoader, SIGNAL(progress(int, int)), this, SLOT(updateProgress(int, int)));
//	nemo_network_t nemoNet = nemoLoader->buildNemoNetwork(currentNetwork, &stopThread);

	testFunction();


	//START TEST CODE - GET MINIMUM NEURON ID AND NETWORK SIZE
//	bool firstTime = false;
//	foreach(NeuronGroup* neurGrp, currentNetwork->getNeuronGroups()){
//		if(firstTime){
//			startNeuronID = neurGrp->getStartNeuronID();
//			firstTime = false;
//		}
//		else if(neurGrp->getStartNeuronID() < startNeuronID)
//			startNeuronID = neurGrp->getStartNeuronID();
//	}
//	networkSize = currentNetwork->size();
	//END TEST CODE


	//Reset the daos in the SpikeStream network
	currentNetwork->setNetworkDao(Globals::getNetworkDao());
	currentNetwork->setArchiveDao(Globals::getArchiveDao());

	//Clean up
//	delete nemoLoader;


	//TEST CODE
	//Initialize the nemo network
//	nemo_network_t nemoNet = nemo_new_network();
//
//	nemo_status_t result = nemo_add_neuron(nemoNet, 1, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7);
//	if(result != NEMO_OK)
//		throw SpikeStreamException("Error code returned from Nemo when adding neuron." + QString(nemo_strerror()));
//
//	result = nemo_add_neuron(nemoNet, 2, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7);
//	if(result != NEMO_OK)
//		throw SpikeStreamException("Error code returned from Nemo when adding neuron." + QString(nemo_strerror()));
//
//	result = nemo_add_neuron(nemoNet, 3, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7);
//	if(result != NEMO_OK)
//		throw SpikeStreamException("Error code returned from Nemo when adding neuron." + QString(nemo_strerror()));
//
//	result = nemo_add_neuron(nemoNet, 4, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7);
//	if(result != NEMO_OK)
//		throw SpikeStreamException("Error code returned from Nemo when adding neuron." + QString(nemo_strerror()));
//	//END TEST CODE
//



	//Load the network into the simulator
//	nemoSimulation = nemo_new_simulation(nemoNet, nemoConfig);
//	if(nemoSimulation == NULL) {
//		throw SpikeStreamSimulationException(QString("Failed to create Nemo simulation: ") + nemo_strerror());
//	}

	if(!stopThread)
		simulationLoaded = true;
}


/*! Plays the current simulation */
void NemoWrapper::runSimulation(){
	//Check simulation is loaded
	if(!simulationLoaded)
		throw SpikeStreamSimulationException("Cannot run simulation - no simulation loaded.");

	//Declare variables to use in the loop
	QTime startTime;
	unsigned int elapsedTime_ms;

	while(!stopThread){
		//Record the current time
		startTime = QTime::currentTime();

		//Advance simulation one step
		stepSimulation();

		//Only display one time step in step mode
//		if (stepMode){
//			stopThread = true;
//			stepMode = false;
//		}
//		//Sleep until the next time step
//		else {
			//Lock mutex so that update time interval cannot change during this calculation
			mutex.lock();

			//Sleep if task was completed in less than the prescribed interval
			elapsedTime_ms = startTime.msecsTo(QTime::currentTime());
			if(elapsedTime_ms < updateInterval_ms){
				//Sleep for remaning time
				usleep(1000 * (updateInterval_ms - elapsedTime_ms));
			}

			//Unlock mutex
			mutex.unlock();
//		}

		//Wait for graphics to update if we are monitoring the simulation
		while(!stopThread && waitForGraphics)
			usleep(1000);
	}
}


/*! Puts class into error state */
void NemoWrapper::setError(const QString& errorMessage){
	error = true;
	this->errorMessage = errorMessage;
	stopThread = true;
}



//TEST METHOD TO REPLACE NEMO
//void NemoWrapper::getRandomFiringNeurons(QList<unsigned int> &neurIDList, unsigned  percent){
//	unsigned numNeur = (unsigned)rint((percent / 100.0) * networkSize);
//	neurIDList.clear();
//	for(unsigned i=0; i<numNeur; ++i){
//		neurIDList.append(startNeuronID + rand() % networkSize);
//	}
//}



