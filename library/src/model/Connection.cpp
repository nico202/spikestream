#include "Connection.h"
#include "Util.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;


/*! Constructor */
Connection::Connection (unsigned int fromNeuronID, unsigned int toNeuronID, float delay, float weight){
	if(delay > DELAY_MAX)
		throw SpikeStreamException("Delay out of range: " + QString::number(delay) + "; maximum possible delay value: " + QString::number(DELAY_MAX));
	if(weight > WEIGHT_MAX)
		throw SpikeStreamException("Weight out of range: " + QString::number(weight) + "; maximum possible weight value: " + QString::number(WEIGHT_MAX));
	if(weight < WEIGHT_MIN)
		throw SpikeStreamException("Weight out of range: " + QString::number(weight) + "; minimum possible weight value: " + QString::number(WEIGHT_MIN));

	this->fromNeuronID = fromNeuronID;
	this->toNeuronID = toNeuronID;
	this->delay = (unsigned short) rint(delay* DELAY_FACTOR);
	this->weight = (short) rint(weight * WEIGHT_FACTOR);
	this->tempWeight = this->weight;

//	cout<<"SIZE OF CONNECTION: "<<sizeof(*this)<<endl;
}


/*! Copy constructor */
Connection::Connection(const Connection& conn){
    this->fromNeuronID = conn.fromNeuronID;
    this->toNeuronID = conn.toNeuronID;
    this->delay = conn.delay;
    this->weight = conn.weight;
    this->tempWeight = conn.tempWeight;
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Assignment operator */
Connection& Connection::operator=(const Connection& rhs){
    //Check for self assignment
    if(this == &rhs)
	return *this;

    this->fromNeuronID = rhs.fromNeuronID;
    this->toNeuronID = rhs.toNeuronID;
    this->delay = rhs.delay;
    this->weight = rhs.weight;
    this->tempWeight = rhs.tempWeight;

    return *this;
}


/*! Prints out information about the connection. */
void Connection::print(){
   cout<<"Connection: fromNeuronID="<<fromNeuronID;
   cout<<" toNeuronID="<<toNeuronID<<"; delay="<<delay<<"; weight="<<weight<<"; tempWeight="<<tempWeight<<endl;
}

