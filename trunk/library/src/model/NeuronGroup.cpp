#include "NeuronGroup.h"
#include "SpikeStreamException.h"
using namespace spikestream;

/*! Constructor */
NeuronGroup::NeuronGroup(const NeuronGroupInfo& info){
	this->info = info;
	neuronMap = new NeuronMap();
	loaded = false;
	startNeuronID = 0;
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
Neuron* NeuronGroup::addNeuron(float xPos, float yPos, float zPos){
	Neuron* tmpNeuron = new Neuron(xPos, yPos, zPos);
	if(neuronMap->contains((unsigned int)neuronMap->size()))
		throw SpikeStreamException("Temporary neuron ID conflicts with neuron ID already in group.");
	(*neuronMap)[(unsigned int) neuronMap->size()] = tmpNeuron;
	return tmpNeuron;
}


/*! Adds a layer to the group with the specified width and height.
	Temporary neuron ids are used and the neurons are appended to the neurons already in the group. */
void NeuronGroup::addLayer(int width, int height, int xPos, int yPos, int zPos){
	for(int x=xPos; x < (xPos + width); ++x){
		for(int y=yPos; y < (yPos + height); ++y){
			addNeuron(x, y, zPos);
		}
	}
}


/*! Clears all of the neurons that are currently loaded */
void NeuronGroup::clearNeurons(){
	NeuronMap::iterator endNeuronMap = neuronMap->end();
	for(NeuronMap::iterator iter = neuronMap->begin(); iter != endNeuronMap; ++iter){
		delete iter.value();
	}
	neuronMap->clear();
}


/*! Returns true if the neuron group contains a neuron with the specified id. */
bool NeuronGroup::contains(unsigned int neurID){
	if(neuronMap->contains(neurID))
		return true;
	return false;
}


/*! Returns true if a neuron with this specification exists in this group */
bool NeuronGroup::contains(unsigned int neurID, float x, float y, float z){
	if(!neuronMap->contains(neurID))
		return false;;
	if((*neuronMap)[neurID]->getXPos() != x || (*neuronMap)[neurID]->getYPos() != y || (*neuronMap)[neurID]->getZPos() != z)
		return false;
	return true;
}


/*! Returns the ID of the neuron at a specified location */
unsigned int NeuronGroup::getNeuronAtLocation(const Point3D& point){
	NeuronMap::iterator mapEnd = neuronMap->end();//Saves accessing this function multiple times
	for(NeuronMap::iterator iter=neuronMap->begin(); iter != mapEnd; ++iter){
		if(iter.value()->getLocation() == point)
			return iter.key();
	}
	throw SpikeStreamException("No neuron at location "+ point.toString());
}


/*! Returns the location of the neuron with the specified ID */
Point3D& NeuronGroup::getNeuronLocation(unsigned int neuronID){
	if(!neuronMap->contains(neuronID))
		throw SpikeStreamException("Neuron ID '" + QString::number(neuronID) + "' could not be found.");
	return (*neuronMap)[neuronID]->getLocation();
}


/*! Returns a parameter with the specified key.
	Throws an exception if the parameter cannot be found */
double NeuronGroup::getParameter(const QString &key){
	if(!parameterMap.contains(key))
		throw SpikeStreamException("Cannot find parameter with key: " + key + " in neuron group with ID " + QString::number(info.getID()));
	return  parameterMap[key];
}


/*! Returns the number of neurons in the group. */
int NeuronGroup::size(){
	return neuronMap->size();
}



