#include "Box.h"
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


/*! Moves box by the specified distance */
void Box::translate(float dx, float dy, float dz){
    x1 += dx;
    x2 += dx;
    y1 += dy;
    y2 += dy;
    z1 += dz;
    z2 += dz;
}


