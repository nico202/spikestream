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
		This class is linked to the database to enable lazy loading of connections and neurons. */
    class Network : public QObject {
		Q_OBJECT

		public:
			Network(const QString& name, const QString& description, const DBInfo& networkDBInfo, const DBInfo& archiveDBInfo);
			Network(const NetworkInfo& networkInfo, const DBInfo& networkDBInfo, const DBInfo& archiveDBInfo);
			~Network();

			void addConnectionGroups(QList<ConnectionGroup*>& connectionGroupList, bool checkNetworkLocked=true);
			void addNeuronGroups(QList<NeuronGroup*>& neuronGroupList);
			void cancel();
			void clearError();
			bool connectionGroupIsLoaded(unsigned int connGrpID);
			bool containsNeuron(unsigned int neurID);
			bool containsNeuronGroup(unsigned int neuronGroupID);//UNTESTED
			void deleteConnectionGroups(QList<unsigned int>& conGrpIDList);
			void deleteNeuronGroups(QList<unsigned int>& neurGrpIDList);
			Box getBoundingBox();
			unsigned int getID() { return info.getID(); }
			QString getErrorMessage();
			int getConnectionGroupCount() { return connGrpMap.size(); }
			QList<ConnectionGroup*> getConnectionGroups();
			ConnectionGroup* getConnectionGroup(unsigned int id);
			ConnectionGroupInfo getConnectionGroupInfo(unsigned int id);
			QList<ConnectionGroupInfo> getConnectionGroupsInfo(unsigned int synapseTypeID);
			QList<ConnectionGroupInfo> getConnectionGroupsInfo();
			QList<Connection*> getConnections(unsigned int connectionMode, unsigned int singleNeuronID, unsigned int toNeuronID);
			int getNeuronGroupCount() { return neurGrpMap.size(); }
			Box getNeuronGroupBoundingBox(unsigned int neurGrpID);
			QList<unsigned int> getNeuronGroupIDs();
			QList<unsigned int> getConnectionGroupIDs();
			NeuronGroup* getNeuronGroup(unsigned int id);
			QList<NeuronGroup*> getNeuronGroups();
			NeuronGroupInfo getNeuronGroupInfo(unsigned int id);
			QList<NeuronGroupInfo> getNeuronGroupsInfo();
			QList<NeuronGroupInfo> getNeuronGroupsInfo(unsigned int neuronTypeID);
			int getNumberOfToConnections(unsigned int neuronID);
			int getNumberOfCompletedSteps();
			int getTotalNumberOfSteps();
			bool isBusy();
			bool isError() { return error; }
			bool hasArchives();
			void load();
			void loadWait();
			bool neuronGroupIsLoaded(unsigned int neurGrpID);
			void setError(const QString& errorMsg);
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

			/*! Hash map of the neuron groups in the network */
			QHash<unsigned int, NeuronGroup*> neurGrpMap;

			/*! Holds information about the network database */
			DBInfo networkDBInfo;

			/*! Holds information about the archive database */
			DBInfo archiveDBInfo;

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

			/*! Set to true when there is an error loading  */
			bool error;

			/*! Loading error message */
			QString errorMessage;

			/*! List of neuron groups that are being added to the network */
			QList<NeuronGroup*> newNeuronGroups;

			/*! List of connection groups that are being added to the network */
			QList<ConnectionGroup*> newConnectionGroups;

			/*! List of ids of neuron groups to delete */
			QList<unsigned int> deleteNeuronGroupIDs;

			/*! List of ids of connection groups to delete */
			QList<unsigned int> deleteConnectionGroupIDs;

			/*! Current task being undertaken by the neuron thread.*/
			int currentNeuronTask;

			/*! Current task being undertaken by the connection thread.*/
			int currentConnectionTask;

			static const int ADD_NEURONS_TASK = 1;
			static const int DELETE_CONNECTIONS_TASK = 2;
			static const int DELETE_NEURONS_TASK = 3;
			static const int LOAD_NEURONS_TASK = 4;
			static const int ADD_CONNECTIONS_TASK = 5;
			static const int LOAD_CONNECTIONS_TASK = 6;


			//============================  METHODS  ==============================
			void checkConnectionGroupID(unsigned int id);
			void checkNeuronGroupID(unsigned int id);
			void checkThread();
			void deleteConnectionGroups();
			void deleteNeuronGroups();
			bool filterConnection(Connection* connection, unsigned connectionMode);
			void loadConnectionGroupsInfo();
			void loadNeuronGroupsInfo();

    };

}

#endif//NETWORK_H

