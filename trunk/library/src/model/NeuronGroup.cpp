#include "NeuronGroup.h"
using namespace spikestream;

/*! Constructor */
NeuronGroup::NeuronGroup(const NeuronGroupInfo& info){
    this->info = info;
    neuronMap = NULL;
}


/*! Destructor */
NeuronGroup::~NeuronGroup(){
    if(neuronMap != NULL)
	delete neuronMap;
}


/*--------------------------------------------------------- */
/*-----                PUBLIC METHODS                 ----- */
/*--------------------------------------------------------- */

/*! Returns true if information about neurons has been added
    to the neuron group. */
bool NeuronGroup::neuronsLoaded(){
    if(neuronMap == NULL)
	return false;
    return true;
}








