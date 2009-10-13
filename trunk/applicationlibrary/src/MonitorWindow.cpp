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
#include "MonitorWindow.h"
#include "Debug.h"

//Qt includes
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qwidget.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <QMoveEvent>

//Other includes
#include <iostream>
using namespace std;


/*! Constructor for monitoring of simulation. */
MonitorWindow::MonitorWindow(SimulationManager *simMan, QWidget *parent) : Q3DockWindow (parent, "New monitor window") {
	//This constructor is used in live monitoring mode
	simulationMode = true;

	//Store reference to simulation manager
	simulationManager = simMan;

	//Default mode is to monitor neurons
	monitorNeurons = true;

	//Set default size of window outside dock
	windowWidth = 200;
	windowHeight = 200;
	xPos = 20;
	yPos = 20;
	finishedUndockingWindow = false;
	oldVisibility = false;

	//Set up properties of monitor window
	setResizeEnabled(true); 
	setFixedExtentWidth(100);
	setFixedExtentHeight(100);
	setCloseMode(Q3DockWindow::Always);
	hide();
	
	//Listen for when its place changed so that window can be resized
	connect(this, SIGNAL(placeChanged(Q3DockWindow::Place)), this, SLOT(windowPlaceChanged(Q3DockWindow::Place))); 

	//Listen for when window becomes hidden so that monitoring can be switched off
	connect(this, SIGNAL(visibilityChanged(bool)), this, SLOT(windowVisibilityChanged(bool)));
}


/*! This constructor is used for playing back an archive. */
MonitorWindow::MonitorWindow(QWidget *parent) : Q3DockWindow (parent, "New monitor window"){
	simulationMode = false;

	//Set up properties of monitor window
	setResizeEnabled(true); 
	setFixedExtentWidth(100);
	show();
	
	//Set default size of window outside dock
	windowWidth = 200;
	windowHeight = 200;
	finishedUndockingWindow = false;

	//Listen for when its place changed so that window can be resized
	connect(this, SIGNAL(placeChanged(Q3DockWindow::Place)), this, SLOT(windowPlaceChanged(Q3DockWindow::Place))); 
}


/*! Destructor. */
MonitorWindow::~MonitorWindow(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING MONITOR WINDOW"<<endl;
	#endif//MEMORY_DEBUG
}


//-------------------------------------------------------------------------
//-------------------------- PUBLIC METHODS -------------------------------
//-------------------------------------------------------------------------

/*! Accessor method for the ID of the neuron group id being monitored
	by this class. */
unsigned int MonitorWindow::getNeuronGrpID(){
	return neuronGrpID;
}


/*! Sets whether this class is monitoring spikes from a neuron group
	or neuron firing patterns. */
void MonitorWindow::setMonitorType(bool monNeurs){
	if(monitorNeurons != monNeurs){//Need to take action
		//See if we are actively monitoring at this moment
		if(this->isVisible()){//Window is actively monitoring so need to change type
			simulationManager->stopMonitoringNeuronGroup(neuronGrpID, monitorNeurons);
			simulationManager->monitorNeuronGroup(neuronGrpID, monNeurs);
		}
		/*Otherwise window is not visible so should not be actively
			monitoring anything. In both cases need to store the current type of monitoring */
		monitorNeurons = monNeurs;
	}
}


/*! Sets the neuron group id for this window. */
void MonitorWindow::setNeuronGrpID(unsigned int neurGrpID){
	neuronGrpID = neurGrpID;
}


//----------------------------------------------------------------------------
//---------------- PROTECTED METHODS INHERITED FROM QDOCKWINDOW --------------
//----------------------------------------------------------------------------

/*! Stores the location that it is moved to so that it can be moved to
	this location when it is next undocked. */
void MonitorWindow::moveEvent(QMoveEvent *){
	if(this->place() == Q3DockWindow::OutsideDock){
		if(finishedUndockingWindow){
			xPos = this->pos().x();
			yPos = this->pos().y();
		}
	}
}


/*! Stores the size that it is resized to so that it can be resized
	correctly when it is next undocked. */
void MonitorWindow::resizeEvent(QResizeEvent *){
	//Store the current width and height if it has been resized outside of the dock
	if(this->place() == Q3DockWindow::OutsideDock){
		if(finishedUndockingWindow){
			windowWidth = this->size().width();
			windowHeight = this->size().height();
		}
	}
}


//-------------------------------------------------------------------------
//----------------------------- SLOTS -------------------------------------
//-------------------------------------------------------------------------

/*! Called when the window's place changes and moves and resizes
	the window based on stored values. Otherwise default values
	are used for the window. */
void MonitorWindow::windowPlaceChanged(Q3DockWindow::Place place){
	if(place == Q3DockWindow::OutsideDock){
		//Move and resize window
		resize(windowWidth, windowHeight);
		move(xPos, yPos);

		//Record if undocking is complete
		if(!finishedUndockingWindow)
			finishedUndockingWindow = true;
	}
	else
		finishedUndockingWindow = false;
}


/*! Controls whether the simulation manager requests spike information 
	for this neuron group. */
void MonitorWindow::windowVisibilityChanged(bool newVisibility){
	/* Only change the monitoring if the visibility has actually changed
		Otherwise get this method called even when windows are hidden. */
	if(newVisibility == oldVisibility)
		return;
	oldVisibility = newVisibility;

	//Double check that this is not being called in archive mode.
	if(!simulationMode){
		cerr<<"MonitorWindow: THIS SHOULD NOT BE CALLED IN ARCHIVE MODE"<<endl;
		return;
	}
	//Control the monitoring depending on whether this window is visible or not
	if(newVisibility){
		simulationManager->monitorNeuronGroup(neuronGrpID, monitorNeurons);
	}
	else{
		simulationManager->stopMonitoringNeuronGroup(neuronGrpID, monitorNeurons);
	}
}


