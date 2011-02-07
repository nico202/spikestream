//SpikeStream includes
#include "Pattern.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

//Other includes
#include <iostream>
using namespace std;


/*! Construtor */
Pattern::Pattern(){
}


/*! Copy constructor */
Pattern::Pattern(const Pattern& patt){
	this->name = patt.name;
	this->boxList = patt.boxList;
	this->pointList = patt.pointList;
}


/*! Destructor */
Pattern::~Pattern(){
}


/*----------------------------------------------------------*/
/*------                PUBLIC METHODS                ------*/
/*----------------------------------------------------------*/

/*! Adds a box to the pattern */
void Pattern::addBox(const Box& box){
	boxList.append(box);
}


/*! Adds a point to the pattern. */
void Pattern::addPoint(const Point3D& point){
	pointList.append(point);
}


/*! Returns true if the pattern contains the specified point. */
bool Pattern::contains(const Point3D& point){
	//Is the point contained within any of the boxes?
	QList<Box>::iterator boxListEnd = boxList.end();
	for(QList<Box>::iterator iter = boxList.begin(); iter!=boxListEnd; ++iter){
		if(iter->contains(point))
			return true;
	}

	//Does the point match any of the points?
	QList<Point3D>::iterator pointListEnd = pointList.end();
	for(QList<Point3D>::iterator iter = pointList.begin(); iter!=pointListEnd; ++iter){
		if(point == *iter)
			return true;
	}
	return false;
}


/*! Returns a copy of this pattern that is centred on the supplied box. */
Pattern Pattern::getAlignedPattern(const Box& box) const{
	//Get a copy of this pattern
	Pattern alignedPattern(*this);

	//Get a box enclosing this pattern
	Box thisBoundingBox = Box::getEnclosingBox(boxList, pointList);

	//Get the centres of the new and current patterns
	Point3D thisCentre = thisBoundingBox.centre();

	Point3D boxCentre = box.centre();
	float dx = boxCentre.getXPos() - thisCentre.getXPos();
	float dy = boxCentre.getYPos() - thisCentre.getYPos();
	float dz = boxCentre.getZPos() - thisCentre.getZPos();

	//Translate the new pattern and return it.
	alignedPattern.translate(dx, dy, dz);
	return alignedPattern;
}


/*! Prints out the pattern */
void Pattern::print() const {
	cout<<"-------------  Pattern: "<<name.toStdString()<<" ---------------------"<<endl;
	QList<Box>::const_iterator boxListEnd = boxList.end();
	for(QList<Box>::const_iterator iter = boxList.begin(); iter!=boxListEnd; ++iter){
		cout<<iter->toString().toStdString()<<endl;
	}

	//Does the point match any of the points?
	QList<Point3D>::const_iterator pointListEnd = pointList.end();
	for(QList<Point3D>::const_iterator iter = pointList.begin(); iter!=pointListEnd; ++iter){
		cout<<iter->toString().toStdString()<<endl;
	}
}

/*! Resets everything in the pattern */
void Pattern::reset(){
	name = "Unnamed";
	boxList.clear();
	pointList.clear();
}


/*! Translates the pattern by the specified amount. */
void Pattern::translate(float dx, float dy, float dz){
	//Translate the boxes
	QList<Box>::iterator boxListEnd = boxList.end();
	for(QList<Box>::iterator iter = boxList.begin(); iter!=boxListEnd; ++iter){
		(*iter).translate(dx, dy, dz);
	}

	//Translate the points
	QList<Point3D>::iterator pointListEnd = pointList.end();
	for(QList<Point3D>::iterator iter = pointList.begin(); iter!=pointListEnd; ++iter){
		(*iter).translate(dx, dy, dz);
	}
}


/*! Assignment operator */
Pattern& Pattern::operator=(const Pattern& rhs){
	if(this == &rhs)
		return *this;

	this->name = rhs.name;
	this->boxList = rhs.boxList;
	this->pointList = rhs.pointList;

	return *this;
}


/*! Equality operator */
bool Pattern::operator==(const Pattern& rhs){
	if(this == &rhs)//Same object
		return true;

	if( (this->name == rhs.name) && (this->boxList == rhs.boxList) && (this->pointList == rhs.pointList))
		return true;
	return false;
}

