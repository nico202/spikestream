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

#ifndef MONITORDATASET_H
#define MONITORDATASET_H

//Qwt includes
#include "qwt_data.h"
#include "qwt_double_rect.h"


//------------------------- Monitor Dataset --------------------------------
/*! Holds the data for the graphs of neuron and synapse data that are 
	displayed by the NeuronMonitor and SynapseMonitor.
	Implements scrolling of the data so that latest elements overwrite 
	earlier ones.

	NOTE: All the data variables are pointers so that a copy can be made and 
	the new class is updated in sync with the old one. It is done this way 
	because QwtPlot calls copy method, but want to keep refernce to original 
	class and update the original class before plotting. */
//--------------------------------------------------------------------------

class MonitorDataset : public QwtData {

	public:
 		MonitorDataset(int bufSize, double rangeL, double rangeH);
		MonitorDataset();
		~MonitorDataset();
		void addPoint(double xVal, double yVal);
		QwtDoubleRect boundingRect() const;
		QwtDoubleRect* boundingRectRef() const;
		void cleanUp();
		MonitorDataset* copy() const;
		size_t size() const;
		double x(size_t i) const;
		double y(size_t i) const;


	private:
		//==================================== VARIABLES ===================================
		/*! Array holding the X data.*/
		double* xArray;

		/*! Array holding the Y data.*/
		double* yArray;

		/*! Position at which data is added in the array. It is a circular buffer.*/
		int* insertPos;

		/*! Size of the circular buffer.*/
		int* bufferSize;

		/*! How much the x axis increases with each additional point. This is to manage
			scrolling behaviour of the bounds rectangle.*/
		double* xIncrement;

		/*! Previous X value.*/
		double* xOld;

		/*! Minimum Y value.*/
		double* yMin;

		/*! Maximum Y value.*/
		double* yMax;

		/*! When the buffer is full start scrolling behaviour.*/
		bool* bufferFull;

		/*! The bounds of the data. */
		QwtDoubleRect* bndRect;


		//=================================== METHODS =====================================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		MonitorDataset (const MonitorDataset&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		MonitorDataset operator = (const MonitorDataset&);

};


#endif//MONITORDATASET_H
