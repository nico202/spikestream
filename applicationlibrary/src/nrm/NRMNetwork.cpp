//SpikeStream includes
#include "NRMNetwork.h"
#include "NRMException.h"
#include "NRMConstants.h"
using namespace spikestream;

//Other includes
#include <sstream>
#include <iostream>
using namespace std;


/*! Constructor */
NRMNetwork::NRMNetwork(){
	configVersion = -1;
}


/*! Destructor */
NRMNetwork::~NRMNetwork(){
	//Clean up input layers
	for(QHash<int, NRMInputLayer*>::iterator iter = inputLayerMap.begin(); iter != inputLayerMap.end(); ++iter){
		delete iter.value();
	}

	//Clean up neural layers
	for(QHash<int, NRMNeuralLayer*>::iterator iter = neuralLayerMap.begin(); iter != neuralLayerMap.end(); ++iter){
		delete iter.value();
	}
}


/*! Adds an input layer to the network with a specified id */
void NRMNetwork::addInputLayer(int id, NRMInputLayer* inputLayer){
	if(inputLayerMap.contains(id)){
		ostringstream msg;
		msg<<"Input layer with id "<<id<<" already exists";
		throw NRMException(msg.str());
	}
	inputLayerMap[id] = inputLayer;
}


/*! Adds a neural layer to the network with a specified id */
void NRMNetwork::addNeuralLayer(int id, NRMNeuralLayer* neuralLayer){
	if(neuralLayerMap.contains(id)){
		ostringstream msg;
		msg<<"Neural layer with id "<<id<<" already exists";
		throw NRMException(msg.str());
	}
	neuralLayerMap[id] = neuralLayer;
}


/*! Instructs the neural layers to create the random connections.
		These are generated from the same random seed each time. */
void NRMNetwork::createConnections(){
	for(QHash<int, NRMNeuralLayer*>::iterator iter = neuralLayerMap.begin(); iter != neuralLayerMap.end(); ++iter){
		iter.value()->createConnections();
	}
}


/*! Returns all of the inputs */
QList<NRMInputLayer*> NRMNetwork::getAllInputs(){
	QList<NRMInputLayer*> tmpList;
	for(QHash<int, NRMInputLayer*>::iterator iter = inputLayerMap.begin(); iter != inputLayerMap.end(); ++iter){
		tmpList<<iter.value();
	}
	return tmpList;
}


/*! Obtains the version of the configuration file that was used to load the network */
int NRMNetwork::getConfigVersion(){
	return configVersion;
}


/*! Returns all of the inputs that work as frames in images */
QList<NRMInputLayer*> NRMNetwork::getFramedImageInputs(){
	QList<NRMInputLayer*> tmpList;
	for(QHash<int, NRMInputLayer*>::iterator iter = inputLayerMap.begin(); iter != inputLayerMap.end(); ++iter){
		if(iter.value()->winType == FRAME_WIN || iter.value()->winType == _FRAME_WIN)
			tmpList<<iter.value();
	}
	return tmpList;
}


/*! Returns the number of input layers */
int NRMNetwork::getInputLayerCount(){
	return inputLayerMap.size();
}



/*! Returns the number of neural layers */
int NRMNetwork::getNeuralLayerCount(){
	return neuralLayerMap.size();
}


/*! Returns all of the inputs that are basic panels */
QList<NRMInputLayer*> NRMNetwork::getPanelInputs(){
	QList<NRMInputLayer*> tmpList;
	for(QHash<int, NRMInputLayer*>::iterator iter = inputLayerMap.begin(); iter != inputLayerMap.end(); ++iter){
		if(iter.value()->winType == MULTI_WIN)
			tmpList<<iter.value();
	}
	return tmpList;
}


/*! Returns input by ID */
NRMInputLayer* NRMNetwork::getInputById(int id){
	if(inputLayerMap.contains(id))
		return inputLayerMap[id];
	throw NRMException("Input layer ID could not be found");
}


/*! Returns all of the neural layers that have been loaded. */
QList<NRMNeuralLayer*> NRMNetwork::getAllNeuralLayers(){
	QList<NRMNeuralLayer*> tmpList;
	for(QHash<int, NRMNeuralLayer*>::iterator iter = neuralLayerMap.begin(); iter != neuralLayerMap.end(); ++iter){
		tmpList<<iter.value();
	}
	return tmpList;
}


/*! Returns information about the neural layer with a specific id */
NRMNeuralLayer* NRMNetwork::getNeuralLayerById(int id){
	if(neuralLayerMap.contains(id))
		return neuralLayerMap[id];
	throw NRMException("Neural layer ID could not be found");
}


/*! Returns the requested layer with the object type used to discriminate between
	neural and input layers */
NRMLayer* NRMNetwork::getLayerById(int layerId, int objectType){
	if( objectType == MAGNUS_IN_OB || objectType == MAGNUS_PREV_OB || objectType == MAGNUS_STATE_OB){
		return getNeuralLayerById(layerId);
	}
	else if(objectType == IMG_INPUT_OB){
		return this->getInputById(layerId);
	}
	throw NRMException("Object type not recognized: ", objectType);
}


/*! Returns a list of the neural layers that have been trained. */
QList<NRMNeuralLayer*> NRMNetwork::getTrainedNeuralLayers(){
    QList<NRMNeuralLayer*> tmpList;
    for(QHash<int, NRMNeuralLayer*>::iterator iter = neuralLayerMap.begin(); iter != neuralLayerMap.end(); ++iter){
	if(iter.value()->isTrained())
	    tmpList<<iter.value();
    }
    return tmpList;
}


/*! Sets the config version */
void NRMNetwork::setConfigVersion(int version){
	configVersion = version;
}


/*! Prints out the connections */
void NRMNetwork::printConnections(){
    for(QHash<int, NRMNeuralLayer*>::iterator iter = neuralLayerMap.begin(); iter != neuralLayerMap.end(); ++iter){
	QList<NRMConnection*> connectionList = iter.value()->getConnections();
	for(int conCtr=0; conCtr<connectionList.size(); ++conCtr){
	    connectionList[conCtr]->print();
	    cout<<"======================  NEURAL CONNECTIONS  ========================"<<endl;
	    for(int neurCtr=0; neurCtr<iter.value()->getSize(); ++neurCtr){
		connectionList[conCtr]->printNeuronConnections(neurCtr);
	    }
	}
    }
}


/*! Prints out information about the input layers stored in this class */
void NRMNetwork::printInputLayers(){
	for(QHash<int, NRMInputLayer*>::iterator iter = inputLayerMap.begin(); iter != inputLayerMap.end(); ++iter){
		cout<<"===========  Input Layer id="<<iter.key()<<"  ============"<<endl;
		iter.value()->print();
		cout<<endl;
	}
}


/*! Debugging method to print out information about the neural layers stored in this class */
void NRMNetwork::printNeuralLayers(){
	for(QHash<int, NRMNeuralLayer*>::iterator iter = neuralLayerMap.begin(); iter != neuralLayerMap.end(); ++iter){
		cout<<"===========  Neural Layer id="<<iter.key()<<"  ============"<<endl;
		iter.value()->print();
		cout<<endl;
	}
}


