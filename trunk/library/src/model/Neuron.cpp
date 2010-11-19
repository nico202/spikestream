//SpikeStream includes
#include "Neuron.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;


/*! Constructor */
Neuron::Neuron(float xPos, float yPos, float zPos){
    location.setXPos(xPos);
    location.setYPos(yPos);
    location.setZPos(zPos);
    id = 0;
}


/*! Constructor */
Neuron::Neuron(unsigned int id, float xPos, float yPos, float zPos){
    this->id = id;
    location.setXPos(xPos);
    location.setYPos(yPos);
    location.setZPos(zPos);
    id = 0;
}



/*! Destructor */
Neuron::~Neuron(){
}


/*! Prints out information about this neuron */
void Neuron::print(){
	cout<<"Neuron. id: "<<id<<endl;
}


