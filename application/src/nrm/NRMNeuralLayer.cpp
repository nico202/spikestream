
//SpikeStream includes
#include "NRMNeuralLayer.h"
#include "NRMException.h"

//Other includes
#include <iostream>
using namespace std;

/*! Constructor */
NRMNeuralLayer::NRMNeuralLayer(){
	trainingStringCount = 0;
	type = NRM_NEURAL_LAYER;
}


/*! Destructor */
NRMNeuralLayer::~NRMNeuralLayer(){
	//Clean up connections
	for(QList<NRMConnection*>::iterator iter = connectionList.begin(); iter != connectionList.end(); ++iter){
		delete *iter;
	}
}


/*! Adds a connection path between two layers.
	This method stores high level details about the connection. */
void NRMNeuralLayer::addConnectionPath(NRMConnection* connection){
	connectionList.append(connection);
}


/*! Creates all of the detailed connections between the neurons.
	Load of messy NRM code! */
void NRMNeuralLayer::createConnections(){
	for(QList<NRMConnection*>::iterator iter = connectionList.begin(); iter != connectionList.end(); ++iter){
		(*iter)->createConnections();
	}
}


/*! Builds the neurons.
	NOTE: This method should only be called after the size has been set!*/
void NRMNeuralLayer::createNeurons(){
	if(width == 0 || height == 0)
		throw NRMException ("Width and/or height are zero when building neurons");
	neuronArray = new NRMNeuron*[width * height];
	for(unsigned int i=0; i<width*height; ++i)
		neuronArray[i] = new NRMNeuron();
}


/*! Returns the connection at a specific position in the array - termed an id here */
NRMConnection* NRMNeuralLayer::getConnectionById(int id){
	if(id >= connectionList.size())
		throw NRMException("Connection id is out of bounds: ", id);
	return connectionList[id];
}


/*! Returns the connections */
QList<NRMConnection*> NRMNeuralLayer::getConnections(){
	return connectionList;
}


/*! Returns the neuron */
NRMNeuron* NRMNeuralLayer::getNeuron(unsigned int neurNum){
	if(neurNum >= getSize())
		throw NRMException("Training neuron number is out of range: ", neurNum);
	return neuronArray[neurNum];
}


/*! Returns the total number of connections in the layer */
unsigned int NRMNeuralLayer::getNumberOfConnections(){
	unsigned int tmpNumConns = 0;
	for(QList<NRMConnection*>::iterator iter = connectionList.begin(); iter != connectionList.end(); ++iter){
		tmpNumConns += (*iter)->conParams.numCons * this->getSize();
	}
	return tmpNumConns;
}


/*! Returns the number of connections per neuron in the layer */
unsigned int NRMNeuralLayer::getNumConnsPerNeuron(){
	unsigned int tmpNumConns = 0;
	for(QList<NRMConnection*>::iterator iter = connectionList.begin(); iter != connectionList.end(); ++iter){
		tmpNumConns += (*iter)->conParams.numCons;
	}
	return tmpNumConns;
}


/*! Pattern array is a bit representation of the input to the layer. The size is
	the number of total input connections divided by 8 because each byte represents
	8 connections */
unsigned int NRMNeuralLayer::getPatternArraySize(){
	unsigned int tmpConnTotal = this->getNumConnsPerNeuron();
	if(tmpConnTotal % 8 != 0)
		return (tmpConnTotal / 8) + 1;
	return tmpConnTotal / 8;
}


/*! Prints out the details of this neural layer */
void NRMNeuralLayer::print(){
	cout<<"   Name: "<<frameName<<endl;
	cout<<"   Width: "<<width<<endl;
	cout<<"   Height: "<<height<<endl;
	cout<<"   x: "<<x<<endl;
	cout<<"   y: "<<y<<endl;
	cout<<"   Generalisation: "<<generalisation<<endl;
	cout<<"   Spreading: "<<spreading<<endl;
	cout<<"   AltParam1: "<<altParam1<<endl;
	cout<<"   AltParam2: "<<altParam2<<endl;
	cout<<"   Colour Planes: "<<colPlanes<<endl;
	cout<<"   Type: "<<type<<endl;
	cout<<"   inTrack: "<<inTrack<<endl;
}


/*! Prints out the connections stored in the network */
void NRMNeuralLayer::printConnections(){
	for(QList<NRMConnection*>::iterator iter = connectionList.begin(); iter != connectionList.end(); ++iter){
		(*iter)->print();
	}
}


/*! Prints out the connections for a specific neuron */
void NRMNeuralLayer::printNeuronConnections(unsigned int neurNum){
	for(QList<NRMConnection*>::iterator iter = connectionList.begin(); iter != connectionList.end(); ++iter){
		(*iter)->printNeuronConnections(neurNum);
	}
}


/*! Prints out the training of the layer */
void NRMNeuralLayer::printTraining(){
	for(unsigned int i=0; i<this->getSize(); ++i){
		neuronArray[i]->printTraining();
	}
}
