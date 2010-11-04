//SpikeStream includes
#include "GlobalVariables.h"
#include "NeuronGroup.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

//Initialize static variables
unsigned NeuronGroup::neuronIDCounter = LAST_NEURON_ID + 1;

/*! Constructor */
NeuronGroup::NeuronGroup(const NeuronGroupInfo& info){
	this->info = info;
	neuronMap = new NeuronMap();
	startNeuronID = 0;
	calculateBoundingBox = false;
	positionMapBuilt = false;
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
	//Get temporary ID for the neuron - this must be unique and not exist in the current map
	unsigned tmpID = getTemporaryID();

	//Create neuron class
	Neuron* tmpNeuron = new Neuron(tmpID, xPos, yPos, zPos);

	//Store neuron class in ID map
	if(neuronMap->contains(tmpID))
		throw SpikeStreamException("Automatically generated temporary neuron ID clashes with one in the network. New ID=" + QString::number(tmpID));
	(*neuronMap)[tmpID] = tmpNeuron;

	//Finish off
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


/*! Builds map that allows iteration by geometrically close neurons. */
void NeuronGroup::buildPositionMap(){
	neuronPositionMap.clear();

	//Work through all neurons
	NeuronMap::iterator mapEnd = neuronMap->end();
	for(NeuronMap::iterator iter=neuronMap->begin(); iter != mapEnd; ++iter){
		//Store link between neuron position and neuron class
		uint64_t tmpKey = getPositionKey(iter.value()->getXPos(), iter.value()->getYPos(), iter.value()->getZPos());
		if(neuronPositionMap.contains(tmpKey))
			throw SpikeStreamException("Position key clashes with one in the position map. Key=" + QString::number(tmpKey));
		neuronPositionMap[tmpKey] = iter.value();
	}

	//Set flag to record that we have built position map
	positionMapBuilt = true;
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

	//Expand outer edge of box so that it contains all neurons
	boundingBox.x1 += 0.1f;
	boundingBox.y1 += 0.1f;
	boundingBox.z1 += 0.1f;

	//Return calculated box
	calculateBoundingBox = false;
	return boundingBox;
}


/*! Returns the ID of the neuron group */
unsigned NeuronGroup::getID(){
	return info.getID();
}


/*! Returns the nearest neuron to the specified point.
	When more than one neurons are found, only the first is returned.
	FIXME: CURRENTLY A COMPLETE SEARCH - OPTIMIZE! */
Neuron* NeuronGroup::getNearestNeuron(const Point3D& point){
	double minDist = 0, tmpDist;
	bool firstTime;
	Neuron* closestNeuron;
	NeuronMap::iterator mapEnd = neuronMap->end();//Saves accessing this function multiple times
	for(NeuronMap::iterator iter=neuronMap->begin(); iter != mapEnd; ++iter){
		tmpDist = iter.value()->getLocation().distance(point);
		if(firstTime){
			minDist = tmpDist;
			closestNeuron = iter.value();
			firstTime = false;
		}
		else{
			if(tmpDist < minDist){
				minDist = tmpDist;
				closestNeuron = iter.value();
			}
		}
	}
	return closestNeuron;
}


/*! Returns the ID of the neuron at a specified location */
unsigned int NeuronGroup::getNeuronIDAtLocation(const Point3D& point){
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


/*! Returns neurons contained within the specified box */
QList<Neuron*> NeuronGroup::getNeurons(const Box& box){
	QList<Neuron*> tmpList;
	NeuronMap::iterator mapEnd = neuronMap->end();
	for(NeuronMap::iterator iter=neuronMap->begin(); iter != mapEnd; ++iter){
		if( box.contains(iter.value()->getLocation()) )
			tmpList.append(iter.value());
	}
	return tmpList;
}


/*! Returns the ID of the neuron group's type */
unsigned NeuronGroup::getNeuronTypeID(){
	return getInfo().getNeuronTypeID();
}


/*! Returns a parameter with the specified key.
	Throws an exception if the parameter cannot be found */
double NeuronGroup::getParameter(const QString &key){
	if(!parameterMap.contains(key))
		throw SpikeStreamException("Cannot find parameter with key: " + key + " in neuron group with ID " + QString::number(info.getID()));
	return  parameterMap[key];
}


/*! Returns a 64 bit key that encodes the position of the neuron in 3D.
	Enables topographic iteration through the neuron group.
	Can't use two's complement because want an ordered sequence
	starting  */
uint64_t NeuronGroup::getPositionKey(int xPos, int yPos, int zPos){
	if(xPos < 0 || yPos < 0 || zPos < 0)
		throw SpikeStreamException("This method currently only works with positive positions.");

	//Check positions are in range for this type of encoding
	if(xPos > 2097151 || yPos > 2097151 || zPos > 2097151)
		throw SpikeStreamException("X, Y or Z position out of range. Must be less than or equal to 2097151.");

	//Create key
	uint64_t newKey = xPos;
	newKey <<= 21;
	newKey |= yPos;
	newKey <<= 21;
	newKey |= zPos;
	return newKey;
}


/*! Converts a position key into a point */
Point3D NeuronGroup::getPointFromPositionKey(uint64_t positionKey){
	uint64_t keyExtractor = 2097151;
	float tmpZPos = (float) (positionKey & keyExtractor);
	positionKey >>= 21;
	float tmpYPos = (float) (positionKey & keyExtractor);
	positionKey >>= 21;
	float tmpXPos = (float) (positionKey & keyExtractor);
	return Point3D(tmpXPos, tmpYPos, tmpZPos);
}


/*! Returns true if the parameters have been set. */
bool NeuronGroup::parametersSet(){
	if(getInfo().getNeuronType().getParameterCount() == parameterMap.size())
		return true;
	return false;
}


/*! Returns an iterator to the beginning of the position map, enabling topological
	iteration through the group, starting with the z axis, then y, then x.
	Position map is built after first call to this or end method. */
NeuronPositionIterator NeuronGroup::positionBegin(){
	if(!positionMapBuilt)
		buildPositionMap();
	return neuronPositionMap.begin();
}


/*! Returns an iterator to the end of the position map, enabling topological
	iteration through the group, starting with the z axis, then y, then x.
	Position map is built after first call to this or end method. */
NeuronPositionIterator NeuronGroup::positionEnd(){
	if(!positionMapBuilt)
		buildPositionMap();
	return neuronPositionMap.end();
}


/*! Sets the ID of the neuron group */
void NeuronGroup::setID(unsigned int id){
	info.setID(id);
}


/*! Sets the description of the neuron group */
void NeuronGroup::setDescription(const QString& description){
	info.setDescription(description);
}


/*! Sets the name of the neuron group */
void NeuronGroup::setName(const QString& name){
	info.setName(name);
}


/*! Replaces the neuron map with a new neuron map.
	Neurons are not cleaned up because they might be included in the new map. */
void NeuronGroup::setNeuronMap(NeuronMap* newMap){
	delete neuronMap;
	this->neuronMap = newMap;
	neuronGroupChanged();
}


/*! Sets the parameters in the neuron group.
	Throws an exception if these parameters are not appropriate
	for the type of neuron in this group. */
void NeuronGroup::setParameters(QHash<QString, double>& paramMap){
	//Get the neuron type associated with this neuron group
	NeuronType neurType = getInfo().getNeuronType();
	QList<ParameterInfo> paramInfoList = neurType.getParameterInfoList();
	if(paramInfoList.size() != paramMap.size())
		throw SpikeStreamException("NeuronGroup: failed to set parameters. Mismatch between number of parameters.");
	foreach(ParameterInfo paramInfo, paramInfoList){
		if(!paramMap.contains(paramInfo.getName()))
			throw SpikeStreamException("NeuronGroup: failed to set parameters. Missing parameter: " + paramInfo.getName());
	}

	//Parameters are ok - store map.
	this->parameterMap = paramMap;
}


/*! Returns the number of neurons in the group. */
int NeuronGroup::size(){
	return neuronMap->size();
}


/*--------------------------------------------------------- */
/*-----                PRIVATE METHODS                ----- */
/*--------------------------------------------------------- */

/*! Returns a temporary ID for adding connections */
unsigned NeuronGroup::getTemporaryID(){
	return ++neuronIDCounter;
}


/*! Records that neuron group has changed to trigger bounding box recalculation. */
void NeuronGroup::neuronGroupChanged(){
	calculateBoundingBox = true;
	positionMapBuilt = false;
}
