#include "NeuronGroup.h"
#include "SpikeStreamException.h"
using namespace spikestream;

#include <QDebug>


/*! Constructor */
NeuronGroup::NeuronGroup(const NeuronGroupInfo& info){
	this->info = info;
	neuronMap = new NeuronMap();
	loaded = false;
	startNeuronID = 0;
	calculateBoundingBox = false;
}


/*! Destructor */
NeuronGroup::~NeuronGroup(){
	//Deletes neuron map and all its dynamically allocated objects
	if(neuronMap != NULL){
		clearNeurons();
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
	(*neuronMap)[getTemporaryID()] = tmpNeuron;
	neuronGroupChanged();
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
	neuronGroupChanged();
}


/*! Clears all of the neurons that are currently loaded */
void NeuronGroup::clearNeurons(){
	NeuronMap::iterator endNeuronMap = neuronMap->end();
	for(NeuronMap::iterator iter = neuronMap->begin(); iter != endNeuronMap; ++iter){
		delete iter.value();
	}
	neuronMap->clear();
	neuronGroupChanged();
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

/*! Returns the bounding box of the neuron group.
	Only calculates bounding box if neuron group has changed. */
Box NeuronGroup::getBoundingBox(){
	//Return current box if nothing has changed about the neuron group
	if(!calculateBoundingBox)
		return boundingBox;

	//Calculate the bounding box
	Neuron* tmpNeuron;
	bool firstTime = true;
	NeuronMap::iterator mapEnd = neuronMap->end();//Saves accessing this function multiple times
	for(NeuronMap::iterator iter=neuronMap->begin(); iter != mapEnd; ++iter){
		tmpNeuron = iter.value();
		if(firstTime){//Take first neuron as starting point
			boundingBox = Box(tmpNeuron->getXPos(), tmpNeuron->getYPos(), tmpNeuron->getZPos(),
								tmpNeuron->getXPos(), tmpNeuron->getYPos(), tmpNeuron->getZPos()
							);
			firstTime = false;
		}
		else{//Expand box to include subsequent neurons
			if(tmpNeuron->getXPos() < boundingBox.x1)
				boundingBox.x1 = tmpNeuron->getXPos();
			if(tmpNeuron->getYPos() < boundingBox.y1)
				boundingBox.y1 = tmpNeuron->getYPos();
			if(tmpNeuron->getZPos() < boundingBox.z1)
				boundingBox.z1 = tmpNeuron->getZPos();

			if(tmpNeuron->getXPos() > boundingBox.x2)
				boundingBox.x2 = tmpNeuron->getXPos();
			if(tmpNeuron->getYPos() > boundingBox.y2)
				boundingBox.y2 = tmpNeuron->getYPos();
			if(tmpNeuron->getZPos() > boundingBox.z2)
				boundingBox.z2 = tmpNeuron->getZPos();
		}
	}

	//Return calculated box
	calculateBoundingBox = false;
	return boundingBox;
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


/*! Replaces the neuron map with a new neuron map.
	Neurons are not cleaned up because they might be included in the new map. */
void NeuronGroup::setNeuronMap(NeuronMap* newMap){
	delete neuronMap;
	this->neuronMap = newMap;
	neuronGroupChanged();
}


/*! Returns the number of neurons in the group. */
int NeuronGroup::size(){
	return neuronMap->size();
}


/*--------------------------------------------------------- */
/*-----                PRIVATE METHODS                ----- */
/*--------------------------------------------------------- */

/*! Returns a temporary ID that does not exist in the current hash map */
unsigned NeuronGroup::getTemporaryID(){
	unsigned tmpID = (unsigned int)neuronMap->size();
	while(neuronMap->contains(tmpID))
		++tmpID;
	return tmpID;
}


/*! Records that neuron group has changed to trigger bounding box recalculation. */
void NeuronGroup::neuronGroupChanged(){
	calculateBoundingBox = true;
}
