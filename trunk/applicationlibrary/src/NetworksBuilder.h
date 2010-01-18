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
	    void addTraining(unsigned int neuronID, QString trainingStr, bool output);
	    void addConnectionGroup(unsigned int networkID, ConnectionGroup& connGrp);
	    void runThread(NetworkDaoThread& thread);
    };

}

#endif//NETWORKSBUILDER_H

