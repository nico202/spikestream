#ifndef NETWORKDAOTHREAD_H
#define NETWORKDAOTHREAD_H

//SpikeStream includes
#include "AbstractDao.h"
#include "DBInfo.h"
#include "ConnectionGroup.h"
#include "NeuronGroup.h"
using namespace spikestream;

//Qt includes
#include <QThread>


namespace spikestream {

    /*! Thread-based wrapper for the SpikeStreamNetwork database that handles
	heavy tasks that may take several minutes. */
    class NetworkDaoThread : public QThread, public AbstractDao {
	public:
	    NetworkDaoThread(const DBInfo& dbInfo);
	    ~NetworkDaoThread();
	    void clearError();
	    bool isError() { return error; }
	    bool isRunning() { return !stopThread; }
	    QString getErrorMessage() { return errorMessage; }
	    void prepareAddConnectionGroup(unsigned int networkID, ConnectionGroup* connGrp);
	    void prepareAddNeuronGroup(unsigned int networkID, NeuronGroup* neurGrp);
	    void prepareLoadConnections(QList<ConnectionGroup*>& connGrpList);
	    void prepareLoadConnections(ConnectionGroup* connGrp);
	    void prepareLoadNeurons(const QList<NeuronGroup*>& neurGrpList);
	    void prepareLoadNeurons(NeuronGroup* neurGrp);
	    void run();
	    void stop();


	private:
	    //========================  VARIABLES  =========================
	    /*! Neuron group being added to the network */
	    NeuronGroup* neuronGroup;

	    /*! Connection group being added to the network */
	    ConnectionGroup* connectionGroup;

	    /*! List of connection groups to load connections into */
	    QList<ConnectionGroup*> connectionGroupList;

	    /*! List of neuron groups to load neurons into */
	    QList<NeuronGroup*> neuronGroupList;

	    /*! Id of network to which neuron or connection group are added */
	    unsigned int networkID;

	    /*! Records which task is being undertaken - task IDs are defined below */
	    unsigned int currentTask;

	    /*! When set to true the run method is exited. */
	    bool stopThread;

	    /*! Exceptions do not work across threads, so need to record when an error takes place when
		 the thread is executing a task.*/
	    bool error;

	    /*! Error message */
	    QString errorMessage;

	    const static unsigned int NO_TASK_DEFINED = 1;
	    const static unsigned int ADD_CONNECTION_GROUP_TASK = 2;
	    const static unsigned int ADD_NEURON_GROUP_TASK = 3;
	    const static unsigned int LOAD_NEURONS_TASK = 4;
	    const static unsigned int LOAD_CONNECTIONS_TASK = 5;

	    //========================  METHODS  ===========================
	    void addNeuronGroup();
	    void addConnectionGroup();
	    void loadConnections();
	    void loadNeurons();
	    void setError(QString msg);
    };

}

#endif//NETWORKDAOTHREAD_H
