//SpikeStream includes
#include "Point3D.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

//Other includes
#include <math.h>


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


/*--------------------------------------------------------- */
/*-----                PUBLIC METHODS                 ----- */
/*--------------------------------------------------------- */

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
bool Point3D::operator ==(const Point3D& point) const{
    if(this->xPos == point.xPos && this->yPos == point.yPos && this->zPos == point.zPos)
		return true;
    return false;
}


/*! Comparison non equality operator */
bool Point3D::operator !=(const Point3D& point) const{
    if(this->xPos == point.xPos && this->yPos == point.yPos && this->zPos == point.zPos)
		return false;
    return true;
}


/*! Returns the distance between this point and the specified point. */
float Point3D::distance(const Point3D& point) const{
	float result = (xPos - point.xPos) * (xPos - point.xPos);
	result += (yPos - point.yPos) * (yPos - point.yPos);
	result += (zPos - point.zPos) * (zPos - point.zPos);
	return sqrt(result);
}


/*! Returns a string representation of the point. */
QString Point3D::toString() const{
    QString tmpStr = "(";
    tmpStr += QString::number(xPos) + ", ";
    tmpStr += QString::number(yPos) + ", ";
    tmpStr += QString::number(zPos) + ")";
    return tmpStr;
}


/*! Translates the point by the specified amount  */
void Point3D::translate(float dx, float dy, float dz){
	qDebug()<<"BEFORE: xPos="<<xPos<<"; yPos="<<yPos<<"; zPos="<<zPos;
	qDebug()<<"BEFORE: dx="<<dx<<"; dy="<<dy<<"; dz="<<dz;
	xPos += dx;
	yPos += dy;
	zPos += dz;
	qDebug()<<"AFTER: xPos="<<xPos<<"; yPos="<<yPos<<"; zPos="<<zPos;
}

