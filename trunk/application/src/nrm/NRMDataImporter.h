#ifndef NRMDATAIMPORTER_H
#define NRMDATAIMPORTER_H

//SpikeStream includes
#include "DBInfo.h"
#include "NRMDataSet.h"
#include "NRMNetwork.h"
#include "Network.h"
#include "NetworkDaoThread.h"
using namespace spikestream;

//Qt includes
#include <QString>
#include <QThread>

namespace spikestream {

    class NRMDataImporter : public QThread {
	public:
	    NRMDataImporter(const DBInfo& networkDBInfo, const DBInfo& archiveDBInfo);
	    ~NRMDataImporter();
	    bool isError(){ return error; }
	    QString getErrorMessage() { return errorMessage; }
	    void prepareAddArchives(NRMNetwork* nrmNetwork, Network* network, NRMDataSet* nrmDataset);
	    void prepareAddConnections(NRMNetwork* nrmNetwork, Network* network);
	    void run();
	    void stop();

	private:
	    //========================  VARIABLES  ==========================
	    /*! Used to add connections to the database */
	    NetworkDaoThread* networkDaoThread;

	    /*! Information about the archive database */
	    DBInfo archiveDBInfo;

	    /*! Records which task is being undertaken - task IDs are defined below */
	    unsigned int currentTask;

	    /*! Task of adding connections to the database */
	    static const int ADD_CONNECTIONS_TASK = 1;

	    /*! Task of adding archives to the database */
	    static const int ADD_ARCHIVES_TASK = 2;

	    /*! When set to true the run method is exited. */
	    bool stopThread;

	    /*! Exceptions do not work across threads, so need to record when an error takes place when
		 the thread is executing a task.*/
	    bool error;

	    /*! Error message */
	    QString errorMessage;

	    /*! NRM Network containing the data being imported */
	    NRMNetwork* nrmNetwork;

	    /*! SpikeStream network with all the neuron groups and which handles the database stuff */
	    Network* network;

	    /*! NRM data set */
	    NRMDataSet* nrmDataset;


	    //==========================  METHODS  ===========================
	    void addArchives();
	    void addConnections();
	    void clearError();
	    void setError(const QString& errMsg);
    };

}

#endif//NRMDATAIMPORTER_H

