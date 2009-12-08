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

#ifndef MONITORAREA_H
#define MONITORAREA_H

//SpikeStream includes
#include "DBInterface.h"
#include "MonitorWindow.h"
#include "NetworkMonitor.h"
#include "SimulationManager.h"
#include "NeuronGroup.h"
#include "SpikeStreamApplication.h"

//Qt includes
#include <qwidget.h>
#include <q3dockarea.h>
#include <qstring.h>


//------------------------- Monitor Area ----------------------------------
/*! Extends QDockArea and holds a number of monitor windows with the
	possibility of arranging them and undocking them, etc.

	NOTE Had problems creating QDockWindows at runtime, so instead create
	all of them at once and then add NetworkMonitors to them and hide/show
	them as required.This creates a limit on the number of monitoring windows
	that can be used at one time, which is set in GlobalVariables.h. */
//-------------------------------------------------------------------------

class MonitorArea : public Q3DockArea {
	Q_OBJECT

	public:
		MonitorArea(DBInterface*, SimulationManager*, QWidget *parent);
		MonitorArea(ArchiveManager*, QWidget *parent);
		~MonitorArea();
		void addMonitorWindow(NeuronGroup neurGrp);
		void connectToManager();
		void dockMonitorWindows();
		void hideMonitorWindows();
		void loadSimulation();
		void resetMonitorWindows();
		void rewindNetworkMonitors();
		void showAllMonitorWindows(bool monitorType);
		void showMonitorWindow(unsigned int neuronGrpID, bool monitorType);
		void undockMonitorWindows();


	private:
		//========================== VARIABLES ===========================
		//References
		DBInterface *networkDBInterface;
		SimulationManager *simulationManager;
		ArchiveManager *archiveManager;
		SpikeStreamApplication *spikeStrApp;

		/*! Array holding references to all the monitor windows.*/
		MonitorWindow **monitorWindowArray;

		/*! The insertion point in the monitorWindowArray */
		int endPos;

		/*! Holds references to all the network monitors.*/
		map<unsigned int, NetworkMonitor*> networkMonitorMap;

		/*! Records whether this monitor area is monitoring a simulation
			or playing back an archive */
		bool simulationMode;

		/*! Used to determine whether the windows should be arranged
			automatically, this records whether this is the first undock
			of the monitor area. */
		bool firstTimeUndock;


		//========================== METHODS =============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		MonitorArea (const MonitorArea&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		MonitorArea operator = (const MonitorArea&);

};


#endif //MONITORAREA_H


