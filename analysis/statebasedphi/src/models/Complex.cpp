//SpikeStream includes
#include "Complex.h"
using namespace spikestream;

/*! Standard constructor */
Complex::Complex(unsigned int id, unsigned int timeStep, double phi, QList<unsigned int>& neuronIDs){
    this->id = id;
    this->timeStep = timeStep;
    this->phi = phi;
    this->neuronIDs = neuronIDs;
}


/*! Empty constructor */
Complex::Complex(){
    id = 0;
    timeStep = 0;
    phi = 0;
}


/*! Copy constructor */
Complex::Complex(const Complex& cmplx){
    this->id = cmplx.id;
    this->timeStep = cmplx.timeStep;
    this->phi = cmplx.phi;
    this->neuronIDs = cmplx.neuronIDs;
}


/*! Destructor */
Complex::~Complex(){
}


/*! Assignment operator */
Complex& Complex::operator=(const Complex& rhs){
    //Check for self assignment
    if(this == &rhs)
	return *this;

    this->id = rhs.id;
    this->timeStep = rhs.timeStep;
    this->phi = rhs.phi;
    this->neuronIDs = rhs.neuronIDs;
    return *this;
}


/*! Returns neuron ids as a string */
QString Complex::getNeuronIDString() const{
    QString tmpStr;
    foreach(unsigned int neurID, neuronIDs)
	tmpStr += QString::number(neurID) + ",";
    tmpStr.truncate(tmpStr.size() - 1);
    return  tmpStr;
}















