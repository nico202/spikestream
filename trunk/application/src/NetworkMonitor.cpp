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
#include "NetworkMonitor.h"
#include "PVMMessages.h"
#include "Debug.h"
#include "Utilities.h"
#include "GlobalVariables.h"
#include "SpikeStreamMainWindow.h"

//Qt includes
#include <qmessagebox.h>

//Other includes
#include "mysql++.h"
#include "pvm3.h"
using namespace mysqlpp;


/* Declare and initialise static variables. */
GLfloat NetworkMonitor::marginL = 5.0f;
GLfloat NetworkMonitor::marginW = 5.0f;


/*! Constructor for live simulation monitoring. */
NetworkMonitor::NetworkMonitor(NeuronGroup neuronGrp, DBInterface *netDBInter, QWidget *parent) : QGLWidget(parent, "Network Monitor"){
	//Store database reference
	networkDBInterface = netDBInter;

	//Set up a short version of this reference
	spikeStrApp = SpikeStreamMainWindow::spikeStreamApplication;

	//Record that we are live monitoring
	liveMonitoring = true;

	//Store parameters of the neuron group that is being monitored
	neuronGrpID = neuronGrp.neuronGrpID;
	neuronGrpWidth = neuronGrp.width;
	neuronGrpLength = neuronGrp.length;

	//Set message time step to zero.
	messageTimeStep = 0;
	
	//Initialise numberOfSpikes to zero
	numberOfSpikes = 0;

	//Initialize array for receiving spikes
	unpackArray = new unsigned int[MAX_NUMBER_OF_SPIKES];
	
	/* Extract information about neuron group from database. 
		Need the number of neurons in the group and the lowest neuronID in the group
		By assuming that the neuron group is a continuous number of neuronIDs starting at 
		neuronGrpStart it becomes easy to achieve random access to any neuron using the 
		neuronID */
	int startXPos, startYPos;
	try{
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"SELECT COUNT(NeuronID), MIN(NeuronID), MAX(NeuronID) FROM Neurons WHERE NeuronGrpID = "<<neuronGrpID;
                StoreQueryResult countResults = query.store();
		Row neurGrpPropRow(*countResults.begin());//Should only be one row
		numberOfNeurons = Utilities::getUInt((std::string)neurGrpPropRow["COUNT(NeuronID)"]);
		startNeuronID = Utilities::getUInt((std::string)neurGrpPropRow["MIN(NeuronID)"]);
	
		/* Get the edge position and spacing of the neuron group */
		query.reset();
		query<<"SELECT X, Y FROM NeuronGroups WHERE NeuronGrpID = "<<neuronGrpID;
                StoreQueryResult startGrpPosRes = query.store();
		Row startGrpPosRow(*startGrpPosRes.begin());//Should only be one row
		startXPos = Utilities::getInt((std::string)startGrpPosRow["X"]);
		startYPos = Utilities::getInt((std::string)startGrpPosRow["Y"]);
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"NetworkMonitor: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query extracting neuron information: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "NeuralNetwork Database Error", errorString);
		return;
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"NetworkMonitor: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown extracting neuron information: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "NeuralNetwork Database Error", errorString);
		return;
	}
	catch(std::exception& er){// Catch-all for std exceptions
		cerr<<"NetworkMonitor: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown extracting neuron information: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "NeuralNetwork Database Error", errorString);
		return;
	}

	
	/* Extract the X and Y position of the neurons normalised so that coordinates start
		from the edge of the neuron group and not from the origin. */
	try{
		Connection* fastTmpConnection = networkDBInterface->getNewConnection();
		Query fastQuery = fastTmpConnection->query();
		fastQuery.reset();
		fastQuery<<"SELECT NeuronID, X, Y FROM Neurons WHERE NeuronGrpID = "<<neuronGrpID;
                UseQueryResult neuronPosRes = fastQuery.use();
		Row neuronPosRow;
		if(neuronPosRes){
			while (neuronPosRow = neuronPosRes.fetch_row()) {
				unsigned int neuronID = Utilities::getUInt((std::string)neuronPosRow["NeuronID"]);
				int xPos = Utilities::getInt((std::string)neuronPosRow["X"]);
				int yPos = Utilities::getInt((std::string)neuronPosRow["Y"]);
				
				//Subtract start position so that xPos and yPos are relative to startPos
				xPos -= startXPos;
				yPos -= startYPos;
	
				//Add to position maps
				neuronXPosMap[neuronID] = (GLfloat)xPos;
				neuronYPosMap[neuronID] = (GLfloat)yPos;
			}
		}
		else{
			cerr<<"NetworkMonitor: CANNOT RETRIEVE NEURON DETAILS: "<<fastQuery.error()<<endl;
		}

		//Close temporary connection
                fastTmpConnection->disconnect();
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"NetworkMonitor: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query adding layer to table: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Add Layer Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"NetworkMonitor: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown adding layer to table: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Add Layer Error", errorString);
	}
	catch(std::exception& er){// Catch-all for std exceptions
		cerr<<"NetworkMonitor: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown adding layer to table: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Add Layer Error", errorString);
	}

	
	//Store frame width and height
	frameWidth = frameSize().width();
	frameHeight = frameSize().height();

	/* Set the scaling factors taking account of the space at the top for the time step
		 and the margin around the drawing area. */
	scaleFactorWidth = ((GLfloat)frameWidth - 2.0f * marginW)/(GLfloat)neuronGrpWidth;
	if(scaleFactorWidth < 0.0f)
		scaleFactorWidth = 0.0f;

	scaleFactorLength = ((GLfloat)frameHeight - 2.0f * marginL - 20.0f )/(GLfloat)neuronGrpLength;
	if(scaleFactorLength < 0.0f)
		scaleFactorLength = 0.0f;

	//Calculate the size to draw vertices
	setPointSize();

	//Set up the font
	arialFont = new QFont( "Arial", 10);

	//Initialise any other variables
	map1Active = true;
	drawingMapPointer = &firingNeuronMap1;
	bufferMapPointer = &firingNeuronMap2;
}


/*! Constructor for archive monitoring. */
NetworkMonitor::NetworkMonitor(NeuronGroup neuronGrp, QWidget *parent) : QGLWidget(parent, "Network Monitor"){
	//Set up a short version of this reference
	spikeStrApp = SpikeStreamMainWindow::spikeStreamApplication;

	//Record that we are monitoring an archive
	liveMonitoring = false;

	//Store parameters of the neuron group that is being monitored
	neuronGrpID = neuronGrp.neuronGrpID;
	neuronGrpWidth = neuronGrp.width;
	neuronGrpLength = neuronGrp.length;

	//Initialise variables
	messageTimeStep = 0;

	/* Set up maps linking neuronIDs to positions. This works in the same way as
		the method that is used to create the neuron groups. Any changes there will
		need to be reflected here as well. */
	unsigned int neuronIDCounter = neuronGrp.startNeuronID;
	for(unsigned int yPos = 0; yPos < neuronGrpLength; ++yPos){
		for(unsigned int xPos = 0; xPos < neuronGrpWidth; ++xPos){
			neuronXPosMap[neuronIDCounter] = (GLfloat)xPos;
			neuronYPosMap[neuronIDCounter] = (GLfloat)yPos;
			++neuronIDCounter;
		}
	}

	//Store frame width and height
	frameWidth = frameSize().width();
	frameHeight = frameSize().height();

	/* Set the scaling factors taking account of the space at the top for the time step
		 and the margin around the drawing area. */
	scaleFactorWidth = ((GLfloat)frameWidth - 2.0f * marginW)/(GLfloat)neuronGrpWidth;
	if(scaleFactorWidth < 0.0f)
		scaleFactorWidth = 0.0f;

	scaleFactorLength = ((GLfloat)frameHeight - 2.0f * marginL - 20.0f )/(GLfloat)neuronGrpLength;
	if(scaleFactorLength < 0.0f)
		scaleFactorLength = 0.0f;

	//Calculate the size of the vertices
	setPointSize();

	//Set up font
	arialFont = new QFont( "Arial", 10);

	//Initialise any other variables
	map1Active = true;
	drawingMapPointer = &firingNeuronMap1;
	bufferMapPointer = &firingNeuronMap2;
}


/*! Destructor. */
NetworkMonitor::~ NetworkMonitor(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING NETWORK MONITOR"<<endl;
	#endif//MEMORY_DEBUG

	delete arialFont;

	//Delete unpack array
	if(liveMonitoring)
		delete [] unpackArray;

}


//----------------------------------------------------------------------------
//---------------              PUBLIC METHODS                -----------------
//----------------------------------------------------------------------------

/*! Resets the network monitor, possibly at the end of a simulation or archive display. */
void NetworkMonitor::reset(){
	numberOfSpikes = 0;
	messageTimeStep = 0;
	firingNeuronMap1.clear();
	firingNeuronMap2.clear();
	update();
}


//----------------------------------------------------------------------------
//--------------- PROTECTED METHODS INHERITED FROM QGL -----------------------
//----------------------------------------------------------------------------

/*! Called to initialise OpenGL. */
void NetworkMonitor::initializeGL(){
	/* Lock the main mutex so that NetworkViewer's OpenGL commands do not  
		clash with this classes' OpenGL commands. This only works for events
		generated whilst this class is drawing. */
	spikeStrApp->lock();

    /* Set a subtly different background depending on whether we are
		live monitoring or playing back an archive. */
	if(liveMonitoring)
		glClearColor(1.0f, 0.93f, 0.93f, 0.0f);//Very pale red
	else
		glClearColor(0.93f, 1.0f, 0.93f, 0.0f);//Very pale green

	//Dark red drawing colour
	glColor3f(0.2f, 0.0f, 0.0f);

	/*Unlock the main mutex so that NetworkViewer can process events and send OpenGL 
		commands. */
	spikeStrApp->unlock();
}


/*! Called to paint OpenGL. */
void NetworkMonitor::paintGL(){
	/*Lock the main mutex so that NetworkViewer's OpenGL commands do not  
		clash with this classes' OpenGL commands. When this is in the 
		middle of rendering, NetworkViewer cannot start rendering initiated
		by events.*/
	spikeStrApp->lock();

	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT);

	//Print simulation step at the top of the screen
	renderText(2, 12, "Time step: " + QString::number(messageTimeStep), *arialFont);

	//Set point size
	glPointSize(pointSize);

	//Store orginal state of the matrix
	glPushMatrix();

	//Move a bit in so that vertices are not off the edge
	glTranslated(marginW, marginL, 0.0);
	
	//Draw the neurons in the active map
	glBegin(GL_POINTS);
		for(map<unsigned int, bool>::iterator iter = drawingMapPointer->begin(); iter != drawingMapPointer->end(); ++iter){
			glVertex2f(neuronXPosMap[iter->first] * scaleFactorWidth, neuronYPosMap[iter->first] * scaleFactorLength);
		}
	glEnd();

	//Restore orginal state of the matrix
	glPopMatrix();

	//Check for errors
	checkOpenGLErrors();

	/*Unlock the main mutex so that NetworkViewer can process events and send OpenGL 
		commands. */
	spikeStrApp->unlock();
}


/*! Called when frame is resized. */
void NetworkMonitor::resizeGL(int frameW, int frameH){
	/*Lock the main mutex so when this is in the
		middle of sending OpenGL commands, NetworkViewer cannot start
		sending OpenGL commands initiated through events. */
	spikeStrApp->lock();

	//Store the width and height for paint method
	frameWidth = frameW;
	frameHeight = frameH;

	/* Set the scaling factors taking account of the space at the top for the time step
		 and the margin around the drawing area. */
	scaleFactorWidth = ((GLfloat)frameWidth - 2.0f * marginW)/(GLfloat)neuronGrpWidth;
	if(scaleFactorWidth < 0.0f)
		scaleFactorWidth = 0.0f;

	scaleFactorLength = ((GLfloat)frameHeight - 2.0f * marginL - 20.0f )/(GLfloat)neuronGrpLength;
	if(scaleFactorLength < 0.0f)
		scaleFactorLength = 0.0f;

	//Set the vertex size
	setPointSize();
	
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if(frameHeight == 0)
        frameHeight = 1;

    glViewport(0, 0, frameWidth, frameHeight);

    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

	// Psuedo window coordinates
	gluOrtho2D(0.0, (GLfloat) frameWidth, 0.0f, (GLfloat) frameHeight);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	//Check for OpenGL errors
	checkOpenGLErrors();

	/*Unlock the main mutex so that NetworkViewer can process events and send OpenGL 
		commands. */
	spikeStrApp->unlock();
}


//------------------------------------------------------------------------
//------------------------- PRIVATE METHODS ------------------------------
//------------------------------------------------------------------------

/*! Checks for errors in OpenGL. */ 
void NetworkMonitor::checkOpenGLErrors(){
	GLenum err = glGetError();
	while(err != GL_NO_ERROR){
		cerr<<"NetworkMonitor OpenGL ERROR: "<<gluErrorString(err)<<endl;
		cerr.flush();
		err = glGetError();
	}
}


/*! Debug method that prints out the mapping between neuron id and position. */
void NetworkMonitor::printPositionMaps(){
	cout<<"X Position map: "<<endl;
	for(map<unsigned int, GLfloat>::iterator iter = neuronXPosMap.begin(); iter != neuronXPosMap.end(); ++iter){
		cout<<"\tNeuronID: "<<iter->first<<"; XPos: "<<iter->second<<endl;
	}
	cout<<"Y Position map: "<<endl;
	for(map<unsigned int, GLfloat>::iterator iter = neuronYPosMap.begin(); iter != neuronYPosMap.end(); ++iter){
		cout<<"\tNeuronID: "<<iter->first<<"; YPos: "<<iter->second<<endl;
	}
}


/*! Processes a list of firing neurons from the task simulating a neuron group. */
void NetworkMonitor::processFiringNeuronList(){
	//Get the time step of the message
	int info = pvm_upkuint(&messageTimeStep, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"NetworkMonitor: ERROR EXTRACTING MESSAGE TIME STEP; TASK ID = "<<pvm_mytid()<<"; messageTimeStep: "<<messageTimeStep<<endl;
			return;
		}
	#endif//PVM_DEBUG

	// Extract the number of neurons in the message
	info = pvm_upkuint(&numberOfSpikes, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"NetworkMonitor: ERROR NUMBER OF FIRING NEURONS FROM MESSAGE; TASK ID = "<<pvm_mytid()<<"; numberOfSpikes: "<<numberOfSpikes<<endl;
			return;
		}
	#endif//PVM_DEBUG

	//Unpack the list of firing neurons 
	info = pvm_upkuint(unpackArray, numberOfSpikes, 1);//Unpack neuron ids as integers
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"NetworkMonitor: ERROR UNPACKING UNSIGNED INT FROM MESSAGE. NUMBER OF SPIKES = "<<numberOfSpikes<<endl;
			return;
		}
	#endif//PVM_DEBUG
	
	//Add the neuron ids to the firing neuron map
	for(unsigned int i=0; i<numberOfSpikes; ++i){
		//Add it to the buffer map
		(*bufferMapPointer)[ unpackArray[i] ] = true;
	}

	#ifdef SPIKE_DEBUG
		cout<<"NetworkMonitor: NeuronGrpID = "<<neuronGrpID<<". Number of neurons processed: "<<numberOfSpikes<<" at timeStep: "<<messageTimeStep<<endl;
	#endif//SPIKE_DEBUG

	//Swap maps and repaint widget.
	swapMaps();
}


/*! Processes a list of spikes from the task simulating the neuron group that is being monitored. */
void NetworkMonitor::processSpikeList(){
	//Get the time step of the message
	int info = pvm_upkuint(&messageTimeStep, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"NetworkMonitor: ERROR EXTRACTING MESSAGE TIME STEP; TASK ID = "<<pvm_mytid()<<"; messageTimeStep: "<<messageTimeStep<<endl;
			return;
		}
	#endif//PVM_DEBUG

	// Extract the number of spikes in the message
	info = pvm_upkuint(&numberOfSpikes, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"NetworkMonitor: ERROR NUMBER OF FIRING NEURONS FROM MESSAGE; TASK ID = "<<pvm_mytid()<<"; numberOfSpikes: "<<numberOfSpikes<<endl;
			return;
		}
	#endif//PVM_DEBUG

	//Unpack the list of spikes 
	info = pvm_upkuint(unpackArray, numberOfSpikes, 1);//Unpack from and to neuron ids as two shorts compressed into an integer
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"NetworkMonitor: ERROR UNPACKING UNSIGNED INT FROM MESSAGE. NUMBER OF SPIKES = "<<numberOfSpikes<<endl;
			return;
		}
	#endif//PVM_DEBUG
	
	//Process the spikes to get the from ids
	for(unsigned int i=0; i<numberOfSpikes; ++i){
		/* Add the from neuron ID to the from key.
			Only need the from neuron id because we are monitoring spikes from a layer*/
		unpkFromNeurID = (unsigned short) unpackArray[i];
		unpkFromNeurID += startNeuronID;

		//Add it to the buffer map
		(*bufferMapPointer)[ unpkFromNeurID ] = true;
	}

	#ifdef SPIKE_DEBUG
		cout<<"NetworkMonitor: NeuronGrpID = "<<neuronGrpID<<". Number of spikes processed: "<<numberOfSpikes<<" at timeStep: "<<messageTimeStep<<endl;
	#endif//SPIKE_DEBUG

	//Swap the active maps and updates the display
	swapMaps();
}


/*! Use scaleFactorWidth to set point size appropriately. 
		Since neuron groups could be rectangular, but vertexes are always square it is 
		probably too much effort trying to make this perfect. */
void NetworkMonitor::setPointSize(){
	pointSize = scaleFactorWidth * 0.9;
	if(pointSize < 0.1)
		pointSize = 0.1;
	else if(pointSize > 5.0)
		pointSize = 5.0;
}


/*! Sets the current timestep. */
void NetworkMonitor::setTimeStep(unsigned int timeStep){
	messageTimeStep = timeStep;
}


/*! Locks the application to prevent processing of events that triggers the draw method
	then swaps the reference so that it points to a different active map. */
void NetworkMonitor::swapMaps(){
	/*Lock the mutex so main application can't post events to this thread 
		whilst maps are being swapped. */
	spikeStrApp->lock();

	//Change the active map
	map1Active = !map1Active;

	//Clear the map that is not being used and point the map pointer to the active map
	if(map1Active){
		drawingMapPointer = &firingNeuronMap1;
		bufferMapPointer = &firingNeuronMap2;
		firingNeuronMap2.clear();
	}
	else{
		drawingMapPointer = &firingNeuronMap2;
		bufferMapPointer = &firingNeuronMap1;
		firingNeuronMap1.clear();
	}

	//Update the display
	//FIXME Qt instructions say to use updateGL() here, but this crashes it.
	update();

	//Unlock the mutex
	spikeStrApp->unlock();
}





