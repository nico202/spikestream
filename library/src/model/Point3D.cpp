//SpikeStream includes
#include "Point3D.h"
using namespace spikestream;


/*! Empty constructor, creates a point at the origin */
Point3D::Point3D(){
    xPos = 0.0f;
    yPos = 0.0f;
    zPos = 0.0f;
}


/*! Constructor */
Point3D::Point3D(float xPos, float yPos, float zPos){
    this->xPos = xPos;
    this->yPos = yPos;
    this->zPos = zPos;
}


/*! Copy constructor */
Point3D::Point3D(const Point3D& point){
    this->xPos = point.xPos;
    this->yPos = point.yPos;
    this->zPos = point.zPos;
}


/*! Destructor */
Point3D::~Point3D(){
}


/*! Assignment operator */
Point3D& Point3D::operator =(const Point3D& point){
    if(&point == this)
	return *this;

    this->xPos = point.xPos;
    this->yPos = point.yPos;
    this->zPos = point.zPos;
    return *this;
}


/*! Comparison equality operator */
bool Point3D::operator ==(const Point3D& point){
    if(this->xPos == point.xPos && this->yPos == point.yPos && this->zPos == point.zPos)
	return true;
    return false;
}


/*! Comparison non equality operator */
bool Point3D::operator !=(const Point3D& point){
    if(this->xPos == point.xPos && this->yPos == point.yPos && this->zPos == point.zPos)
	return false;
    return true;
}


QString Point3D::toString() const{
    QString tmpStr = "(";
    tmpStr += QString::number(xPos) + ", ";
    tmpStr += QString::number(yPos) + ", ";
    tmpStr += QString::number(zPos) + ")";
    return tmpStr;
}







