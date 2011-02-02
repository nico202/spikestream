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


/*! Equality operator */
bool Box::operator==(const Box& rhs){
	if(this == &rhs)//Same object
		return true;

	if(this->x1 == rhs.x1 && this->y1 == rhs.y1 && this->z1 == rhs.z1 &&
		this->x2 == rhs.x2 && this->y2 == rhs.y2 && this->z2 == rhs.z2)
		return true;

	return false;
}


/*! Returns the point at the centre of the box . */
Point3D Box::centre() const{
	Point3D point(
			0.5*(x1 + x2),
			0.5*(y1 + y2),
			0.5*(z1 + z2)
	);
	return point;
}


/*! Returns true if the box contains the point.
	Points on the border of the box are considered to be within the box. */
bool Box::contains(const Point3D& point) const{
	if(point.getXPos() < x1 || point.getYPos() < y1 || point.getZPos() < z1
				|| point.getXPos() > x2 || point.getYPos() > y2 || point.getZPos() > z2)
		return false;
	return true;
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


/*! Returns a box enclosing the boxes in the list */
Box Box::getEnclosingBox(const QList<Box>& boxList){
	Box newBox;
	bool firstTime = true;
	for(int i=0; i<boxList.size(); ++i){
		const Box& tmpBox = boxList.at(i);
		if(firstTime){
			newBox = tmpBox;
			firstTime = false;
		}
		else{
			if(tmpBox.x1 < newBox.x1)
				newBox.x1 = tmpBox.x1;
			if(tmpBox.y1 < newBox.y1)
				newBox.y1 = tmpBox.y1;
			if(tmpBox.z1 < newBox.z1)
				newBox.z1 = tmpBox.z1;

			if(tmpBox.x2 > newBox.x2)
				newBox.x2 = tmpBox.x2;
			if(tmpBox.y2 > newBox.y2)
				newBox.y2 = tmpBox.y2;
			if(tmpBox.z2 > newBox.z2)
				newBox.z2 = tmpBox.z2;
		}
	}
	return newBox;
}


/*! Returns a box enclosing the boxes in the list */
Box Box::getEnclosingBox(const QList<Point3D>& pointList){
	Box newBox;
	bool firstTime = true;
	for(int i=0; i<pointList.size(); ++i){
		const Point3D& tmpPnt = pointList.at(i);
		if(firstTime){
			newBox.setCoordinates(tmpPnt.getXPos(), tmpPnt.getYPos(), tmpPnt.getZPos(),
								  tmpPnt.getXPos(), tmpPnt.getYPos(), tmpPnt.getZPos());
			firstTime = false;
		}
		else{
			if(tmpPnt.getXPos() < newBox.x1)
				newBox.x1 = tmpPnt.getXPos();
			if(tmpPnt.getYPos() < newBox.y1)
				newBox.y1 = tmpPnt.getYPos();
			if(tmpPnt.getZPos() < newBox.z1)
				newBox.z1 = tmpPnt.getZPos();

			if(tmpPnt.getXPos() > newBox.x2)
				newBox.x2 = tmpPnt.getXPos();
			if(tmpPnt.getYPos() > newBox.y2)
				newBox.y2 = tmpPnt.getYPos();
			if(tmpPnt.getZPos() > newBox.z2)
				newBox.z2 = tmpPnt.getZPos();
		}
	}
	return newBox;
}


/*! Returns a box enclosing the boxes and points in the list */
Box Box::getEnclosingBox(const QList<Box> &boxList, const QList<Point3D> &pointList){
	Box newBox;
	QList<Box> tmpBoxList = boxList;//Overcome constant constraint
	if(!pointList.isEmpty()){
		newBox = getEnclosingBox(pointList);
		tmpBoxList.append(newBox);
	}
	if(!tmpBoxList.isEmpty()){
		newBox = getEnclosingBox(tmpBoxList);
	}
	return newBox;
}


/*! Returns the width of the box */
float Box::getWidth() const{
	if( (x2-x1) >= 0.0f )
		return x2-x1;
	return x1-x2;
}


/*! Returns the length of the box */
float Box::getLength() const{
	if( (y2-y1) >= 0.0f )
		return y2-y1;
	return y1-y2;
}


/*! Returns the height of the box. */
float Box::getHeight() const{
	if( (z2-z1) >= 0.0f )
		return z2-z1;
	return z1-z2;
}


/*! Returns true if there is an overlap between this box and the box in the argument. */
bool Box::intersects(const Box &box) const{
	if((box.x1 >= x1 && box.x1 <= x2) || (box.x2 >= x1 && box.x2 <= x2)){//Overlap on X axis
		if((box.y1 >= y1 && box.y1 <= y2) || (box.y2 >= y1 && box.y2 <= y2)){//Overlap on Y axis
			if((box.z1 >= z1 && box.z1 <= z2) || (box.z2 >= z1 && box.z2 <= z2)){//Overlap on Z axis
				return true;
			}
		}
	}
	return false;
}


/*! Sets the coordinates of the box */
void Box::setCoordinates(float x1, float y1, float z1, float x2, float y2, float z2){
	this->x1 = x1;
	this->x2 = x2;
	this->y1 = y1;
	this->y2 = y2;
	this->z1 = z1;
	this->z2 = z2;
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


/*! Returns a string describing the box */
QString Box::toString() const {
	QString tmpStr = "Box x1=" + QString::number(x1) + "; y1=" + QString::number(y1) + "; z1=" + QString::number(z1);
	tmpStr += "; x2=" + QString::number(x2) + "; y2=" + QString::number(y2) + "; z2=" + QString::number(z2);
	return tmpStr;
}


