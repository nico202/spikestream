//SpikeStream includes
#include "Box.h"
#include "SpikeStreamException.h"
using namespace spikestream;


/*! Default constructor */
Box::Box(){
    x1 = 0;
    y1 = 0;
    z1 = 0;
    x2 = 0;
    y2 = 0;
    z2 = 0;
}


/*! Standard constructor */
Box::Box(float x1, float y1, float z1, float x2, float y2, float z2){
    this->x1 = x1;
    this->y1 = y1;
    this->z1 = z1;
    this->x2 = x2;
    this->y2 = y2;
    this->z2 = z2;
}


/*! Copy constructor */
Box::Box(const Box& box){
    this->x1 = box.x1;
    this->y1 = box.y1;
    this->z1 = box.z1;
    this->x2 = box.x2;
    this->y2 = box.y2;
    this->z2 = box.z2;
}


/*! Destructor */
Box::~Box(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Assignment operator */
Box& Box::operator=(const Box& rhs){
	if(this == &rhs)
		return *this;

	this->x1 = rhs.x1;
	this->y1 = rhs.y1;
	this->z1 = rhs.z1;
	this->x2 = rhs.x2;
	this->y2 = rhs.y2;
	this->z2 = rhs.z2;

	return *this;

}


/*! Expands the box by the specified percentage */
void Box::expand_percent(float percent){
    //Check input
    if(percent < 0.0f)
	throw SpikeStreamException("Method does not support expansion by a negative number.");

    float xExpansion = (1.0f + percent / 100.0f) * (x2-x1) - (x2-x1);
    x1 -= xExpansion / 2.0f;
    x2 += xExpansion / 2.0f;

    float yExpansion = (1.0f + percent / 100.0f) * (y2-y1) - (y2-y1);
    y1 -= yExpansion / 2.0f;
    y2 += yExpansion / 2.0f;

    float zExpansion = (1.0f + percent / 100.0f) * (z2-z1) - (z2-z1);
    z1 -= zExpansion / 2.0f;
    z2 += zExpansion / 2.0f;
}


/*! Moves box by the specified distance */
void Box::translate(float dx, float dy, float dz){
    x1 += dx;
    x2 += dx;
    y1 += dy;
    y2 += dy;
    z1 += dz;
    z2 += dz;
}


