//SpikeStream includes
#include "SpikeStreamAnalysisException.h"
#include "Subset.h"
using namespace spikestream;


/*! Constructor */
Subset::Subset(const QList<unsigned int>* neuronIDListPtr){
    this->neuronIDListPtr = neuronIDListPtr;
    networkSize = neuronIDListPtr->size();
    subsetSize = 0;

    if(networkSize % 32 == 0)
	neurIndxSize = networkSize / 32;
    else
	neurIndxSize = networkSize / 32 + 1;

    //Create and initialize array storing neuron indexes
    neurIndxArray = new unsigned int[neurIndxSize];
    for(int i=0; i<neurIndxSize; ++i){
	neurIndxArray[i] = 0;
    }
}


/*! Destructor */
Subset::~Subset(){
    delete [] neurIndxArray;
}


/*-------------------------------------------------------------*/
/*-------                 PUBLIC METHODS                 ------*/
/*-------------------------------------------------------------*/

/*! Stores the index position of the neuron in a format that is efficient
    for the contains operation */
void Subset::addNeuronIndex(int index){
    //Break index down into the array and bit positions
    int arrayPos = index / 32;
    int bitPos = index % 32;

    //Check we are in range
    if(arrayPos >= neurIndxSize)
	throw SpikeStreamAnalysisException("Array index out of bounds when adding neuron index.");

    //Flip the bit corresponding to the index
    neurIndxArray[arrayPos] |= 1<<bitPos;

    //Record the current size
    ++subsetSize;
}


/*! Returns true if this subset contains the specfied subset */
bool Subset::contains(Subset* subset){
    //Subset does not contain itself
    if(this == subset)
	return false;

    //This subset can only contain a smaller larger subset
    if(subsetSize <= subset->size())
	return false;

    //Check that network size is identical
    if(this->neurIndxSize != subset->neurIndxSize)
	throw SpikeStreamAnalysisException("Network sizes do not match when comparing subsets.");

    /* Work through the two neuron index arrays
	The AND of each entry should equal the contained subset if it is contained */
    for(int i=0; i<neurIndxSize; ++i){
	if( (neurIndxArray[i] & subset->neurIndxArray[i]) != subset->neurIndxArray[i])
	    return false;
    }
}


/*! Returns a list of the neuron ids in the subset*/
QList<unsigned int> Subset::getNeuronIDs(){
    //List to hold the ids
    QList<unsigned int> tmpList;

    //Work through the indexes and add each neuron id to the list.
    for(int i=0; i<neurIndxSize; ++i){
	//Work through all of the bits in the integer
	for(int j=0; j<32; ++j){
	    if(neurIndxArray[i] & 1<<j){
		tmpList.append( (*neuronIDListPtr)[i*32 + j] );
	    }
	}
    }

    //Return the list of neuron ids
    return tmpList;
}
