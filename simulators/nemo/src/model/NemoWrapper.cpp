//SpikeStream includes
#include "Globals.h"
#include "NemoLoader.h"
#include "NemoWrapper.h"
#include "PerformanceTimer.h"
#include "SpikeStreamException.h"
#include "SpikeStreamSimulationException.h"
#include "STDPFunctions.h"
#include "Util.h"
using namespace spikestream;

//Other includes
#include "boost/random.hpp"

//Outputs debugging information for NeMo calls
//#define DEBUG_INJECT_CURRENT
//#define DEBUG_INJECT_NOISE
//#define DEBUG_LEARNING
//#define DEBUG_LOAD
//#define DEBUG_PARAMETERS
//#define DEBUG_PERFORMANCE
//#define DEBUG_STEP
//#define DEBUG_WEIGHTS


/*! Constructor */
NemoWrapper::NemoWrapper() : AbstractSimulation() {
	//Initialise variables
	currentTaskID = NO_TASK_DEFINED;
	simulationLoaded = false;
	stopThread = true;
	archiveMode = false;
	monitorFiringNeurons = false;
	monitorMembranePotential = false;
	monitor = true;
	monitorWeights = false;
	updateInterval_ms = 500;
	patternNeuronGroupID = 0;
	sustainPattern = false;
	sustainCurrent = false;
	waitInterval_ms = 200;

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

/*! Adds a manager to interface with a device */
void NemoWrapper::addDeviceManager(AbstractDeviceManager* deviceManager){
	deviceManagerList.append(deviceManager);
}


/*! Cancels the loading of a simulation */
void NemoWrapper::cancelLoading(){
	stopThread = true;
}


/*! Cancels the resetting of weights */
void NemoWrapper::cancelResetWeights(){
	weightResetCancelled = true;
}


/*! Cancels the saving of weights */
void NemoWrapper::cancelSaveWeights(){
	weightSaveCancelled = true;
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

	//Get the network
	if(!Globals::networkLoaded())
		throw SpikeStreamSimulationException("Cannot load simulation: no network loaded.");
	Network* currentNetwork = Globals::getNetwork();

	//Set up the archive info
	archiveInfo.reset();
	archiveInfo.setNetworkID(currentNetwork->getID());

	//Build the Nemo network
	NemoLoader* nemoLoader = new NemoLoader();
	connect(nemoLoader, SIGNAL(progress(int, int)), this, SLOT(updateProgress(int, int)));
	#ifdef DEBUG_LOAD
		qDebug()<<"About to build nemo network.";
	#endif//DEBUG_LOAD
	nemo_network_t nemoNet = nemoLoader->buildNemoNetwork(currentNetwork, volatileConGrpMap, &stopThread);
	#ifdef DEBUG_LOAD
		qDebug()<<"Nemo network successfully built.";
	#endif//DEBUG_LOAD

	//Clean up loader
	delete nemoLoader;

	//Set the STDP functionn in the configuration
	#ifdef DEBUG_LEARNING
		STDPFunctions::print(stdpFunctionID);
	#endif
	nemo_set_stdp_function(nemoConfig,
					   STDPFunctions::getPreArray(stdpFunctionID),
					   STDPFunctions::getPreLength(stdpFunctionID),
					   STDPFunctions::getPostArray(stdpFunctionID),
					   STDPFunctions::getPostLength(stdpFunctionID),
					   STDPFunctions::getMinWeight(stdpFunctionID),
					   STDPFunctions::getMaxWeight(stdpFunctionID)
	);
	stdpReward = STDPFunctions::getReward(stdpFunctionID);
	applySTDPInterval = STDPFunctions::getApplySTDPInterval(stdpFunctionID);

	//Load the network into the simulator
	#ifdef DEBUG_LOAD
		qDebug()<<"About to load nemo network into simulator.";
	#endif//DEBUG_LOAD
	nemoSimulation = nemo_new_simulation(nemoNet, nemoConfig);
	if(nemoSimulation == NULL) {
		throw SpikeStreamSimulationException(QString("Failed to create Nemo simulation: ") + nemo_strerror());
	}
	#ifdef DEBUG_LOAD
		qDebug()<<"Nemo network successfully loaded into simulator.";
	#endif//DEBUG_LOAD

	if(!stopThread)
		simulationLoaded = true;
}


/*! Plays the simulation */
void NemoWrapper::playSimulation(){
	if(!simulationLoaded)
		throw SpikeStreamException("Cannot run simulation - no simulation loaded.");

	//Do nothing if we are already in play mode
	if(currentTaskID == RUN_SIMULATION_TASK)
		return;

	runMutex.lock();
	currentTaskID = RUN_SIMULATION_TASK;
	runMutex.unlock();

}


/*! Resets the temporary weights to the stored values. */
void NemoWrapper::resetWeights(){
	if(!simulationLoaded)
		throw SpikeStreamException("Cannot reset weights - no simulation loaded.");
	weightsReset = false;
	weightResetCancelled = false;
	currentTaskID = RESET_WEIGHTS_TASK;
}


/*! Saves the volatile weights to the database */
void NemoWrapper::saveWeights(){
	if(!simulationLoaded)
		throw SpikeStreamException("Cannot save weights - no simulation loaded.");
	weightsSaved = false;
	weightSaveCancelled = false;
	currentTaskID = SAVE_WEIGHTS_TASK;
}

/*! Injects the specified amount of current in the specified percentage of randomly
	selected neurons at each time step. Throws an exception if the number of neurons is
	greater than the number of neurons in the network. */
void  NemoWrapper::setInjectCurrent(unsigned neuronGroupID, double percentage, double current, bool sustain){
	sustainCurrent = sustain;
	sustainNoise = sustain;//Both are controlled with a single check box

	//Run checks
	if(!simulationLoaded)
		throw SpikeStreamException("Current cannot be injected when a simulation is not loaded.");
	if(percentage < 0.0 || percentage > 100.0)
		throw SpikeStreamException("Injecting current. Percentage is out of range");
	if(!Globals::getNetwork()->containsNeuronGroup(neuronGroupID))
		throw SpikeStreamException("Injecting current. Neuron group ID cannot be found in current network: " + QString::number(neuronGroupID));

	//Calculate number of neurons to fire and store it in the map
	unsigned numNeuronsToFire = Util::rUInt( (percentage / 100.0) * (double)Globals::getNetwork()->getNeuronGroup(neuronGroupID)->size());
	if(numNeuronsToFire > (unsigned)Globals::getNetwork()->getNeuronGroup(neuronGroupID)->size())
		throw SpikeStreamException("Number of neurons to fire is greater than neuron group size: " + QString::number(numNeuronsToFire) );

	injectCurrentMap[neuronGroupID] = QPair<unsigned, double>(numNeuronsToFire, current);
}


/*! Forces neurons with the specified IDs to fire in the next time step. */
void NemoWrapper::setFiringNeuronIDs(QList<neurid_t>& neurIDList){
	//Run checks
	if(!simulationLoaded)
		throw SpikeStreamException("Neurons cannot be fired when a simulation is not loaded.");

	//Pointer to current network
	Network* nwPtr = Globals::getNetwork();

	QList<neurid_t>::iterator neurIDListEnd = neurIDList.end();
	for(QList<neurid_t>::iterator iter = neurIDList.begin(); iter != neurIDListEnd; ++iter){
		if(!nwPtr->containsNeuron(*iter))
			throw SpikeStreamException("Neuron ID " + QString::number(*iter) + " cannot be found in the network.");
		neuronIDsToFire.append(*iter);
	}
}


/*! Injects the specified amount of current into the specified neurons in the next time step. */
void NemoWrapper::setInjectCurrentNeuronIDs(QList<neurid_t>& neurIDList, double current){
	//Run checks
	if(!simulationLoaded)
		throw SpikeStreamException("Neurons cannot be injected with current when a simulation is not loaded.");

	//Pointer to current network
	Network* nwPtr = Globals::getNetwork();

	QList<neurid_t>::iterator neurIDListEnd = neurIDList.end();
	for(QList<neurid_t>::iterator iter = neurIDList.begin(); iter != neurIDListEnd; ++iter){
		if(!nwPtr->containsNeuron(*iter))
			throw SpikeStreamException("Neuron ID " + QString::number(*iter) + " cannot be found in the network.");
		neuronIDCurrentMap[*iter] = current;
	}
}


/*! Forces the specified percentage of neurons in the specified neuron group to fire at the
	next time step. Throws an exception if the neuron group ID does not exist in the current
	network or if the percentage is < 0 or > 100. */
void  NemoWrapper::setInjectNoise(unsigned neuronGroupID, double percentage, bool sustain){
	sustainNoise = sustain;
	sustainCurrent = sustain;//Both are controlled with a single check box

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
}


/*! Steps through a single time step */
void NemoWrapper::stepSimulation(){
	if(!simulationLoaded)
		throw SpikeStreamException("Cannot step simulation - no simulation loaded.");

	//Do nothing if we are already in step mode
	if(currentTaskID == STEP_SIMULATION_TASK)
		return;

	//Lock mutex to prevent interference with current task
	runMutex.lock();

	//Set to step mode for when run loop is next ready
	currentTaskID = STEP_SIMULATION_TASK;

	//Release mutex
	runMutex.unlock();
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
			runMutex.lock();

			//Run simulation
			if(currentTaskID == RUN_SIMULATION_TASK){
				runNemo();
			}
			//Step simulation
			else if(currentTaskID == STEP_SIMULATION_TASK){
				stepNemo();
			}
			//Reset weights
			else if(currentTaskID == RESET_WEIGHTS_TASK){
				resetNemoWeights();
			}
			//Save weights
			else if(currentTaskID == SAVE_WEIGHTS_TASK){
				saveNemoWeights();
			}
			//Set neuron parameters
			else if(currentTaskID == SET_NEURON_PARAMETERS_TASK){
				setNeuronParametersInNemo();
			}
			//Do nothing
			else if(currentTaskID == NO_TASK_DEFINED){
				;//Do nothing
			}
			//Task ID not recognized
			else{
				throw SpikeStreamException("Task ID not recognizedL: " + QString::number(currentTaskID));
			}

			//Reset task ID
			currentTaskID = NO_TASK_DEFINED;//Don't want it to continue stepping.

			runMutex.unlock();

			//Short sleep waiting for the next command
			msleep(waitInterval_ms);
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
void NemoWrapper::setArchiveMode(bool newArchiveMode, const QString& description){
	if(newArchiveMode && !simulationLoaded)
		throw SpikeStreamSimulationException("Cannot switch archive mode on unless simulation is loaded.");

	/* Create archive if this is the first time the mode has been set
		Use globals archive dao because this method is called from a separate thread */
	if(archiveInfo.getID() == 0){
		archiveInfo.setDescription(description);
		Globals::getArchiveDao()->addArchive(archiveInfo);
		Globals::getEventRouter()->archiveListChangedSlot();
	}
	//Rename archive if one is already loaded
	else{
		archiveInfo.setDescription(description);
		Globals::getArchiveDao()->setArchiveProperties(archiveInfo.getID(), description);
		Globals::getEventRouter()->archiveListChangedSlot();
	}

	this->archiveMode = newArchiveMode;
}


/*! Sets a firing pattern along with the neuron group.
	The pattern can be injected for one time step or continuously. */
void NemoWrapper::setFiringInjectionPattern(const Pattern& pattern, unsigned neuronGroupID, bool sustain){
	//Lock mutex to prevent multiple threads accessing injection vector
	mutex.lock();

	sustainPattern = sustain;

	//Get copy of the pattern that is aligned on the centre of the neuron group
	NeuronGroup* neurGrp = Globals::getNetwork()->getNeuronGroup(neuronGroupID);
	Pattern alignedPattern( pattern.getAlignedPattern(neurGrp->getBoundingBox()) );

	//Add neurons that are contained within the pattern
	NeuronMap::iterator neuronMapEnd = neurGrp->end();
	for(NeuronMap::iterator iter = neurGrp->begin(); iter != neuronMapEnd; ++iter){
		if(alignedPattern.contains( (*iter)->getLocation() ) ){
			injectionPatternVector.push_back( (*iter)->getID());
		}
	}
	//Release mutex
	mutex.unlock();
}


/*! Sets a current injection pattern along with the neuron group.
	The pattern can be injected for one time step or continuously. */
void NemoWrapper::setCurrentInjectionPattern(const Pattern& pattern, float current, unsigned neuronGroupID, bool sustain){
	//Lock mutex to prevent multiple threads accessing current vectors
	mutex.lock();

	sustainPattern = sustain;

	//Get copy of the pattern that is aligned on the centre of the neuron group
	NeuronGroup* neurGrp = Globals::getNetwork()->getNeuronGroup(neuronGroupID);
	Pattern alignedPattern( pattern.getAlignedPattern(neurGrp->getBoundingBox()) );

	//Add neurons that are contained within the pattern
	NeuronMap::iterator neuronMapEnd = neurGrp->end();
	for(NeuronMap::iterator iter = neurGrp->begin(); iter != neuronMapEnd; ++iter){
		if(alignedPattern.contains( (*iter)->getLocation() ) ){
			injectionCurrentNeurIDVector.push_back( (*iter)->getID());
			injectionCurrentVector.push_back(current);
		}
	}

	mutex.unlock();
}


/*! Sets the monitor mode, which controls whether firing neuron data is extracted
	from the simulation at each time step */
void NemoWrapper::setMonitorNeurons(bool firing, bool membranePotential){
	if( (firing || membranePotential) && !simulationLoaded)
		throw SpikeStreamSimulationException("Cannot switch neuron monitor mode on unless simulation is loaded.");
	if(firing && membranePotential)
		throw SpikeStreamSimulationException("Cannot monitor firing neurons and membrane potential at the same time.");

	//Store new monitoring mode
	this->monitorFiringNeurons = firing;
	this->monitorMembranePotential = membranePotential;
}


/*! Controls whether the time step is updated at each time step. */
void NemoWrapper::setMonitor(bool mode){
	this->monitor = mode;
}


/*! Sets wrapper to retrieve the weights from NeMo and save them to the current weight
	field in the database. Does this operation every time step until it is turned off. */
void  NemoWrapper::setMonitorWeights(bool enable){
	monitorWeights = enable;
}


/*! Sets the parameters of the neurons within NeMo */
void NemoWrapper::setNeuronParameters(unsigned neuronGroupID, QHash<QString, double> parameterMap){
	if(!simulationLoaded)
		throw SpikeStreamException("Cannot set neuron parameters - no simulation loaded.");
	this->neuronGroupID = neuronGroupID;
	neuronParameterMap = parameterMap;
	currentTaskID = SET_NEURON_PARAMETERS_TASK;
}


/*! Sets the parameters of the synapses within NeMo */
void NemoWrapper::setSynapseParameters(unsigned, QHash<QString, double>){
	if(!simulationLoaded)
		throw SpikeStreamException("Cannot set synapse parameters - no simulation loaded.");
	throw SpikeStreamException("This method should not be called because it is not implemented.");
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

/*! Adds neurons due to noise at the end of the injectNeuronID vector.
	These neurons will be forced to fire at the next time step.
	Returns the number of neurons added. */
unsigned NemoWrapper::addInjectFiringNeuronIDs(){
	//Calculate total number of firing neurons
	unsigned arraySize = 0, arrayCounter = 0;
	for(QHash<unsigned, unsigned>::iterator iter = injectNoiseMap.begin(); iter != injectNoiseMap.end(); ++iter)
		arraySize += iter.value();

	//Add a random selection of neuron ids from each group
	unsigned randomIndex, neurGrpSize, numSelectedNeurons;
	QHash<unsigned, bool> addedNeurIndxMap;//Prevent duplicates
	for(QHash<unsigned, unsigned>::iterator iter = injectNoiseMap.begin(); iter != injectNoiseMap.end(); ++iter){
		//Get list of neuron ids
		QList<unsigned> neuronIDList = Globals::getNetwork()->getNeuronGroup(iter.key())->getNeuronIDs();
		neurGrpSize = neuronIDList.size();
		addedNeurIndxMap.clear();
		numSelectedNeurons = iter.value();
		#ifdef DEBUG_INJECT_NOISE
			qDebug()<<"Selecting random neurons for injecting noise. Num neurons: "<<neurGrpSize<<"; num neurons to select: "<<numSelectedNeurons;
		#endif//DEBUG_INJECT_NOISE

		//Select indexes from the list of neuron ids
		while((unsigned)addedNeurIndxMap.size() < numSelectedNeurons){
			randomIndex = Util::getRandom(0, neurGrpSize);//Get random position in list of neuron ids
			if(!addedNeurIndxMap.contains(randomIndex)){//New index
				if(arrayCounter >= arraySize)//Sanity check
					throw SpikeStreamException("Error adding noise injection neuron ids - array counter out of range.");
				injectionPatternVector.push_back(neuronIDList.at(randomIndex));//Add neuron id to pattern vector
				#ifdef DEBUG_INJECT_NOISE
					qDebug()<<"Random index: "<<randomIndex<<"; Inject noise neuron ID: "<<neuronIDList.at(randomIndex);
				#endif//DEBUG_INJECT_NOISE
				addedNeurIndxMap[randomIndex] = true;//Record the fact that we have selected this ID
				++arrayCounter;
			}
		}
	}

	//Sanity check, then return number of neurons added
	if(arrayCounter != arraySize)
		throw SpikeStreamException("Error adding inject noise neuron IDs. Array counter: " + QString::number(arrayCounter) + "; Array size: " + QString::number(arraySize));

	//Add neurons that are specified to fire during this time step
	QList<neurid_t>::iterator neurIDListFireEnd = neuronIDsToFire.end();
	for(QList<neurid_t>::iterator iter = neuronIDsToFire.begin(); iter != neurIDListFireEnd; ++iter){
		injectionPatternVector.push_back(*iter);
		++arraySize;
	}
	neuronIDsToFire.clear();

	//Add firing neuron IDs from plugins
	for(int i=0; i<deviceManagerList.size(); ++i){
		QList<neurid_t>::iterator outputNeuronsEnd = deviceManagerList[i]->outputNeuronsEnd();
		for(QList<neurid_t>::iterator iter =  deviceManagerList[i]->outputNeuronsBegin(); iter != outputNeuronsEnd; ++iter){
			injectionPatternVector.push_back(*iter);
			++arraySize;
		}
	}

	//Return the number of neurons that have been added
	return arraySize;
}


/*! Adds current to a random selection of neurons */
unsigned NemoWrapper::addInjectCurrentNeuronIDs(){
	//Calculate total number of current neurons
	unsigned arraySize = 0, arrayCounter = 0;
	for(QHash<unsigned, QPair<unsigned, double> >::iterator iter = injectCurrentMap.begin(); iter != injectCurrentMap.end(); ++iter)
		arraySize += iter.value().first;

	//Add a random selection of neuron ids from each group
	unsigned randomIndex, neurGrpSize, numSelectedNeurons;
	double injectCurrentAmount;
	QHash<unsigned, bool> addedNeurIndxMap;//Prevent duplicates
	for(QHash<unsigned, QPair<unsigned, double> >::iterator iter = injectCurrentMap.begin(); iter != injectCurrentMap.end(); ++iter){
		//Get list of neuron ids
		QList<unsigned> neuronIDList = Globals::getNetwork()->getNeuronGroup(iter.key())->getNeuronIDs();
		neurGrpSize = neuronIDList.size();
		addedNeurIndxMap.clear();
		numSelectedNeurons = iter.value().first;
		injectCurrentAmount = iter.value().second;

		//Select indexes from the list of neuron ids
		while((unsigned)addedNeurIndxMap.size() < numSelectedNeurons){
			randomIndex = Util::getRandom(0, neurGrpSize);//Get random position in list of neuron ids
			if(!addedNeurIndxMap.contains(randomIndex)){//New index
				if(arrayCounter >= arraySize)//Sanity check
					throw SpikeStreamException("Error adding current injection neuron ids - array counter out of range.");
				injectionCurrentNeurIDVector.push_back(neuronIDList.at(randomIndex));//Add neuron id to pattern vector
				injectionCurrentVector.push_back(injectCurrentAmount);
				addedNeurIndxMap[randomIndex] = true;//Record the fact that we have selected this ID
				++arrayCounter;
			}
		}
	}

	//Sanity check, then return number of neurons added
	if(arrayCounter != arraySize)
		throw SpikeStreamException("Error adding inject noise neuron IDs. Array counter: " + QString::number(arrayCounter) + "; Array size: " + QString::number(arraySize));

	//Add neurons that have specified amount of current
	QHash<neurid_t, double>::iterator neurIDCurrentMapEnd = neuronIDCurrentMap.end();
	for(QHash<neurid_t, double>::iterator iter = neuronIDCurrentMap.begin(); iter != neurIDCurrentMapEnd; ++iter){
		injectionCurrentNeurIDVector.push_back(iter.key());//Add neuron id to current vector
		injectionCurrentVector.push_back(iter.value());
		#ifdef DEBUG_INJECT_CURRENT
			qDebug()<<"TimeStep: "<<timeStepCounter<<". Injecting "<<iter.value()<<" current into neuron "<<iter.key();
		#endif//DEBUG_INJECT_CURRENT
		++arraySize;
	}
	neuronIDCurrentMap.clear();

	//Return the number of neurons that have been added
	return arraySize;
}


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


/*! Extracts the membrane potential for all the neurons from the simulation. */
void NemoWrapper::getMembranePotential(){
	membranePotentialMap.clear();

	float tmpMemPot, maxMemPot = 0.0f, minMemPot = 0.0f;
	QList<NeuronGroup*> neurGrpList = Globals::getNetwork()->getNeuronGroups();
	for(int i=0; i<neurGrpList.size(); ++i){
		NeuronMap::iterator neurGrpListEnd = neurGrpList.at(i)->end();
		for(NeuronMap::iterator iter = neurGrpList.at(i)->begin(); iter != neurGrpListEnd; ++iter){
			checkNemoOutput(nemo_get_membrane_potential(nemoSimulation, iter.key(), &tmpMemPot), "Error getting membrane potential.");
			membranePotentialMap[iter.key()] = tmpMemPot;
			if(tmpMemPot > maxMemPot)
				maxMemPot = tmpMemPot;
			if(tmpMemPot < minMemPot)
				minMemPot = tmpMemPot;
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
		#ifdef DEBUG_PERFORMANCE
			qDebug()<<"Elapsed time: "<<elapsedTime_ms<<"; update interval: "<<updateInterval_ms;
		#endif//DEBUG_PERFORMANCE
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


/*! Resets the temporary weights to the saved weights */
void NemoWrapper::resetNemoWeights(){
	//Return immediately if there is nothing to save
	if(volatileConGrpMap.size() == 0){
		weightsReset = true;
		weightsSaved = true;
		return;
	}

	//Work through all connection groups
	Network* currentNetwork = Globals::getNetwork();
	for(QHash<unsigned, synapse_id*>::iterator conGrpIter = volatileConGrpMap.begin(); conGrpIter != volatileConGrpMap.end(); ++conGrpIter){
		ConnectionGroup* tmpConGrp = currentNetwork->getConnectionGroup(conGrpIter.key());
		ConnectionIterator endConGrp = tmpConGrp->end();
		for(ConnectionIterator conIter = tmpConGrp->begin(); conIter != endConGrp; ++conIter){
			//Copy current weights to temporary weights
			conIter->setTempWeight(conIter->getWeight());

			//Check for cancellation
			if(weightResetCancelled){
				Globals::getEventRouter()->weightsChangedSlot();
				return;
			}
		}
	}

	//Inform other classes about weight change
	Globals::getEventRouter()->weightsChangedSlot();

	//Weight are saved and reset
	weightsReset = true;
	weightsSaved = true;
}


/*! Saves the weights from the network into the database */
void NemoWrapper::saveNemoWeights(){
	//Return immediately if there is nothing to save
	if(volatileConGrpMap.size() == 0){
		weightsSaved = true;
		return;
	}

	//Update temporary weights in network to match weights in NeMo
	updateNetworkWeights();

	//Copy temporary weight to weight field in network
	Network* currentNetwork = Globals::getNetwork();
	unsigned cntr = 0;
	for(QHash<unsigned, synapse_id*>::iterator conGrpIter = volatileConGrpMap.begin(); !weightSaveCancelled && conGrpIter != volatileConGrpMap.end(); ++conGrpIter){
		currentNetwork->copyTempWeightsToWeights(conGrpIter.key());
		++cntr;
		emit progress(cntr, volatileConGrpMap.size());
	}

	//Inform other classes about weight change and saved state of network
	Globals::getEventRouter()->weightsChangedSlot();
	Globals::getEventRouter()->networkListChangedSlot();

	//Weight saving is complete
	weightsSaved = true;
}


/*! Puts class into error state */
void NemoWrapper::setError(const QString& errorMessage){
	currentTaskID = NO_TASK_DEFINED;
	error = true;
	this->errorMessage = errorMessage;
	stopThread = true;
}


/*! Sets the neuron parameters in the NeMo simulation.
	FIXME: CODE COPIED FROM NEMO LOADER; PUT SOMEWHERE GENERIC. */
void NemoWrapper::setNeuronParametersInNemo(){
	if(neuronGroupID == 0){
		throw SpikeStreamException("Failed to set neuron parameters. NeuronGroupID has not been set.");
	}

	//Get the neuron group
	NeuronGroup* tmpNeurGrp = Globals::getNetwork()->getNeuronGroup(neuronGroupID);

	//Set the parameters depending on the type of neuron.
	if(tmpNeurGrp->getInfo().getNeuronType().getDescription() == "Izhikevich Excitatory Neuron"){
		setExcitatoryNeuronParameters(tmpNeurGrp);
	}
	else{
		setInhibitoryNeuronParameters(tmpNeurGrp);
	}
}


/*! Sets the parameters in an excitatory neuron group */
void NemoWrapper::setExcitatoryNeuronParameters(NeuronGroup* neuronGroup){
	//Get the parameters
	float a = neuronGroup->getParameter("a");
	float b = neuronGroup->getParameter("b");
	float c_1 = neuronGroup->getParameter("c_1");
	float d_1 = neuronGroup->getParameter("d_1");
	float d_2 = neuronGroup->getParameter("d_2");
	float v = neuronGroup->getParameter("v");
	float sigma = neuronGroup->getParameter("sigma");

	//Create the random number generator (from: nemo/examples/random1k.cpp)
	rng_t rng;
	urng_t ranNumGen( rng, boost::uniform_real<double>(0, 1) );

	//Set parameters in the neurons
	float c, d, u, rand1, rand2;
	NeuronMap::iterator neurGrpEnd = neuronGroup->end();
	for(NeuronMap::iterator iter = neuronGroup->begin(); iter != neurGrpEnd; ++iter){
		//Get random numbers
		rand1 = ranNumGen();
		rand2 = ranNumGen();

		//Calculate excitatory neuron parameters
		c = v + c_1 * rand1 * rand2;
		d = d_1 - d_2 * rand1 * rand2;
		u = b * v;

		#ifdef DEBUG_PARAMETERS
			qDebug()<<"Setting excitatory neuron parameters for neuron "<<iter.key()<<". a="<<a<<"; b="<<b<<"; c="<<c<<"; d="<<d<<"; u="<<u<<"; v="<<v<<"; sigma="<<sigma;
		#endif//DEBUG_PARAMETERS

		//Set parameters in neuron
		checkNemoOutput(
			nemo_set_neuron_s(
					nemoSimulation,
					iter.key(),
					a, b, c, d, u, v, sigma
			),
			"Failed to set Izhikevich excitatory neuron parameters."
		);
	}
}


/*! Sets the parameters in an inhibitory neuron group */
void NemoWrapper::setInhibitoryNeuronParameters(NeuronGroup* neuronGroup){
	//Extract inhibitory neuron parameters
	float a_1 = neuronGroup->getParameter("a_1");
	float a_2 = neuronGroup->getParameter("a_2");
	float b_1 = neuronGroup->getParameter("b_1");
	float b_2 = neuronGroup->getParameter("b_2");
	float d = neuronGroup->getParameter("d");
	float v = neuronGroup->getParameter("v");
	float sigma = neuronGroup->getParameter("sigma");

	//Create the random number generator (from: nemo/examples/random1k.cpp)
	rng_t rng;
	urng_t ranNumGen( rng, boost::uniform_real<double>(0, 1) );

	//Set parameters in the neurons
	float a, b, u, rand1, rand2;
	NeuronMap::iterator neurGrpEnd = neuronGroup->end();
	for(NeuronMap::iterator iter = neuronGroup->begin(); iter != neurGrpEnd; ++iter){
		//Get random numbers
		rand1 = ranNumGen();
		rand2 = ranNumGen();

		//Calculate inhibitory neuron parameters
		a = a_1 + a_2 * rand1;
		b = b_1 - b_2 * rand2;
		u = b * v;

		//Set parameters in neuron FIXME
		checkNemoOutput(
			nemo_set_neuron_s(
					nemoSimulation,
					iter.key(),
					a, b, v, d, u, v, sigma
			),
			"Failed to set Izhikevich inhibitory neuron parameters."
		);
	}
}


/*! Advances the simulation by one step */
void NemoWrapper::stepNemo(){
	unsigned *firedArray, numFiredNeurons = 0, numCurrentNeurons = 0;
	size_t firedCount;
	firingNeuronList.clear();

	//---------------------------------------
	//     Step simulation
	//---------------------------------------
	//Add inject noise neurons to end of injection vector
	if(!injectNoiseMap.isEmpty() || !neuronIDsToFire.isEmpty() || !deviceManagerList.isEmpty())
		numFiredNeurons = addInjectFiringNeuronIDs();

	if(!injectCurrentMap.isEmpty() || !neuronIDCurrentMap.isEmpty())
		numCurrentNeurons = addInjectCurrentNeuronIDs();

	#ifdef DEBUG_STEP
		qDebug()<<"About to step nemo.";
	#endif//DEBUG_STEP
	checkNemoOutput(
		nemo_step(
			nemoSimulation,
			&injectionPatternVector.front(),
			injectionPatternVector.size(),
			&injectionCurrentNeurIDVector.front(),
			&injectionCurrentVector.front(),
			injectionCurrentNeurIDVector.size(),
			&firedArray,
			&firedCount
		),
		"Nemo error on step." );
	#ifdef DEBUG_STEP
		qDebug()<<"Nemo successfully stepped.";
	#endif//DEBUG_STEP

	//Empty noise injection map if we are not sustaining it
	if(!sustainNoise)
		injectNoiseMap.clear();

	//Clear inject current parameters if we are not sustaining it
	if(!sustainCurrent){
		injectCurrentMap.clear();
	}

	//Delete pattern if it is not sustained
	if(!sustainPattern){
		injectionPatternVector.clear();
		injectionCurrentNeurIDVector.clear();
		injectionCurrentVector.clear();
	}
	//Delete neurons added to end of vector from noise or current
	else {
		if(numFiredNeurons > 0){
			injectionPatternVector.erase(injectionPatternVector.end() - numFiredNeurons, injectionPatternVector.end());
		}
		if(numCurrentNeurons > 0){
			injectionCurrentNeurIDVector.erase(injectionCurrentNeurIDVector.end() - numCurrentNeurons, injectionCurrentNeurIDVector.end());
			injectionCurrentVector.erase(injectionCurrentVector.end() - numCurrentNeurons, injectionCurrentVector.end());
		}
	}


	//---------------------------------------------------------
	//        Pass list of firing neurons to other classes
	//---------------------------------------------------------
	if(archiveMode || (monitorFiringNeurons && monitor) || !deviceManagerList.isEmpty()){
		//Add firing neuron ids to list
		for(unsigned i=0; i<firedCount; ++i)
			firingNeuronList.append(firedArray[i]);
		#ifdef DEBUG_STEP
			if(firedCount > 0)
				qDebug()<<"Number of firing neurons: "<<firedCount;
		#endif//DEBUG_STEP

		//Store firing neurons in database
		if(archiveMode){
			archiveDao->addArchiveData(archiveInfo.getID(), timeStepCounter, firingNeuronList);
		}

		//Pass firing neurons to device managers and step device managers
		for(int i=0; i<deviceManagerList.size(); ++i){
			deviceManagerList[i]->setInputNeurons(timeStepCounter, firingNeuronList);
			deviceManagerList[i]->step();
		}
	}


	//-----------------------------------------------
	//         Extract membrane potential
	//-----------------------------------------------
	if(monitor && monitorMembranePotential){
		#ifdef DEBUG_STEP
			qDebug()<<"About to read membrane potential.";
		#endif//DEBUG_STEP
		getMembranePotential();
		#ifdef DEBUG_STEP
			qDebug()<<"Successfully read membrane potential.";
		#endif//DEBUG_STEP
	}


	//--------------------------------------------
	//               Apply STDP
	//--------------------------------------------
	if(!volatileConGrpMap.isEmpty()){
		if(timeStepCounter % applySTDPInterval == 0){
			nemo_apply_stdp(nemoSimulation, stdpReward);
			#ifdef DEBUG_LEARNING
				qDebug()<<"Applying STDP. TimeStepCounter="<<timeStepCounter<<"; applySTDPInterval="<<applySTDPInterval;
			#endif
		}
	}


	//--------------------------------------------
	//             Retrieve weights
	//--------------------------------------------
	if(monitorWeights && monitor){
		updateNetworkWeights();

		//Inform other classes that weights have changed
		Globals::getEventRouter()->weightsChangedSlot();
	}

	if(monitor){
		/* Set flag to cause thread to wait for graphics to update.
			This is needed even if we are just running a time step counter */
		waitForGraphics = true;

		//Inform listening classes that this time step has been processed
		if(monitorFiringNeurons)
			emit timeStepChanged(timeStepCounter, firingNeuronList);
		else if (monitorMembranePotential)
			emit timeStepChanged(timeStepCounter, membranePotentialMap);
		else
			emit(timeStepChanged(timeStepCounter));
	}

	//Update time step counter
	++timeStepCounter;
}


/*! Unloads NeMo and sets the simulation loaded state to false. */
void NemoWrapper::unloadNemo(){
	/* Unlock mutex if it is still locked.
		need to call try lock in case  mutex is unlocked, in which case calling
		unlock again may cause a crash */
	mutex.tryLock();
	mutex.unlock();

	runMutex.tryLock();
	runMutex.unlock();

	//Clean up dynamic arrays in volatile connection group map
	for(QHash<unsigned, synapse_id*>::iterator conGrpIter = volatileConGrpMap.begin(); conGrpIter != volatileConGrpMap.end(); ++conGrpIter){
		delete conGrpIter.value();
	}
	volatileConGrpMap.clear();

	//Clean up sustain noise etc.
	sustainNoise = false;
	injectNoiseMap.clear();
	sustainCurrent = false;
	injectCurrentMap.clear();
	sustainPattern = false;
	injectionPatternVector.clear();
	injectionCurrentNeurIDVector.clear();
	injectionCurrentVector.clear();

	simulationLoaded = false;
	archiveInfo.reset();
}


/*! Updates the weights in the network with weights from NeMo.
	NOTE: Must be called from within the NeMo thread. */
void NemoWrapper::updateNetworkWeights(){
	if(!simulationLoaded){
		throw SpikeStreamException("Failed to update network weights. Simulation not loaded.");
	}

	//Work through all of the volatile connection groups
	for(QHash<unsigned, synapse_id*>::iterator conGrpIter = volatileConGrpMap.begin(); conGrpIter != volatileConGrpMap.end(); ++conGrpIter){
		//Get the volatile connection group and matching array of nemo synapse IDs
		ConnectionGroup* tmpConGrp = Globals::getNetwork()->getConnectionGroup(conGrpIter.key());
		synapse_id* synapseIDArray = conGrpIter.value();
		double weightFactor = tmpConGrp->getParameter("weight_factor");//Amount by which weight was multiplied when connection was added to NeMo

		//Work through connection group and query volatile connections
		float* weightArray;//Will point to array of returned weights
		unsigned conCntr = 0;
		ConnectionIterator conGrpEnd = tmpConGrp->end();
		for(ConnectionIterator conIter = tmpConGrp->begin(); conIter != conGrpEnd; ++conIter){
			#ifdef DEBUG_WEIGHTS
				qDebug()<<"About to query weights: nemo synapseID="<<synapseIDArray[0]<<" spikestream synapse id="<<conIter->getID();
			#endif//DEBUG_WEIGHTS

			//Query weight
			checkNemoOutput( nemo_get_weights(nemoSimulation, &synapseIDArray[conCntr], 1, &weightArray), "Error getting weights." );

			//Update weight in connection
			conIter->setTempWeight(weightArray[0] / weightFactor);

			#ifdef DEBUG_WEIGHTS
				qDebug()<<"TimeStep: "<<timeStepCounter<<"; Weight query complete: weight="<<weightArray[0]<<" nemo synapse id="<<synapseIDArray[conCntr];
			#endif//DEBUG_WEIGHTS

			//Increase counter for accessing synapse id array
			++conCntr;
		}
	}
}


