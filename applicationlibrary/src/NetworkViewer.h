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

#ifndef NETWORKVIEWER_H
#define NETWORKVIEWER_H

//SpikeStream includes
#include "DBInterface.h"
#include "NeuronGroupHolder.h"
#include "ConnectionGroupHolder.h"
#include "SpikeStreamApplication.h"

//Qt includes
#include <q3accel.h>
#include <qgl.h>
#include <qsplashscreen.h>
#include <q3progressbar.h>
#include <q3progressdialog.h>
#include <qevent.h>
#include <qtimer.h>

//Other includes
#include <vector>
#include <map>
#include <google/dense_hash_map>
using HASH_NAMESPACE::hash;
using GOOGLE_NAMESPACE::dense_hash_map;
using namespace std;


/*! A simple RGB color struct used to hold the highlight colour.
	Use this instead of QColor to minimise space. */
struct RGBColor {
	float red;
	float green;
	float blue;
};


/*! The type of the map used to store highlighted neurons. */
typedef dense_hash_map<unsigned int, RGBColor*> HighlightMap;

/*! The type of the map used to store the color of highlighted neurons. */
typedef map<RGBColor*, bool> HighlightColorMap;


//-------------------------- Network Viewer -------------------------------
/*! Generates a three dimensional view of the network and connections using
	OpenGL. Stores a data representation of all the neurons and connections.
	Parameters for this display are set using the Network Viewer Properties 
	widget. Some of the methods are adapted from gltools, provided in the 
	OpenGL Superbible, Third Edition.*/

/* FIXME THERE IS CURRENTLY SOME CONFLIGT BETWEEN THIS CLASS IN FULL RENDER
	MODE AND THE NETWORK MONITOR DUE TO INTERLACING OF OPENGL COMMANDS. */
//-------------------------------------------------------------------------

class NetworkViewer : public QGLWidget {
	Q_OBJECT

	public:
		NetworkViewer(QWidget *parent, QSplashScreen*, DBInterface*, unsigned int);
		~NetworkViewer();
		void addHighlight(unsigned int neurID, RGBColor* highlightColor);
		void cancelRenderProgress();
		void clearHighlights();
		void deleteConnectionGroup(unsigned int ID);
		void deleteNeuronGroup(unsigned int ID);
		vector<unsigned int>* getConnectionViewVector();
		void loadConnectionGroup(unsigned int, bool);
		void loadDefaultClippingVolume();
		void loadNeuronGroup(unsigned int, bool);
		void reloadEverything();
		void refresh();
		void resetView();
		void setConnectionView(vector<unsigned int>);
		void setFullRenderMode(bool);
		void setLayerView(vector<unsigned int>);
		void setMaxAutoLoadConnGrpSize(unsigned int);
		void setNetworkViewerProperties(QWidget*);
		void setNeuronConnectionMode(bool mode, unsigned int neuronGroup1, bool betwMode, unsigned int neuronGroup2);
		void setNeuronFilterMode(bool mode, char minWeight, char maxWeight, bool showFrom, bool showTo, bool updateDisplay);
		void setRenderDelay(double);
		void setRenderProgressBar(Q3ProgressBar*);
		void showConnections(bool);
		void zoomAboveLayer(unsigned int layID);
		void zoomToLayer(unsigned int layID);


    protected:
		//Methods inherited from QGLWidget
		void initializeGL();
		void paintGL();
		void resizeGL(int width, int height);


	private slots:
		void acceleratorKeyPressed (int acceleratorID);
		void setFullRender();


	private:
		//=========================== VARIABLES =======================================
		/*! Links keys to changes in graphics, perspective etc..*/
		Q3Accel *keyboardAccelerator;

		/*! Hold a reference to the network viewer properties to enable the neuron number 
		to be updated. Because of tricky stuff with headers, this is stored as a reference
		to Widget, not the class itself which is then cast as network viewer properties at 
		run time.*/
		//FIXME CHANGE THIS TO SIGNALS/ SLOTS
		QWidget *networkViewerProperties;

		/*! Short version of reference to main QApplication */
		SpikeStreamApplication *spikeStrApp;

		/*! Reference to database handling class.*/
		DBInterface* dbInterface;

		/*! Angle in DEGREES for OpenGL perspective. Controls how much of scene is clipped.*/
		GLfloat perspective_angle;

		/*! For OpenGL perspective. Controls how much of scene is clipped.*/
		GLfloat perspective_near;

		/*! For OpenGL perspective. Controls how much of scene is clipped.*/
		GLfloat perspective_far;

		/*! Amount that scene is rotated around X axis.*/
		GLfloat sceneRotateX;

		/*! Amount that scene is rotated around Z axis.*/
		GLfloat sceneRotateZ;

		/*! CameraMatrix holds the location and coordinate system of the camera.*/
		GLfloat cameraMatrix [16];
		
		/*! Used for rotating camera matrix.*/
		GLfloat rotationMatrix [16];

		/*! Default view in which all neuron groups should be visible.*/
		ClippingVolume defaultClippingVol;

		/*! Map holding neurons that are to be highlighted and their highlight color.*/
		HighlightMap highlightMap;

		/*! Stores all of the colors so that they can be deleted. Several neurons may
			 point to the same colour so need to track the colours separately. */
		HighlightColorMap highlightColorMap;

		/*! Reduces checking of highlight map. Set to true when highlighting is enabled.*/
		bool highlightMode;

		//Declare pointers here to save redeclaring them whilst drawing graphics
		float* xPosPtr;
		float* yPosPtr;
		float* zPosPtr;
		unsigned int *neuronIDPtr;
		float* fromXPtr;
		float* fromYPtr;
		float* fromZPtr;
		float* toXPtr;
		float* toYPtr;
		float* toZPtr;
		char* weightPtr;
		unsigned int *fromNeuronIDPtr;
		unsigned int *toNeuronIDPtr;

		/*! In full render mode, the network viewer sets full render to true after render delay
			When full render is true the full render is displayed.*/
		bool fullRenderMode;

		/*! Determines whether we perform a full render or just vertices.*/
		bool fullRender;

		/*! Delay between the last keystroke and start of full render in full render mode.*/
		int renderDelay_ms;

		/*! Timer to start full render after the delay.*/
		QTimer *renderTimer;

		/*! Cancels render.*/
		bool cancelRender;

		/*! Reference to the progress bar in the network viewer properties.*/
		Q3ProgressBar *renderProgressBar;
		
		/*! Displays the progress as the neuron and connection groups are loaded.*/
		Q3ProgressDialog *loadProgressDialog;
		
		/*! In draw connections mode the connections are drawn.*/
		bool drawConnections;
		
		/*! In neuron connection mode, the connections to a single neuron are shown
			When neuron filter mode is on, these connections are filtered according to 
			various criteria.*/
		bool neuronConnectionMode;
		bool neuronFilterMode;
		
		/*! Store the details about the neurons for drawing using OpenGL.*/
		map<unsigned int, NeuronGroupHolder*> neuronGrpMap;

		/*! Store the details about the connections for drawing using OpenGL.*/
		map<unsigned int, ConnectionGroupHolder*> connectionGrpMap;
		
		/*! Vectors of the IDs of the visible neuron groups.*/
		vector<unsigned int> layerViewVector;

		/*! Vectors of the IDs of the visible connection groups.*/
		vector<unsigned int> connectionViewVector;
		
		/*! The neuron that is normally displayed in single neuron mode.*/
		unsigned int firstSingleNeuronID;

		/*! In between mode, the connections between the firstSingle neuron and the second single neuron are displayed.*/
		unsigned int secondSingleNeuronID;

		//Used to navigate around the layer in single neuron mode
		unsigned int maxFirstSingleNeuronID;
		unsigned int minFirstSingleNeuronID;
		unsigned int firstSingleNeuronGroupWidth;
		unsigned int maxSecondSingleNeuronID;
		unsigned int minSecondSingleNeuronID;
		unsigned int secondSingleNeuronGroupWidth;
		unsigned int firstSingleNeurGrpID;
		unsigned int secondSingleNeurGrpID;
		
		/*! Used for filtering weights in single neuron mode.*/
		char maxWeight;

		/*! Used for filtering weights in single neuron mode.*/
		char minWeight;
		
		/*! Show from connections in single neuron mode.*/
		bool showFromConnections;

		/*! Show to connections in single neuron mode.*/
		bool showToConnections; 
		
		/*! Shows connections between two highlighted neurons.*/
		bool showBetweenConnections;
		
		/*! Stores which neurons are connected to the single neuron in from filter mode.*/
		map<unsigned int, bool>fromNeuronMap;

		/*! Stores which neurons are connected to the single neuron in to filter mode.*/
		map<unsigned int, bool>toNeuronMap;

		/*! Maximum sizew of connection group that is automatically loaded. Groups bigger 
			than this only when they are set visible. Set in spikestream.config.*/
		unsigned int maxAutoLoadConnGrpSize;

		/*! When this is true a new display list is created. Otherwise the existing
			display list is used to draw the graphics. */
		bool viewStateChanged;

		/*! Reference to the display list for the graphics. This stores OpenGL
			commands, which can then be played back as a block. */
		GLuint mainDisplayList;

		/*! Records when the rendering is taking place so that accelerator keys
			do not change the settings mid render */
		bool paintingGL;

		/*! During full render, do not want to start render or resize, so record
			whether paintGL() or resizeGL() have been called and then call them
			again at the end of the render. newTempScreen width and height are the
			latest values of the screen width and height if resizeGL() has been called. */
		bool resizeSkipped;

		/*! During full render, do not want to start render or resize, so record
			whether paintGL() or resizeGL() have been called and then call them
			again at the end of the render. newTempScreen width and height are the
			latest values of the screen width and height if resizeGL() has been called. */
		bool paintSkipped;
		int newTempScreenWidth;
		int newTempScreenHeight;


		//=========================== METHODS =======================================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		NetworkViewer (const NetworkViewer&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		NetworkViewer operator = (const NetworkViewer&);

		void checkOpenGLErrors();
		void deleteAllConnectionGroups();
		void deleteAllNeuronGroups();
		void disableFullRender();
		void drawAxes();
		void drawSolidSphere(GLdouble radius, GLint slices, GLint stacks);
		void fillRotationMatrix(float, float, float, float);
		float getFloat(string s);
		void initialiseCameraParameters();
		void initialiseFullRender();
		void loadAllConnectionGroups(QSplashScreen* splashScreen = 0);
		void loadAllNeuronGroups(QSplashScreen* splashScreen = 0);
		void rotateVector(float, float, float, GLfloat[]);
		void rotateXAxis(float angle);
		void rotateZAxis(float angle);
		void viewClippingVolume_Horizontal(ClippingVolume);
		void viewClippingVolume_Vertical(ClippingVolume);

};


#endif//NETWORKVIEWER_H
