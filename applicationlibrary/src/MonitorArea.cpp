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
#include "MonitorArea.h"
#include "Debug.h"
#include "Utilities.h"
#include "GlobalVariables.h"
#include "SpikeStreamMainWindow.h"

//Qt includes
#include <qwidget.h>
#include <qlayout.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <QDesktopWidget>

#include <mysql++.h>
using namespace mysqlpp;


/*! Constructor for live monitoring a simulation. */
MonitorArea::MonitorArea(DBInterface *netDBInter, SimulationManager* simMan, QWidget *parent) : Q3DockArea(Qt::Vertical, Q3DockArea::Normal, parent, "Monitor Docking Area"){
	//This monitor area is being used to monitor a simulation
	simulationMode = true;

	//Store references
	networkDBInterface = netDBInter;
	simulationManager = simMan;
	spikeStrApp = SpikeStreamMainWindow::spikeStreamApplication;//Set up a short version of this reference

	//Set endPos to zero so that new windows are created at zero
	endPos = 0;

	//Initialise boolean variables
	firstTimeUndock = true;

	//Create MonitorWindows and add them to avoid problems dynamically adding qdockwindows
	monitorWindowArray = new MonitorWindow*[MAX_NUMBER_MONITOR_WINDOWS];
	for(int i=0; i < MAX_NUMBER_MONITOR_WINDOWS; ++i){
		monitorWindowArray[i] = new MonitorWindow(simulationManager, this);
		monitorWindowArray[i]->hide();
	}
}


/*! Constructor for when the monitor area is being used to play back an archive. */
MonitorArea::MonitorArea(ArchiveManager *archMan, QWidget *parent) : Q3DockArea(Qt::Vertical, Q3DockArea::Normal, parent, "Monitor Docking Area"){
	//This monitor area is part of the archive widget	
	simulationMode = false;

	//Store references
	archiveManager = archMan;
	spikeStrApp = SpikeStreamMainWindow::spikeStreamApplication;//Set up a short version of this reference

	//Set endPos to zero so that new windows are created at zero
	endPos = 0;

	//Initialise boolean variables
	firstTimeUndock = true;

	//Create MonitorWindows and add them to avoid problems dynamically adding qdockwindows
	monitorWindowArray = new MonitorWindow*[MAX_NUMBER_MONITOR_WINDOWS];
	for(int i=0; i< MAX_NUMBER_MONITOR_WINDOWS; ++i){
		monitorWindowArray[i] = new MonitorWindow(this);
		monitorWindowArray[i]->hide();
	}
}


/*! Destructor. */
MonitorArea::~MonitorArea(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING MONITOR AREA"<<endl;
	#endif//MEMORY_DEBUG

	//Delete all existing network monitors
	/*for(map<unsigned int, NetworkMonitor*>::iterator iter = networkMonitorMap.begin(); iter != networkMonitorMap.end(); ++iter)
		delete iter->second;*/

	//Delete monitor window array. The monitor windows should be deleted by Qt
	//FIXME THIS SEEMS TO CRASH SOMETIMES WHEN EXITING
	delete [] monitorWindowArray;
}


//-------------------------------------------------------------------------
//------------------------- PUBLIC METHODS --------------------------------
//-------------------------------------------------------------------------

/*! Sets up network monitors and monitor windows ready for simulation monitoring or 
	archive playback. */
void MonitorArea::addMonitorWindow(NeuronGroup neuronGrp){
	//Run a check that endPos has not been exceeded
	if(endPos >= MAX_NUMBER_MONITOR_WINDOWS){
		cerr<<"MonitorArea: MAXIMUM NUMBER OF MONITOR WINDOWS HAS BEEN EXCEEDED. CANNOT ADD ANY MORE WINDOWS"<<endl;
		return;
	}

	//Check that there is not a network monitor already for this neuron group
	if(networkMonitorMap.count(neuronGrp.neuronGrpID) > 0){
		cerr<<"MonitorArea: NETWORK MONITOR ALREADY EXISTS FOR THIS NEURON GROUP!"<<endl;
		return;
	}

	//Add the network monitor 
	if(simulationMode){
		networkMonitorMap[neuronGrp.neuronGrpID] = new NetworkMonitor(neuronGrp, networkDBInterface, monitorWindowArray[endPos]);
		monitorWindowArray[endPos]->setCaption(neuronGrp.name + " [" + QString::number(neuronGrp.neuronGrpID) + "]");
		monitorWindowArray[endPos]->setNeuronGrpID(neuronGrp.neuronGrpID);
		monitorWindowArray[endPos]->setWidget(networkMonitorMap[neuronGrp.neuronGrpID]);
	}
	else{//Archive mode
		networkMonitorMap[neuronGrp.neuronGrpID] = new NetworkMonitor(neuronGrp, monitorWindowArray[endPos]);
		monitorWindowArray[endPos]->setCaption(neuronGrp.name + " [" + QString::number(neuronGrp.neuronGrpID) + "]");
		monitorWindowArray[endPos]->setWidget(networkMonitorMap[neuronGrp.neuronGrpID]);
		monitorWindowArray[endPos]->show();
	}

	/* Increase the record of the position to add in the array */
	++endPos;
}


/*! Called at the start of a simulation or archive playback
	to set the network monitors in the 	manager. 
	This enables the network monitors to be directly updated from the manager. */
void MonitorArea::connectToManager(){
	if(simulationMode)
		simulationManager->setNetworkMonitors(networkMonitorMap);
	else
		archiveManager->setNetworkMonitors(networkMonitorMap);
}


/*! Docks all monitor windows. */
void MonitorArea::dockMonitorWindows(){
	for(int i=0; i<endPos; ++i){
		if(!monitorWindowArray[i]->isHidden() && (monitorWindowArray[i]->place() == Q3DockWindow::OutsideDock))//Check to see if it is hidden or not
			monitorWindowArray[i]->dock();
	}
}


/*! Hides all monitor windows. */
void MonitorArea::hideMonitorWindows(){
	for(int i=0; i<endPos; ++i){
		monitorWindowArray[i]->hide();
	}
}


/*! Loads up the network monitors and adds them to the window ready to be hidden or
	shown during the simulation. */
void MonitorArea::loadSimulation(){
	//Get a list of neuron group IDs along with their width and length
	try{
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"SELECT NeuronGrpID, Name, Width, Length FROM NeuronGroups";
                StoreQueryResult result = query.store();
                for(StoreQueryResult::iterator iter = result.begin(); iter != result.end(); ++iter){
			Row neurGrpRow(*iter);
			
			//Extract parameters of neuron group from database
			NeuronGroup neuronGrp;
			neuronGrp.width = Utilities::getUInt((std::string)neurGrpRow["Width"]);
			neuronGrp.length = Utilities::getUInt((std::string)neurGrpRow["Length"]);
			neuronGrp.neuronGrpID = Utilities::getUInt((std::string)neurGrpRow["NeuronGrpID"]);
                        neuronGrp.name = ((std::string)neurGrpRow["Name"]).data();
		
			//Add a dock window and network monitor
			addMonitorWindow(neuronGrp);
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"MonitorArea: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query loading neuron group: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Group Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"MonitorArea: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading neuron group: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Group Error", errorString);
	}
	catch(std::exception& er){// Catch-all for std exceptions
		cerr<<"MonitorArea: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading neuron group: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Group Error", errorString);
	}
}


/*! Resets the monitor windows. */
void MonitorArea::resetMonitorWindows(){
	//Hide all monitor windows
	hideMonitorWindows();

	//reset insertion position
	endPos = 0;
	
	//Delete all existing network monitors
	for(map<unsigned int, NetworkMonitor*>::iterator iter = networkMonitorMap.begin(); iter != networkMonitorMap.end(); ++iter)
		delete iter->second;
	networkMonitorMap.clear();
}


/*! Rewinds network monitors back to zero time step. */
void MonitorArea::rewindNetworkMonitors(){
	for(map<unsigned int, NetworkMonitor*>::iterator iter = networkMonitorMap.begin(); iter != networkMonitorMap.end(); ++iter)
		iter->second->reset();
}


/*! Shows all monitor windows. */
void MonitorArea::showAllMonitorWindows(bool monitorNeurons){
	for(int i=0; i<endPos; ++i){
		monitorWindowArray[i]->setMonitorType(monitorNeurons);
		monitorWindowArray[i]->show();
	}
}


/*! Displays the monitor window with the neuron grp id. Linear search is a bit slow,
	This could be speeded up by using a map to store the link between a position in the
	array and the neuronGrpID, but it may not be necessary since it is not a frequent operation. */
void MonitorArea::showMonitorWindow(unsigned int neurGrpID, bool monitorNeurons){
	for(int i=0; i<endPos; ++i){
		if(monitorWindowArray[i]->getNeuronGrpID() == neurGrpID){
			monitorWindowArray[i]->setMonitorType(monitorNeurons);
			monitorWindowArray[i]->show();
			return;
		}
	}
}


/*! Undocks the monitor windows and lays them out within the window if this can be done
	easily. This is only done for the first undocking action since after this the 
	user is presumed to have arranged the windows to their preference. */
void MonitorArea::undockMonitorWindows(){
        QDesktopWidget *desktop = QApplication::desktop();
        int deskTopWidth = desktop->screenGeometry().width();
        int deskTopHeight = desktop->screenGeometry().height();
	int numWindowsInSpace = deskTopWidth / 100;//This is 2 x the width / the width of the current windows (200)
	int currentXPos = 0;
	int currentYPos = 0;

	for(int i=0; i<endPos; ++i){
		if(!monitorWindowArray[i]->isHidden()){//Check to see if it is hidden or not
			monitorWindowArray[i]->undock();

			/* Arrange across the top two lines of the desktop if there is space
				Only do this if there is two rows or less of monitor windows */
			if(firstTimeUndock && i < numWindowsInSpace ){
				monitorWindowArray[i]->move(currentXPos, currentYPos);
				currentXPos += 200;
				if(currentXPos >= (deskTopWidth - 150)){//Don't want window to be more than 50 pixels off screen
					currentXPos = 0;
					currentYPos += 200;
				}
			}
			else if(firstTimeUndock){//Stack up windows in bottom right corner
				monitorWindowArray[i]->move(deskTopWidth - 210, deskTopHeight - 200 - i * 10);
			}
		}
	}
	firstTimeUndock = false;
}


