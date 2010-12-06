//SpikeStream includes
#include "NetworkManager.h"
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
		emit progress(0, 1, "Loading neuron and connection group information.");
		network->load();

		while(network->isBusy() && !stopThread){
			emit progress(network->getNumberOfCompletedSteps(), network->getTotalNumberOfSteps(), "Loading network.");
			msleep(200);
		}

		if(stopThread){
			network->cancel();
			while(network->isBusy()){
				emit progress(0, 1, "Waiting for network to finish clean up.");
				msleep(200);
			}
		}
	}
	catch(SpikeStreamException& ex){
		setError(ex.getMessage());
	}
	catch(...){
		setError("Unrecognized exception thrown.");
	}

	if(network->isError())
		setError(network->getErrorMessage());
	qDebug()<<"Nework mabnager thread finished";

	emit progress(1, 1, "Load complete.");
	stopThread = true;
}


/*! Stores the pointer to the network and starts the load in separate thread. */
void NetworkManager::startLoadNetwork(Network *network){
	this->network = network;
	start();
}



