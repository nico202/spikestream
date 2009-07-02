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

//SpikeStream includes
#include "SimulationClock.h"
#include "Debug.h"

//Other includes
#include <cstring>
#include <iostream>
using namespace std;


/*! Constructor. */
SimulationClock::SimulationClock(){
	//Initialise variables
	timeStep = 0;
	simulationTime = 0.0;
	liveMode = false;
	timeStepDuration_millisec = 0.0;//This should be initialised from the global parameters table
}


/*! Destructor. */
SimulationClock::~SimulationClock(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING SIMULATION CLOCK"<<endl;
	#endif//MEMORY_DEBUG	
}


//----------------------------------------------------------------------------
//----------------------------- PUBLIC METHODS -------------------------------
//----------------------------------------------------------------------------

/*! Returns the time that has elapsed since the beginning of the simulation.
	In live mode this is the actual time elapsed. In non-live mode this is the 
	number of time steps multiplied by the time step duration. */
double SimulationClock::getSimulationTime(){
	return simulationTime;
}


/*! Returns the current time step. */
unsigned int SimulationClock::getTimeStep(){
	return timeStep;
}


/*! Returns the amount of time that the clock advances with each time step
	This is only meaningful in non live mode. */
double SimulationClock::getTimeStepDuration_ms(){
	return timeStepDuration_millisec;
}


//------------------------------------------------------------------------------
//---------------------------- PRIVATE METHODS ---------------------------------
//------------------------------------------------------------------------------

/*! Increases the time step of the simulation and increases the simulation time. */
void SimulationClock::advance(){
	++timeStep;
	if(liveMode){
		gettimeofday(&currentTimeStruct, NULL);
		int microSecondsElapsed = 1000000 * (currentTimeStruct.tv_sec - startTimeStruct.tv_sec) + ( currentTimeStruct.tv_usec - startTimeStruct.tv_usec);
		simulationTime = ((double)microSecondsElapsed)/ 1000.0;
	}
	else{
		simulationTime += timeStepDuration_millisec;
	}
}


/*! Resets the clock. */
void SimulationClock::reset(){
	//Reset time variables
	timeStep = 0;
	simulationTime = 0.0;	

	//In live mode re-initialise time structures
	if(liveMode){
		gettimeofday(&startTimeStruct, NULL);
		gettimeofday(&currentTimeStruct, NULL);
	}
}


/*! In live mode the clock advances in real time. Otherwise the clock
	advances in its own simulation time by timeStepDuration_millisec. */
void SimulationClock::setLiveMode(bool lMode){
	//Reset clock if live mode has changed
	if(liveMode != lMode)
		simulationTime = 0.0;

	//Store new mode
	liveMode = lMode;

	//Initialise time structures used to keep track of real time
	if(liveMode){
		gettimeofday(&startTimeStruct, NULL);
		gettimeofday(&currentTimeStruct, NULL);
	}
}


/*! Sets the amount that the clock will advance with each time step in non
	live mode. */
void SimulationClock::setTimeStepDuration(double tsDuration){
	timeStepDuration_millisec = tsDuration;
}


