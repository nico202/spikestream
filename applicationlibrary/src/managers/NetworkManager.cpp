//SpikeStream includes
#include "NetworkManager.h"
#include "NetworkDaoThread.h"
#include "Globals.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

/*! Constructor */
NetworkManager::NetworkManager() : SpikeStreamThread(){

}


/*! Destructor */
NetworkManager::~NetworkManager(){

}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Cancels the load operation. */
void NetworkManager::cancel(){
	stopThread = true;
}


//Run method inherited from SpikeStreamThread
void NetworkManager::run(){
	stopThread = false;
	clearError();

	try{
		if (currentTask == LOAD_NETWORK_TASK){
			//Start task
			emit progress(0, 0, "Loading neuron and connection group information.", false);
			network->load();

			//Wait for network to finish task
			while(network->isBusy() && !stopThread){
				emit progress(network->getNumberOfCompletedSteps(), network->getTotalNumberOfSteps(), network->getProgressMessage(), true);
				msleep(200);
			}

			//Handle cancel operation
			if(stopThread){
				network->cancel();
				while(network->isBusy()){
					emit progress(0, 1, "Waiting for network to finish clean up.", false);
					msleep(200);
				}
			}
		}
		else if (currentTask == SAVE_NETWORK_TASK){
			//Start task
			network->save();

			//Wait for network to finish task - this operation cannot be cancelled
			while(network->isBusy() && !stopThread){
				emit progress(network->getNumberOfCompletedSteps(), network->getTotalNumberOfSteps(), network->getProgressMessage(), false);
				msleep(200);
			}
		}
		else if (currentTask == DELETE_NETWORK_TASK){
			//Start task
			NetworkDaoThread networkDaoThread(Globals::getNetworkDao()->getDBInfo());
			networkDaoThread.startDeleteNetwork(network->getID());

			//Wait for network dao thread to finish task
			while(networkDaoThread.isRunning()){
				emit progress(networkDaoThread.getNumberOfCompletedSteps(), networkDaoThread.getTotalNumberOfSteps(), networkDaoThread.getProgressMessage(), false);
				msleep(200);
			}
		}
		else{
			throw SpikeStreamException("Current task not recognized: " + QString::number(currentTask));
		}
	}
	catch(SpikeStreamException& ex){
		setError(ex.getMessage());
		qDebug()<<"Exception: "<<ex.getMessage();
	}
	catch(...){
		setError("Unrecognized exception thrown.");
		qDebug()<<"Unknown Exception: ";
	}

	if(network->isError())
		setError(network->getErrorMessage());

	emit progress(1, 1, network->getProgressMessage() + " Complete.", false);
	stopThread = true;
}


/*! Stores the pointer to the network and starts the load in separate thread. */
void NetworkManager::startDeleteNetwork(Network *network){
	this->network = network;
	currentTask = DELETE_NETWORK_TASK;
	start();
}


/*! Stores the pointer to the network and starts the load in separate thread. */
void NetworkManager::startLoadNetwork(Network *network){
	this->network = network;
	currentTask = LOAD_NETWORK_TASK;
	start();
}



/*! Stores the pointer to the network and starts the save in separate thread. */
void NetworkManager::startSaveNetwork(Network *network){
	this->network = network;
	currentTask = SAVE_NETWORK_TASK;
	start();
}



