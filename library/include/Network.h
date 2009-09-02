#ifndef NETWORK_H
#define NETWORK_H

//SpikeStream includes
#include "NeuronGroup.h"
#include "NeuronGroupInfo.h"
#include "ConnectionGroup.h"
#include "ConnectionGroupInfo.h"


namespace SpikeStream {

    class Network {
	public:
	    Network(unsigned int id, NetworkDao* networkDao);
	    ~Network();

	    Volume getMinimumBoundingCuboid();

	    QList<unsigned int> getNeuronGroupIDs();
	    QList<unsigned int> getConnectionGroupIDs();
	    NeuronGroup* getNeuronGroup(unsigned int id);
	    NeuronGroupInfo* getNeuronGroupInfo(unsigned int id);
	    ConnectionGroup* getConnectionGroup(unsigned int id);
	    ConnectionGroupInfo* getConnectionGroupInfo(unsigned int id);

	    void setNetworkDao(NetworkDao* netDao);

	private:
	    //===========================  VARIABLES  ===========================
	    /*! Access to the database layer. Stored here to enable dynamic loading
		of neurons and connections on request.
		NOTE: This will have to be changed if the network is passed between threads. */
	    NetworkDao* networkDao;

	    /*! Hash map of the neuron groups in the network */
	    QHash<NeuronGroup*> neurGrpHash;

	    /*! Hash map of the connection groups in the network */
	    QHash<ConnectionGroup*> connGrpHash;

	    /*! The database ID of the network */
	    unsigned int networkID;
    };

}

#endif//NETWORK_H

