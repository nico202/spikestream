//SpikeStream includes
#include "DatabaseManager.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>


/*! Constructor */
DatabaseManager::DatabaseManager(const DBInfo& networkDBInfo, const DBInfo& archiveDBInfo, const DBInfo& analysisDBInfo){
	networkDao = NULL;
	archiveDao = NULL;
	analysisDao = NULL;
	this->networkDBInfo = networkDBInfo;
	this->archiveDBInfo = archiveDBInfo;
	this->analysisDBInfo = analysisDBInfo;
}


/*! Destuctor */
DatabaseManager::~DatabaseManager(){

}


/*! Sets up class for the clear databases task, which is executed when the thread is run. */
void DatabaseManager::prepareClearDatabases(){
	taskID = CLEAR_DATABASES_TASK;
}


/*! Run method inherited from QThread */
void DatabaseManager::run(){
	clearError();

	//Create databases within the thread
	networkDao = new NetworkDao(networkDBInfo);
	archiveDao = new ArchiveDao(archiveDBInfo);
	analysisDao = new AnalysisDao(analysisDBInfo);

	try{
		switch(taskID){
			case CLEAR_DATABASES_TASK:
				networkDao->deleteAllNetworks();
			break;
			default:
				setError("TaskID not recognized.");
		}
	}
	catch(SpikeStreamException& ex){
		setError(ex.getMessage());
	}
	catch(...){
		setError("DatabaseManager: unknown exception.");
	}
	cleanUp();
}


/*! Clears the error state of the class. */
void DatabaseManager::clearError(){
	error = false;
	errorMessage = "";
}


/*! Sets the class in an error state. Used when running as a thread. */
void DatabaseManager::setError(const QString &errMsg){
	errorMessage = errMsg;
	error = true;
}


void DatabaseManager::cleanUp(){
	if(networkDao != NULL)
		delete networkDao;
	if(archiveDao != NULL)
		delete archiveDao;
	if(analysisDao != NULL)
		delete analysisDao;

	networkDao = NULL;
	archiveDao = NULL;
	analysisDao = NULL;
}


