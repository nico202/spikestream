#include "Connection.h"
#include "Util.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;

//Outputs memory information
//#define MEMORY_DEBUG


/*! Empty constructor */
Connection::Connection(){
	this->id = 0;
	this->fromNeuronID = 0;
	this->toNeuronID = 0;
	this->delay = 0;
	this->weight = 0;
	this->tempWeight = this->weight;

	#ifdef MEMORY_DEBUG
		cout<<"New connection (empty constructor). Size of connection: "<<sizeof(*this)<<endl;
	#endif//MEMORY_DEBUG
}


/*! Constructor */
Connection::Connection (unsigned int fromNeuronID, unsigned int toNeuronID, float delay, float weight){
	if(delay > DELAY_MAX)
		throw SpikeStreamException("Delay out of range: " + QString::number(delay) + "; maximum possible delay value: " + QString::number(DELAY_MAX));
	if(weight > WEIGHT_MAX)
		throw SpikeStreamException("Weight out of range: " + QString::number(weight) + "; maximum possible weight value: " + QString::number(WEIGHT_MAX));
	if(weight < WEIGHT_MIN)
		throw SpikeStreamException("Weight out of range: " + QString::number(weight) + "; minimum possible weight value: " + QString::number(WEIGHT_MIN));

	this->id = 0;
	this->fromNeuronID = fromNeuronID;
	this->toNeuronID = toNeuronID;
	this->delay = (unsigned short) rint(delay* DELAY_FACTOR);
	this->weight = (short) rint(weight * WEIGHT_FACTOR);
	this->tempWeight = this->weight;

	#ifdef MEMORY_DEBUG
		cout<<"New connection (standard constructor no ID). Size of connection: "<<sizeof(*this)<<endl;
	#endif//MEMORY_DEBUG
}


/*! Constructor */
Connection::Connection (unsigned id, unsigned fromNeuronID, unsigned toNeuronID, float delay, float weight){
	if(delay > DELAY_MAX)
		throw SpikeStreamException("Delay out of range: " + QString::number(delay) + "; maximum possible delay value: " + QString::number(DELAY_MAX));
	if(weight > WEIGHT_MAX)
		throw SpikeStreamException("Weight out of range: " + QString::number(weight) + "; maximum possible weight value: " + QString::number(WEIGHT_MAX));
	if(weight < WEIGHT_MIN)
		throw SpikeStreamException("Weight out of range: " + QString::number(weight) + "; minimum possible weight value: " + QString::number(WEIGHT_MIN));

	this->id = id;
	this->fromNeuronID = fromNeuronID;
	this->toNeuronID = toNeuronID;
	this->delay = (unsigned short) rint(delay* DELAY_FACTOR);
	this->weight = (short) rint(weight * WEIGHT_FACTOR);
	this->tempWeight = this->weight;

#ifdef MEMORY_DEBUG
	cout<<"New connection (standard constructor with ID). Size of connection: "<<sizeof(*this)<<endl;
#endif//MEMORY_DEBUG
}


/*! Copy constructor */
Connection::Connection(const Connection& conn){
	this->id = conn.id;
    this->fromNeuronID = conn.fromNeuronID;
    this->toNeuronID = conn.toNeuronID;
    this->delay = conn.delay;
    this->weight = conn.weight;
    this->tempWeight = conn.tempWeight;

	#ifdef MEMORY_DEBUG
		cout<<"New connection (copy constructor). Size of connection: "<<sizeof(*this)<<endl;
	#endif//MEMORY_DEBUG
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Assignment operator */
Connection& Connection::operator=(const Connection& rhs){
    //Check for self assignment
    if(this == &rhs)
	return *this;

	this->id = rhs.id;
    this->fromNeuronID = rhs.fromNeuronID;
    this->toNeuronID = rhs.toNeuronID;
    this->delay = rhs.delay;
    this->weight = rhs.weight;
    this->tempWeight = rhs.tempWeight;

    return *this;
}


/*! Prints out information about the connection. */
void Connection::print(){
   cout<<"Connection: ID="<<id<<"; fromNeuronID="<<fromNeuronID;
   cout<<" toNeuronID="<<toNeuronID<<"; delay="<<delay<<"; weight="<<weight<<"; tempWeight="<<tempWeight<<endl;
}

