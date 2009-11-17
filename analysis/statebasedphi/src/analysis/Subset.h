#ifndef SUBSET_H
#define SUBSET_H

//Qt includes
#include <QList>

namespace spikestream {

    class Subset {
	public:
	    Subset(int networkSize);
	    ~Subset();
	    void addNeuronIndex(int index);
	    bool contains(Subset* subset);
	    QList<unsigned int> getNeuronIDs();

	private:
	    //=====================  VARIABLES  ===================
	    /*! Stores the indexes of the subset neurons. The index positions
		refer to the neuron id list */
	    unsigned int* neurIndxArray;

	    /*! The length of the neuron index array */
	    int neurIndxSize;

	    /*! Pointer to the list of neuron ids in the network */
	    unsigned int* neuronIDListPtr;

	    /*! The phi of the subset */
	    double phi;

	    /*! The number of neurons in the subset */
	    int subsetSize;

	    /*! The number of neurons in the network  */
	    int networkSize;

    };

}

#endif//SUBSET_H

