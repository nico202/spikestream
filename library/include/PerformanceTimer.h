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

#ifndef PERFORMANCETIMER_H
#define PERFORMANCETIMER_H

//Other includes
#include <sys/time.h>
#include <string>
using namespace std;


//--------------------------- Performance Timer ----------------------------
/*! Simple timer to record the time taken for methods or processes in the 
	simulation. */
//--------------------------------------------------------------------------

class PerformanceTimer {

	public:
		PerformanceTimer();
		~PerformanceTimer();
		unsigned int getTime_usec();
		void printTime();
		void start(string procName);
		void stop();


	private:
		//============================== VARIABLES =============================
		/*! Time structures to record the start time.*/
		timeval startTime;

		/*! Time structures to record the start time.*/
		timeval endTime;

		/*! Name of the process or method that is being timed.*/
		string processName;

		/*! When the timer is running this is set to false. Setting this
			to true stops the timer and end time is no longer updated.*/
		bool timerStopped;


		//=============================== METHODS ==============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		PerformanceTimer (const PerformanceTimer&);

		/*! Declare assignment private so it cannot be used.*/
		PerformanceTimer operator = (const PerformanceTimer&);

};


#endif//PERFORMANCETIMER_H

