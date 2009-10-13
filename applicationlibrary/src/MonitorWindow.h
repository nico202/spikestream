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

#ifndef MONITORWINDOW_H
#define MONITORWINDOW_H

//SpikeStream includes
#include "NetworkMonitor.h"
#include "SimulationManager.h"

//Qt includes
#include <qwidget.h>
#include <q3dockwindow.h>
#include <qstring.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <QMoveEvent>

//------------------------- Monitor Window --------------------------------
/*! Displays the activity of a neuron group for both live simulation
	monitoring and the playback of archived neuron and spike patterns. */
//-------------------------------------------------------------------------

class MonitorWindow : public Q3DockWindow{
	Q_OBJECT

	public:
		/* Two different constructors depending on whether this is for
			live monitoring or archive playback. */
		MonitorWindow(SimulationManager*, QWidget *parent);
		MonitorWindow(QWidget* parent);
		~MonitorWindow();
		unsigned int getNeuronGrpID();
		void setMonitorType(bool monitorNeurons);
		void setNeuronGrpID(unsigned int);


	protected:
		//Inherited from QDockWindow
		void moveEvent(QMoveEvent*);
		void resizeEvent(QResizeEvent*);


	private slots:
		void windowPlaceChanged(Q3DockWindow::Place p);
		void windowVisibilityChanged(bool);


	private:
		//============================ VARIABLES ==============================
		/*! Use simulation manager reference to stop or start monitoring of a 
			neuron group when corresponding qdockwindow is made visible */
		SimulationManager *simulationManager;

		/*! Records whether this window is being used for live monitoring or playing 
			back a simulation */
		bool simulationMode;

		/*! Store neuron group ID. 
			Needed so that simulation widget can show or hide windows monitoring
			particular neuron groups. */
		unsigned int neuronGrpID;

		/*! Records whether this window is monitoring the neuron patterns
			or the spike patterns from the neuron group. The spike patterns
			will be spread out in time because of the delay */
		bool monitorNeurons;

		/*! Store window width to save effort resizing window after undocking.*/
		int windowWidth;

		/*! Store window height to save effort resizing window after undocking.*/
		int windowHeight;
		
		/*! Store the location of the window so that it can be moved back to this
			location when the window is undocked. */
		int xPos;

		/*! Store the location of the window so that it can be moved back to this
			location when the window is undocked. */
		int yPos;

		/*! Set miniumum height of window to allow space for the time update above 
			the neural networks.*/
		static const int minimumHeight = 100;

		/*! Records when the process of undocking the window is complete */
		bool finishedUndockingWindow;

		/*! Records previous visiblity status
			This prevents constant requests to simulation manager to cancel messages
			when none are being sent. */
		bool oldVisibility;


		//============================= METHODS ================================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		MonitorWindow (const MonitorWindow&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		MonitorWindow operator = (const MonitorWindow&);

};


#endif //MONITORWINDOW_H



