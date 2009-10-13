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

#ifndef NETWORKMONITOR_H
#define NETWORKMONITOR_H

//SpikeStream includes
#include "DBInterface.h"
#include "NeuronGroup.h"
#include "SpikeStreamApplication.h"

//Qt includes
#include <qwidget.h>
#include <qgl.h>
#include <qmutex.h>


//-------------------------- Network Monitor --------------------------------
/*! Displays the firing of the neural networks either as emitted spike 
	patterns or activity patterns in the layer. Used both for live simulation
	monitoring and for the play back of archived data. Different constructor
	is called depending on how it is being used. */

/* FIXME THERE IS CURRENTLY SOME CONFLIGT BETWEEN THIS CLASS AND THE NETWORK
	VIEWER IN FULL RENDER MODE DUE TO INTERLACING OF OPENGL COMMANDS.

	FIXME THE WINDOW CHANGES TO BLACK WHEN EXPANDED BEYOND A CERTAIN POINT.
	THIS IS A PROBLEM IN SUSE 10.2, BUT NOT IN SUSE 10.0, SO PROBABLY A QT
	VERSION PROBLEM.*/
//----------------------------------------------------------------------------

class NetworkMonitor :  public QGLWidget {
	Q_OBJECT
	
	public:
		NetworkMonitor(NeuronGroup, DBInterface *dbInter, QWidget *parent);
		NetworkMonitor(NeuronGroup, QWidget *parent);
		~NetworkMonitor();
		void reset();
		
		//Allow other classes to manipulate this class
		friend class SimulationManager;
		friend class MonitorWindow;
		friend class NetworkDataXmlHandler;
		friend class ArchiveManager;


	protected:
		//Methods inherited from QGLWidget
		void initializeGL();
		void paintGL();
		void resizeGL(int width, int height);


	private:
		//======================= VARIABLES =================================
		/*! Reference to database handlling class.*/
		DBInterface *networkDBInterface;

		/*! Short version of reference to the main application to halt the 
			processing of events.*/
		SpikeStreamApplication *spikeStrApp;

		/*! Controls whether it is displaying live data from a simulation or playback 
			from an archive.*/
		bool liveMonitoring;

		/*! Array to unpack the spikes into.*/
		unsigned int* unpackArray;

		/*! Define integer here to extract from neuron id.*/
		unsigned int unpkFromNeurID;
		
		/*! First map to hold the firing neurons. 
			Use two maps so that one can be loaded by another process whilst another is being
			used to produce the graphical display. The active map is swapped when loading is 
			complete. Use a map to easily get rid of duplicates.*/
		map<unsigned int, bool> firingNeuronMap1;

		/*! Second map to hold the firing neurons. 
			Use two maps so that one can be loaded by another process whilst another is being
			used to produce the graphical display. The active map is swapped when loading is 
			complete. Use a map to easily get rid of duplicates.*/
		map<unsigned int, bool> firingNeuronMap2;

		/*! Pointer to the map used to draw the current display.*/
		map<unsigned int, bool> *drawingMapPointer;

		/*! Pointer to the map used to load data.*/
		map<unsigned int, bool> *bufferMapPointer;

		/*! Records which of the maps is currently active.*/
		bool map1Active;

		/*! Number of spikes or firing neurons in the message.*/
		unsigned int numberOfSpikes;

		/* Information about the neuron group being monitored and the task ID 
			that is running the simulation for this neuron group */
		unsigned int neuronGrpID;
		int neuronGrpTaskID;
		unsigned int startNeuronID;
		unsigned int numberOfNeurons;

		//Width and height of the neuron group
		unsigned int neuronGrpWidth;
		unsigned int neuronGrpLength;

		//Width and height of the frame
		unsigned int frameWidth;
		unsigned int frameHeight;

		/*! Factor used to scale the x positions depending on the frame size.*/
		GLfloat scaleFactorWidth;

		/*! Factor used to scale the y positions depending on the frame size.*/
		GLfloat scaleFactorLength;

		/*! Size of the vertex representing a neuron firing or a spike.*/
		GLfloat pointSize;
		
		/*! Mapping between a neuron id and an x position in the widget.*/
		map<unsigned int, GLfloat> neuronXPosMap;

		/*! Mapping between a neuron id and a y position in the widget.*/
		map<unsigned int, GLfloat> neuronYPosMap;
		
		/*! The current time step of the message.*/
		unsigned int messageTimeStep;

		/*! Font for displaying the timestep.*/
		QFont *arialFont;

		/*! Vertical margin around drawing area */
		static GLfloat marginL;

		/*! Horizontal margin around drawing area */
		static GLfloat marginW;


		//=========================== METHODS =================================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		NetworkMonitor (const NetworkMonitor&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		NetworkMonitor operator = (const NetworkMonitor&);

		void checkOpenGLErrors();
		void printPositionMaps();
		void processFiringNeuronList();
		void processSpikeList();
		void setPointSize();
		void setTimeStep(unsigned int);
		void swapMaps();

};


#endif //NETWORKMONITOR_H


