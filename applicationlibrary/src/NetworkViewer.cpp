//SpikeStream includes
#include "EventRouter.h"
#include "Globals.h"
#include "GlobalVariables.h"
#include "NetworkViewer.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QMouseEvent>

//Other includes
#include <iostream>
using namespace std;

//Outputs debugging information when enabled
//#define DEBUG

//Defines for OpenGL
/*! Constant for PI. */
#define GL_PI 3.1415f

/* gltools macro to convert degrees to radians and vice versa. */
#define GLT_PI_DIV_180 0.017453292519943296
#define GLT_INV_PI_DIV_180 57.2957795130823229
#define gltDegToRad(x)	((x)*GLT_PI_DIV_180)
#define gltRadToDeg(x)	((x)*GLT_INV_PI_DIV_180)

/* Light and material Data. */
GLfloat fNoLight[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat fLowLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat fDiffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat fBrightLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat specref[] = { 1.0f, 1.0f, 1.0f, 1.0f };


/*! Constructor */
NetworkViewer::NetworkViewer(QWidget* parent) : QGLWidget(parent) {
	//Connect refresh to changes in the display of network or archive
	connect(Globals::getEventRouter(), SIGNAL(networkDisplayChangedSignal()), this, SLOT(refresh()), Qt::QueuedConnection);
	connect(Globals::getEventRouter(), SIGNAL(neuronGroupDisplayChangedSignal()), this, SLOT(refreshNeurons()), Qt::QueuedConnection);
	connect(Globals::getEventRouter(), SIGNAL(weightsChangedSignal()), this, SLOT(refreshConnections()), Qt::QueuedConnection);

	//Connect reset to changes in the network
	connect(Globals::getEventRouter(), SIGNAL(networkChangedSignal()), this, SLOT(reset()), Qt::QueuedConnection);
	connect(Globals::getEventRouter(), SIGNAL(reloadSignal()), this, SLOT(reset()), Qt::QueuedConnection);

	//Initialize variables
	paintGLSkipped = false;
	resizeGLSkipped = false;
	useAxesDisplayList = false;
	useNeuronsDisplayList = false;
	useConnectionsDisplayList = false;
	perspective_angle = 46.0f;
	perspective_near = 1.0f;
	perspective_far = 100000.0f;//Set this to a large number so everything will be visible

	//Connect up signals to navigate around 3D environment
	connect(Globals::getEventRouter(), SIGNAL(moveUpSignal()), this, SLOT(moveUp()), Qt::QueuedConnection);
	connect(Globals::getEventRouter(), SIGNAL(moveDownSignal()), this, SLOT(moveDown()), Qt::QueuedConnection);
	connect(Globals::getEventRouter(), SIGNAL(moveLeftSignal()), this, SLOT(moveLeft()), Qt::QueuedConnection);
	connect(Globals::getEventRouter(), SIGNAL(moveRightSignal()), this, SLOT(moveRight()), Qt::QueuedConnection);
	connect(Globals::getEventRouter(), SIGNAL(moveBackwardSignal()), this, SLOT(moveBackward()), Qt::QueuedConnection);
	connect(Globals::getEventRouter(), SIGNAL(moveForwardSignal()), this, SLOT(moveForward()), Qt::QueuedConnection);
	connect(Globals::getEventRouter(), SIGNAL(resetViewSignal()), this, SLOT(resetView()), Qt::QueuedConnection);
	connect(Globals::getEventRouter(), SIGNAL(rotateUpSignal()), this, SLOT(rotateUp()), Qt::QueuedConnection);
	connect(Globals::getEventRouter(), SIGNAL(rotateDownSignal()), this, SLOT(rotateDown()), Qt::QueuedConnection);
	connect(Globals::getEventRouter(), SIGNAL(rotateLeftSignal()), this, SLOT(rotateLeft()), Qt::QueuedConnection);
	connect(Globals::getEventRouter(), SIGNAL(rotateRightSignal()), this, SLOT(rotateRight()), Qt::QueuedConnection);

	//Set size of widget
	setMinimumSize(200, 60);
	setBaseSize(700, 60);

	//Set display to initial state
	reset();
}


/*! Destructor */
NetworkViewer::~NetworkViewer(){
}


/*----------------------------------------------------------*/
/*-----                PROTECTED METHODS               -----*/
/*----------------------------------------------------------*/

/*! Inherited from QGLWidget */
void NetworkViewer::initializeGL(){
	//White background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	//Create IDs for display lists
	axesDisplayList = glGenLists(1);
	sphereDisplayList =  glGenLists(1);
	neuronsDisplayList =  glGenLists(1);
	connectionsDisplayList =  glGenLists(1);

	//Create objects
	gluSphereObj = gluNewQuadric();
	gluConeObj = gluNewQuadric();
	gluQuadricDrawStyle(gluSphereObj, GLU_FILL);
	gluQuadricNormals(gluSphereObj, GLU_SMOOTH);

	//Build sphere display list
	glNewList(sphereDisplayList, GL_COMPILE);
		gluSphere(
			gluSphereObj,
			Globals::getNetworkDisplay()->getSphereRadius(),
			Globals::getNetworkDisplay()->getSphereQuality()*2,
			Globals::getNetworkDisplay()->getSphereQuality()
		);
	glEndList();
}


/*! Inherited from QGLWidget */
void NetworkViewer::paintGL(){
	//If we are already painting, skip further calls to this method triggered by accelerator keys
	if(Globals::isRendering()){
		paintGLSkipped = true;
		return;
	}

	paintGLSkipped = false;
	cancelRender = false;

	//Record that rendering is in progress to filter out accelerator keys */
	Globals::setRendering(true);

	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Store current matrix state
	glPushMatrix();

	//Set zoom level
	setZoomLevel();

	//Position the camera appropriately
	positionCamera();

	//Lock network display whilst rendering is taking place
	//FIXME: MIGHT WANT TO LOCK NETWORK AS WELL
	Globals::getNetworkDisplay()->lockMutex();

	//Draw the connections before the neurons in full render mode
	if(Globals::getNetworkDisplay()->isFullRenderMode())
		this->initialiseFullRender();
	else
		this->disableFullRender();

	//Draw axes, connections and neurons
	drawAxes();
	drawConnections();
	drawNeurons();

	//Restore the original state of the matrix
	glPopMatrix();

	//Unlock network display
	Globals::getNetworkDisplay()->unlockMutex();

	//Check for OpenGL errors
	checkOpenGLErrors();

	//Render has stopped
	Globals::setRendering(false);

	/* If during the render, start render or resize were called and
		filtered out, need to re-render. */
	if(resizeGLSkipped){
		resizeGL(newTempScreenWidth, newTempScreenHeight);
		paintGLSkipped = true;
	}
	if(paintGLSkipped){
		paintGL();
	}
}


/*! Inherited from QGLWidget */
void NetworkViewer::resizeGL(int screenWidth, int screenHeight){
	if(Globals::isRendering()){
		resizeGLSkipped = true;

		//Store new screen width and height
		newTempScreenWidth = screenWidth;
		newTempScreenHeight = screenHeight;
		return;
	}

	//Reset resizeSkipped
	resizeGLSkipped = false;

	// Prevent a divide by zero, when window is too short (you cant make a window of zero width).
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


/*! Adapted from: http://www.lighthouse3d.com/opengl/picking/index.php?openglway */
void NetworkViewer::mouseDoubleClickEvent (QMouseEvent* event){
	//Window coordinates in OpenGL start in the bottom left, so have to subtract the window height
	int mouseYPos = height() - event->y();
	int mouseXPos = event->x();

	//Determine if control button is down
	bool ctrlBtnDown = false;
	if(event->modifiers() & Qt::ControlModifier){
		ctrlBtnDown = true;
	}

	//Create the select buffer
	int SELECT_BUFFER_SIZE = 512;
	GLuint selectBuffer[SELECT_BUFFER_SIZE];
	glSelectBuffer(SELECT_BUFFER_SIZE,selectBuffer);

	//Get the current viewport
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);

	//Start picking
	glRenderMode(GL_SELECT);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPickMatrix(mouseXPos, mouseYPos, 15, 15, viewport);

	float ratio = (float)viewport[2] / (float)viewport[3];
	gluPerspective(45, ratio, 0.1, 1000);
	glMatrixMode(GL_MODELVIEW);

	//Render neurons with names
	//Store current matrix state
	glPushMatrix();

	//Set zoom level
	setZoomLevel();

	//Position the camera appropriately
	positionCamera();

	//Draw the neurons
	drawNeurons();

	//Restore the matrix state
	glPopMatrix();

	// returning to normal rendering mode
	int hitCount = glRenderMode(GL_RENDER);

	if(hitCount != 0){//Neuron selected
		unsigned int selectedNeuronID = getSelectedNeuron(selectBuffer, hitCount, SELECT_BUFFER_SIZE);
		Globals::getNetworkDisplay()->setSelectedNeuronID(selectedNeuronID, ctrlBtnDown);
	}
	else{
		Globals::getNetworkDisplay()->setSelectedNeuronID(0);
	}

	//Restore render mode and the original projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glFlush();
}


/*----------------------------------------------------------*/
/*-----                PRIVATE SLOTS                   -----*/
/*----------------------------------------------------------*/

/*! Moves the camera up along the Z axis. */
void NetworkViewer::moveUp(){
	cameraMatrix[12] += cameraMatrix[8];
	cameraMatrix[13] += cameraMatrix[9];
	cameraMatrix[14] += cameraMatrix[10];
	updateGL();
}

/*! Moves the camera down along the Z axis. */
void NetworkViewer::moveDown(){
	cameraMatrix[12] -= cameraMatrix[8];
	cameraMatrix[13] -= cameraMatrix[9];
	cameraMatrix[14] -= cameraMatrix[10];
	updateGL();
}


/*! Moves the camera negatively along the X axis. */
void NetworkViewer::moveLeft(){
	cameraMatrix[12] -= cameraMatrix[0];
	cameraMatrix[13] -= cameraMatrix[1];
	cameraMatrix[14] -= cameraMatrix[2];
	updateGL();
}


/*! Moves the camera positively along the X axis. */
void NetworkViewer::moveRight(){
	cameraMatrix[12] += cameraMatrix[0];
	cameraMatrix[13] += cameraMatrix[1];
	cameraMatrix[14] += cameraMatrix[2];
	updateGL();
}


/*! Moves the camera positively along the Y axis */
void NetworkViewer::moveForward(){
	cameraMatrix[12] += cameraMatrix[4];
	cameraMatrix[13] += cameraMatrix[5];
	cameraMatrix[14] += cameraMatrix[6];
	updateGL();
}


/*! Moves the camera negatively along the Y axis */
void NetworkViewer::moveBackward(){
	cameraMatrix[12] -= cameraMatrix[4];
	cameraMatrix[13] -= cameraMatrix[5];
	cameraMatrix[14] -= cameraMatrix[6];
	updateGL();
}


/*! Resets view to default setting in which all network is visible. */
void NetworkViewer::resetView(){
	viewClippingVolume_Horizontal(defaultClippingVol);
	updateGL();
}


/*! Rotates camera clockwise around X axis looking towards the origin. */
void NetworkViewer::rotateUp(){
	sceneRotateX += 2.5f;
	updateGL();
}


/*! Rotates camera anticlockwise around X axis looking towards the origin. */
void NetworkViewer::rotateDown(){
	sceneRotateX -= 2.5f;
	updateGL();
}


/*! Rotates camera anticlockwise around Z axis looking towards the origin. */
void NetworkViewer::rotateLeft(){
	sceneRotateZ += 2.5f;
	updateGL();
}


/*! Rotates camera clockwise around Z axis looking towards the origin. */
void NetworkViewer::rotateRight(){
	sceneRotateZ -= 2.5f;
	updateGL();
}


/*! Re-draws everything in the network */
void NetworkViewer::refresh(){
	useNeuronsDisplayList = false;
	useConnectionsDisplayList = false;
	updateGL();
}


/*! Redraws the neurons */
void NetworkViewer::refreshNeurons(){
	useNeuronsDisplayList = false;
	updateGL();
}


/*! Redraws the weights */
void NetworkViewer::refreshConnections(){
	useConnectionsDisplayList = false;
	updateGL();
}


/*! Resets the view completely typically after the network has changed */
void NetworkViewer::reset(){
	//Load up the volume enclosing the entire network
	loadDefaultClippingVolume();

	//Look at this network horizontally
	viewClippingVolume_Horizontal(defaultClippingVol);

	//Network has changed so need to re-render the display list
	useNeuronsDisplayList = false;
	useConnectionsDisplayList = false;

	//Re-draw
	updateGL();
}



/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Checks for errors in OpenGL. */
void NetworkViewer::checkOpenGLErrors(){
	//Check to see if error has occurred
	GLenum err = glGetError();
	if(err != GL_NO_ERROR){
		//May be more than one error message, so add them all to the error string if there is an error
		QString errMsg = "OpenGL ERROR(S): ";
		while(err != GL_NO_ERROR){
			errMsg += QString("'") + (const char *)gluErrorString(err) + "'; ";
			err = glGetError();
		}

		//Flag error to user
		qCritical()<<errMsg;
	}
}


/*! Draw X, Y and Z axes
	These are drawn so that they cover the clipping volume plus a bit of extra length. */
void NetworkViewer::drawAxes(void){
	//Do nothing if drawAxes is set to false
	if(!Globals::getNetworkDisplay()->isDrawAxes())
		return;

	//Use existing axes display list
	if(useAxesDisplayList){
		glCallList(axesDisplayList);
	}
	//Create new display list with axes
	else{
		glNewList(axesDisplayList, GL_COMPILE_AND_EXECUTE);

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
		glVertex3f(defaultClippingVol.x1 - extraLength, 0.0f, 0.0f);
		glVertex3f(defaultClippingVol.x2 + extraLength, 0.0f, 0.0f);
		//Y Axis
		glVertex3f(0.0f, defaultClippingVol.y1 - extraLength, 0.0f);
		glVertex3f(0.0f, defaultClippingVol.y2 + extraLength, 0.0f);
		//Z Axis
		glVertex3f(0.0f, 0.0f, defaultClippingVol.z1 - extraLength);
		glVertex3f(0.0f, 0.0f, defaultClippingVol.z2 + extraLength);
		glEnd();

		//Work along axes, marking every point with a point
		GLfloat scaleMarkSpacing = 5.0f;

		//Set colour and point size
		glColor3f(0.0f, 0.0f, 1.0f);
		glPointSize(3.0f);

		//Draw markings on X axis
		for(float i=defaultClippingVol.x1 - extraLength; i < defaultClippingVol.x2 + extraLength; i += scaleMarkSpacing){
			glBegin(GL_POINTS);
				glVertex3f(i, 0.0f, 0.0f);
			glEnd();
		}

		//Draw markings on Y axis
		for(float i=defaultClippingVol.y1 - extraLength; i < defaultClippingVol.y2 + extraLength; i += scaleMarkSpacing){
			glBegin(GL_POINTS);
				glVertex3f(0.0f, i, 0.0f);
			glEnd();
		}

		//Draw markings on Z axis
		for(float i=defaultClippingVol.z1 - extraLength; i < defaultClippingVol.z2 + extraLength; i += scaleMarkSpacing){
			glBegin(GL_POINTS);
				glVertex3f(0.0f, 0.0f, i);
			glEnd();
		}

		//Reset line width to original value
		glPopAttrib();

		//Finish recording display list with axes
		glEndList();
		useAxesDisplayList = true;
	}
}


/*! Draws the connections */
void NetworkViewer::drawConnections(){
	//Nothing to do if no network is loaded
	if(!Globals::networkLoaded())
		return;

	//Use existing display list
	if(useConnectionsDisplayList){
		glCallList(connectionsDisplayList);
	}
	//Create new display list with connections
	else{
		glNewList(connectionsDisplayList, GL_COMPILE_AND_EXECUTE);

		//Local variables declared once here to save processing. These point to the points stored in the neurons
		Point3D* fromNeuronPoint;
		Point3D* toNeuronPoint;

		//Get pointer to network that is to be drawn and its associated display
		Network* network = Globals::getNetwork();
		NetworkDisplay* netDisplay = Globals::getNetworkDisplay();
		bool fullRender = netDisplay->isFullRenderMode();

		//Get information about rendering connections
		unsigned weightRenderMode = netDisplay->getWeightRenderMode();
		float weight = 0.0f;//Declare here to save declaring it all the time.
		weightRadiusFactor = netDisplay->getWeightRadiusFactor();
		minimumConnectionRadius = netDisplay->getMinimumConnectionRadius();
		connectionQuality = netDisplay->getConnectionQuality();

		//Default neuron colour
		RGBColor positiveConnectionColor = *netDisplay->getPositiveConnectionColor();
		RGBColor negativeConnectionColor = *netDisplay->getNegativeConnectionColor();

		//Sort out the connection mode
		unsigned int singleNeuronID=0, toNeuronID=0;
		unsigned int connectionMode = netDisplay->getConnectionMode();
		QList<Connection*>& visConList = netDisplay->getVisibleConnectionsList();
		if(connectionMode & CONNECTION_MODE_ENABLED){
			connectedNeuronMap.clear();
			singleNeuronID = netDisplay->getSingleNeuronID();
			if(connectionMode & SHOW_BETWEEN_CONNECTIONS)
				toNeuronID = netDisplay->getToNeuronID();

			//Rebuild list of visible connections
			visConList.clear();
		}

		//Work through the connection groups listed in the network display
		bool drawConnection;
		int thinningThreshold = Globals::getNetworkDisplay()->getConnectionThinningThreshold();
		unsigned ignoreConCnt = 0;
		QList<unsigned int> conGrpIDs = Globals::getNetworkDisplay()->getVisibleConnectionGroupIDs();
		for(QList<unsigned int>::iterator conGrpIter = conGrpIDs.begin(); conGrpIter != conGrpIDs.end(); ++conGrpIter){

			//Pointer to connection group
			ConnectionGroup* conGrp = network->getConnectionGroup(*conGrpIter);
			int numCons = conGrp->size();
			int drawingThreshold = 0;
			if(numCons > thinningThreshold){
				drawingThreshold = Util::rInt(RAND_MAX * ( (double)thinningThreshold/(double)numCons));
			}

			//Get neuron groups for extracting the position information
			NeuronGroup* fromNeuronGroup = network->getNeuronGroup(conGrp->getFromNeuronGroupID());
			NeuronGroup* toNeuronGroup = network->getNeuronGroup(conGrp->getToNeuronGroupID());

			//Draw all the connections in the group
			ConnectionIterator endConGrp = conGrp->end();
			for(ConnectionIterator conIter = conGrp->begin(); conIter != endConGrp; ++ conIter){
				//Get the weight
				weight = conIter->getWeight();
				if( (weightRenderMode & WEIGHT_RENDER_ENABLED) && (weightRenderMode & RENDER_TEMP_WEIGHTS) )
					weight = conIter->getTempWeight();

				//Decide if connection should be drawn, depending on the connection mode and neuron id
				drawConnection = true;
				if(connectionMode & CONNECTION_MODE_ENABLED){
					//Single neuron mode
					if( !(connectionMode & SHOW_BETWEEN_CONNECTIONS) ){
						//Show only connections from a single neuron
						if(connectionMode & SHOW_FROM_CONNECTIONS){
							if( conIter->getFromNeuronID() != singleNeuronID){
								drawConnection = false;
							}
							else{
								connectedNeuronMap[conIter->getToNeuronID()] = weight;
							}
						}
						//Show only connections to a single neuron
						else if(connectionMode & SHOW_TO_CONNECTIONS){
							if( conIter->getToNeuronID() != singleNeuronID){
								drawConnection = false;
							}
							else{
								connectedNeuronMap[conIter->getFromNeuronID()] = weight;//Positive connection
							}
						}
						//Show from and to connections to a single neuron
						else {
							if( (conIter->getFromNeuronID() != singleNeuronID) && (conIter->getToNeuronID() != singleNeuronID) ){
								drawConnection = false;
							}
							else {//Highlight connected neurons
								if( conIter->getFromNeuronID() == singleNeuronID){
									connectedNeuronMap[conIter->getToNeuronID()] = weight;//Positive connection
								}
								else if( conIter->getToNeuronID() == singleNeuronID){
									connectedNeuronMap[conIter->getFromNeuronID()] = weight;//Positive connection
								}
							}
						}
					}
					//Between neuron mode
					else{
						//Only show connections from first neuron to second
						if( conIter->getFromNeuronID() != singleNeuronID || conIter->getToNeuronID() != toNeuronID)
							drawConnection = false;
					}

					//Decide whether to draw connection based on its weight
					if( weight < 0 && (connectionMode & SHOW_POSITIVE_CONNECTIONS) )
						drawConnection = false;
					if( weight >= 0 && (connectionMode & SHOW_NEGATIVE_CONNECTIONS))
						drawConnection = false;

					//Add connection to list of visible connections
					if(drawConnection)
						visConList.append(&(*conIter));

				}
				//Draw all connections, potentially thinned
				else if(drawingThreshold && (rand() > drawingThreshold)){
					drawConnection = false;
					++ignoreConCnt;
				}

				//Draw the connection
				if(drawConnection){
					//Get the position of the from and to neurons
					fromNeuronPoint = &fromNeuronGroup->getNeuronLocation(conIter->getFromNeuronID());
					toNeuronPoint = &toNeuronGroup->getNeuronLocation(conIter->getToNeuronID());

					//Set the colour
					if(weight > 0)
						glColor3f(positiveConnectionColor.red, positiveConnectionColor.green, positiveConnectionColor.blue);
					else if(weight < 0)
						glColor3f(negativeConnectionColor.red, negativeConnectionColor.green, negativeConnectionColor.blue);
					else
						glColor3f(0.0f, 0.0f, 0.0f);

					//Draw the connection
					if(fullRender && (weightRenderMode & WEIGHT_RENDER_ENABLED) ){
						drawWeightedConnection(
								fromNeuronPoint->getXPos(),
								fromNeuronPoint->getYPos(),
								fromNeuronPoint->getZPos(),
								toNeuronPoint->getXPos(),
								toNeuronPoint->getYPos(),
								toNeuronPoint->getZPos(),
								weight
						);
					}
					else{
						//Start drawing lines
						glBegin(GL_LINES);
							glVertex3f(fromNeuronPoint->getXPos(), fromNeuronPoint->getYPos(), fromNeuronPoint->getZPos());
							glVertex3f(toNeuronPoint->getXPos(), toNeuronPoint->getYPos(), toNeuronPoint->getZPos());
						glEnd();//End of line drawing
					}

				}
			}
		}

		//Inform other classes that the list of visible connections has changed.
		if(connectionMode & CONNECTION_MODE_ENABLED){
			Globals::getEventRouter()->visibleConnectionsChangedSlot();
		}

		#ifdef DEBUG
			qDebug()<<"Number of ignored connections = "<<ignoreConCnt;
		#endif//DEBUG

		//Finished creating connection display list
		glEndList();
		useConnectionsDisplayList = true;
	}
}


/*! Draws the visible neurons in the network */
void NetworkViewer::drawNeurons(){
	//Nothing to do if no network is loaded
	if(!Globals::networkLoaded())
		return;

	//Use existing display list
	if(useNeuronsDisplayList){
		glCallList(neuronsDisplayList);
	}
	//Create new display list with connections
	else{
		glNewList(neuronsDisplayList, GL_COMPILE_AND_EXECUTE);

		//Local variables
		RGBColor *tmpColor, tmpColor2;
		unsigned sphereQuality;
		float sphereRadius, weight;

		//Get pointer to network that is to be drawn and its associated display
		Network* network = Globals::getNetwork();
		NetworkDisplay* netDisplay = Globals::getNetworkDisplay();

		//Connection mode
		unsigned int connectionMode = netDisplay->getConnectionMode();

		//Default neuron colour
		RGBColor defaultNeuronColor;
		bool fullRenderMode = netDisplay->isFullRenderMode();//Local copy for speed
		if(fullRenderMode){
			defaultNeuronColor = *netDisplay->getDefaultNeuronColorFullRender();
			sphereQuality = Globals::getNetworkDisplay()->getSphereQuality();
			sphereRadius = Globals::getNetworkDisplay()->getSphereRadius();
		}
		else{
			defaultNeuronColor = *netDisplay->getDefaultNeuronColor();
			glPointSize(netDisplay->getVertexSize());//Set the size of points in OpenGL
		}
		float neuronAlpha = netDisplay->getNeuronTransparency();

		//Get map with colours of neurons
		QHash<unsigned int, RGBColor*> neuronColorMap = netDisplay->getNeuronColorMap();

		//Initialize list of names for clicking on neurons
		glInitNames();

		//Work through the neuron groups listed in the view vector
		QList<unsigned int> neuronGrpIDs = Globals::getNetworkDisplay()->getVisibleNeuronGroupIDs();
		for(QList<unsigned int>::iterator neurGrpIter = neuronGrpIDs.begin(); neurGrpIter != neuronGrpIDs.end(); ++neurGrpIter){
			//Get the map of neurons associated with this group
			NeuronMap* neuronMap = network->getNeuronGroup(*neurGrpIter)->getNeuronMap();

			//Draw the neurons in the map
			NeuronMap::iterator neurMapEnd = neuronMap->end();
			for(NeuronMap::iterator neurIter = neuronMap->begin(); neurIter != neurMapEnd; ++neurIter){

				//Set the color of the neuron
				if(connectionMode & CONNECTION_MODE_ENABLED){
					if(netDisplay->getSingleNeuronID() == (*neurIter)->getID() ){//Single selected neuron
						tmpColor2 = netDisplay->getSingleNeuronColor();
						glColor4f(tmpColor2.red, tmpColor2.green, tmpColor2.blue, neuronAlpha);
					}
					else if(netDisplay->getToNeuronID() == (*neurIter)->getID() ){//To neuron
						tmpColor2 = netDisplay->getToNeuronColor();
						glColor4f(tmpColor2.red, tmpColor2.green, tmpColor2.blue, neuronAlpha);
					}
					else if(connectedNeuronMap.contains((*neurIter)->getID())){//A connected neuron
						weight = connectedNeuronMap[(*neurIter)->getID()];
						if(weight > 0)//Positive connection
							glColor4f(1.0f, 0.0f, 0.0f, neuronAlpha);
						else if(weight < 0)
							glColor4f(0.0f, 0.0f, 1.0f, neuronAlpha);
						else
							glColor4f(0.0f, 0.0f, 0.0f, neuronAlpha);
					}
					else{
						glColor4f(defaultNeuronColor.red, defaultNeuronColor.green, defaultNeuronColor.blue, neuronAlpha);
					}
				}
				else if(neuronColorMap.contains(neurIter.key())){
					tmpColor = neuronColorMap[neurIter.key()];
					glColor4f(tmpColor->red, tmpColor->green, tmpColor->blue, neuronAlpha);
				}
				else{
					glColor4f(defaultNeuronColor.red, defaultNeuronColor.green, defaultNeuronColor.blue, neuronAlpha);
				}

				//Draw the neuron
				glPushName((*neurIter)->getID());
					if(!fullRenderMode){//Draw neurons as a vertex
						glBegin(GL_POINTS);
							glVertex3f(neurIter.value()->getXPos(), neurIter.value()->getYPos(), neurIter.value()->getZPos());
						glEnd();
					}
					else{//Draw neurons as a sphere
						drawSphere(neurIter.value()->getXPos(), neurIter.value()->getYPos(), neurIter.value()->getZPos(), sphereRadius, sphereQuality);
					}
				glPopName();
			}
		}

		//Finished creating neurons display list
		glEndList();
		useNeuronsDisplayList = true;
	}
}


/*! Draws a sphere using OpenGL */
void NetworkViewer::drawSphere(float xPos, float yPos, float zPos, float radius, unsigned quality) {
	glPushMatrix();
	glTranslatef(xPos, yPos, zPos);//Translate to sphere position

	//Call display list to draw a sphere
	glCallList(sphereDisplayList);

	glPopMatrix();
}


/*! Draws a connection whose thickness varies with the weight */
void NetworkViewer::drawWeightedConnection(float x1, float y1, float z1, float x2, float y2, float z2, float weight){
	//Make sure weight is positive
	if(weight < 0.0f)
		weight *= -1.0f;

	//Draw line if we are less than minimum connection weight
	if(weightRadiusFactor * weight < minimumConnectionRadius){
		glBegin(GL_LINES);
			glVertex3f(x1, y1, z1);
			glVertex3f(x2, y2, z2);
		glEnd();//End of line drawing
		return;
	}

	//Calculate vector and angle with Z axis
	float conVect [] = { x2-x1, y2-y1, z2-z1 };

	//Some calculations
	float length = sqrt(pow(conVect[0], 2) + pow(conVect[1], 2) + pow(conVect[2], 2));
	float rotationAngle = GLT_INV_PI_DIV_180 * -1.0f * acos((conVect[2]) / length);//Angle with vertical z axis

	//Store matrix and move to first point
	glPushMatrix();
	glTranslatef(x1, y1, z1);
	glRotatef(rotationAngle, conVect[1], -conVect[0], 0.0f);//Rotate z vertical
	gluCylinder(gluConeObj, 0.0 , weightRadiusFactor * weight, length/2.0, 8, 8);
	glTranslatef(0.0, 0.0, length/2);//Half way up current Z axis
	gluCylinder(gluConeObj, weightRadiusFactor * weight, 0.0, length/2.0, 8, 8);
	glPopMatrix();
}


/*! Sets up OpenGL for a full render */
void NetworkViewer::initialiseFullRender(){
	// Cull backs of polygons
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	// Setup light parameters
	GLfloat fLightPos2[4]   = { defaultClippingVol.x1 + 0.5f * (defaultClippingVol.x2 - defaultClippingVol.x1), defaultClippingVol.y1, defaultClippingVol.z2, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, fLightPos2);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, fLowLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, fLowLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, fBrightLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, fBrightLight);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Mostly use material tracking
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
	glMateriali(GL_FRONT, GL_SHININESS, 128);

	//Transparency settings
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


/*! Switches off everything associated with full render mode. */
void NetworkViewer::disableFullRender(){
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);
}


/*! Point camera towards position specified in struct for camera and rotate scene appropriately */
void NetworkViewer::positionCamera(){    //
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
}


/*! Adapted from gltools
	Fills the 4x4 rotation matrix to enable it to be used to rotate camera frame
	Note that angle is in radians NOT degrees. */
void NetworkViewer::fillRotationMatrix(float angle, float x, float y, float z){
	float vecLength, sinSave, cosSave, oneMinusCos;
	float xx, yy, zz, xy, yz, zx, xs, ys, zs;

	// If NULL vector passed in, this will blow up...
	if(x == 0.0f && y == 0.0f && z == 0.0f){
		qCritical()<<"NetworkViewer: NULL MATRIX PASSED TO fillRotationMatrix.";
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


/*! Identifies the closest neuron to the viewer within the select buffer.
	Returns an invalid neuron ID if no hits are present */
unsigned int NetworkViewer::getSelectedNeuron(GLuint selectBuffer[], int hitCount, int bufferSize){
	if(hitCount == 0)
		return 0;
	if(hitCount * 4 > bufferSize)
		throw SpikeStreamException("Hit data overflows buffer");


	GLuint nameCount, *ptr, minZ, minZName=0, currentMinZ, name;
	ptr = selectBuffer;
	minZ = 0xffffffff;
	for(int i=0; i<hitCount; ++i){
		//First field is the number of names
		nameCount = *ptr;
		if(nameCount != 1)
			throw SpikeStreamException("Unnamed object selected or multiple names associated with the same object.");

		//Second field is minimum depth for this record
		++ptr;
		currentMinZ = *ptr;

		//Third field is the maximum depth for this record
		++ptr;

		//Remaining field for this record is the name - should only be 1 name if we have got this far
		++ptr;
		name = *ptr;

		if(currentMinZ < minZ){
			minZ = currentMinZ;
			minZName = name;
		}

		//Increase pointer to the next hit if it exists
		++ptr;
	}

	//Return the name with the minimum z
	return minZName;
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


/*! Loads up a box that encloses the entire network plus the origin. */
void NetworkViewer::loadDefaultClippingVolume(){
	/*If the network is empty or contains no neurons.
	set up default clipping volume around origin */
	if(!Globals::networkLoaded() || Globals::getNetwork()->size() == 0){
		defaultClippingVol.x1 = -100;
		defaultClippingVol.x2 = 100;
		defaultClippingVol.y1 = -100;
		defaultClippingVol.y2 = 100;
		defaultClippingVol.z1 = -100;
		defaultClippingVol.z2 = 100;
		return;
	}

	//Get box enclosing network
	defaultClippingVol = Globals::getNetwork()->getBoundingBox();

	//Want to include the origin in the starting clip volume
	if(defaultClippingVol.x1 > 0)
		defaultClippingVol.x1 = 0;
	if(defaultClippingVol.x2 < 0)
		defaultClippingVol.x2 =0;
	if(defaultClippingVol.y1 > 0)
		defaultClippingVol.y1 = 0;
	if(defaultClippingVol.y2 < 0)
		defaultClippingVol.y2 = 0;
	if(defaultClippingVol.z1 > 0)
		defaultClippingVol.z1 = 0;
	if(defaultClippingVol.z2 < 0)
		defaultClippingVol.z2 = 0;

	//Expand box by 20% to give a nice view
	defaultClippingVol.expand_percent(20);
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
void NetworkViewer::viewClippingVolume_Horizontal(Box& clipVolume){
	//First set camera parameters to their starting values
	initialiseCameraParameters();

	//Now adjust these parameters to view the whole of the clipping volume
	//X location should be half way along the clipping volume
	cameraMatrix[12] = clipVolume.x1 + (clipVolume.x2 - clipVolume.x1)/2.0f;


	//Camera is looking down the Y axis. So need to move back far enough to see all of clipping volume within perspective
	//First find whether z or x direction is longest
	GLfloat backwardsDistance;
	if((clipVolume.x2 - clipVolume.x1) > (clipVolume.z2 - clipVolume.z1)){//X direction is longest
		backwardsDistance = ((clipVolume.x2 - clipVolume.x1)/2.0f)/tan(gltDegToRad(perspective_angle)/2.0f);
	}
	else{//Z direction is longest
		backwardsDistance = ((clipVolume.z2 - clipVolume.z1)/2.0f)/tan(gltDegToRad(perspective_angle)/2.0f);
	}
	//Now move camera back so that it can see everything in both directions
	//In this case this moves camera back along the negative y direction
	cameraMatrix[13] = -1 * backwardsDistance;

	//Z location should be half way up clipping volume
	cameraMatrix[14] = clipVolume.z1 + (clipVolume.z2 - clipVolume.z1)/2.0f;

	//Forward vector does not need to be calculated because camera is initilised looking along positive Y, which is correct for horizontal view
}


/*! Sets the view so that the perspective fits the clipping volume seen horizontally. */
void NetworkViewer::viewClippingVolume_Vertical(Box& clipVolume){
	//First set camera parameters to their starting values
	initialiseCameraParameters();

	//X location should be half way along the clipping volume
	cameraMatrix[12] = clipVolume.x1 + (clipVolume.x2 - clipVolume.x1)/2.0f;

	//Camera is looking down the Z axis. So need to move back far enough to see all of clipping volume within perspective
	//First find whether y or x direction is longest
	GLfloat backwardsDistance;
	if((clipVolume.x2 - clipVolume.x1) > (clipVolume.y2 - clipVolume.y1)){//X direction is longest
		backwardsDistance = ((clipVolume.x2 - clipVolume.x1)/2.0f)/tan(gltDegToRad(perspective_angle)/2.0f);
	}
	else{//Y direction is longest
		backwardsDistance = ((clipVolume.y2 - clipVolume.y1)/2.0f)/tan(gltDegToRad(perspective_angle)/2.0f);
	}

	//Now move camera back so that it can see everything in both directions
	//To look above need to move up the positive direction of the Z axis
	cameraMatrix[14] = 1.1f*(backwardsDistance + clipVolume.z2);

	//Y location should be half way along clipping volume
	cameraMatrix[13] = clipVolume.y1 + (clipVolume.y2 - clipVolume.y1)/2.0f;

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


/*! Zooms into a particular neuron group or zooms out to show whole network. */
void NetworkViewer::setZoomLevel(){
	if(Globals::getNetworkDisplay()->isZoomEnabled()){
		unsigned int tmpZoomNeurGrpID = Globals::getNetworkDisplay()->getZoomNeuronGroupID();
		if(tmpZoomNeurGrpID == 0)
			zoomDefaultView();
		else{
			if(Globals::getNetworkDisplay()->getZoomStatus() == NetworkDisplay::ZOOM_SIDE)
				zoomToNeuronGroup(tmpZoomNeurGrpID);
			else if (Globals::getNetworkDisplay()->getZoomStatus() == NetworkDisplay::ZOOM_ABOVE)
				zoomAboveNeuronGroup(tmpZoomNeurGrpID);
		}
	}
}


/*! Resets the view so all neural networks can be seen. */
void NetworkViewer::zoomDefaultView(){
	viewClippingVolume_Horizontal(defaultClippingVol);
}


/*! Moves viewing position above selected layer and resizes it appropriately
	Don't need to set viewStateChanged here since the viewing angle is
	outside of the main list. */
void NetworkViewer::zoomAboveNeuronGroup(unsigned int neuronGroupID){
	if(!Globals::networkLoaded())
		return;
	Box neurGrpBox = Globals::getNetwork()->getNeuronGroupBoundingBox(neuronGroupID);
	viewClippingVolume_Vertical(neurGrpBox);
}


/*! Moves viewing position beside selected layer and resizes it appropriately
	Don't need to set viewStateChanged here since the viewing angle is
	outside of the main list. */
void NetworkViewer::zoomToNeuronGroup(unsigned int neuronGroupID){
	if(!Globals::networkLoaded())
		return;
	Box neurGrpBox = Globals::getNetwork()->getNeuronGroupBoundingBox(neuronGroupID);
	viewClippingVolume_Horizontal(neurGrpBox);
}


