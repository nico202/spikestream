#ifndef SUBSET_H
#define SUBSET_H

//Qt includes
#include <QList>

namespace spikestream {

    class Subset {
	public:
	    Subset(const QList<unsigned int>* neuronIDListPtr);
	    ~Subset();
	    void addNeuronIndex(int index);
	    bool contains(Subset* subset);
	    QList<unsigned int> getNeuronIDs();
	    double getPhi() { return phi; }
	    void setPhi(double newPhi) { this->phi = newPhi; }
	    int size() { return subsetSize; }

	private:
	    //=====================  VARIABLES  ===================
	    /*! Stores the indexes of the subset neurons. The index positions
		refer to the neuron id list */
	    unsigned int* neurIndxArray;

	    /*! The length of the neuron index array */
	    int neurIndxSize;

	    /*! Pointer to the list of neuron ids in the network */
	    const QList<unsigned int>* neuronIDListPtr;

	    /*! The phi of the subset */
	    double phi;

	    /*! The number of neurons in the subset */
	    int subsetSize;

	    /*! The number of neurons in the network  */
	    int networkSize;

    };

}

#endif//SUBSET_H

