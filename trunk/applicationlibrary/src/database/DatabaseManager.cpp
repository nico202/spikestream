//SpikeStream includes
#include "DatabaseManager.h"
#include "NetworkDaoThread.h"
#include "ArchiveDao.h"
#include "AnalysisDao.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>


/*! Constructor */
DatabaseManager::DatabaseManager(const DBInfo& networkDBInfo, const DBInfo& archiveDBInfo, const DBInfo& analysisDBInfo) : SpikeStreamThread() {
	this->networkDBInfo = networkDBInfo;
	this->archiveDBInfo = archiveDBInfo;
	this->analysisDBInfo = analysisDBInfo;
}


/*! Destuctor */
DatabaseManager::~DatabaseManager(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Sets up class for the clear databases task and starts thread running. */
void DatabaseManager::startClearDatabases(){
	taskID = CLEAR_DATABASES_TASK;
	start();
}


/*! Run method inherited from QThread */
void DatabaseManager::run(){
	clearError();

	//Create databases within the thread
	NetworkDaoThread networkDaoThread(networkDBInfo);
	ArchiveDao archiveDao(archiveDBInfo);
	AnalysisDao analysisDao(analysisDBInfo);

	try{
		switch(taskID){
			case CLEAR_DATABASES_TASK:{
				analysisDao.deleteAllAnalyses();
				archiveDao.deleteAllArchives();
				QList<NetworkInfo> netInfoList = networkDaoThread.getNetworksInfo();
				foreach(NetworkInfo netInfo, netInfoList){
					networkDaoThread.startDeleteNetwork(netInfo.getID());
					networkDaoThread.wait();
				}
			}
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
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/



