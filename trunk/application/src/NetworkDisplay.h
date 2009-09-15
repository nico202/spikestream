#ifndef NETWORKDISPLAY_H
#define NETWORKDISPLAY_H

namespace spikestream {

    /*! Holds display information about the current network in global scope,
	including which neuron and connection groups are displayed, colours etc. */
    class NetworkDisplay {

	    QList<unsigned int>& getConnectionGroupIDs() { return connectionGroupIDs; }
	    QList<unsigned int>& getNeuronGroupIDs() { return neuronGroupIDs; }


	private:
	    QList<unsigned int> neuronGroupIDs;
	    QList<unsigned int> connectionGroupIDs;


    };

}


#endif//NETWORKDISPLAY_H
