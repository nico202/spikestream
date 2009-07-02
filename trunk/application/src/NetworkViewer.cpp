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
#include "NetworkViewer.h"
#include "SpikeStreamMainWindow.h"
#include "NetworkViewerProperties.h"
#include "Debug.h"
#include "Utilities.h"
#include "PerformanceTimer.h"

//Qt includes
#include <qstring.h>
#include <qmessagebox.h>

//Other includes
#include <mysql++.h>
#include <math.h>
#include <iostream>
using namespace std;
using namespace mysqlpp;


/*! Define a constant for PI. */
#define GL_PI 3.1415f


/* gltools macro to convert degrees to radians and vice versa. */
#define GLT_PI_DIV_180 0.017453292519943296
#define GLT_INV_PI_DIV_180 57.2957795130823229
#define gltDegToRad(x)	((x)*GLT_PI_DIV_180)
#define gltRadToDeg(x)	((x)*GLT_INV_PI_DIV_180)


/* Definitions of colours. */
#define DEFAULT_NEURON_COLOUR_FULL 0.5f,0.5f,0.5f
#define HIGHLIGHT_FIRST_NEURON_COLOUR_FULL 1.0f,0.0f,0.0f
#define HIGHLIGHT_SECOND_NEURON_COLOUR_FULL 0.0f,1.0f,0.0f
#define DEFAULT_NEURON_COLOUR_OUTLINE 0.0f,0.0f,0.0f
#define HIGHLIGHT_FIRST_NEURON_COLOUR_OUTLINE 1.0f,0.0f,0.0f
#define HIGHLIGHT_SECOND_NEURON_COLOUR_OUTLINE 0.0f,1.0f,0.0f
#define CONNECTED_NEURON_COLOUR 1.0f,0.0f,1.0f
#define POSITIVE_CONNECTION_COLOUR 0.718f,0.165f,0.180f
#define NEGATIVE_CONNECTION_COLOUR 0.067f,0.553f,0.110f


/* Light and material Data. */
//FIXME THIS HAS BEEN PUT IN FAIRLY RANDOMLY TO ILLUMINATE THE NEURONS. SOMETHING BETTER COULD BE DONE HERE
GLfloat fLightPos[4]   = { -100.0f, 100.0f, 50.0f, 1.0f };  // Point source
GLfloat fNoLight[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat fLowLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat fogColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat fBrightLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };


/*! Constructor. */
NetworkViewer::NetworkViewer( QWidget *parent, QSplashScreen* splashScreen, DBInterface* dbInter, unsigned int maxAutoLoadConns) : QGLWidget(parent, "Network viewer") {
	//Store a reference to the database interface
	dbInterface = dbInter;

	/* Store short version of reference to main QApplication to enable the processing 
		of events during heavy rendering operations */
	spikeStrApp = SpikeStreamMainWindow::spikeStreamApplication;

	//View state needs to be loaded at start up
	viewStateChanged = true;

	//Not currently painting GL
	paintingGL = false;
	
	/* Set max size of connection group that is automatically loaded. This value 
		is taken from configuration file neuronapplication.config. */
	maxAutoLoadConnGrpSize = maxAutoLoadConns;
	
	//Set up load progress dialog
	loadProgressDialog = new Q3ProgressDialog(this, "load progress", true);
	
	//Set up parameters to control the viewing perspective
	perspective_angle = 46.0f;
	perspective_near = 1.0f;
	perspective_far = 100000.0f;//Set this to a large number so everything will be visible
	
	//Set up timer for rendering
	renderTimer = new QTimer( this );
	connect( renderTimer, SIGNAL(timeout()), this, SLOT(setFullRender()) );
	
	//Initialise the camera and frame parameters. 
	//This must be done before loading the database, which changes some of the initialised parameters
	initialiseCameraParameters();
	fullRenderMode = false;
	fullRender = false;
	renderDelay_ms =1000;
	drawConnections = true;
	neuronConnectionMode = false;
	firstSingleNeuronID = 0;
	secondSingleNeuronID = 0;
	minFirstSingleNeuronID = 0;
	maxFirstSingleNeuronID = 0;
	firstSingleNeuronGroupWidth = 0;
	minSecondSingleNeuronID = 0;
	maxSecondSingleNeuronID = 0;
	secondSingleNeuronGroupWidth = 0;
	neuronFilterMode = false;
	maxWeight = 127;
	minWeight = -128;
	showFromConnections = true;
	showToConnections = true;
	showBetweenConnections = false;

	//Set up highlighting
	highlightMode = false;
	highlightMap.set_empty_key(EMPTY_NEURON_ID_KEY);
	highlightMap.set_deleted_key(DELETED_NEURON_ID_KEY);

	//Load up neuron Group data from database
	loadAllNeuronGroups(splashScreen);
	
	//Load connection group data from database
	loadAllConnectionGroups(splashScreen);
	
	//Set up keyboard accelerators for the network viewer
	//Use accelerators rather than key press event to avoid problems about focus
	keyboardAccelerator = new Q3Accel( this );
	
	//Add all  the key combinations that will be required.
	keyboardAccelerator->insertItem(Qt::Key_Up);
	keyboardAccelerator->insertItem(Qt::Key_Down);
	keyboardAccelerator->insertItem(Qt::Key_Left);
	keyboardAccelerator->insertItem(Qt::Key_Right);
	keyboardAccelerator->insertItem(Qt::CTRL + Qt::Key_Up);
	keyboardAccelerator->insertItem(Qt::CTRL + Qt::Key_Down);
	keyboardAccelerator->insertItem(Qt::CTRL + Qt::Key_Left);
	keyboardAccelerator->insertItem(Qt::CTRL + Qt::Key_Right);
	keyboardAccelerator->insertItem(Qt::SHIFT + Qt::Key_Up);
	keyboardAccelerator->insertItem(Qt::SHIFT + Qt::Key_Down);
	keyboardAccelerator->insertItem(Qt::SHIFT + Qt::Key_Left);
	keyboardAccelerator->insertItem(Qt::SHIFT + Qt::Key_Right);
	keyboardAccelerator->insertItem(Qt::ALT + Qt::Key_Up);
	keyboardAccelerator->insertItem(Qt::ALT + Qt::Key_Down);
	keyboardAccelerator->insertItem(Qt::ALT + Qt::Key_Left);
	keyboardAccelerator->insertItem(Qt::ALT + Qt::Key_Right);
	keyboardAccelerator->insertItem(Qt::CTRL + Qt::ALT + Qt::Key_Up);
	keyboardAccelerator->insertItem(Qt::CTRL + Qt::ALT + Qt::Key_Down);
	keyboardAccelerator->insertItem(Qt::CTRL + Qt::ALT + Qt::Key_Left);
	keyboardAccelerator->insertItem(Qt::CTRL + Qt::ALT + Qt::Key_Right);
	keyboardAccelerator->insertItem(Qt::CTRL + Qt::Key_R);
	keyboardAccelerator->insertItem(Qt::CTRL + Qt::Key_Equal);
	keyboardAccelerator->insertItem(Qt::CTRL + Qt::Key_Minus);
	 
	 //Connect up accelerator with the method that will process the key events
	 connect (keyboardAccelerator, SIGNAL(activated(int)), this, SLOT(acceleratorKeyPressed(int)));
}


/*! Destructor. */
NetworkViewer::~NetworkViewer(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING NETWORK VIEWER"<<endl;
	#endif//MEMORY_DEBUG

	//Delete all neuron group holders. These handle the deletion of their arrays
	for(map<unsigned int, NeuronGroupHolder*>::iterator iter = neuronGrpMap.begin(); iter != neuronGrpMap.end(); ++iter)
		delete iter->second;
		
	//Delete all connection group holders. These handle the deletion of their arrays
	for(map<unsigned int, ConnectionGroupHolder*>::iterator iter = connectionGrpMap.begin(); iter != connectionGrpMap.end(); ++iter)
		delete iter->second;
}


//----------------------------------------------------------------------------------------------
//------------------------------------- PUBLIC METHODS  ----------------------------------------
//----------------------------------------------------------------------------------------------

/*! Adds a neuron to be highlighted.*/
void NetworkViewer::addHighlight(unsigned int neurID, RGBColor* highlightColor){
	highlightMap[neurID] = highlightColor;
	highlightColorMap[highlightColor] = true;
	highlightMode = true;
	viewStateChanged = true;
}


/*! Cancels the render in full render mode. */
void NetworkViewer::cancelRenderProgress(){
	cancelRender = true;
}


/*! Clears all of the current highlights. */
void NetworkViewer::clearHighlights(){
	highlightMode = false;
	highlightMap.clear();

	//Clean up colors allocatd on the heap
	for(HighlightColorMap::iterator iter = highlightColorMap.begin(); iter != highlightColorMap.end(); ++iter)
		delete iter->first;

	viewStateChanged = true;
	updateGL();
}


/*! Removes the data structures for a connection group after it has been deleted from the database
	by the ConnectionWidget. */
void NetworkViewer::deleteConnectionGroup(unsigned int connGrpID){
	//Delete connection group holder pointed to by map
	delete connectionGrpMap[connGrpID];

	//Remove neuron group holder from map
	connectionGrpMap.erase(connGrpID);
	
	//Remove neuronGroupID from view vector
	vector<unsigned int>::iterator connViewIter;
	for(connViewIter = connectionViewVector.begin(); connViewIter != connectionViewVector.end(); ++connViewIter){
		if(*connViewIter == connGrpID){
			connectionViewVector.erase(connViewIter);
			break;
		}
	}
	//Record the fact that the view state has changed
	viewStateChanged = true;

	//Update the display
	updateGL();
}


/*! Removes the data structures for a neuron group after it has been deleted from the database
	by the LayerWidget. Also removes any connections to this neuron group. 
	Exception handling for this class should be done by the calling method. */
void NetworkViewer::deleteNeuronGroup(unsigned int neuronGrpID){
	//Delete neuron group holder pointed to by map
	delete neuronGrpMap[neuronGrpID];

	//Remove neuron group holder from map
	neuronGrpMap.erase(neuronGrpID);
	
	//Remove neuronGroupID from view vector
	vector<unsigned int>::iterator layerViewIter;
	for(layerViewIter = layerViewVector.begin(); layerViewIter != layerViewVector.end(); ++layerViewIter){
		if(*layerViewIter == neuronGrpID){
			layerViewVector.erase(layerViewIter);
			break;
		}
	}
	//Delete the connection groups involving this neuron group
	try{
		Query query = dbInterface->getQuery();
		query.reset();
		query<<"SELECT ConnGrpID FROM ConnectionGroups WHERE FromNeuronGrpID = "<<neuronGrpID<<" OR ToNeuronGrpID = "<<neuronGrpID;

                StoreQueryResult connRes = query.store();
                for(StoreQueryResult::iterator connIter = connRes.begin(); connIter != connRes.end(); ++connIter){
			Row connRow(*connIter);
			unsigned short connGrpID = Utilities::getUShort((std::string)connRow["ConnGrpID"]);
			deleteConnectionGroup(connGrpID);
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"NetworkViewer: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when removing connection group: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connection Group Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"NetworkViewer: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown removing connection group: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connection Group Error", errorString);
	}
	catch(std::exception& er){// Catch-all for std exceptions
		cerr<<"NetworkViewer: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown removing connection group: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connection Group Error", errorString);
	}

	//Sort out any variables linked to the neuron group
	if(neuronGrpID == firstSingleNeurGrpID){
		firstSingleNeurGrpID = 0;
		minFirstSingleNeuronID = 0;
		maxFirstSingleNeuronID = 0;
		firstSingleNeuronGroupWidth = 0;
		showBetweenConnections = false;
	}
	if(neuronGrpID == secondSingleNeurGrpID){
		secondSingleNeurGrpID = 0;
		maxSecondSingleNeuronID = 0;
		minSecondSingleNeuronID = 0;
		secondSingleNeuronGroupWidth= 0;
		showBetweenConnections = false;
	}

	//Record the fact that the view state has changed
	viewStateChanged = true;

	//Update the display
	updateGL();
}


/*! Returns the vector containing the connections that are showing. */
vector<unsigned int>* NetworkViewer::getConnectionViewVector(){
	return &connectionViewVector;
}


/*! Loads a connection group with connGrpID from the database
	Only automatically loads connection groups less than 1000000 unless a 
	progress bar is being displayed.
	Exceptions should be handled by the class that calls this method. */
void NetworkViewer::loadConnectionGroup(unsigned int connGrpID, bool showProgressDialog) {
	Query query = dbInterface->getQuery();
	
	//Get number of connections in connection group
	query.reset();
	query<<"SELECT COUNT(*) FROM Connections WHERE ConnGrpID = "<<connGrpID;
        StoreQueryResult connCountRes = query.store();
	Row connCountRow(*connCountRes.begin());
	unsigned int numberOfConnections = Utilities::getUInt((std::string)connCountRow.at(0));
	if((numberOfConnections > maxAutoLoadConnGrpSize) && !showProgressDialog)
		return;//Do not automatically load large connection groups
		
	//If using progress bar, set up with the number of neurons to be loaded
	int progressCount = 0;
	if(showProgressDialog){
		loadProgressDialog->setTotalSteps(numberOfConnections);
		loadProgressDialog->setLabelText("Loading connection group " + QString::number(connGrpID));
	}
	
	//Get type of connection group
	query.reset();
	query<<"SELECT ConnType, FromNeuronGrpID, ToNeuronGrpID FROM ConnectionGroups WHERE ConnGrpID = "<<connGrpID;
        StoreQueryResult connGrpRes = query.store();
	Row connGrpRow(*connGrpRes.begin());
	unsigned short connType = Utilities::getUShort((std::string)connGrpRow["ConnType"]);
	ConnectionGroupHolder *connGrpHolder = new ConnectionGroupHolder();
	connGrpHolder->connectionType = connType;
		
	//Get the pre and post neuron group holders. 
	//These contain the data about the X, Y and Z positions and by using these from memory, many database accesses can be avoided
	unsigned int fromNeuronGrpID = Utilities::getUInt((std::string)connGrpRow["FromNeuronGrpID"]);
	unsigned int toNeuronGrpID = Utilities::getUInt((std::string)connGrpRow["ToNeuronGrpID"]);
	NeuronGroupHolder *fromNeurGrpHolder = neuronGrpMap[fromNeuronGrpID];
	NeuronGroupHolder *toNeurGrpHolder = neuronGrpMap[toNeuronGrpID];
	//Also get start IDs for the from and to neuron group holder
	unsigned int fromNeurGrpStartID = fromNeurGrpHolder->startNeuronID;
	unsigned int toNeurGrpStartID = toNeurGrpHolder->startNeuronID;
	
	//Now get pre and post neuron ids for all connections in this group
	query.reset();
	query<<"SELECT PreSynapticNeuronID, PostSynapticNeuronID, Weight FROM Connections WHERE ConnGrpID ="<<connGrpID;
        UseQueryResult res = query.use();
	
	//Initialise arrays in connection group holder
	connGrpHolder->numberOfConnections = numberOfConnections;
	connGrpHolder->fromNeuronIDArray = new unsigned int[numberOfConnections];
	connGrpHolder->fromXArray = new float[numberOfConnections];
	connGrpHolder->fromYArray = new float[numberOfConnections];
	connGrpHolder->fromZArray = new float[numberOfConnections];
	connGrpHolder->toNeuronIDArray = new unsigned int[numberOfConnections];
	connGrpHolder->toXArray = new float[numberOfConnections];
	connGrpHolder->toYArray = new float[numberOfConnections];
	connGrpHolder->toZArray = new float[numberOfConnections];
	connGrpHolder->weightArray = new char[numberOfConnections];
	
	//Set up pointers to the arrays
	unsigned int *fromNeuronIDPtr = connGrpHolder->fromNeuronIDArray;
	float *fromXPtr = connGrpHolder->fromXArray;
	float *fromYPtr = connGrpHolder->fromYArray;
	float *fromZPtr = connGrpHolder->fromZArray;
	unsigned int *toNeuronIDPtr = connGrpHolder->toNeuronIDArray;
	float *toXPtr = connGrpHolder->toXArray;
	float *toYPtr = connGrpHolder->toYArray;
	float *toZPtr = connGrpHolder->toZArray;
	char *weightPtr = connGrpHolder->weightArray;
	
	//Work through the list of pre and post neurons to extract and store the positions of these neurons
	Row prePostNeuronRow;
	if(res){
                while (prePostNeuronRow = res.fetch_row()){
                        unsigned int preNeuronID = Utilities::getUInt((std::string)prePostNeuronRow.at(0));
                        unsigned int postNeuronID = Utilities::getUInt((std::string)prePostNeuronRow.at(1));

                        //Extract the weight. This is stored as a char to save space
                        *weightPtr = (char)Utilities::getShort((std::string)prePostNeuronRow.at(2));
                        ++weightPtr;

                        //Load up position of the from neuron
                        *fromNeuronIDPtr = preNeuronID;
                        *fromXPtr = fromNeurGrpHolder->xPosArray[preNeuronID - fromNeurGrpStartID];
                        *fromYPtr = fromNeurGrpHolder->yPosArray[preNeuronID - fromNeurGrpStartID];
                        *fromZPtr = fromNeurGrpHolder->zPosArray[preNeuronID - fromNeurGrpStartID];
                        ++fromNeuronIDPtr;
                        ++fromXPtr;
                        ++fromYPtr;
                        ++fromZPtr;

                        //Load up position of the to neuron
                        *toNeuronIDPtr = postNeuronID;
                        *toXPtr = toNeurGrpHolder->xPosArray[postNeuronID - toNeurGrpStartID];
                        *toYPtr = toNeurGrpHolder->yPosArray[postNeuronID - toNeurGrpStartID];
                        *toZPtr = toNeurGrpHolder->zPosArray[postNeuronID - toNeurGrpStartID];
                        ++toNeuronIDPtr;
                        ++toXPtr;
                        ++toYPtr;
                        ++toZPtr;

                        //Increase the progress count. Update the progress bar every 1000;
                        if(showProgressDialog){
                                ++progressCount;
                                if((progressCount % 1000)== 0)
                                        loadProgressDialog->setProgress(progressCount);
                        }
                }

		//Store new connection group
		connectionGrpMap[connGrpID] = connGrpHolder;
		
		//Hide progress bar if showing
		if(showProgressDialog)
			loadProgressDialog->setProgress(progressCount);
	}
	else{
		cerr<<"CANNOT RETRIEVE CONNECTION GROUP DETAILS: "<<query.error()<<endl;
		loadProgressDialog->reset();
	}

	//Record the fact that the view state has changed
	viewStateChanged = true;

	//Update the display if we are not loading up for the first time
	if(showProgressDialog)
		updateGL();
}


/*! Loads the default clipping volume from the database.
	Exception handling should be done by the class that calls this method. */
void NetworkViewer::loadDefaultClippingVolume(){
	//Need the maximum and minimum x, y and z values
	Query query = dbInterface->getQuery();
	query.reset();
	query<<"SELECT * FROM NeuronGroups";
        StoreQueryResult neuronGrpRes = query.store();
	if(neuronGrpRes.size() == 0){//No neurons in database, so set up default clipping volume around origin
		defaultClippingVol.minX = -100;
		defaultClippingVol.maxX = 100;
		defaultClippingVol.minY = -100;
		defaultClippingVol.maxY = 100;
		defaultClippingVol.minZ = -100;
		defaultClippingVol.maxZ = 100;
	}
	else{//Set default clipping volume so that it includes all layers and the origin.
		query<<"SELECT MIN(X) FROM Neurons";
                StoreQueryResult minXRes = query.store();
		Row minXRow(*minXRes.begin());//Assume that there is only one result
		defaultClippingVol.minX = getFloat((std::string)minXRow.at(0));
		query.reset();
		query<<"SELECT MAX(X) FROM Neurons";
                StoreQueryResult maxXRes = query.store();
		Row maxXRow(*maxXRes.begin());//Assume that there is only one result
		defaultClippingVol.maxX = getFloat((std::string)maxXRow.at(0));
		query.reset();
		query<<"SELECT MIN(Y) FROM Neurons";
                StoreQueryResult minYRes = query.store();
		Row minYRow(*minYRes.begin());//Assume that there is only one result
		defaultClippingVol.minY = getFloat((std::string)minYRow.at(0));
		query.reset();
		query<<"SELECT MAX(Y) FROM Neurons";
                StoreQueryResult maxYRes = query.store();
		Row maxYRow(*maxYRes.begin());//Assume that there is only one result
		defaultClippingVol.maxY = getFloat((std::string)maxYRow.at(0));
		query.reset();
		query<<"SELECT MIN(Z) FROM Neurons";
                StoreQueryResult minZRes = query.store();
		Row minZRow(*minZRes.begin());//Assume that there is only one result
		defaultClippingVol.minZ = getFloat((std::string)minZRow.at(0));
		query.reset();
		query<<"SELECT MAX(Z) FROM Neurons";
                StoreQueryResult maxZRes = query.store();
		Row maxZRow(*maxZRes.begin());//Assume that there is only one result
		defaultClippingVol.maxZ = getFloat((std::string)maxZRow.at(0));
		
		//Want to include the origin in the starting clip volume
		if(defaultClippingVol.minX >0)
			defaultClippingVol.minX = 0;
		if(defaultClippingVol.maxX < 0)
			defaultClippingVol.maxX =0;
		if(defaultClippingVol.minY >0)
			defaultClippingVol.minY = 0;
		if(defaultClippingVol.maxY < 0)
			defaultClippingVol.maxY =0;
		if(defaultClippingVol.minZ > 0)
			defaultClippingVol.minZ = 0;
		if(defaultClippingVol.maxZ < 0)
			defaultClippingVol.maxZ = 0;
	}
}


/*! Loads up a neuron group with a particular neuron ID
	This is used to load up layers at the beginning of the application 
	and called by LayerWidget when a new layer is created.
	This method will return immediately if the neuron group has already
	been loaded. A neuron group needs to be explicitly deleted before
	it can be reloaded.
	Exception handling for this should be done by the invoking method. */
void NetworkViewer::loadNeuronGroup(unsigned int neuronGroupID, bool showProgressDialog){
	//Check to see if neuron group is already loaded
	if(neuronGrpMap.count(neuronGroupID) > 0){
		cout<<"NetworkViewer: Neuron group "<<neuronGroupID<<" already loaded"<<endl;
		return;
	}

	//Get number of neurons in neuron group
	Query query = dbInterface->getQuery();
	query.reset();
	query<<"SELECT COUNT(*) FROM Neurons WHERE NeuronGrpID = "<<neuronGroupID;
        StoreQueryResult neuronCountRes = query.store();
	Row neuronCountRow(*neuronCountRes.begin());
	unsigned int numberOfNeurons = Utilities::getUInt((std::string)neuronCountRow.at(0));
	
	//If using progress bar, set up with the number of neurons to be loaded
	int progressCount = 0;
	if(showProgressDialog){
		loadProgressDialog->setTotalSteps(numberOfNeurons);
		loadProgressDialog->setLabelText("Loading neuron group " + QString::number(neuronGroupID));
	}

	//Get neuronType for this neuron group
	query.reset();
	query<<"SELECT NeuronType FROM NeuronGroups WHERE NeuronGrpID = "<<neuronGroupID;
        StoreQueryResult neuronGrpRes = query.store();
	Row neuronGrpRow(*neuronGrpRes.begin());
	unsigned short neuronType = Utilities::getUShort((std::string)neuronGrpRow["NeuronType"]);
	NeuronGroupHolder *neurGrpHolder = new NeuronGroupHolder();
	neurGrpHolder->neuronType = neuronType;
	
	//Get locations for all neurons in this group from the database
	query.reset();
	query<<"SELECT NeuronID, X, Y, Z FROM Neurons WHERE NeuronGrpID ="<<neuronGroupID<<" ORDER BY NeuronID";
        UseQueryResult res = query.use();//UseQueryResult is much more efficient for large queries
	bool firstTimeNeurons = true;//Used for the clipping volume and to get the startID
	
	//Initialise arrays in neuron group holder
	neurGrpHolder->numberOfNeurons = numberOfNeurons;
	neurGrpHolder->neuronIDArray = new unsigned int[numberOfNeurons];
	neurGrpHolder->xPosArray = new float[numberOfNeurons];
	neurGrpHolder->yPosArray = new float[numberOfNeurons];
	neurGrpHolder->zPosArray = new float[numberOfNeurons];
	unsigned int *neuronIDPtr = neurGrpHolder->neuronIDArray;
	float *xPosPtr = neurGrpHolder->xPosArray;
	float *yPosPtr = neurGrpHolder->yPosArray;
	float *zPosPtr = neurGrpHolder->zPosArray;
		
	//Fill arrays in neuron group holder with coordinates of neurons
	Row neuronRow;
	if(res){
                while (neuronRow = res.fetch_row()){
                        //Store the NeuronID and x, y, and z coordinates of the neuron
                        *neuronIDPtr = Utilities::getUInt((std::string)neuronRow.at(0));
                        *xPosPtr = getFloat((std::string)neuronRow[1]);
                        *yPosPtr = getFloat((std::string)neuronRow[2]);
                        *zPosPtr = getFloat((std::string)neuronRow[3]);

                        //Adjust the clipping volume of the neural network if necessary
                        if(firstTimeNeurons){
                                neurGrpHolder->startNeuronID = *neuronIDPtr;//Store the startID
                                neurGrpHolder->clippingVolume.maxX = *xPosPtr;
                                neurGrpHolder->clippingVolume.maxY = *yPosPtr;
                                neurGrpHolder->clippingVolume.maxZ = *zPosPtr;
                                neurGrpHolder->clippingVolume.minX = *xPosPtr;
                                neurGrpHolder->clippingVolume.minY = *yPosPtr;
                                neurGrpHolder->clippingVolume.minZ = *zPosPtr;
                                firstTimeNeurons = false;
                        }
                        else{
                                if(*xPosPtr > neurGrpHolder->clippingVolume.maxX)
                                        neurGrpHolder->clippingVolume.maxX = *xPosPtr;
                                else if(*xPosPtr < neurGrpHolder->clippingVolume.minX)
                                        neurGrpHolder->clippingVolume.minX = *xPosPtr;
                                if(*yPosPtr > neurGrpHolder->clippingVolume.maxY)
                                        neurGrpHolder->clippingVolume.maxY = *yPosPtr;
                                else if(*yPosPtr < neurGrpHolder->clippingVolume.minY)
                                        neurGrpHolder->clippingVolume.minY = *yPosPtr;
                                if(*zPosPtr > neurGrpHolder->clippingVolume.maxZ)
                                        neurGrpHolder->clippingVolume.maxZ = *zPosPtr;
                                else if(*zPosPtr < neurGrpHolder->clippingVolume.minZ)
                                        neurGrpHolder->clippingVolume.minZ = *zPosPtr;
                        }

                        //Increase the pointers
                        ++neuronIDPtr;
                        ++xPosPtr;
                        ++yPosPtr;
                        ++zPosPtr;

                        //Increase the progress count. Update the progress bar every 1000;
                        if(showProgressDialog){
                                ++progressCount;
                                if((progressCount % 1000)== 0)
                                        loadProgressDialog->setProgress(progressCount);
                        }
                }

		firstTimeNeurons = true;
		neuronGrpMap[neuronGroupID] = neurGrpHolder;
		
		//Hide progress bar if showing
		if(showProgressDialog)
			loadProgressDialog->setProgress(progressCount);
		
		//Add ID to view vector. Default is to view all layers.
		layerViewVector.push_back(neuronGroupID);
	}
	else{//An error retrieving result from database
		cerr<<"CANNOT RETRIEVE CONNECTION GROUP DETAILS: "<<query.error()<<endl;
		loadProgressDialog->reset();
		firstTimeNeurons = true;
	}

	//View state has changed
	viewStateChanged = true;

	//Update the display if we are not loading up for the first time
	if(showProgressDialog)
		updateGL();
}


/*! Called by external classes to repaint the display. */
void NetworkViewer::refresh(){
	updateGL();
}


/*! Called after a new database has been loaded. */
void NetworkViewer::reloadEverything(){
	deleteAllNeuronGroups();
	deleteAllConnectionGroups();
	loadAllNeuronGroups();
	loadAllConnectionGroups();

	//Record the fact that the view state has changed
	viewStateChanged = true;

	//Update the display
	updateGL();
}


/*! Resets the view so all neural networks can be seen. */
void NetworkViewer::resetView(){
	viewClippingVolume_Horizontal(defaultClippingVol);
}


/*! Sets the visible connection groups. */
void NetworkViewer::setConnectionView(vector<unsigned int> connIDVect){
	//Check to see if all of the selected connection groups have been loaded.
	//Larger connection groups are not loaded automatically at start up to reduce initialisation time
	for(vector<unsigned int>::iterator iter = connIDVect.begin(); iter != connIDVect.end(); ++iter){
		if(!connectionGrpMap[*iter]){//Connection has not been loaded
			try{
				loadConnectionGroup(*iter, true);
			}
			catch (const BadQuery& er) {// Handle any query errors
				cerr<<"NetworkViewer: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
				QString errorString = "Bad query loading connection group: \"";
				errorString += er.what();
				errorString += "\"";
				QMessageBox::critical( 0, "Load Connection Group Error", errorString);
			}
			catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
				cerr<<"NetworkViewer: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
				QString errorString = "Exception thrown loading connection group: \"";
				errorString += er.what();
				errorString += "\"";
				QMessageBox::critical( 0, "Load Connection Group Error", errorString);
			}
			catch(std::exception& er){// Catch-all for std exceptions
				cerr<<"NetworkViewer: STD EXCEPTION \""<<er.what()<<"\""<<endl;
				QString errorString = "Exception thrown loading connection group: \"";
				errorString += er.what();
				errorString += "\"";
				QMessageBox::critical( 0, "Load Connection Group Error", errorString);
			}
		}
	}
	
	//Now all connection groups should be loaded, set the vector containing 
	//the list of visible connection group IDs
	connectionViewVector = connIDVect;
	
	//Update connection details table to reflect changes in the view
	((NetworkViewerProperties*)networkViewerProperties)->reloadConnections();

	//Record fact that view state has changed
	viewStateChanged = true;

	//Update the display
	updateGL();
}


/*! Controls whether neurons are represented as lit spheres or simple vertices. */
void NetworkViewer::setFullRenderMode(bool enableFullRender){
	fullRenderMode = enableFullRender;

	//Record the fact that the view state has changed
	viewStateChanged = true;

	//Set up or disable full render mode
	if(fullRenderMode){
		initialiseFullRender();
		
		//Render scene in high quality after render delay
		renderTimer->stop();
		renderTimer->start(renderDelay_ms, TRUE);
	}
	else{
		disableFullRender();
		updateGL();
	}
}


/*! Sets the visible neuron groups. */
void NetworkViewer::setLayerView(vector<unsigned int> layerIDVect){
	layerViewVector = layerIDVect;
	viewStateChanged = true;//Record the fact that the view state has changed
	updateGL();
}


/*! Sets the maximum size of connection group that is automatically loaded at startup. */
void NetworkViewer::setMaxAutoLoadConnGrpSize(unsigned int maxConnGrpSize){
	maxAutoLoadConnGrpSize = maxConnGrpSize;
}


/*! Sets the network viewer properties to enable communication. */
void NetworkViewer::setNetworkViewerProperties(QWidget *nwViewerProps){
	networkViewerProperties = nwViewerProps;
}


/*! Switches on and off whether the connections to a single neuron are displayed
	Also controls whether all connections to that neuron are displayed or only the connections
	within a particular connection group. 0 indicates that all connections are to be displayed. */
void NetworkViewer::setNeuronConnectionMode(bool neurConnMode, unsigned int firstNeurGrpID, bool betweenMode, unsigned int secondNeurGrpID){
	//Store data passed by method
	neuronConnectionMode = neurConnMode;
	showBetweenConnections = betweenMode;

	try{
		if(neuronConnectionMode && (firstSingleNeurGrpID != firstNeurGrpID)){//Only load if neuron group has changed
			firstSingleNeurGrpID = firstNeurGrpID;
	
			//Need to set starting neuron for the mode. This is the neuron at the position of the neuron group
			Query query = dbInterface->getQuery();
			query.reset();
			//Start by getting the X, Y, Z position of the neuron group
			query<<"SELECT X, Y, Z, Width FROM NeuronGroups WHERE NeuronGrpID = "<<firstNeurGrpID;
                        StoreQueryResult firstGrpPosRes = query.store();
			Row firstGrpPosRow = (*firstGrpPosRes.begin());
			
			//Now find the neuron at this position in the layer. This will be the starting neuron
			query.reset();
			query<<"SELECT NeuronID FROM Neurons WHERE X = "<<(std::string)firstGrpPosRow["X"]<<" AND Y = "<<(std::string)firstGrpPosRow["Y"]<<" AND Z = "<<(std::string)firstGrpPosRow["Z"];
                        StoreQueryResult firstNeuronRes = query.store();
			Row firstNeuronRow = (*firstNeuronRes.begin());
			firstSingleNeuronID = Utilities::getUInt((std::string)firstNeuronRow["NeuronID"]);
			((NetworkViewerProperties*)networkViewerProperties)->setFirstSingleNeuronNumber(firstSingleNeuronID);//Update network viewer properties with neuron number
			
			//Need to get the first and last IDs of this neuron group to enable user to move around in the layer with the arrow keys
			query.reset();
			query<<"SELECT MIN(NeuronID), MAX(NeuronID) FROM Neurons WHERE NeuronGrpID = "<<firstNeurGrpID;
                        StoreQueryResult firstMinMaxNeurIDRes = query.store();
			Row firstMinMaxNeurRow(*firstMinMaxNeurIDRes.begin());
			minFirstSingleNeuronID = Utilities::getInt((std::string)firstMinMaxNeurRow["MIN(NeuronID)"]);
			maxFirstSingleNeuronID = Utilities::getInt((std::string)firstMinMaxNeurRow["MAX(NeuronID)"]);
			
			//Get the length of the neuron group
			firstSingleNeuronGroupWidth = Utilities::getInt((std::string)firstGrpPosRow["Width"]);
		}
		//Disable navigation when neuron connection mode is off
		else if(!neuronConnectionMode) {
			firstSingleNeurGrpID = 0;
		}
		
		//Load up all the stuff for the second neuron if in between mode
		if(neuronConnectionMode && showBetweenConnections && (secondSingleNeurGrpID != secondNeurGrpID)){//Only load if neuron group has changed
			secondSingleNeurGrpID = secondNeurGrpID;
	
			//Need to set starting neuron for the mode. This is the neuron at the position of the neuron group
			Query query = dbInterface->getQuery();
			query.reset();
			//Start by getting the X, Y, Z position of the neuron group
			query<<"SELECT X, Y, Z, Width FROM NeuronGroups WHERE NeuronGrpID = "<<secondNeurGrpID;
                        StoreQueryResult secondGrpPosRes = query.store();
			Row secondGrpPosRow = (*secondGrpPosRes.begin());
			
			//Now find the neuron at this position in the layer. This will be the starting neuron
			query.reset();
			query<<"SELECT NeuronID FROM Neurons WHERE X = "<<(std::string)secondGrpPosRow["X"]<<" AND Y = "<<(std::string)secondGrpPosRow["Y"]<<" AND Z = "<<(std::string)secondGrpPosRow["Z"];
                        StoreQueryResult secondNeuronRes = query.store();
			Row secondNeuronRow = (*secondNeuronRes.begin());
			secondSingleNeuronID = Utilities::getUInt((std::string)secondNeuronRow["NeuronID"]);
			((NetworkViewerProperties*)networkViewerProperties)->setSecondSingleNeuronNumber(secondSingleNeuronID);//Update network viewer properties with neuron number
			
			//Need to get the first and last IDs of this neuron group to enable user to move around in the layer with the arrow keys
			query.reset();
			query<<"SELECT MIN(NeuronID), MAX(NeuronID) FROM Neurons WHERE NeuronGrpID = "<<secondNeurGrpID;
                        StoreQueryResult secondMinMaxNeurIDRes = query.store();
			Row secondMinMaxNeurRow(*secondMinMaxNeurIDRes.begin());
			minSecondSingleNeuronID = Utilities::getInt((std::string)secondMinMaxNeurRow["MIN(NeuronID)"]);
			maxSecondSingleNeuronID = Utilities::getInt((std::string)secondMinMaxNeurRow["MAX(NeuronID)"]);
			
			//Get the length of the neuron group
			secondSingleNeuronGroupWidth = Utilities::getInt((std::string)secondGrpPosRow["Width"]);
		}
		//Not using second neuron group so disable navigation in this layer
		else if (!neuronConnectionMode || !showBetweenConnections) {
			secondSingleNeurGrpID = 0;
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"NetworkViewer: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query setting neuron connection mode: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connection View Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"NetworkViewer: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown setting neuron connection mode: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connection View Error", errorString);
	}
	catch(std::exception& er){// Catch-all for std exceptions
		cerr<<"NetworkViewer: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown setting neuron connection mode: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connection View Error", errorString);
	}

	//Record that view state has changed
	viewStateChanged = true;

	//Update the graphical display
	updateGL();
}


/*! Controls which connections are visible during single neuron connection mode. */
void NetworkViewer::setNeuronFilterMode(bool mode, char minW, char maxW, bool showFrom, bool showTo, bool updateDisplay){
	neuronFilterMode = mode;
	
	if(neuronFilterMode){
		//First check that min and max weight are ok
		short minW_short = (short)minW;//Needed to facilitate comparison
		short maxW_short = (short)maxW;//Needed to facilitate comparison
		if((minW_short < -128) || (minW_short > 127) || (maxW_short < -128) || (maxW_short > 127)){
			cerr<<"NetworkViewer: MIN OR MAX WEIGHTS ARE OUT OF RANGE"<<endl;
			QMessageBox::critical( 0, "Filter Error", "Min or max weights are out of range.");
			return;
		}

		//Set min and max weights
		minWeight = minW;
		maxWeight = maxW;
		
		//Set from/ to showing options
		showFromConnections = showFrom;
		showToConnections = showTo;
	}

	//Record that view state has changed
	viewStateChanged = true;

	//Update graphical display
	if(updateDisplay)
		updateGL();
}


/*! Sets the delay between the last key press and the start of the full render. */
void NetworkViewer::setRenderDelay(double renderDelay_sec){
	renderDelay_ms = (int)(renderDelay_sec * 1000.0);
}


/*! Store a reference to the progress bar to display rendering progress. */
void NetworkViewer::setRenderProgressBar(Q3ProgressBar *progBar){
	renderProgressBar = progBar;
}


/*! Switches connection drawing mode on or off. */
void NetworkViewer::showConnections(bool sc){
	drawConnections = sc;

	//Record that view state has changed
	viewStateChanged = true;

	//Update the graphical display
	updateGL();
}


/*! Moves viewing position above selected layer and resizes it appropriately
	Don't need to set viewStateChanged here since the viewing angle is
	outside of the main list. */
void NetworkViewer::zoomAboveLayer(unsigned int layerID){
	if(layerID == 0)
		viewClippingVolume_Vertical(defaultClippingVol);
	else{
		NeuronGroupHolder *tempNeurGrpHolder = neuronGrpMap[layerID];
		viewClippingVolume_Vertical(tempNeurGrpHolder->clippingVolume);
	}
	updateGL();
}


/*! Moves viewing position beside selected layer and resizes it appropriately
	Don't need to set viewStateChanged here since the viewing angle is
	outside of the main list. */
void NetworkViewer::zoomToLayer(unsigned int layerID){
	if(layerID == 0)
		viewClippingVolume_Horizontal(defaultClippingVol);
	else{
		NeuronGroupHolder *tempNeurGrpHolder = neuronGrpMap[layerID];
		viewClippingVolume_Horizontal(tempNeurGrpHolder->clippingVolume);
	}
	updateGL();
}


//--------------------------------------------------------------------------------------
//------------------- PROTECTED METHODS INHERITED FROM QGLWidget  ----------------------
//--------------------------------------------------------------------------------------

/*! Sets up the rendering context. */
void NetworkViewer::initializeGL(){
	//Check progress bar has been set
	if(renderProgressBar == NULL){
		cerr<<"NetworkViewer: RENDER PROGRESS BAR HAS NOT BEEN SET!"<<endl;
		QMessageBox::critical( 0, "Error", "Render progress bar has not been set.");
		return;
	}

	if(fullRenderMode)
		initialiseFullRender();

	//White background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	//Create a unique id for the main display list
	mainDisplayList = glGenLists(1);
}


/*! Draw the scene. */
void NetworkViewer::paintGL(){
	if(paintingGL){
		#ifdef RENDER_DEBUG
			cout<<"RENDER IN PROGRESS, RETURNING"<<endl;
			cout.flush();
		#endif//RENDER_DEBUG
		paintSkipped = true;
		return;
	}

	paintSkipped = false;

	/* Record that painting is in process so that accelerator keys can be filtered out */
	paintingGL = true;

	/* SpikeStreamApplication records the time at which the render starts so that
		it can filter out accelerator keys pressed during the render. */
	spikeStrApp->startRender();

	#ifdef RENDER_DEBUG
		cout<<"RENDER STARTED"<<endl;
		cout.flush();
	#endif//RENDER_DEBUG

	//Reset the cancel render
	cancelRender = false;

	// Clear the window with current clearing color and store matrix state
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
	
	//Point camera towards position specified in struct for camera
	gluLookAt(
		//Location defined by fourth column of camera matrix
		cameraMatrix[12], cameraMatrix[13], cameraMatrix[14],
		//Forward is the Y axis so add this to position
		(cameraMatrix[12] + cameraMatrix[4]), (cameraMatrix[13] + cameraMatrix[5]), (cameraMatrix[14] + cameraMatrix[6]),
		//Up is the Z axis
		cameraMatrix[8], cameraMatrix[9], cameraMatrix[10]
	);
		
	//Rotate scene
	glRotatef(sceneRotateX, 1.0f, 0.0f, 0.0f);
	glRotatef(sceneRotateZ, 0.0f, 0.0f, 1.0f);
	
	// Draw the axes
	drawAxes();
	
	//Clear the maps to hold the ids of the neurons connecting from and to the single neuron
	if(neuronConnectionMode){
		fromNeuronMap.clear();
		toNeuronMap.clear();
	}

	/* Use an existing display list if one has already been created
		Only use display lists when not in full render mode. They speed up
		the graphics considerably, but crash when they are too big. */
	PerformanceTimer perfTimer;
	if(!viewStateChanged && !fullRender){
		#ifdef RENDER_DEBUG
			perfTimer.start("DISPLAY LIST RENDER");//Time the render
		#endif//RENDER_DEBUG

		//Call the display list
		glCallList(mainDisplayList);

		#ifdef RENDER_DEBUG
			perfTimer.printTime();//Display duration of render
		#endif//RENDER_DEBUG

		//Painting has finished, can now process the next accelerator key
		paintingGL = false;

		//Record time at which render has stopped
		spikeStrApp->stopRender();

		//Check for OpenGL errors
		checkOpenGLErrors();

		#ifdef RENDER_DEBUG
			cout<<"RENDER STOPPED, CHECKED FOR ERRORS"<<endl;
			cout.flush();
		#endif//RENDER_DEBUG
		return;
	}
	

	/* No existing list, so need to redraw graphics into the display list
		Only do this when not in full render mode to avoid memory problems */
	if(!fullRender){
		//Start recording new display list
		glNewList(mainDisplayList, GL_COMPILE_AND_EXECUTE);

		#ifdef RENDER_DEBUG
			perfTimer.start("COMPLETE RENDER STORING LIST");
		#endif//RENDER_DEBUG
	}
	else{
		#ifdef RENDER_DEBUG
			perfTimer.start("COMPLETE RENDER NOT STORING LIST");
		#endif//RENDER_DEBUG
	}
	
	//============================
	//    DRAW THE CONNECTIONS 
	//============================
	if(drawConnections){
		//Set connection colour to blue
		glColor3f(0.0f, 0.0f, 1.0f);
		
		//NeuronConnectionMode: ON: Connections to a single neuron will be displayed
		if(neuronConnectionMode){
			//Work through all the neuron groups listed in the view vector
			//Only draw those which match the neuron ID
			//Filter all connections not between minWeight and maxWeight
			if(neuronFilterMode){
				vector<unsigned int>::iterator connGroupIter;
				for(connGroupIter = connectionViewVector.begin(); connGroupIter != connectionViewVector.end(); ++connGroupIter){
					fromNeuronIDPtr = connectionGrpMap[*connGroupIter]->fromNeuronIDArray;
					fromXPtr = connectionGrpMap[*connGroupIter]->fromXArray;
					fromYPtr = connectionGrpMap[*connGroupIter]->fromYArray;
					fromZPtr = connectionGrpMap[*connGroupIter]->fromZArray;
					toNeuronIDPtr = connectionGrpMap[*connGroupIter]->toNeuronIDArray;
					toXPtr = connectionGrpMap[*connGroupIter]->toXArray;
					toYPtr = connectionGrpMap[*connGroupIter]->toYArray;
					toZPtr = connectionGrpMap[*connGroupIter]->toZArray;
					weightPtr = connectionGrpMap[*connGroupIter]->weightArray;

					//Set up progress bar for rendering
					if(fullRender){
						renderProgressBar->reset();
						renderProgressBar->setTotalSteps(connectionGrpMap[*connGroupIter]->numberOfConnections);
						((NetworkViewerProperties*)networkViewerProperties)->setRenderProgressLabel("Rendering connection group " + QString::number(*connGroupIter));
					}

					glBegin(GL_LINES);
						for(unsigned int i=0; i<connectionGrpMap[*connGroupIter]->numberOfConnections; ++i){
							//Show connections between the two specified neurons
							if(showBetweenConnections){
								if( (firstSingleNeuronID == *fromNeuronIDPtr && secondSingleNeuronID == *toNeuronIDPtr) || (secondSingleNeuronID == *fromNeuronIDPtr && firstSingleNeuronID == *toNeuronIDPtr) ){
									if((*weightPtr <= maxWeight) && (*weightPtr >= minWeight)){
										
										//Store a list of the to neuron IDs if only from connections are showing
										if(showFromConnections && !showToConnections){
											toNeuronMap[*toNeuronIDPtr] = true;
										}
										//Store a list of the from neuron IDs if only to connections are showing
										else if(showToConnections && !showFromConnections){
											fromNeuronMap[*fromNeuronIDPtr] = true;
										}
										
										//Set the colour of the connection
										if(*weightPtr >= 0)
											glColor3f(POSITIVE_CONNECTION_COLOUR);
										else
											glColor3f(NEGATIVE_CONNECTION_COLOUR);
											
										//Draw the connection
										glVertex3f(*fromXPtr, *fromYPtr, *fromZPtr);
										glVertex3f(*toXPtr, *toYPtr, *toZPtr);
									}
								}
							}
							//Only draw connections to selected neuron
							else if(((firstSingleNeuronID == *fromNeuronIDPtr) && showFromConnections) || ((firstSingleNeuronID == *toNeuronIDPtr) && showToConnections)){
								if((*weightPtr <= maxWeight) && (*weightPtr >= minWeight)){
									
									//Store a list of the to neuron IDs if only from connections are showing
									if(showFromConnections && !showToConnections){
										toNeuronMap[*toNeuronIDPtr] = true;
									}
									//Store a list of the from neuron IDs if only to connections are showing
									else if(showToConnections && !showFromConnections){
										fromNeuronMap[*fromNeuronIDPtr] = true;
									}
									
									//Set the colour of the connection
									if(*weightPtr >= 0)
										glColor3f(POSITIVE_CONNECTION_COLOUR);
									else
										glColor3f(NEGATIVE_CONNECTION_COLOUR);
										
									//Draw the connection
									glVertex3f(*fromXPtr, *fromYPtr, *fromZPtr);
									glVertex3f(*toXPtr, *toYPtr, *toZPtr);
								}
							}
							++fromNeuronIDPtr;
							++fromXPtr;
							++fromYPtr;
							++fromZPtr;
							++toNeuronIDPtr;
							++toXPtr;
							++toYPtr;
							++toZPtr;
							++weightPtr;

							//Update progress bar in full render mode
							if(fullRender){
								//Update progress bar and check for cancellation
								if(i % 1000 == 0){
									renderProgressBar->setProgress(i);
									spikeStrApp->processEvents();
									if(cancelRender)
									break;
								}
							}
						}
					glEnd();
				}
			}
			//Neuron filter mode is off.
			//Draw all selected connection groups that connect to this neuron
			else{
				vector<unsigned int>::iterator connGroupIter;
				for(connGroupIter = connectionViewVector.begin(); connGroupIter != connectionViewVector.end(); ++connGroupIter){
					fromNeuronIDPtr = connectionGrpMap[*connGroupIter]->fromNeuronIDArray;
					fromXPtr = connectionGrpMap[*connGroupIter]->fromXArray;
					fromYPtr = connectionGrpMap[*connGroupIter]->fromYArray;
					fromZPtr = connectionGrpMap[*connGroupIter]->fromZArray;
					toNeuronIDPtr = connectionGrpMap[*connGroupIter]->toNeuronIDArray;
					toXPtr = connectionGrpMap[*connGroupIter]->toXArray;
					toYPtr = connectionGrpMap[*connGroupIter]->toYArray;
					toZPtr = connectionGrpMap[*connGroupIter]->toZArray;
					weightPtr = connectionGrpMap[*connGroupIter]->weightArray;

					//Set up progress bar for rendering
					if(fullRender){
						renderProgressBar->reset();
						renderProgressBar->setTotalSteps(connectionGrpMap[*connGroupIter]->numberOfConnections);
						((NetworkViewerProperties*)networkViewerProperties)->setRenderProgressLabel("Rendering connection group " + QString::number(*connGroupIter));
					}

					glBegin(GL_LINES);
						for(unsigned int i=0; i<connectionGrpMap[*connGroupIter]->numberOfConnections; ++i){
							if(showBetweenConnections){
								if( (firstSingleNeuronID == *fromNeuronIDPtr && secondSingleNeuronID == *toNeuronIDPtr) || (secondSingleNeuronID == *fromNeuronIDPtr && firstSingleNeuronID == *toNeuronIDPtr) ){
									if(*weightPtr >= 0)
										glColor3f(POSITIVE_CONNECTION_COLOUR);
									else
										glColor3f(NEGATIVE_CONNECTION_COLOUR);
									glVertex3f(*fromXPtr, *fromYPtr, *fromZPtr);
									glVertex3f(*toXPtr, *toYPtr, *toZPtr);
								}

							}
							//Only draw connections to selected neuron
							else if((firstSingleNeuronID == *fromNeuronIDPtr) || (firstSingleNeuronID == *toNeuronIDPtr)){
								if(*weightPtr >= 0)
									glColor3f(POSITIVE_CONNECTION_COLOUR);
								else
									glColor3f(NEGATIVE_CONNECTION_COLOUR);
								glVertex3f(*fromXPtr, *fromYPtr, *fromZPtr);
								glVertex3f(*toXPtr, *toYPtr, *toZPtr);
							}
							++fromNeuronIDPtr;
							++fromXPtr;
							++fromYPtr;
							++fromZPtr;
							++toNeuronIDPtr;
							++toXPtr;
							++toYPtr;
							++toZPtr;
							++weightPtr;

							//Update progress bar in full render mode
							if(fullRender){
								//Update progress bar and check for cancellation
								if(i % 1000 == 0){
									renderProgressBar->setProgress(i);
									spikeStrApp->processEvents();
									if(cancelRender)
									break;
								}
							}
							else{//Filter out events during render
								if(i % 1000 == 0){
									spikeStrApp->processEvents();
								}
							}
						}
					glEnd();
				}
			}
		}
		else{//Draw connections to all neurons
			//Work through all the neuron groups listed in the view vector
			vector<unsigned int>::iterator connGroupIter;
			for(connGroupIter = connectionViewVector.begin(); connGroupIter != connectionViewVector.end(); ++connGroupIter){
				fromXPtr = connectionGrpMap[*connGroupIter]->fromXArray;
				fromYPtr = connectionGrpMap[*connGroupIter]->fromYArray;
				fromZPtr = connectionGrpMap[*connGroupIter]->fromZArray;
				toXPtr = connectionGrpMap[*connGroupIter]->toXArray;
				toYPtr = connectionGrpMap[*connGroupIter]->toYArray;
				toZPtr = connectionGrpMap[*connGroupIter]->toZArray;
				weightPtr = connectionGrpMap[*connGroupIter]->weightArray;

				//Set up progress bar for rendering
				if(fullRender){
					renderProgressBar->reset();
					renderProgressBar->setTotalSteps(connectionGrpMap[*connGroupIter]->numberOfConnections);
					((NetworkViewerProperties*)networkViewerProperties)->setRenderProgressLabel("Rendering connection group " + QString::number(*connGroupIter));
				}

				glBegin(GL_LINES);
					for(unsigned int i=0; i < connectionGrpMap[*connGroupIter]->numberOfConnections; ++i){
						if(*weightPtr >= 0)
							glColor3f(POSITIVE_CONNECTION_COLOUR);
						else
							glColor3f(NEGATIVE_CONNECTION_COLOUR);
						glVertex3f(*fromXPtr, *fromYPtr, *fromZPtr);
						glVertex3f(*toXPtr, *toYPtr, *toZPtr);
						++fromXPtr;
						++fromYPtr;
						++fromZPtr;
						++toXPtr;
						++toYPtr;
						++toZPtr;
						++weightPtr;

						//Update progress bar in full render mode
						if(fullRender){
							//Update progress bar and check for cancellation
							if(i % 1000 == 0){
								renderProgressBar->setProgress(i);
								spikeStrApp->processEvents();
								if(cancelRender)
								break;
							}
						}
					}
				glEnd();
			}
		}
	}
	
		
	//============================
	//     DRAW THE NEURONS 
	//============================
	/* This can be done in outline or full render.
		Separate sections of code are used to draw neurons depending on whether the 
		singleNeuronConnection mode is on. This is to prevent unnecessary if statements 
		within parts of the code that need to run as quickly as possible
	*/
	//DRAW NEURONS AS SPHERES WITH LIGHTING EFFECTS
	if(fullRender && !cancelRender){
		//Create iterator to work through visible layers
		vector<unsigned int>::iterator layerViewIter;
		
		// HIGHLIGHT SINGLE NEURON IN RED
		if(neuronConnectionMode){//This highlights a single neuron in red
			//Work through the neuron groups listed in the view vector
			for(layerViewIter = layerViewVector.begin(); layerViewIter != layerViewVector.end(); ++layerViewIter){
				neuronIDPtr = neuronGrpMap[*layerViewIter]->neuronIDArray;
				xPosPtr = neuronGrpMap[*layerViewIter]->xPosArray;
				yPosPtr = neuronGrpMap[*layerViewIter]->yPosArray;
				zPosPtr = neuronGrpMap[*layerViewIter]->zPosArray;

				//Set up progress bar and label for rendering
				renderProgressBar->reset();
				renderProgressBar->setTotalSteps(neuronGrpMap[*layerViewIter]->numberOfNeurons);
				((NetworkViewerProperties*)networkViewerProperties)->setRenderProgressLabel("Rendering neuron group " + QString::number(*layerViewIter));

				for(unsigned int i=0; i<neuronGrpMap[*layerViewIter]->numberOfNeurons; ++i){
					glPushMatrix();//Store current state of matrix
					glTranslatef(*xPosPtr, *yPosPtr, *zPosPtr);//Translate to sphere position
					if(*neuronIDPtr == firstSingleNeuronID)
						glColor3f(HIGHLIGHT_FIRST_NEURON_COLOUR_FULL);
					else if(*neuronIDPtr == secondSingleNeuronID && showBetweenConnections)
						glColor3f(HIGHLIGHT_SECOND_NEURON_COLOUR_FULL);
					else if(fromNeuronMap[*neuronIDPtr])//Paint neurons connecting from themselves to this neuron a differnt colour
						glColor3f(CONNECTED_NEURON_COLOUR);
					else if(toNeuronMap[*neuronIDPtr])
						glColor3f(CONNECTED_NEURON_COLOUR);
					else if(highlightMode && highlightMap.count(*neuronIDPtr))
						glColor3f(highlightMap[*neuronIDPtr]->red, highlightMap[*neuronIDPtr]->green, highlightMap[*neuronIDPtr]->blue);
					else
						glColor3f(DEFAULT_NEURON_COLOUR_FULL);
					drawSolidSphere(0.1f, 21, 11);
					glPopMatrix();
					++neuronIDPtr;
					++xPosPtr;
					++yPosPtr;
					++zPosPtr;

					//Update progress bar and check for cancellation
					if(i % 1000 == 0){
						renderProgressBar->setProgress(i);
						spikeStrApp->processEvents();
						if(cancelRender)
							break;
					}
				}
			}
		}
		
		// PAINT ALL NEURONS THE SAME COLOUR
		else if(!cancelRender){
			//Work through the neuron groups listed in the view vector
			for(layerViewIter = layerViewVector.begin(); (layerViewIter != layerViewVector.end()) && !cancelRender; ++layerViewIter){
				xPosPtr = neuronGrpMap[*layerViewIter]->xPosArray;
				yPosPtr = neuronGrpMap[*layerViewIter]->yPosArray;
				zPosPtr = neuronGrpMap[*layerViewIter]->zPosArray;

				//Set up progress bar and label for rendering
				renderProgressBar->reset();
				renderProgressBar->setTotalSteps(neuronGrpMap[*layerViewIter]->numberOfNeurons);
				((NetworkViewerProperties*)networkViewerProperties)->setRenderProgressLabel("Rendering neuron group " + QString::number(*layerViewIter));

				if(highlightMode){
					neuronIDPtr = neuronGrpMap[*layerViewIter]->neuronIDArray;
					for(unsigned int i=0; i<neuronGrpMap[*layerViewIter]->numberOfNeurons; ++i){
						glPushMatrix();//Store current state of matrix
						glTranslatef(*xPosPtr, *yPosPtr, *zPosPtr);//Translate to sphere position
						if(highlightMode && highlightMap.count(*neuronIDPtr))
							glColor3f(highlightMap[*neuronIDPtr]->red, highlightMap[*neuronIDPtr]->green, highlightMap[*neuronIDPtr]->blue);
						else
							glColor3f(DEFAULT_NEURON_COLOUR_FULL);
						drawSolidSphere(0.1f, 21, 11);
						glPopMatrix();
						++neuronIDPtr;
						++xPosPtr;
						++yPosPtr;
						++zPosPtr;
	
						if(i % 1000 == 0){
							renderProgressBar->setProgress(i);
							spikeStrApp->processEvents();
							if(cancelRender)
								break;
						}
					}
				}
				else{
					//Set drawing colour to default
					glColor3f(DEFAULT_NEURON_COLOUR_FULL);
					for(unsigned int i=0; i<neuronGrpMap[*layerViewIter]->numberOfNeurons; ++i){
						glPushMatrix();//Store current state of matrix
						glTranslatef(*xPosPtr, *yPosPtr, *zPosPtr);//Translate to sphere position
						drawSolidSphere(0.1f, 21, 11);
						glPopMatrix();
						++xPosPtr;
						++yPosPtr;
						++zPosPtr;
	
						if(i % 1000 == 0){
							renderProgressBar->setProgress(i);
							spikeStrApp->processEvents();
							if(cancelRender)
								break;
						}
					}
				}
			}
		}

		//Finish off render bar information
		renderProgressBar->setProgress(renderProgressBar->totalSteps());
		((NetworkViewerProperties*)networkViewerProperties)->setRenderProgressLabel("Render complete");
		spikeStrApp->processEvents();
	}

	//DRAW NEURONS AS VERTICES
	else{
		vector<unsigned int>::iterator layerViewIter;
		
		//NEURONCONNECTIONMODE ON: HIGHLIGHT SINGLE NEURON
		if(neuronConnectionMode){
		
			//FILTERMODE ON: ONLY SHOW NEURONS WHOSE CONNECTIONS MATCH THE FILTER CRITERIA
			if(neuronFilterMode){
				for(layerViewIter = layerViewVector.begin(); layerViewIter != layerViewVector.end(); ++layerViewIter){
					neuronIDPtr = neuronGrpMap[*layerViewIter]->neuronIDArray;
					xPosPtr = neuronGrpMap[*layerViewIter]->xPosArray;
					yPosPtr = neuronGrpMap[*layerViewIter]->yPosArray;
					zPosPtr = neuronGrpMap[*layerViewIter]->zPosArray;
					glPointSize(5.0f);
					glBegin(GL_POINTS);
						for(unsigned int i=0; i<neuronGrpMap[*layerViewIter]->numberOfNeurons; ++i){
							if(*neuronIDPtr == firstSingleNeuronID)
								glColor3f(HIGHLIGHT_FIRST_NEURON_COLOUR_OUTLINE);
							else if(*neuronIDPtr == secondSingleNeuronID && showBetweenConnections)
								glColor3f(HIGHLIGHT_SECOND_NEURON_COLOUR_OUTLINE);
							else if(fromNeuronMap[*neuronIDPtr])//Paint neurons connecting from themselves to this neuron a differnt colour
								glColor3f(CONNECTED_NEURON_COLOUR);
							else if(toNeuronMap[*neuronIDPtr])
								glColor3f(CONNECTED_NEURON_COLOUR);
							else if(highlightMode && highlightMap.count(*neuronIDPtr))//Highlight mode
								glColor3f(highlightMap[*neuronIDPtr]->red, highlightMap[*neuronIDPtr]->green, highlightMap[*neuronIDPtr]->blue);
							else
								glColor3f(DEFAULT_NEURON_COLOUR_OUTLINE);
							glVertex3f(*xPosPtr, *yPosPtr, *zPosPtr);
							++neuronIDPtr;
							++xPosPtr;
							++yPosPtr;
							++zPosPtr;
						}
					glEnd();
				}
			}
			
			//FILTERMODE OFF: DRAW ALL NEURONS CONNECTED TO THE HIGHLIGHTED NEURON
			else{
				for(layerViewIter = layerViewVector.begin(); layerViewIter != layerViewVector.end(); ++layerViewIter){
					neuronIDPtr = neuronGrpMap[*layerViewIter]->neuronIDArray;
					xPosPtr = neuronGrpMap[*layerViewIter]->xPosArray;
					yPosPtr = neuronGrpMap[*layerViewIter]->yPosArray;
					zPosPtr = neuronGrpMap[*layerViewIter]->zPosArray;
					glPointSize(5.0f);
					glBegin(GL_POINTS);
						for(unsigned int i=0; i<neuronGrpMap[*layerViewIter]->numberOfNeurons; ++i){
							if(*neuronIDPtr == firstSingleNeuronID)
								glColor3f(HIGHLIGHT_FIRST_NEURON_COLOUR_OUTLINE);
							else if(*neuronIDPtr == secondSingleNeuronID && showBetweenConnections)
								glColor3f(HIGHLIGHT_SECOND_NEURON_COLOUR_OUTLINE);
							else if(highlightMode && highlightMap.count(*neuronIDPtr))//Highlight mode
								glColor3f(highlightMap[*neuronIDPtr]->red, highlightMap[*neuronIDPtr]->green, highlightMap[*neuronIDPtr]->blue);
							else
								glColor3f(DEFAULT_NEURON_COLOUR_OUTLINE);
							glVertex3f(*xPosPtr, *yPosPtr, *zPosPtr);
							++neuronIDPtr;
							++xPosPtr;
							++yPosPtr;
							++zPosPtr;
						}
					glEnd();
				}
			}
		}
		
		//NEURONCONNECTIONMODE OFF: DRAW ALL NEURONS THE SAME COLOUR
		else{
			//Work through the neuron groups listed in the view vector
			for(layerViewIter = layerViewVector.begin(); layerViewIter != layerViewVector.end(); ++layerViewIter){
				xPosPtr = neuronGrpMap[*layerViewIter]->xPosArray;
				yPosPtr = neuronGrpMap[*layerViewIter]->yPosArray;
				zPosPtr = neuronGrpMap[*layerViewIter]->zPosArray;
				glPointSize(5.0f);
				glBegin(GL_POINTS);
					if(highlightMode){
						neuronIDPtr = neuronGrpMap[*layerViewIter]->neuronIDArray;
						for(unsigned int i=0; i<neuronGrpMap[*layerViewIter]->numberOfNeurons; ++i){
							if(highlightMap.count(*neuronIDPtr))
								glColor3f(highlightMap[*neuronIDPtr]->red, highlightMap[*neuronIDPtr]->green, highlightMap[*neuronIDPtr]->blue);
							else
								glColor3f(DEFAULT_NEURON_COLOUR_OUTLINE);
							glVertex3f(*xPosPtr, *yPosPtr, *zPosPtr);
							++neuronIDPtr;
							++xPosPtr;
							++yPosPtr;
							++zPosPtr;
						}
					}
					else{
						//Set drawing Colour to default
						glColor3f(DEFAULT_NEURON_COLOUR_OUTLINE);
						for(unsigned int i=0; i<neuronGrpMap[*layerViewIter]->numberOfNeurons; ++i){
							glVertex3f(*xPosPtr, *yPosPtr, *zPosPtr);
							++xPosPtr;
							++yPosPtr;
							++zPosPtr;
						}
					}
				glEnd();
			}
		}
	}
	
	//Restore the original state of the matrix
    glPopMatrix();

	//End of display list
	if(!fullRender){
		glEndList();
	}
	#ifdef RENDER_DEBUG
		perfTimer.printTime();
	#endif//RENDER_DEBUG

	//Have now created the display list so set viewStateChanged to false
	viewStateChanged = false;

	//Painting has finished, can now process the next accelerator key
	paintingGL = false;

	//Record time at which render stops to filter keyboard events during render
	spikeStrApp->stopRender();

	//Check for OpenGL errors
	checkOpenGLErrors();

	#ifdef RENDER_DEBUG
		cout<<"RENDER STOPPED, CHECKED FOR ERRORS"<<endl;
		cout.flush();
	#endif//RENDER_DEBUG

	/* If during the render, start render or resize were called and 
		filtered out, need to re-render. */
	if(resizeSkipped){
		resizeGL(newTempScreenWidth, newTempScreenHeight);
		paintSkipped = true;
	}
	if(paintSkipped){
		paintGL();
	}
}


/*! Called when the window is resized. Recalculates the viewport and clipping volume. */
void NetworkViewer::resizeGL(int screenWidth, int screenHeight){
	if(paintingGL){
		resizeSkipped = true;
	
		//Store new screen width and height
		newTempScreenWidth = screenWidth;
		newTempScreenHeight = screenHeight;
		return;
	}

	//Reset resizeSkipped
	resizeSkipped = false;

    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if(screenHeight == 0)
        screenHeight = 1;

    glViewport(0, 0, screenWidth, screenHeight);

    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
    // Set the clipping volume
	GLfloat aspectRatio = (GLfloat)screenWidth / (GLfloat)screenHeight;	
	gluPerspective(perspective_angle, aspectRatio, perspective_near, perspective_far);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	
	//Check for errors
	checkOpenGLErrors();
}


//----------------------------------------------------------------------------------------------
//----------------------------------------- SLOTS ----------------------------------------------
//----------------------------------------------------------------------------------------------

/*! Changes the viewpoint depending on which keys are pressed. 
	Controls movement around the 3D environment. */
void NetworkViewer::acceleratorKeyPressed (int acceleratorID){
	/* Do not want to process accelerator key events triggered by spikeStrApp->processEvents()
		whilst the full render is working since this can break out of the display list loop
		and cause openGL errors.*/
	if(paintingGL)
		return;

	//Disable full render. Do not want full render whilst the camera is moving.
	renderTimer->stop();//Do not want an earlier timing event to switch full render back on
	if(fullRenderMode && fullRender)
		disableFullRender();
	
	//Get the key sequence
	int keySequence = keyboardAccelerator->key(acceleratorID);
	
	//Do different types of movement etc. depending on the key sequence
	switch(keySequence){
		//CTRL + arrow keys rotates scene around X and Z axis
                case (Qt::CTRL + Qt::Key_Up)://Rotates scene around X axis
			sceneRotateX += 2.5f;
		break;
                case (Qt::CTRL + Qt::Key_Down)://Rotates scene around X axis
			sceneRotateX -= 2.5f;
		break;
                case (Qt::CTRL + Qt::Key_Left)://Rotates scene around Z axis
			sceneRotateZ += 2.5f;
		break;
                case(Qt::CTRL + Qt::Key_Right)://Rotates scene around Z axis
			sceneRotateZ -= 2.5f;
		break;
		
		//CTRL + Equal and Minus keys moves forward and backwards
		//Moves forward from current position
		//Y axis is forward direction so add Y vector to position
                case(Qt::CTRL + Qt::Key_Equal):
			cameraMatrix[12] += cameraMatrix[4];
			cameraMatrix[13] += cameraMatrix[5];
			cameraMatrix[14] += cameraMatrix[6];
		break;
                case(Qt::CTRL + Qt::Key_Minus)://Moves backwards from current position
			cameraMatrix[12] -= cameraMatrix[4];
			cameraMatrix[13] -= cameraMatrix[5];
			cameraMatrix[14] -= cameraMatrix[6];
		break;
		
		//Resets the view to starting position
                case(Qt::CTRL + Qt::Key_R):
			resetView();
		break;

		//SHIFT + arrow keys rotates camera position around X and Z axes
                case (Qt::SHIFT + Qt::Key_Up):
			rotateXAxis(0.1f);
		break;
                case (Qt::SHIFT + Qt::Key_Down):
			rotateXAxis(-0.1f);
		break;
                case (Qt::SHIFT + Qt::Key_Left):
			rotateZAxis(0.1f);
		break;
                case(Qt::SHIFT + Qt::Key_Right):
			rotateZAxis(-0.1f);
		break;

		//ALT + arrow keys are used to change the single neuron whose connections are displayed. The arrows increase and decrease the X and Y values
		/* Layers are created by filling up each Y direction (length) for each X (width) position.
			The neuron ids are sequentially increased when the layer is created.
			So, to move horizontally just add the length to the neuronID up to the total length
			To move vertically, just add the width to the neuronID up to the total width
			Need to explicitly create an int copy of singleNeuronID. Otherwise comparison can
			become very large when it goes below zero
		*/
		//Select a neuron further along X positive
                case(Qt::ALT + Qt::Key_Right):
			if(neuronConnectionMode){
				if(firstSingleNeurGrpID == 0)//Check that we are in the right mode and a first neuron group is loaded
					break;
				if((firstSingleNeuronID + 1 - minFirstSingleNeuronID) % firstSingleNeuronGroupWidth != 0){//New value is not starting a new column
					++firstSingleNeuronID;
					((NetworkViewerProperties*)networkViewerProperties)->setFirstSingleNeuronNumber(firstSingleNeuronID);
					viewStateChanged = true;
				}
			}
		break;
		//Select a neuron further along X negative
                case(Qt::ALT + Qt::Key_Left):
			if(neuronConnectionMode){
				if(firstSingleNeurGrpID == 0)//Check that we are in the right mode and a first neuron group is loaded
					break;
				if((firstSingleNeuronID - minFirstSingleNeuronID) % firstSingleNeuronGroupWidth != 0){//Only move down if it is not at the bottom of a column
					firstSingleNeuronID--;
					((NetworkViewerProperties*)networkViewerProperties)->setFirstSingleNeuronNumber(firstSingleNeuronID);
					viewStateChanged = true;
				}
			}
		break;
		//Select a neuron further along Y positive
                case(Qt::ALT + Qt::Key_Up):
			if(neuronConnectionMode){
				if(firstSingleNeurGrpID == 0)//Check that we are in the right mode and a first neuron group is loaded
					break;
				if(firstSingleNeuronID + firstSingleNeuronGroupWidth <= maxFirstSingleNeuronID){
					firstSingleNeuronID += firstSingleNeuronGroupWidth;
					((NetworkViewerProperties*)networkViewerProperties)->setFirstSingleNeuronNumber(firstSingleNeuronID);
					viewStateChanged = true;
				}
			}
		break;
		//Select a neuron further along Y negative
                case(Qt::ALT + Qt::Key_Down):
			if(neuronConnectionMode){
				if(firstSingleNeurGrpID == 0)//Check that we are in the right mode and a first neuron group is loaded
					break;
				if(firstSingleNeuronID < firstSingleNeuronGroupWidth)//Integers are unsigned so a negative number will be very large
					break;
				if((firstSingleNeuronID - firstSingleNeuronGroupWidth) >= minFirstSingleNeuronID){
					firstSingleNeuronID -= firstSingleNeuronGroupWidth;
					((NetworkViewerProperties*)networkViewerProperties)->setFirstSingleNeuronNumber(firstSingleNeuronID);
					viewStateChanged = true;
				}
			}
		break;

		/* CTRL and ALT are used to control the position of the second neuron when selecting
			all the connections between two neurons. */
		//Select a neuron further along X positive
                case(Qt::CTRL + Qt::ALT + Qt::Key_Right):
			if(neuronConnectionMode){
				if(secondSingleNeurGrpID == 0)//Check that we are in the right mode and a second neuron group is loaded
					break;
				if((secondSingleNeuronID + 1 - minSecondSingleNeuronID)% secondSingleNeuronGroupWidth != 0){//New value is not starting a new column
					++secondSingleNeuronID;
					((NetworkViewerProperties*)networkViewerProperties)->setSecondSingleNeuronNumber(secondSingleNeuronID);
					viewStateChanged = true;
				}
			}
		break;
		//Select a neuron further along X negative
                case(Qt::CTRL + Qt::ALT + Qt::Key_Left):
			if(neuronConnectionMode){
				if(secondSingleNeurGrpID == 0)//Check that we are in the right mode and a second neuron group is loaded
					break;
				if((secondSingleNeuronID - minSecondSingleNeuronID) % secondSingleNeuronGroupWidth != 0){//Only move down if it is not at the bottom of a column
					secondSingleNeuronID--;
					((NetworkViewerProperties*)networkViewerProperties)->setSecondSingleNeuronNumber(secondSingleNeuronID);
					viewStateChanged = true;
				}
			}
		break;
		//Select a neuron further along Y positive
                case(Qt::CTRL + Qt::ALT + Qt::Key_Up):
			if(neuronConnectionMode){
				if(secondSingleNeurGrpID == 0)//Check that we are in the right mode and a second neuron group is loaded
					break;
				if(secondSingleNeuronID + secondSingleNeuronGroupWidth <= maxSecondSingleNeuronID){
					secondSingleNeuronID += secondSingleNeuronGroupWidth;
					((NetworkViewerProperties*)networkViewerProperties)->setSecondSingleNeuronNumber(secondSingleNeuronID);
					viewStateChanged = true;
				}
			}
		break;
		//Select a neuron further along Y negative
                case(Qt::CTRL + Qt::ALT + Qt::Key_Down):
			if(neuronConnectionMode){
				if(secondSingleNeurGrpID == 0)//Check that we are in the right mode and a second neuron group is loaded
					break;
				if(secondSingleNeuronID < secondSingleNeuronGroupWidth)//Integers are unsigned, so this condition will create a large positive number
					break;
				if((secondSingleNeuronID - secondSingleNeuronGroupWidth) >= minSecondSingleNeuronID){
					secondSingleNeuronID -= secondSingleNeuronGroupWidth;
					((NetworkViewerProperties*)networkViewerProperties)->setSecondSingleNeuronNumber(secondSingleNeuronID);
					viewStateChanged = true;
				}
			}
		break;

		//Arrow keys without CTRL or SHIFT translate viewing position horizontally and vertically
		//Move along Z and X axes defined in cameraMatrix
                case (Qt::Key_Up):
			cameraMatrix[12] += cameraMatrix[8];
			cameraMatrix[13] += cameraMatrix[9];
			cameraMatrix[14] += cameraMatrix[10];
		break;
                case (Qt::Key_Down):
			cameraMatrix[12] -= cameraMatrix[8];
			cameraMatrix[13] -= cameraMatrix[9];
			cameraMatrix[14] -= cameraMatrix[10];			
		break;
                case (Qt::Key_Left):
			cameraMatrix[12] -= cameraMatrix[0];
			cameraMatrix[13] -= cameraMatrix[1];
			cameraMatrix[14] -= cameraMatrix[2];;
		break;
                case(Qt::Key_Right):
			cameraMatrix[12] += cameraMatrix[0];
			cameraMatrix[13] += cameraMatrix[1];
			cameraMatrix[14] += cameraMatrix[2];
		break;
		
		//This accelerator should only have been set up with known key sequences
		//So something is wrong if the default is reached
		default: 
			cerr<<"NetworkViewer: KEY ACCELERATOR KEY SEQUENCE NOT RECOGNISED: "<<keySequence<<endl;
			QMessageBox::critical( 0, "Error", "Key accelerator sequence not recognized");
	}

	//Update the graphics display. Only reach this point if the key sequence has been recognised
	updateGL();
	
	//Set timer to start high quality rendering after appropriate delay
	if(fullRenderMode){
		renderTimer->start(renderDelay_ms, TRUE );
	}
}


/*! Switches on full render. Full render mode must be enabled first. 
	Generally triggered after renderDelay_ms. */
void NetworkViewer::setFullRender(){
	viewStateChanged = true;//Record the fact that the view state has changed
	if(fullRenderMode){
		fullRender = true;
		initialiseFullRender();
		updateGL();
	}
}


//----------------------------------------------------------------------------------------------
//------------------------------ PRIVATE UTILITY METHODS ---------------------------------------
//----------------------------------------------------------------------------------------------

/*! Checks for errors in OpenGL. */
void NetworkViewer::checkOpenGLErrors(){
	GLenum err = glGetError();
	while(err != GL_NO_ERROR){
		cerr<<"NetworkViewer OpenGL ERROR: "<<gluErrorString(err)<<endl;
		cerr.flush();
		err = glGetError();
	}
}


/*! Deletes all of the connection groups being displayed. */
void NetworkViewer::deleteAllConnectionGroups(){
	//Delete all data structures associated with each connection group
	for(map<unsigned int, ConnectionGroupHolder*>::iterator iter = connectionGrpMap.begin(); iter != connectionGrpMap.end(); ++iter){
		delete iter->second;
	}

	//Clear the references
	connectionGrpMap.clear();
	connectionViewVector.clear();
}


/*! Deletes all the neuron groups without deleteing associated connection groups. */
void NetworkViewer::deleteAllNeuronGroups(){
	//Delete neuron group holders
	for(map<unsigned int, NeuronGroupHolder*>::iterator iter = neuronGrpMap.begin(); iter != neuronGrpMap.end(); ++iter)
		delete iter->second;

	//Empty the maps and vectors
	neuronGrpMap.clear();
	layerViewVector.clear();

	//Sort out any variables linked to the neuron group
	firstSingleNeurGrpID = 0;
	minFirstSingleNeuronID = 0;
	maxFirstSingleNeuronID = 0;
	firstSingleNeuronGroupWidth = 0;
	secondSingleNeurGrpID = 0;
	maxSecondSingleNeuronID = 0;
	minSecondSingleNeuronID = 0;
	secondSingleNeuronGroupWidth = 0;
	showBetweenConnections = false;
}


/*! Switches off everything associated with full render mode. */
void NetworkViewer::disableFullRender(){
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_COLOR_MATERIAL);
	fullRender = false;
	viewStateChanged = true;//Want to rebuild the display list with vertices not spheres
}


/*! Draw X, Y and Z axes
	These are drawn so that they cover the clipping volume plus a bit of extra length. */
void NetworkViewer::drawAxes(void){
	//Set the drawing colour to red
	glColor3f(1.0f, 0.0f, 0.0f);
	
	//Store current line width
	glPushAttrib(GL_LINE_BIT);
	
	//Set wide line 
	glLineWidth(2.0f);
	
	//Draw the axes, extending extraLength beyond clipping volume
    GLfloat extraLength = 20.0f;
	
	//Draw the main axes
    glBegin(GL_LINES);
		//X Axis
		glVertex3f(defaultClippingVol.minX - extraLength, 0.0f, 0.0f);
        glVertex3f(defaultClippingVol.maxX + extraLength, 0.0f, 0.0f);
		//Y Axis
		glVertex3f(0.0f, defaultClippingVol.minY - extraLength, 0.0f);
        glVertex3f(0.0f, defaultClippingVol.maxY + extraLength, 0.0f);
		//Z Axis
		glVertex3f(0.0f, 0.0f, defaultClippingVol.minZ - extraLength);
        glVertex3f(0.0f, 0.0f, defaultClippingVol.maxZ + extraLength);
	glEnd();
	
	//Work along axes, marking every point with a point
	GLfloat scaleMarkSpacing = 5.0f;
	
	//Set colour and point size
	glColor3f(0.0f, 0.0f, 1.0f);
	glPointSize(3.0f);
	
	//Draw markings on X axis
	for(float i=defaultClippingVol.minX - extraLength; i< defaultClippingVol.maxX + extraLength; i += scaleMarkSpacing){
		glBegin(GL_POINTS);
			glVertex3f(i, 0.0f, 0.0f);
		glEnd();
	}
	
	//Draw markings on Y axis
	for(float i=defaultClippingVol.minY - extraLength; i< defaultClippingVol.maxY + extraLength; i += scaleMarkSpacing){
		glBegin(GL_POINTS);
			glVertex3f(0.0f, i, 0.0f);
		glEnd();
	}
	
	//Draw markings on Z axis
	for(float i=defaultClippingVol.minZ - extraLength; i< defaultClippingVol.maxZ + extraLength; i += scaleMarkSpacing){
		glBegin(GL_POINTS);
			glVertex3f(0.0f, 0.0f, i);
		glEnd();
	}
	
	//Reset line width to original value
	glPopAttrib();
}


/*! Used to represent neurons as a solid sphere
	Adapted from gltools
	FIXME A BETTER NEURON REPRESENTATION COULD BE CREATED - PERHAPS USING TEXTURE MAPPING. */
void NetworkViewer::drawSolidSphere(GLdouble radius, GLint slices, GLint stacks){
	GLUquadricObj *pObj = gluNewQuadric();
  	gluQuadricDrawStyle(pObj, GLU_FILL);
  	gluQuadricNormals(pObj, GLU_SMOOTH);
  /* NOTE If we ever changed/used the texture or orientation state
     of quadObj, we'd need to change it to the defaults here
     with gluQuadricTexture and/or gluQuadricOrientation. */
  	gluSphere(pObj, radius, slices, stacks);
}


/*! Adapted from gltools
	Fills the 4x4 rotation matrix to enable it to be used to rotate camera frame
	Note that angle is in radians NOT degrees. */
void NetworkViewer::fillRotationMatrix(float angle, float x, float y, float z){
    float vecLength, sinSave, cosSave, oneMinusCos;
    float xx, yy, zz, xy, yz, zx, xs, ys, zs;

    // If NULL vector passed in, this will blow up...
    if(x == 0.0f && y == 0.0f && z == 0.0f){
        cerr<<"NetworkViewer: NULL MATRIX PASSED TO fillRotationMatrix.";
		QMessageBox::critical( 0, "Error", "Null matrix passed to fillRotationMatrix.");
		return;
    }

    // Scale vector
    vecLength = (float)sqrt( x*x + y*y + z*z );

    // Rotation matrix is normalized
    x /= vecLength;
    y /= vecLength;
    z /= vecLength;

    sinSave = (float)sin(angle);
    cosSave = (float)cos(angle);
    oneMinusCos = 1.0f - cosSave;

    xx = x * x;
    yy = y * y;
    zz = z * z;
    xy = x * y;
    yz = y * z;
    zx = z * x;
    xs = x * sinSave;
    ys = y * sinSave;
    zs = z * sinSave;

    rotationMatrix[0] = (oneMinusCos * xx) + cosSave;
    rotationMatrix[4] = (oneMinusCos * xy) - zs;
    rotationMatrix[8] = (oneMinusCos * zx) + ys;
    rotationMatrix[12] = 0.0f;

    rotationMatrix[1] = (oneMinusCos * xy) + zs;
    rotationMatrix[5] = (oneMinusCos * yy) + cosSave;
    rotationMatrix[9] = (oneMinusCos * yz) - xs;
    rotationMatrix[13] = 0.0f;

    rotationMatrix[2] = (oneMinusCos * zx) - ys;
    rotationMatrix[6] = (oneMinusCos * yz) + xs;
    rotationMatrix[10] = (oneMinusCos * zz) + cosSave;
    rotationMatrix[14] = 0.0f;

    rotationMatrix[3] = 0.0f;
    rotationMatrix[7] = 0.0f;
    rotationMatrix[11] = 0.0f;
    rotationMatrix[15] = 1.0f;
}


/*! Converts strings to floats. */
float NetworkViewer::getFloat(string s){
        QString qStr(s.data());
	bool ok = true;
	float newFloat = qStr.toFloat(&ok);
	if(!ok){
                cerr<<"NetworkViewer: STRING TO FLOAT CONVERSION ERROR WITH STRING: \""<<qStr.toStdString()<<"\""<<endl;
		QMessageBox::critical( 0, "Conversion Error", "String to float conversion error.");
		exit(1);
	}
	return newFloat;
}


/*! Sets the camera parameters to their starting values. */
void NetworkViewer::initialiseCameraParameters(){
	//Scene rotate parameters are used to control the rotation of the scene
	sceneRotateX = 0.0f;
	sceneRotateZ = 0.0f;
	
	//Set up camera looking down positive Y direction with z axis pointing up and positive X to the right
	//X axis for camera frame of reference
	cameraMatrix[0] = 1.0f;
	cameraMatrix[1] = 0.0f;
	cameraMatrix[2] = 0.0f;
	cameraMatrix[3] = 0.0f;
	
	//Y axis for camera frame of reference
	cameraMatrix[4] = 0.0f;
	cameraMatrix[5] = 1.0f;
	cameraMatrix[6] = 0.0f;
	cameraMatrix[7] = 0.0f;
	
	//Z axis for camera frame of reference
	cameraMatrix[8] = 0.0f;
	cameraMatrix[9] = 0.0f;
	cameraMatrix[10] = 1.0f;
	cameraMatrix[11] = 0.0f;
	
	//Location for camera frame of reference
	cameraMatrix[12] = 0.0f;
	cameraMatrix[13] = -5.0f;
	cameraMatrix[14] = 0.0f;
}


/*! Sets up the graphics to do a detailed rendering of the network. */
//FIXME THIS COULD DO WITH SOME CHECKING AND REFINEMENT ALONG WITH THE REST OF THE FULL RENDER STUFF
void NetworkViewer::initialiseFullRender(){
	// Cull backs of polygons
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	
	// Setup light parameters
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, fNoLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, fLowLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, fBrightLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, fBrightLight);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	// Mostly use material tracking
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMateriali(GL_FRONT, GL_SHININESS, 128);
}


/*! Loads up all of the connection group data so that it can be displayed quickly. */
void NetworkViewer::loadAllConnectionGroups(QSplashScreen* splashScreen){
	cout<<"Network viewer: loading connection groups";

	try{
		//First get a list of ConnectionGrpIDs and store a data structure for each group
		Query query = dbInterface->getQuery();
		query.reset();
		query<<"SELECT ConnGrpID, ConnType FROM ConnectionGroups";
                StoreQueryResult connGrpRes = query.store();
                for(StoreQueryResult::iterator connGrpIter = connGrpRes.begin(); connGrpIter != connGrpRes.end(); ++connGrpIter){
			Row connGrpRow(*connGrpIter);
			unsigned int connGrpID = Utilities::getUInt((std::string)connGrpRow["ConnGrpID"]);
			if(splashScreen !=0)
				splashScreen->message("Loading connection group: " + QString::number(connGrpID), Qt::AlignBottom, QColor(255, 255, 255));
			loadConnectionGroup(connGrpID, false);
			cout<<".";
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"NetworkViewer: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query loading connection groups: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Load Connection Group Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"NetworkViewer: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading connection groups: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Load Connection Group Error", errorString);
	}
	catch(std::exception& er){// Catch-all for std exceptions
		cerr<<"NetworkViewer: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading connection groups: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Load Connection Group Error", errorString);
	}

	cout<<" complete."<<endl;
}


/*! Loads up all the neuron groups into a data structure to enable rapid display. */
void NetworkViewer::loadAllNeuronGroups(QSplashScreen* splashScreen){
	cout<<"Network viewer: loading layers";

	try{
		//First get a list of NeuronGrpIDs and store a data structure for each group
		Query query = dbInterface->getQuery();
		query.reset();
		query<<"SELECT NeuronGrpID, NeuronType FROM NeuronGroups";
                StoreQueryResult neuronGrpRes = query.store();
                for(StoreQueryResult::iterator neuronGrpIter = neuronGrpRes.begin(); neuronGrpIter != neuronGrpRes.end(); ++neuronGrpIter){
			Row neuronGrpRow(*neuronGrpIter);
			unsigned int neuronGrpID = Utilities::getUInt((std::string)neuronGrpRow["NeuronGrpID"]);
			if(splashScreen != 0)
				splashScreen->message("Loading neuron group: " + QString::number(neuronGrpID), Qt::AlignBottom, QColor(255, 255, 255));
			loadNeuronGroup(neuronGrpID, false);
			cout<<".";
		}
		
		//Load up the clipping volume from the database. This volume includes all of the neural networks
		loadDefaultClippingVolume();
		viewClippingVolume_Horizontal(defaultClippingVol);
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"NetworkViewer: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query loading neuron groups: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Load Neuron Group Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"NetworkViewer: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading neuron groups: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Load Neuron Group Error", errorString);
	}
	catch(std::exception& er){// Catch-all for std exceptions
		cerr<<"NetworkViewer: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading neuron groups: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Load Neuron Group Error", errorString);
	}
	
	cout<<" complete."<<endl;//Loading of neuron data is finished
}


/*! Adapated from gltools
	Rotates a vector using a 4x4 matrix. Translation column is ignored. */
void NetworkViewer::rotateVector(GLfloat x, GLfloat y, GLfloat z, GLfloat result[]){
    result[0] = rotationMatrix[0] * x + rotationMatrix[4] * y + rotationMatrix[8] *  z;
    result[1] = rotationMatrix[1] * x + rotationMatrix[5] * y + rotationMatrix[9] *  z;
    result[2] = rotationMatrix[2] * x + rotationMatrix[6] * y + rotationMatrix[10] * z;
}


/*! Rotates camera around its own X axis. */
void NetworkViewer::rotateXAxis(float angle){
	//Rotating around X axis so pass vector defining this axis for the camera to fillRotationMatrix
	fillRotationMatrix(angle, cameraMatrix[0], cameraMatrix[1], cameraMatrix[2]);
	
	//Create temporary array to hold result
	GLfloat resultArray[3];
	
	//First rotate the Y axis around X axis
	rotateVector(cameraMatrix[4], cameraMatrix[5], cameraMatrix[6], resultArray);
	
	//Copy result array into cameraMatrix. Could probably simplify this
	cameraMatrix[4] = resultArray[0];
	cameraMatrix[5] = resultArray[1];
	cameraMatrix[6] = resultArray[2];
	
	//Now rotate the Z axis around X axis
	rotateVector(cameraMatrix[8], cameraMatrix[9], cameraMatrix[10], resultArray);
	
	//Copy result array into cameraMatrix. Could probably simplify this
	cameraMatrix[8] = resultArray[0];
	cameraMatrix[9] = resultArray[1];
	cameraMatrix[10] = resultArray[2];
}


/*! Rotates camera around its own Z axis. */
void NetworkViewer::rotateZAxis(float angle){
	//Rotating around Z axis, so pass vector defining this axis for the camera to fillRotationMatrix
	fillRotationMatrix(angle, cameraMatrix[8], cameraMatrix[9], cameraMatrix[10]);
	
	//Create temporary array to hold result
	GLfloat resultArray[3];
	
	//First rotate the X axis around Z axis
	rotateVector(cameraMatrix[0], cameraMatrix[1], cameraMatrix[2], resultArray);
	
	//Copy result array into cameraMatrix. Could probably simplify this
	cameraMatrix[0] = resultArray[0];
	cameraMatrix[1] = resultArray[1];
	cameraMatrix[2] = resultArray[2];
	
	//Now rotate the Y axis around Z axis
	rotateVector(cameraMatrix[4], cameraMatrix[5], cameraMatrix[6], resultArray);
	
	//Copy result array into cameraMatrix. Could probably simplify this
	cameraMatrix[4] = resultArray[0];
	cameraMatrix[5] = resultArray[1];
	cameraMatrix[6] = resultArray[2];

}


/*! Sets the view so that the perspective fits the clipping volume seen horizontally. */
void NetworkViewer::viewClippingVolume_Horizontal(ClippingVolume clipVolume){
	//First set camera parameters to their starting values
	initialiseCameraParameters();

	//Now adjust these parameters to view the whole of the clipping volume
	//X location should be half way along the clipping volume
	cameraMatrix[12] = clipVolume.minX + (clipVolume.maxX - clipVolume.minX)/2.0f;
	
	
	//Camera is looking down the Y axis. So need to move back far enough to see all of clipping volume within perspective
	//First find whether z or x direction is longest
	GLfloat backwardsDistance;
	if((clipVolume.maxX - clipVolume.minX) > (clipVolume.maxZ - clipVolume.minZ)){//X direction is longest
		backwardsDistance = ((clipVolume.maxX - clipVolume.minX)/2.0f)/tan(gltDegToRad(perspective_angle)/2.0f);
	}
	else{//Z direction is longest
		backwardsDistance = ((clipVolume.maxZ - clipVolume.minZ)/2.0f)/tan(gltDegToRad(perspective_angle)/2.0f);	
	}
	//Now move camera back so that it can see everything in both directions
	//In this case this moves camera back along the negative y direction
	cameraMatrix[13] = -1 * backwardsDistance;
	
	//Z location should be half way up clipping volume
	cameraMatrix[14] = clipVolume.minZ + (clipVolume.maxZ - clipVolume.minZ)/2.0f;
	
	//Forward vector does not need to be calculated because camera is initilised looking along positive Y, which is correct for horizontal view	
}


/*! Sets the view so that the perspective fits the clipping volume seen horizontally. */
void NetworkViewer::viewClippingVolume_Vertical(ClippingVolume clipVolume){
	//First set camera parameters to their starting values
	initialiseCameraParameters();
	
	//X location should be half way along the clipping volume
	cameraMatrix[12] = clipVolume.minX + (clipVolume.maxX - clipVolume.minX)/2.0f;
	
	//Camera is looking down the Z axis. So need to move back far enough to see all of clipping volume within perspective
	//First find whether y or x direction is longest
	GLfloat backwardsDistance;
	if((clipVolume.maxX - clipVolume.minX) > (clipVolume.maxY - clipVolume.minY)){//X direction is longest
		backwardsDistance = ((clipVolume.maxX - clipVolume.minX)/2.0f)/tan(gltDegToRad(perspective_angle)/2.0f);
	}
	else{//Y direction is longest
		backwardsDistance = ((clipVolume.maxY - clipVolume.minY)/2.0f)/tan(gltDegToRad(perspective_angle)/2.0f);	
	}

	//Now move camera back so that it can see everything in both directions
	//To look above need to move up the positive direction of the Z axis 
	cameraMatrix[14] = 1.1f*(backwardsDistance + clipVolume.maxZ);
	
	//Y location should be half way along clipping volume
	cameraMatrix[13] = clipVolume.minY + (clipVolume.maxY - clipVolume.minY)/2.0f;
	
	//X axis stays the same	
	//Change camera Y axis so that it is pointing down negative Z
	cameraMatrix[4] = 0.0f;
	cameraMatrix[5] = 0.0f;
	cameraMatrix[6] = -1.0f;
	
	//Change camera Z axis so that it points along positive Y
	cameraMatrix[8] = 0.0f;
	cameraMatrix[9] = 1.0f;
	cameraMatrix[10] = 0.0f;
}



