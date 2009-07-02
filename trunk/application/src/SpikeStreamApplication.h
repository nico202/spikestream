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

#ifndef SPIKESTREAMAPPLICATION_H
#define SPIKESTREAMAPPLICATION_H

//Qt includes
#include <qapplication.h>

//Other includes
#include <sys/time.h>


//--------------------- Spike Stream Application --------------------------
/*! Inherits from QApplication so that it can filter out XEvents during 
	slow renders. Is the QApplication for the application. */
//-------------------------------------------------------------------------

class SpikeStreamApplication : public QApplication {
	Q_OBJECT

	public:
                SpikeStreamApplication(int & argc, char ** argv);
		~SpikeStreamApplication();
		bool renderInProgress();
		void startRender();
		void stopRender();


	protected:
		//Inherited from QApplication
		bool x11EventFilter( XEvent * );


	private:
		//============================ VARIABLES ==============================
		/*! Records the duration of each render.*/
		unsigned int renderDuration_ms;

		/*! Records the time of the last key press at the start of the render.*/
		unsigned int startRenderKeyEventTime;

		/*! Records the time of each key press event.*/
		unsigned int keyEventTime;

		/*! Time structure to record the start of the render.*/
		timeval startRenderTime;

		/*! Time structure to record the end of the render.*/
		timeval stopRenderTime;

		/*! Records when rendering is in progress.*/
		bool rendering;


		//============================ METHODS ================================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		SpikeStreamApplication(const SpikeStreamApplication&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		SpikeStreamApplication operator=(const SpikeStreamApplication&);

};


#endif//SPIKESTREAMAPPLICATION_H


