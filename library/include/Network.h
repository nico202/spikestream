#ifndef NETWORK_H
#define NETWORK_H

//SpikeStream includes
#include "ArchiveDao.h"
#include "Box.h"
#include "NetworkDao.h"
#include "NetworkDaoThread.h"
#include "NetworkInfo.h"
#include "NeuronGroup.h"
#include "NeuronGroupInfo.h"
#include "ConnectionGroup.h"
#include "ConnectionGroupInfo.h"
#include "RGBColor.h"
using namespace spikestream;

namespace spikestream {

    /*! Class holding information about a neural network in the database.
	This class is linked to the database to enable lazy loading of connections and neurons.
	This is a read only class. All adding to the database should be done through the networkDao
	and then this class should be reloaded.
	NOTE: This class is not thread safe because of its database dependency.
	The network dao needs to be set appropriately if using it from a different thread */
    class Network : public QObject {
	Q_OBJECT

	public:
	    Network(NetworkDao* networkDao, ArchiveDao* archiveDao, const QString& name, const QString& description);
	    Network(const NetworkInfo& networkInfo, NetworkDao* networkDao, ArchiveDao* archiveDao);
	    ~Network();

	    void addConnectionGroups(QList<ConnectionGroup*>& connectionGroupList, bool checkNetworkLocked=true);
	    void addNeuronGroups(QList<NeuronGroup*>& neuronGroupList);
	    void cancel();
	    void clearError() { error = false; errorMessage = ""; }
	    bool connectionGroupIsLoaded(unsigned int connGrpID);
	    bool containsNeuron(unsigned int neurID);
	    Box getBoundingBox();
	    unsigned int getID() { return info.getID(); }
	    QString getErrorMessage() { return  errorMessage; }
	    int getConnectionGroupCount() { return connGrpMap.size(); }
	    ConnectionGroup* getConnectionGroup(unsigned int id);
	    ConnectionGroupInfo getConnectionGroupInfo(unsigned int id);
	    QList<ConnectionGroupInfo> getConnectionGroupsInfo();
	    int getNumberOfToConnections(unsigned int neuronID);
	    int getTotalNumberOfSteps() { return totalNumberOfSteps; }
	    int getNumberOfCompletedSteps();
	    int getNeuronGroupCount() { return neurGrpMap.size(); }
	    Box getNeuronGroupBoundingBox(unsigned int neurGrpID);
	    QList<unsigned int> getNeuronGroupIDs();
	    QList<unsigned int> getConnectionGroupIDs();
	    NeuronGroup* getNeuronGroup(unsigned int id);
	    NeuronGroupInfo getNeuronGroupInfo(unsigned int id);
	    QList<NeuronGroupInfo> getNeuronGroupsInfo();
	    bool isBusy();
	    bool isError() { return error; }
	    bool isLocked();
	    void load();
	    bool neuronGroupIsLoaded(unsigned int neurGrpID);
	    void setNetworkDao(NetworkDao* netDao) { this->networkDao = netDao; }
	    int size();

	signals:
	    void taskFinished();

	private slots:
	    void connectionThreadFinished();
	    void neuronThreadFinished();

	private:
	    //===========================  VARIABLES  ===========================
	    /*! Information about the network */
	    NetworkInfo info;

	    /*! Access to the database layer. Stored here to enable dynamic loading
		of neurons and connections on request.
		NOTE: This will have to be changed if the network is passed between threads. */
	    NetworkDao* networkDao;

	    /*! Access to the database layer. Stored here to enable network to prevent modifications
		if it is associated with archives.
		NOTE: This will have to be changed if the network is passed between threads. */
	    ArchiveDao* archiveDao;

	    /*! Hash map of the neuron groups in the network */
	    QHash<unsigned int, NeuronGroup*> neurGrpMap;

	    /*! Hash map of the connection groups in the network */
	    QHash<unsigned int, ConnectionGroup*> connGrpMap;

	    /*! Used for painting and highlighting the neurons.
		Neurons with an entry in this map are painted with the specified color */
	    QHash<unsigned int, RGBColor*> neuronColorMap;

	    /*! Records whether this class is carrying out operations with a separate thread. */
	    bool threadRunning;

	    /*! NetworkDaoThread to run in the background and handle neuron loading. */
	    NetworkDaoThread* neuronNetworkDaoThread;

	    /*! NetworkDaoThread to run in the background and handle connection loading. */
	    NetworkDaoThread* connectionNetworkDaoThread;

	    /*! The total number of steps that the task involves. */
	    int totalNumberOfSteps;

	    /*! Set to true when there is an error loading  */
	    bool error;

	    /*! Loading error message */
	    QString errorMessage;

	    /*! List of neuron groups that are being added to the network */
	    QList<NeuronGroup*> newNeuronGroups;

	    /*! Current task being undertaken by the neuron thread.*/
	    int currentNeuronTask;

	    /*! Current task being undertaken by the connection thread.*/
	    int currentConnectionTask;

	    static const int ADD_NEURONS_TASK = 1;
	    static const int LOAD_NEURONS_TASK = 2;
	    static const int ADD_CONNECTIONS_TASK = 3;
	    static const int LOAD_CONNECTIONS_TASK = 4;



	    //============================  METHODS  ==============================
	    void checkNeuronGroupID(unsigned int id);
	    void checkConnectionGroupID(unsigned int id);
	    void deleteConnectionGroups();
	    void deleteNeuronGroups();
	    void loadConnectionGroupsInfo();
	    void loadNeuronGroupsInfo();

    };

}

#endif//NETWORK_H
