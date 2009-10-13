/***************************************************************************
 *   SpikeStream Application                                               *
 *   Copyright (C) 2007 by David Gamez                                     *
 *   david@davidgamez.eu                                                   *
 *   Version 0.1                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

//SpikeStream includes
#include "MonitorDataset.h"
#include "Debug.h"

//Other includes
#include <iostream>
using namespace std;


/*! Standard constructor that initialises everything. */
MonitorDataset::MonitorDataset(int bufSize, double rangeL, double rangeH) : QwtData(){
	/* Initialise the data used in this class. These are all pointers so need to be
		allocated on the heap. */
	//Dataset starts inserting at zero and starts reading at zero	
	insertPos = new int;
	*insertPos = 0;

	//Store buffer size
	bufferSize = new int;
	*bufferSize = bufSize;

	//Initialise parameters that will be used to learn the range once the buffer is full
	xIncrement = new double;
	*xIncrement = 0.0;
	xOld =new double;
	*xOld = 0.0;
	yMin = new double;
	*yMin = 0.0;
	yMax = new double;
	*yMax = 0.0;
	bufferFull = new bool;
	*bufferFull = false;

	//Initialise bounds rectangle to the default range
    //QwtDoubleRect(double left, double top, double width, double height);
	bndRect = new QwtDoubleRect(0.0, rangeL, *bufferSize, rangeH - rangeL);

	//Create arrays to hold the x and y values
	xArray = new double[*bufferSize];
	yArray = new double[*bufferSize];

	//Initialise arrays to zero
	for(int i=0; i<*bufferSize; ++i){
		xArray[i] = 0.0;
		yArray[i] = 0.0;
	}
}


/*! Constructor that does not initialise - used by copy(). */
MonitorDataset::MonitorDataset(){
}


/*! Destructor. This does not do any cleaning up because a reference copy is made of this
	class and so cleaning up might entail this destructor being called twice. Need to 
	call cleanUp() explicitly to delete the data structures. */
MonitorDataset::~MonitorDataset(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING MONITOR DATASET"<<endl;
	#endif//MEMORY_DEBUG
}


//-----------------------------------------------------------------------
//--------------------------- PUBLIC METHODS ----------------------------
//-----------------------------------------------------------------------

/*! Adds a point to the dataset. This is added at insertPos, which is rotated so that 
	the oldest point is overwritten. */
void MonitorDataset::addPoint(double xVal, double yVal){
	//Store points
	xArray[*insertPos] = xVal;
	yArray[*insertPos] = yVal;

	//Work out how fast we are advancing with each additional point
	*xIncrement = xVal - *xOld;
	*xOld = xVal;

	//Store y range
	if(yVal > *yMax)
		*yMax = yVal;
	else if(yVal < *yMin)
		*yMin = yVal;

	//Advance insert position
	++(*insertPos);
	*insertPos %= *bufferSize;
	if(*insertPos == 0)
		//Record that buffer has been filled up, so we can now start advancing the window
		*bufferFull = true;

	//If we have enough data points.
	if(*bufferFull){
		/* Check that xVal is visible. It can vanish when the monitoring is stopped
			whilst the simulation is running. */
		if(xVal < bndRect->left() || xVal > (bndRect->left() + bndRect->width())){
			//Reposition drawing point 19/20 of the way along the bound rectangle
			double leftPos = xVal - ( ((double)*bufferSize * 19.0 ) / 20.0) * (*xIncrement);
			bndRect->setLeft(leftPos);//(
		}

		/*Adjust position of the rectangle based on the ranges
		 We have advanced bufferWidth times, each time with increment xIncrement
			so width of window should be bufferWidth * xIncrement. */
		bndRect->setRect(bndRect->left() + *xIncrement, *yMin, *bufferSize * (*xIncrement), *yMax - *yMin);
	}
	else{
		//Want to start adjusting the width straight away
		bndRect->setWidth(*bufferSize * (*xIncrement));
	}
}


/*! Returns the bounding rectangle of the dataset. */
QwtDoubleRect MonitorDataset::boundingRect() const{
    return *bndRect;
}


/*! Returns a reference to the bounding rectangle of the dataset. */
QwtDoubleRect* MonitorDataset::boundingRectRef() const{
   return bndRect;
}


/*! Deletes everything in the class.
	Carry this out here instead of the destructor because copy() returns a copy
	of the references and so the destructor could be invoked twice on the same
	objects. */
void MonitorDataset::cleanUp(){
	#ifdef MEMORY_DEBUG
		cout<<"CLEANING UP MONITOR DATASET"<<endl;
	#endif//MEMORY_DEBUG

	//Delete everything allocated on the stack.
	delete [] xArray;
	delete [] yArray;
	delete insertPos;
	delete bufferSize;
	delete xIncrement;
	delete xOld;
	delete yMin;
	delete yMax;
	delete bufferFull;
	delete bndRect;
}


/*! Copies references so that new class updates in sync with old.
	Copy all the references to data and arrays and the rectangle. Do it this way
	because QwtPlot uses a copy of this dataset and invokes methods on the copy. */
MonitorDataset* MonitorDataset::copy() const{
	MonitorDataset* tempMonitorData = new MonitorDataset();
	tempMonitorData->bufferSize = bufferSize;
	tempMonitorData->xIncrement = xIncrement;
	tempMonitorData->xOld = xOld;
	tempMonitorData->yMin = yMin;
	tempMonitorData->yMax = yMax;
	tempMonitorData->bndRect = bndRect;
	tempMonitorData->xArray = xArray;
	tempMonitorData->yArray = yArray;
	tempMonitorData->insertPos = insertPos;
	tempMonitorData->bufferFull = bufferFull;

	return tempMonitorData;
}


/*! Returns the number of data points held by this class. */
size_t MonitorDataset::size() const{
	return *bufferSize;
}


/*! Returns the x value at position i. */
double MonitorDataset::x(size_t i) const{
//	cout<<"READING X: bufferSize="<<*bufferSize<<"; i="<<i<<"; insertPos="<<*insertPos<<"; VALUE = "<<xArray[(i + *insertPos) % *bufferSize]<<endl;
	return xArray[(i + *insertPos) % *bufferSize];
}


/*! Returns the y value at position i. */
double MonitorDataset::y(size_t i) const{
	return yArray[(i + *insertPos) % *bufferSize];
}


