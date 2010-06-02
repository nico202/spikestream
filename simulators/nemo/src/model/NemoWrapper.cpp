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

	//Construct the information about the parameters and their default values
	buildParameters();
}


/*! Destructor */
NemoWrapper::~NemoWrapper(){
	if(simulationLoaded)
		unloadSimulation();

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
		switch(currentTaskID){
			case LOAD_SIMULATION_TASK:
				loadSimulation();
			break;
			case RUN_SIMULATION_TASK:
				runSimulation();
			break;
			default:
				throw SpikeStreamException("Task ID not recognized.");
		}
	}
	catch(SpikeStreamException& ex){
		setError(ex.getMessage());
	}
	catch(...){
		setError("An unknown error occurred while NemoWrapper thread was running.");
	}
	stopThread = true;
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


/*! Stops the thread running */
void NemoWrapper::stop(){
	stopThread = true;
}


/*! Unloads the current simulation */
void NemoWrapper::unloadSimulation(){
	if(nemoSimulation != NULL){
		delete nemoSimulation;
		nemoSimulation = NULL;
		//FIXME: DELETE NETWORK??
	}
}


/*----------------------------------------------------------*/
/*-----                  PRIVATE SLOTS                 -----*/
/*----------------------------------------------------------*/

/*! Called by other classes being executed by this class to inform this
	class about progress with an operation */
void NemoWrapper::updateProgress(int stepsComplete, int totalSteps){
	qDebug()<<"NemoWrapper, progress: "<<stepsComplete<<" out of "<<totalSteps;
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


/*! Clears the error state */
void NemoWrapper::clearError(){
	error = false;
	errorMessage = "";
}


/*! Loads the simulation into the CUDA hardware */
void NemoWrapper::loadSimulation(){
	simulationLoaded = false;
	nemoSimulation = NULL;

	//Get the network
	if(!Globals::networkLoaded())
		throw SpikeStreamSimulationException("Cannot load simulation: no network loaded.");
	Network* currentNetwork = Globals::getNetwork();

	//Create network and archive daos and set them in the network
	NetworkDao* netDao = new NetworkDao(Globals::getNetworkDao()->getDBInfo());
	ArchiveDao* archDao = new ArchiveDao(Globals::getArchiveDao()->getDBInfo());
	currentNetwork->setNetworkDao(netDao);
	currentNetwork->setArchiveDao(archDao);

	//Set up the configuration with the appropriate parameters
	nemo::Configuration nemoConfig;

	//Build the Nemo network
	NemoLoader* nemoLoader = new NemoLoader();
	connect(nemoLoader, SIGNAL(progress(int, int)), this, SLOT(updateProgress(int, int)));
	nemo::Network* nemoNet = nemoLoader->buildNemoNetwork(currentNetwork, &stopThread);

	//Load the network into the simulator
	nemoSimulation = nemo::Simulation::create(*nemoNet, nemoConfig);
	if(nemoSimulation == NULL) {
		throw SpikeStreamSimulationException("Failed to create Nemo simulation - null returned.");
	}

	if(!stopThread)
		simulationLoaded = true;

	//Reset the daos in the network
	currentNetwork->setNetworkDao(Globals::getNetworkDao());
	currentNetwork->setArchiveDao(Globals::getArchiveDao());

	//Clean up
	delete nemoLoader;
	delete netDao;
	delete archDao;
}


/*! Plays the current simulation */
void NemoWrapper::runSimulation(){
	//Check simulation is loaded

	int cntr = 0;
	while(!stopThread){
		//Advance simulation one step

		qDebug()<<"Playing simulation. Counter="<<cntr;
		this->sleep(2);
		++cntr;

		//Emit signal with time step


		//Archive spikes if this is enabled


		//Update 3D display if this is enabled


		//Wait for time to elapse or until user presses step again
	}
}


/*! Puts class into error state */
void NemoWrapper::setError(const QString& errorMessage){
	error = true;
	this->errorMessage = errorMessage;
	stopThread = true;
}

