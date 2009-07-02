/***************************************************************************
 *   SpikeStream Library                                                   *
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
#include "PerformanceTimer.h"

//Other includes
#include <iostream>
using namespace std;


/*! Constructor */
PerformanceTimer::PerformanceTimer(){
	//Timer has not been started yet
	timerStopped = true;

	//Initialise the time values to zero
	startTime.tv_sec = 0;
	startTime.tv_usec = 0;
	endTime.tv_sec = 0;
	endTime.tv_usec = 0;
}


/*! Destructor. */
PerformanceTimer::~PerformanceTimer(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING PERFORMANCE TIMER"<<endl;
	#endif//MEMORY_DEBUG
}


//------------------------------------------------------------------------
//------------------------ PUBLIC METHODS --------------------------------
//------------------------------------------------------------------------

/*! Returns how long since the timer was started in microseconds. */
unsigned int PerformanceTimer::getTime_usec(){
	//Update the current time if the timer is still running
	if(!timerStopped)
		gettimeofday(&endTime, NULL);
	unsigned int timerTime_us = 1000000 * (endTime.tv_sec - startTime.tv_sec) + endTime.tv_usec - startTime.tv_usec;
	return timerTime_us;
}


/*! Prints out the time since it started. */
void PerformanceTimer::printTime(){
	//Update the current time if the timer is still running
	if(!timerStopped)
		gettimeofday(&endTime, NULL);
	unsigned int timerTime_us = 1000000 * (endTime.tv_sec - startTime.tv_sec) + endTime.tv_usec - startTime.tv_usec;
	cout<<processName<<" took "<<timerTime_us<<" microseconds"<<endl;
}


/*! Starts the timer by recording the current time. */
void PerformanceTimer::start(string procName){
	processName = procName;
	gettimeofday(&startTime, NULL);
	gettimeofday(&endTime, NULL);//Also initialise the end time
	timerStopped = false;
}


/*! Stops the timer. */
void PerformanceTimer::stop(){
	gettimeofday(&endTime, NULL);
	timerStopped = true;
}

