//SpikeStream includes
#include "SpikeStreamAnalysisException.h"
#include "Cluster.h"
using namespace spikestream;


/*! Standard constructor */
Cluster::Cluster(unsigned int id, unsigned int timeStep, double liveliness, QHash<unsigned int, double>& neuronLivelinessMap){
	this->id = id;
	this->timeStep = timeStep;
	this->liveliness = liveliness;
	this->neuronLivelinessMap = neuronLivelinessMap;
}


/*! Constructor with neuron id list.
	This sets all values in the liveliness map to zero. */
Cluster::Cluster(unsigned int id, unsigned int timeStep, double liveliness, QList<unsigned int>& neuronIDList){
	this->id = id;
	this->timeStep = timeStep;
	this->liveliness = liveliness;
	foreach(unsigned int neurID, neuronIDList)
		neuronLivelinessMap[neurID] = 0.0;
}


/*! Empty constructor */
Cluster::Cluster(){
	id = 0;
	timeStep = 0;
	liveliness = 0;
}


/*! Copy constructor */
Cluster::Cluster(const Cluster& clstr){
	this->id = clstr.id;
	this->timeStep = clstr.timeStep;
	this->liveliness = clstr.liveliness;
	this->neuronLivelinessMap = clstr.neuronLivelinessMap;
}


/*! Destructor */
Cluster::~Cluster(){
}


/*! Assignment operator */
Cluster& Cluster::operator=(const Cluster& rhs){
	//Check for self assignment
	if(this == &rhs)
		return *this;

	this->id = rhs.id;
	this->timeStep = rhs.timeStep;
	this->liveliness = rhs.liveliness;
	this->neuronLivelinessMap = rhs.neuronLivelinessMap;
	return *this;
}


/*-------------------------------------------------------------*/
/*-------                 PUBLIC METHODS                 ------*/
/*-------------------------------------------------------------*/

/*! Returns all of the neuron ids in the cluster */
QList<unsigned int> Cluster::getNeuronIDs() const{
	return neuronLivelinessMap.keys();
}


/*! Returns neuron ids as a string */
QString Cluster::getNeuronIDString() const{
	QString tmpStr;
	for(QHash<unsigned int, double>::const_iterator iter = neuronLivelinessMap.begin(); iter != neuronLivelinessMap.end(); ++iter)
		tmpStr += QString::number(iter.key()) + ",";
	tmpStr.truncate(tmpStr.size() - 1);
	return tmpStr;
}
