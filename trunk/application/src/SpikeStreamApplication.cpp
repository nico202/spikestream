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
#include "SpikeStreamApplication.h"
#include "Debug.h"

//Other includes
#include <X11/Xlib.h>
#include <iostream>
using namespace std;


/*! Constructor. */
SpikeStreamApplication::SpikeStreamApplication(int & argc, char ** argv) : QApplication(argc, argv){
	renderDuration_ms = 0;
	startRenderKeyEventTime = 0;
	rendering = false;
}


/*! Destructor. */
SpikeStreamApplication::~SpikeStreamApplication(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING SPIKE STREAM APPLICATION"<<endl;
	#endif//MEMORY_DEBUG
}


//--------------------------------------------------------------------------
//-------------------------- PUBLIC METHODS --------------------------------
//--------------------------------------------------------------------------

/*! Returns true if the render is currently in progress. */
bool SpikeStreamApplication::renderInProgress(){
	return rendering;
}


/*! Called when the network viewer starts the render and records the time
	at this point and the time of the last key event. */
void SpikeStreamApplication::startRender(){
	rendering = true;
	gettimeofday(&startRenderTime, NULL);
	startRenderKeyEventTime = keyEventTime;
}


/*! Records the time at which the render completes and calculates the duration
	of the render. */
void SpikeStreamApplication::stopRender(){
	rendering = false;
	gettimeofday(&stopRenderTime, NULL);
	renderDuration_ms = 1000 * (stopRenderTime.tv_sec - startRenderTime.tv_sec) + (stopRenderTime.tv_usec - startRenderTime.tv_usec) / 1000;
}


//---------------------------------------------------------------------------
//------------------------- PROTECTED METHODS -------------------------------
//---------------------------------------------------------------------------

/*! Method inherited from QApplication that is called whenever an X11 XEvent is
	received by the application. Returning true filters the event, returning
	false passes the event on to the application. 
	This method filters events that are received whilst the NetworkViewer is
	rendering. Since display lists are used, have no control during the render
	so have to look for events that were generated during the time window of
	the render. This method is specific to Linux. */
bool SpikeStreamApplication::x11EventFilter( XEvent * xEvent){

	//Look for events from the keyboard
	if(xEvent->type == KeyPress || xEvent->type == KeyRelease){
		//Record time of key event
		keyEventTime = xEvent->xkey.time;

		//If the key event arrived during the render, ignore it.
		if((keyEventTime - startRenderKeyEventTime) < renderDuration_ms){
			return true;
		}
	}
	return false;
}



