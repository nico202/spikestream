//SpikeStream includes
#include "Neuron.h"
using namespace spikestream;

/*! Constructor */
Neuron::Neuron(float xPos, float yPos, float zPos){
    location.setXPos(xPos);
    location.setYPos(yPos);
    location.setZPos(zPos);
    id = 0;
}


/*! Destructor */
Neuron::~Neuron(){
}


