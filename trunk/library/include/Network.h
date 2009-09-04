#ifndef NETWORK_H
#define NETWORK_H

//SpikeStream includes
#include "Box.h"
#include "NetworkDao.h"
#include "NetworkInfo.h"
#include "NeuronGroup.h"
#include "NeuronGroupInfo.h"
#include "ConnectionGroup.h"
#include "ConnectionGroupInfo.h"
#include "RGBColor.h"
using namespace spikestream;

namespace spikestream {

    /*! NOTE: This class is not thread safe because of its database dependency.
	The network dao needs to be set appropriately if using it from a different thread */
    class Network {
	public:
	    Network(const NetworkInfo& networkInfo, NetworkDao* networkDao);
	    ~Network();

	    Box getMinimumBoundingBox();

	    QList<unsigned int> getNeuronGroupIDs();
	    QList<unsigned int> getConnectionGroupIDs();
	    NeuronGroup* getNeuronGroup(unsigned int id);
	    NeuronGroupInfo getNeuronGroupInfo(unsigned int id);
	    ConnectionGroup* getConnectionGroup(unsigned int id);
	    ConnectionGroupInfo getConnectionGroupInfo(unsigned int id);
	    void loadNetwork();
	    void setNetworkDao(NetworkDao* netDao);

	private:
	    //===========================  VARIABLES  ===========================
	    /*! Information about the network */
	    NetworkInfo networkInfo;

	    /*! Access to the database layer. Stored here to enable dynamic loading
		of neurons and connections on request.
		NOTE: This will have to be changed if the network is passed between threads. */
	    NetworkDao* networkDao;

	    /*! Hash map of the neuron groups in the network */
	    QHash<unsigned int, NeuronGroup*> neurGrpMap;

	    /*! Hash map of the connection groups in the network */
	    QHash<unsigned int, ConnectionGroup*> connGrpMap;

	    /*! The database ID of the network */
	    unsigned int networkID;

	    /*! Whether the network is editable or not. Networks are locked when they are associated
		with archived data and have to be copied to be editable again */
	    bool locked;

	    /*! Used for painting and highlighting the neurons.
		Neurons with an entry in this map are painted with the specified color */
	    QHash<unsigned int, RGBColor*> neuronColorMap;

	    //============================  METHODS  ==============================
	    void checkNeuronGroupID(unsigned int id);
	    void checkConnectionGroupID(unsigned int id);
	    void loadConnectionGroupsInfo();
	    void loadConnections(unsigned int connGrpId);
	    void loadNeuronGroupsInfo();
	    void loadNeurons(unsigned int neurGrpID);
    };

}

#endif//NETWORK_H

