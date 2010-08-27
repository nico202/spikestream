#include "Connection.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;


/*! Constructor */
Connection::Connection (unsigned int fromNeuronID, unsigned int toNeuronID, float delay, float weight){
    id = 0;
    connectionGroupID = 0;
    this->fromNeuronID = fromNeuronID;
    this->toNeuronID = toNeuronID;
    this->delay = delay;
    this->weight = weight;
	this->tempWeight = weight;
}


/*! Constructor */
Connection::Connection (unsigned int id, unsigned int conGrpID, unsigned int fromNeuronID, unsigned int toNeuronID, float delay, float weight){
    this->id = id;
    this->connectionGroupID = conGrpID;
    this->fromNeuronID = fromNeuronID;
    this->toNeuronID = toNeuronID;
    this->delay = delay;
    this->weight = weight;
	this->tempWeight = weight;
}


/*! Copy constructor */
Connection::Connection(const Connection& conn){
    this->id = conn.id;
    this->connectionGroupID = conn.connectionGroupID;
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

    this->id = rhs.id;
    this->connectionGroupID = rhs.connectionGroupID;
    this->fromNeuronID = rhs.fromNeuronID;
    this->toNeuronID = rhs.toNeuronID;
    this->delay = rhs.delay;
    this->weight = rhs.weight;
    this->tempWeight = rhs.tempWeight;

    return *this;
}


/*! Prints out information about the connection. */
void Connection::print(){
   cout<<"Connection: id="<<id<<"; connectionGroupID="<<connectionGroupID<<"; fromNeuronID="<<fromNeuronID;
   cout<<" toNeuronID="<<toNeuronID<<"; delay="<<delay<<"; weight="<<weight<<"; tempWeight="<<tempWeight<<endl;
}

