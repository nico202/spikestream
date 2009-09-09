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
	    NetworkDaoThread(const DBInfo& dbInfo, unsigned int networkID, ConnectionGroup* connGrp);
	    NetworkDaoThread(const DBInfo& dbInfo, unsigned int networkID, NeuronGroup* neurGrp);
	    ~NetworkDaoThread();
	    void run();
	    void stop();


	private:
	    //========================  VARIABLES  =========================
	    NeuronGroup* neuronGroup;
	    ConnectionGroup* connectionGroup;
	    unsigned int networkID;
	    unsigned int currentTask;

	    bool stopThread;

	    const static unsigned int NO_TASK_DEFINED = 1;
	    const static unsigned int ADD_CONNECTION_GROUP_TASK = 2;
	    const static unsigned int ADD_NEURON_GROUP_TASK = 3;

	    //========================  METHODS  ===========================
	    void addNeuronGroup();
	    void addConnectionGroup();
    };

}

#endif//NETWORKDAOTHREAD_H
