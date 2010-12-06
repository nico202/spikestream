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
			void run();
			void startLoadNetwork(Network* network);

		signals:
			void progress(int stepsCompleted, int totalSteps, QString message);

		private:
			//================  VARIABLES  ==================
			/*! The network that is being loaded */
			Network* network;
	};


}

#endif//NETWORKMANAGER_H
