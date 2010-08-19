//SpikeStream includes
#include "Globals.h"
#include "NemoLoader.h"
#include "NemoWrapper.h"
#include "SpikeStreamException.h"
#include "SpikeStreamSimulationException.h"
#include "STDPFunctions.h"
#include "Util.h"
using namespace spikestream;


/*! Constructor */
NemoWrapper::NemoWrapper(){
	//Initialise variables
	currentTaskID = NO_TASK_DEFINED;
	simulationLoaded = false;
	stopThread = true;
	archiveMode = false;
	monitorMode = false;
	updateInterval_ms = 500;

	//Zero is the default STDP function
	stdpFunctionID = 0;

	//Get a nemo configuration object initialized with the default values
	nemoConfig = nemo_new_configuration();
}


/*! Destructor */
NemoWrapper::~NemoWrapper(){
	if(simulationLoaded)
		unloadSimulation();
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Cancels the loading of a simulation */
void NemoWrapper::cancelLoading(){
	stopThread = true;
}


/*! Returns true if simulation is currently being played */
bool NemoWrapper::isSimulationRunning(){
	if(currentTaskID == RUN_SIMULATION_TASK || currentTaskID == STEP_SIMULATION_TASK)
		return true;
	return false;
}


/*! Loads the simulation into the CUDA hardware.
	This method should only be invoked in the thread within which NeMo is played.
	It has been made public for testing purposes. */
void NemoWrapper::loadNemo(){
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

	//Build the Nemo network
	NemoLoader* nemoLoader = new NemoLoader();
	connect(nemoLoader, SIGNAL(progress(int, int)), this, SLOT(updateProgress(int, int)));
	nemo_network_t nemoNet = nemoLoader->buildNemoNetwork(currentNetwork, &stopThread);

	//Reset the daos in the SpikeStream network
	currentNetwork->setNetworkDao(Globals::getNetworkDao());
	currentNetwork->setArchiveDao(Globals::getArchiveDao());

	//Clean up loader
	delete nemoLoader;

	//Set the STDP functionn in the configuration - FIXME
//	nemo_set_stdp_function(nemoConfig,
//					   STDPFunctions::getPre(stdpFunctionID),
//					   STDPFunctions::getPreLength(stdpFunctionID),
//					   STDPFunctions::getPost(stdpFunctionID),
//					   STDPFunctions::getPostLength(stdpFunctionID),
//					   STDPFunctions::getMinWeight(stdpFunctionID),
//					   STDPFunctions::getMaxWeight(stdpFunctionID)
//	);

	//Output summary of backend configuration
	const char* backendDesc;
	checkNemoOutput(nemo_backend_description(nemoConfig, &backendDesc), "Error getting backend description.");
	qDebug()<<"Creating simulation with configuration: "<<backendDesc;

	//Load the network into the simulator


	//LOGGING - DELETE LATER
	checkNemoOutput(nemo_log_stdout(nemoConfig), "Error setting logging.");
	int cudaDev;
	checkNemoOutput(nemo_cuda_device(nemoConfig, &cudaDev), "Error getting CUDA device from NeMo");
	qDebug()<<"CUDA Device="<<cudaDev;

	nemoSimulation = nemo_new_simulation(nemoNet, nemoConfig);
	if(nemoSimulation == NULL) {
		throw SpikeStreamSimulationException(QString("Failed to create Nemo simulation: ") + nemo_strerror());
	}

	qDebug()<<"Simulation created successfully";

	if(!stopThread)
		simulationLoaded = true;
}


/*! Plays the simulation */
void NemoWrapper::playSimulation(){
	if(!simulationLoaded)
		throw SpikeStreamException("Cannot run simulation - no simulation loaded.");
	currentTaskID = RUN_SIMULATION_TASK;
}


/*! Forces the specified percentage of neurons in the specified neuron group to fire at the
	next time step. Throws an exception if the neuron group ID does not exist in the current
	network or if the percentage is < 0 or > 100. */
void  NemoWrapper::setInjectNoise(unsigned neuronGroupID, double percentage){
	//Run checks
	if(!simulationLoaded)
		throw SpikeStreamException("Noise cannot be injected when a simulation is not loaded.");
	if(percentage < 0.0 || percentage > 100.0)
		throw SpikeStreamException("Injecting noise. Percentage is out of range");
	if(!Globals::getNetwork()->containsNeuronGroup(neuronGroupID))
		throw SpikeStreamException("Injecting noise. Neuron group ID cannot be found in current network: " + QString::number(neuronGroupID));

	//Calculate number of neurons to fire and store it in the map
	injectNoiseMap[neuronGroupID] = Util::rUInt( (percentage / 100.0) * (double)Globals::getNetwork()->getNeuronGroup(neuronGroupID)->size());
	if(injectNoiseMap[neuronGroupID] > (unsigned)Globals::getNetwork()->getNeuronGroup(neuronGroupID)->size())
		throw SpikeStreamException("Number of neurons to fire is greater than neuron group size: " + QString::number(injectNoiseMap[neuronGroupID]));
	qDebug()<<"NETWORK SIZE: "<<Globals::getNetwork()->getNeuronGroup(neuronGroupID)->size()<<"; NUM NEUR TO FIRE: "<<injectNoiseMap[neuronGroupID];
}


/*! Steps through a single time step */
void NemoWrapper::stepSimulation(){
	if(!simulationLoaded)
		throw SpikeStreamException("Cannot step simulation - no simulation loaded.");
	currentTaskID = STEP_SIMULATION_TASK;
}


// Run method inherited from QThread
void NemoWrapper::run(){
	stopThread = false;
	clearError();

	try{
		//Create thread specific network and archive daos
		networkDao = new NetworkDao(Globals::getNetworkDao()->getDBInfo());
		archiveDao = new ArchiveDao(Globals::getArchiveDao()->getDBInfo());

		//Load up the simulation and reset the task ID
		loadNemo();
		currentTaskID = NO_TASK_DEFINED;

		//Wait for run or step command
		while(!stopThread){
			//Run simulation
			if(currentTaskID == RUN_SIMULATION_TASK){
				runNemo();
			}
			//Step simulation
			else if(currentTaskID == STEP_SIMULATION_TASK){
				stepNemo();
				currentTaskID = NO_TASK_DEFINED;//Don't want it to continue stepping.
			}
			//Do nothing
			else if(currentTaskID == NO_TASK_DEFINED){
				;//Do nothing
			}
			//Task ID not recognized
			else{
				throw SpikeStreamException("Task ID not recognized.");
			}

			//Short sleep waiting for the next command
			msleep(200);
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

	unloadNemo();

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
void NemoWrapper::setArchiveMode(bool newArchiveMode){
	if(newArchiveMode && !simulationLoaded)
		throw SpikeStreamSimulationException("Cannot switch archive mode on unless simulation is loaded.");

	/* Create archive if this is the first time the mode has been set
		Use globals archive dao because this method is called from a separate thread */
	if(archiveInfo.getID() == 0){
		Globals::getArchiveDao()->addArchive(archiveInfo);
		Globals::getEventRouter()->archiveListChangedSlot();
	}

	//Flush buffer if archiving is being turned on when monitoring is off
	if(simulationLoaded && newArchiveMode && !archiveMode && !monitorMode){
		mutex.lock();//Prevent interference with step
		checkNemoOutput( nemo_flush_firing_buffer(nemoSimulation), "Nemo error flushing firing buffer" );
		mutex.unlock();
	}

	this->archiveMode = newArchiveMode;
}


/*! Sets the monitor mode, which controls whether data is extracted from the simulation at each time step */
void NemoWrapper::setMonitorMode(bool newMonitorMode){
	if(newMonitorMode && !simulationLoaded)
		throw SpikeStreamSimulationException("Cannot switch monitor mode on unless simulation is loaded.");

	//Flush buffer if monitoring is being turned on when archiving is off
	if(simulationLoaded && newMonitorMode && !monitorMode && !archiveMode){
		mutex.lock();//Prevent interference with step
		checkNemoOutput( nemo_flush_firing_buffer(nemoSimulation), "Nemo error flushing firing buffer" );
		mutex.unlock();
	}

	//Store new monitoring mode
	this->monitorMode = newMonitorMode;
}


/*! Stops the simulation playing without exiting the thread. */
void NemoWrapper::stopSimulation(){
	currentTaskID = NO_TASK_DEFINED;
}


/*! Unloads the current simulation and exits run method. */
void NemoWrapper::unloadSimulation(){
	stopThread = true;
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

/*! Checks the output from a nemo function call and throws exception if there is an error */
void NemoWrapper::checkNemoOutput(nemo_status_t result, const QString& errorMessage){
	if(result != NEMO_OK)
		throw SpikeStreamException(errorMessage + ": " + nemo_strerror());
}


/*! Clears the error state */
void NemoWrapper::clearError(){
	error = false;
	errorMessage = "";
}


/*! Fills the supplied array with a selection of neurons to force to fire at the next time step. */
void NemoWrapper::fillInjectNoiseArray(unsigned*& array, int* arraySize){
	//Calculate total number of firing neurons
	*arraySize = 0;
	for(QHash<unsigned, unsigned>::iterator iter = injectNoiseMap.begin(); iter != injectNoiseMap.end(); ++iter)
		*arraySize = *arraySize + iter.value();

	//Create array
	int arrayCounter = 0;
	array = new unsigned[*arraySize];

	//Fill array with a random selection of neuron ids from each group
	unsigned randomIndex, neurGrpSize, numSelectedNeurons;
	QHash<unsigned, bool> addedNeurIndxMap;//Prevent duplicates
	for(QHash<unsigned, unsigned>::iterator iter = injectNoiseMap.begin(); iter != injectNoiseMap.end(); ++iter){
		//Get list of neuron ids
		QList<unsigned> neuronIDList = Globals::getNetwork()->getNeuronGroup(iter.key())->getNeuronIDs();
		neurGrpSize = neuronIDList.size();
		addedNeurIndxMap.clear();
		numSelectedNeurons = iter.value();

		qDebug()<<"START NEURON ID: "<<Globals::getNetwork()->getNeuronGroup(iter.key())->getStartNeuronID();

		//Select indexes from the list of neuron ids
		while((unsigned)addedNeurIndxMap.size() < numSelectedNeurons){
			randomIndex = Util::getRandom(0, neurGrpSize);//Get random position in list of neuron ids
			if(!addedNeurIndxMap.contains(randomIndex)){//New index
				if(arrayCounter >= *arraySize)//Sanity check
					throw SpikeStreamException("Error adding noise injection neuron ids - array counter out of range.");
				array[arrayCounter] = neuronIDList.at(randomIndex);//Add neuron id to array
				addedNeurIndxMap[randomIndex] = true;//Record the fact that we have selected this ID
				++arrayCounter;
			}
		}
	}
}


/*! Plays the current simulation */
void NemoWrapper::runNemo(){
	//Check simulation is loaded
	if(!simulationLoaded)
		throw SpikeStreamSimulationException("Cannot run simulation - no simulation loaded.");

	//Declare variables to use in the loop
	QTime startTime;
	unsigned int elapsedTime_ms;

	while(currentTaskID == RUN_SIMULATION_TASK && !stopThread){
		//Record the current time
		startTime = QTime::currentTime();

		//Lock mutex so that update time interval cannot change during this calculation
		mutex.lock();

		//Advance simulation one step
		stepNemo();

		//Sleep if task was completed in less than the prescribed interval
		elapsedTime_ms = startTime.msecsTo(QTime::currentTime());
		if(elapsedTime_ms < updateInterval_ms){
			//Sleep for remaning time
			usleep(1000 * (updateInterval_ms - elapsedTime_ms));
		}

		//Unlock mutex
		mutex.unlock();

		//Wait for graphics to update if we are monitoring the simulation
		while(!stopThread && waitForGraphics)
			usleep(1000);
	}

	//Inform other classes that simulation has stopped playing
	emit simulationStopped();
}


/*! Puts class into error state */
void NemoWrapper::setError(const QString& errorMessage){
	error = true;
	this->errorMessage = errorMessage;
	stopThread = true;
}


/*! Advances the simulation by one step */
void NemoWrapper::stepNemo(){
	firingNeuronList.clear();

	//Handle injection of noise into neuron groups
	if(!injectNoiseMap.isEmpty()){
		//Build array of neurons to inject noise into
		unsigned* injectNoiseNeurIDArr = NULL;
		int injectNoiseArrSize;
		fillInjectNoiseArray(injectNoiseNeurIDArr, &injectNoiseArrSize);

		qDebug()<<"Inject noise array size: "<<injectNoiseArrSize;
		for(int i=0; i<injectNoiseArrSize; ++i)
			qDebug()<<"Inject noise array entry==================== "<<injectNoiseNeurIDArr[i];

		//Advance simulation
		qDebug()<<"About to step nemo with injection of noise";
		//checkNemoOutput( nemo_step(nemoSimulation, injectNoiseNeurIDArr, injectNoiseArrSize), "Nemo error on step" );
		nemo_step(nemoSimulation, injectNoiseNeurIDArr, injectNoiseArrSize);
		qDebug()<<"Nemo successfully stepped with injection of noise.";

		//Clean up noise array and empty inject noise map
		delete [] injectNoiseNeurIDArr;
		injectNoiseMap.clear();
	}
	//Advance the simulation one time step
	else{
		checkNemoOutput( nemo_step(nemoSimulation, 0, 0), "Nemo error on step" );
	}

	//Retrieve list of firing neurons
	if(archiveMode || monitorMode){
		unsigned* cycles;
		unsigned* nidx;
		unsigned nfired;
		unsigned ncycles;
		checkNemoOutput( nemo_read_firing(nemoSimulation, &cycles, &nidx, &nfired, &ncycles), "Nemo error reading firing neurons" );

		//Add firing neuron ids to list
		for(unsigned i=0; i<nfired; ++i)
			firingNeuronList.append(nidx[i]);
	}
	//Not monitoring, just flush buffer
	else{
		checkNemoOutput( nemo_flush_firing_buffer(nemoSimulation), "Nemo error flushing firing buffer" );
	}

	//Store firing neurons in database
	if(archiveMode){
		archiveDao->addArchiveData(archiveInfo.getID(), timeStepCounter, firingNeuronList);
	}

	/* Set flag to cause thread to wait for graphics to update.
		This is needed even if we are just running a time step counter */
	waitForGraphics = true;

	//Inform listening classes that this time step has been processed
	emit timeStepChanged(timeStepCounter, firingNeuronList);

	//Update time step counter
	++timeStepCounter;
}


/*! Unloads NeMo and sets the simulation loaded state to false. */
void NemoWrapper::unloadNemo(){
	simulationLoaded = false;
	Globals::setSimulationLoaded(simulationLoaded);
}

