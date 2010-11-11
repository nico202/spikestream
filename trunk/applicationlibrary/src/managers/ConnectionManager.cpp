//SpikeStream includes
#include "ArchiveDao.h"
#include "ConnectionManager.h"
#include "Globals.h"
#include "Network.h"
#include "NetworkDao.h"
#include "SpikeStreamException.h"
using namespace spikestream;


/*! Constructor */
ConnectionManager::ConnectionManager() : SpikeStreamThread() {
}


/*! Destructor */
ConnectionManager::~ConnectionManager(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Prepares for deletion of connection groups and starts separate thread to execute task. */
void ConnectionManager::deleteConnectionGroups(QList<unsigned>& connectionGroupIDs){
	deleteConnectionGroupIDs = connectionGroupIDs;
	this->start();
}


//Run method inherited from QThread
void ConnectionManager::run(){
	clearError();
	stopThread = false;

	try{
		//Delete the connection group
		Network* currentNetwork = Globals::getNetwork();
		Globals::getNetwork()->deleteConnectionGroups(deleteConnectionGroupIDs);

		//Wait for network to finish adding connection groups
		while(currentNetwork->isBusy()){
			msleep(250);
		}

		//Check for errors
		if(currentNetwork->isError())
			setError(currentNetwork->getErrorMessage());
	}
	catch (SpikeStreamException& ex){
		setError(ex.getMessage());
	}
	catch(...){
		setError("An unknown error occurred.");
	}

	stopThread = true;
}

