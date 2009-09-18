#include "NeuronGroup.h"
#include "SpikeStreamException.h"
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

/*! Adds a neuron to the group using a temporary ID. This ID is replaced
    by the actual ID when the group is added to the network and database. */
void addNeuron(float xPos, float yPos, float zPos){
    Point3D tmpPoint = new Point3D(xPos, yPos, zPos);
    neuronMap[neuronMap->size()] = tmpPoint;
}


/*! Clears all of the neurons that are currently loaded */
void NeuronGroup::clearNeurons(){
    NeuronMap::iterator endNeuronMap = neuronMap->end();
    for(NeuronMap::iterator iter = neuronMap->begin(); iter != endNeuronMap; ++iter){
	delete iter.value();
    }
    neuronMap->clear();
}


/*! Returns true if a neuron with this specification exists in this group */
bool NeuronGroup::contains(unsigned int neurID, float x, float y, float z){
    if(!neuronMap->contains(neurID))
	return false;;
    if((*neuronMap)[neurID]->xPos != x || (*neuronMap)[neurID]->yPos != y || (*neuronMap)[neurID]->zPos != z)
	return false;
    return true;
}


/*! Returns the number of neurons in the group.
    A SpikeStreamException is thrown if the neuron group has not been loaded. */
int NeuronGroup::size(){
    if(!isLoaded())
	throw SpikeStreamException("size() cannot be called on a neuron group that has not been loaded!");
    return neuronMap->size();
}



