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

			void addConnectionGroups(QList<ConnectionGroup*>& connectionGroupList);
			void addNeuronGroups(QList<NeuronGroup*>& neuronGroupList);
			void cancel();
			void clearError();
			bool containsNeuron(unsigned int neurID);
			bool containsNeuronGroup(unsigned int neuronGroupID);//UNTESTED
			void copyTempWeightsToWeights(unsigned conGrpID);//UNTESTED
			void deleteConnectionGroups(QList<unsigned int>& deleteConGrpIDList);
			void deleteNeuronGroups(QList<unsigned int>& deleteNeurGrpIDList);
			Box getBoundingBox();
			unsigned int getID() { return info.getID(); }
			QString getErrorMessage();
			int getConnectionCount(unsigned conGrpID);
			int getConnectionGroupCount() { return connGrpMap.size(); }
			QList<ConnectionGroup*> getConnectionGroups();
			ConnectionGroup* getConnectionGroup(unsigned int id);
			ConnectionGroupInfo getConnectionGroupInfo(unsigned int id);
			QList<ConnectionGroupInfo> getConnectionGroupsInfo(unsigned int synapseTypeID);
			QList<ConnectionGroupInfo> getConnectionGroupsInfo();
			int getNeuronGroupCount() { return neurGrpMap.size(); }
			Box getNeuronGroupBoundingBox(unsigned int neurGrpID);
			NeuronGroup* getNeuronGroupFromNeuronID(unsigned neuronID);
			QList<unsigned int> getNeuronGroupIDs();
			QString getProgressMessage();
			QList<unsigned int> getConnectionGroupIDs();
			NeuronGroup* getNeuronGroup(unsigned int id);
			QList<NeuronGroup*> getNeuronGroups();
			NeuronGroupInfo getNeuronGroupInfo(unsigned int id);
			QList<NeuronGroupInfo> getNeuronGroupsInfo();
			QList<NeuronGroupInfo> getNeuronGroupsInfo(unsigned int neuronTypeID);
			int getNumberOfCompletedSteps();
			int getTotalNumberOfSteps();
			bool isBusy();
			bool isError() { return error; }
			bool isPrototypeMode() { return prototypeMode; }
			bool isSaved();
			bool isTransient() { return transient; }
			bool hasArchives();
			void load();
			void loadWait();
			bool overlaps(const Box& box);
			void save();
			void setError(const QString& errorMsg);
			void setConnectionGroupProperties(unsigned conGrpID, const QString& description);
			void setNeuronGroupParameters(unsigned neurGrpID, QHash<QString, double> paramMap);
			void setNeuronGroupProperties(unsigned neurGrpID, const QString& name, const QString& description);
			void setPrototypeMode(bool mode);
			void setTransient(bool transient) { this->transient = transient; }
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

			/*! Message giving feedback about current task. */
			QString progressMessage;

			/*! List of neuron groups that are being added to the network */
			QHash<unsigned, NeuronGroup*> newNeuronGroupMap;

			/*! List of connection groups that are being added to the network */
			QHash<unsigned, ConnectionGroup*> newConnectionGroupMap;

			/*! List of ids of neuron groups to delete */
			QList<unsigned> deleteNeuronGroupIDs;

			/*! List of ids of connection groups to delete */
			QList<unsigned> deleteConnectionGroupIDs;

			/*! Current task being undertaken by the neuron thread.*/
			int currentNeuronTask;

			/*! Current task being undertaken by the connection thread.*/
			int currentConnectionTask;

			/*! Records whether network is in prototype mode.
				In prototype mode the network is loaded into memory and changes are not immediately
				saved to the database. The network has to be saved before archives or analyses
				can be carried out or the weights saved. */
			bool prototypeMode;

			/*! Records if the network is transient or not.
				A transient network that has not been saved will be deleted from the database
				when SpikeStream shuts down. */
			bool transient;

			/*! Set to true when the parameters of a neuron group that is already stored are changed. */
			bool neuronGroupParametersChanged;

			/*! Set to true when the parameters of a connection group that is already stored are changed. */
			bool connectionGroupParametersChanged;

			/*! Map holding IDs of connection groups whose weights are not saved */
			QHash<unsigned, bool> volatileConnectionGroupMap;

			static const int ADD_NEURONS_TASK = 1;
			static const int DELETE_CONNECTIONS_TASK = 2;
			static const int DELETE_NEURONS_TASK = 3;
			static const int LOAD_NEURONS_TASK = 4;
			static const int ADD_CONNECTIONS_TASK = 5;
			static const int LOAD_CONNECTIONS_TASK = 6;
			static const int SAVE_NETWORK_TASK = 7;

			/*! Records when a connections related task has been cancelled. */
			bool connectionTaskCancelled;

			/*! Records when a neuron related task has been cancelled. */
			bool neuronTaskCancelled;


			//============================  METHODS  ==============================
			void checkConnectionGroupID(unsigned int id);
			void checkNeuronGroupID(unsigned int id);
			void checkThread();
			void clearConnectionGroups();
			void clearNeuronGroups();
			void deleteConnectionGroupFromMemory(unsigned conGrpID);
			void deleteNeuronGroupFromMemory(unsigned neurGrpID);
			bool filterConnection(Connection* connection, unsigned connectionMode);
			unsigned getTemporaryConGrpID();
			unsigned getTemporaryNeurGrpID();
			void initializeVariables();
			void loadConnectionGroupsInfo();
			void loadNeuronGroupsInfo();
			void startAddConnectionGroups();
			void startAddNeuronGroups();
			void startDeleteConnectionGroups(QList<unsigned>& deleteConGrpIDList);
			void startDeleteNeuronGroups(QList<unsigned>& deleteNeurGrpIDList);
			void updateConnectionGroupsAfterSave();
			void updateNeuronGroupsAfterSave();

    };

}

#endif//NETWORK_H

