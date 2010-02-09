#ifndef NETWORKSBUILDER_H
#define NETWORKSBUILDER_H

//SpikeStream includes
#include "NetworkDaoThread.h"
using namespace spikestream;


namespace spikestream {

	class NetworksBuilder {
		public:
			NetworksBuilder();
			virtual ~NetworksBuilder();

		protected:
			//=====================  METHODS  ===========================
			virtual void addTraining(unsigned int neuronID, QString trainingStr, bool output);
			virtual void addConnectionGroup(unsigned int networkID, ConnectionGroup& connGrp);
			virtual void runThread(NetworkDaoThread& thread);
		};

}

#endif//NETWORKSBUILDER_H

