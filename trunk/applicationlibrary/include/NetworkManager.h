#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

//SpikeStream includes
#include "Network.h"
#include "SpikeStreamThread.h"

namespace spikestream {

	/*! Responsible for running heavy network-related tasks in a separate thread.
		Currently only loads networks, but should be made responsible for all tasks. */
	class NetworkManager : public SpikeStreamThread {
		Q_OBJECT

		public:
			NetworkManager();
			~NetworkManager();
			void cancel();
			int getCurrentTask() { return currentTask; }
			void run();
			void startDeleteNetwork(unsigned networkID);
			void startLoadNetwork(Network* network);
			void startSaveNetwork(Network* network);

			//================  VARIABLES  ==================
			/*! undefined task id */
			static const int UNDEFINED_TASK = 0;

			/*! Deleting network task. */
			static const int DELETE_NETWORK_TASK = 1;

			/*! Task of loading the network */
			static const int LOAD_NETWORK_TASK = 2;

			/*! Task of saving the network */
			static const int SAVE_NETWORK_TASK = 3;


		signals:
			void progress(int stepsCompleted, int totalSteps, QString message, bool showCancelButton);


		private:
			//================  VARIABLES  ==================
			/*! The network that is being loaded or saved. */
			Network* network;

			/*! ID of the network that is being deleted */
			unsigned networkID;

			/*! Current task being undertaken by the thread. */
			int currentTask;


	};


}

#endif//NETWORKMANAGER_H
