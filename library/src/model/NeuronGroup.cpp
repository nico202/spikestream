#include "NeuronGroup.h"
using namespace spikestream;

/*! Constructor */
NeuronGroup::NeuronGroup(const NeuronGroupInfo& info){
    this->info = info;
    neuronMap = new NeuronMap();
    loaded = false;
}


/*! Destructor */
NeuronGroup::~NeuronGroup(){
    //Deletes neuron map and all its dynamically allocated objects
    if(neuronMap != NULL){
	NeuronMap::iterator mapEnd = neuronMap->end();//Saves accessing this function multiple times
	for(NeuronMap::iterator iter=neuronMap->begin(); iter != mapEnd; ++iter){
	    delete iter.value();
	}
	delete neuronMap;
    }
}


/*--------------------------------------------------------- */
/*-----                PUBLIC METHODS                 ----- */
/*--------------------------------------------------------- */

/*! Returns true if a neuron with this specification exists in this group */
bool NeuronGroup::contains(unsigned int neurID, float x, float y, float z){
    if(!neuronMap->contains(neurID))
	return false;;
    if((*neuronMap)[neurID]->x != x || (*neuronMap)[neurID]->y != y || (*neuronMap)[neurID]->z != z)
	return false;
    return true;
}



