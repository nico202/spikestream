//SpikeStream includes
#include "Globals.h"
#include "NemoLoader.h"
#include "NemoWrapper.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Nemo includes
//#include "nemo.hpp"
//using namespace nemo;


/*! Constructor */
NemoWrapper::NemoWrapper(){
	//Initialise variables
	currentTaskID = NO_TASK_DEFINED;
	simulationLoaded = false;
	stopThread = true;
}


/*! Destructor */
NemoWrapper::~NemoWrapper(){

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


/*! Stops the thread running */
void NemoWrapper::stop(){
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

/*! Clears the error state */
void NemoWrapper::clearError(){
	error = false;
	errorMessage = "";
}


/*! Loads the simulation into the CUDA hardware */
void NemoWrapper::loadSimulation(){
	simulationLoaded = false;
	Network* currentNetwork = Globals::getNetwork();

	//Create network and archive daos and set them in the network
	NetworkDao* netDao = new NetworkDao(Globals::getNetworkDao()->getDBInfo());
	ArchiveDao* archDao = new ArchiveDao(Globals::getArchiveDao()->getDBInfo());
	currentNetwork->setNetworkDao(netDao);
	currentNetwork->setArchiveDao(archDao);

	//Load the simulation
	NemoLoader* nemoLoader = new NemoLoader();
	connect(nemoLoader, SIGNAL(progress(int, int)), this, SLOT(updateProgress(int, int)));
	//nemo::Simulation* nemoSim = nemoLoader->loadSimulation(currentNetwork, &stopThread);

	//Load the network into the simulator
	int cntr = 0;
	while(cntr < 20 && !stopThread){
		qDebug()<<"LOADING SIMULATION. COUNTER="<<cntr;
		sleep(1);
		emit progress(cntr, 20);
		++cntr;
	}

	simulationLoaded = true;

	//Reset the daos in the network
	currentNetwork->setNetworkDao(Globals::getNetworkDao());
	currentNetwork->setArchiveDao(Globals::getArchiveDao());

	//Clean up
	delete nemoLoader;
	delete netDao;
	delete archDao;
	simulationLoaded = true;
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

