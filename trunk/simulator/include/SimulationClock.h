/***************************************************************************
 *   SpikeStream Simulation                                                *
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

#ifndef SIMULATIONCLOCK_H
#define SIMULATIONCLOCK_H

//Other includes
#include <sys/time.h>


//--------------------------- Simulation Clock -----------------------------
/*! Keeps track of the time and the time step of the simulation. Time is in 
	milliseconds. Each time step includes the processing of all messages up 
	to the sending of the spike lists to other tasks, after which the time 
	step is advanced. In live mode the time is advanced in real time; in 
	other modes it is advanced by the time step duration. */
//--------------------------------------------------------------------------

class SimulationClock {

	public:
		SimulationClock();
		~SimulationClock();
		double getSimulationTime();
		unsigned int getTimeStep();
		double getTimeStepDuration_ms();

		/* Make SpikeStreamSimulation a friend so that only this class
			can call advance(), reset() and other methods on the clock. */
		friend class SpikeStreamSimulation;


	private:
		//========================= VARIABLES =============================
		/*! Holds the number of update cycles of the simulation.*/
		unsigned int timeStep;

		/*! The amount of simulated time for each time step.*/
		double timeStepDuration_millisec;

		/*! The total amount of simulated time that has elapsed in millisedonds.*/
		double simulationTime;

		/*! Controls whether simulationTime is derived by adding timeStep_millisec
			or getting the actual system time.*/
		bool liveMode;

		/*! Structure used to get and hold the start simulation time 
			for live mode operation.*/
		timeval startTimeStruct;

		/*! Structure used to get and hold the current simulation time 
			for live mode operation.*/
		timeval currentTimeStruct;


		//========================== METHODS ==============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		SimulationClock (const SimulationClock&);

		/*! Declare assignment private so it cannot be used.*/
		SimulationClock operator = (const SimulationClock&);

		void advance();
		void reset();
		void setLiveMode(bool lMode);
		void setTimeStepDuration(double);

};


#endif //SIMULATIONCLOCK_H


