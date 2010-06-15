//SpikeStream includes
#include "EventRouter.h"
#include "Globals.h"
#include "GlobalVariables.h"
#include "NetworkViewer.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QMouseEvent>

//Other includes
#include <iostream>
using namespace std;

//Defines for OpenGL
/*! Constant for PI. */
#define GL_PI 3.1415f

/* gltools macro to convert degrees to radians and vice versa. */
#define GLT_PI_DIV_180 0.017453292519943296
#define GLT_INV_PI_DIV_180 57.2957795130823229
#define gltDegToRad(x)	((x)*GLT_PI_DIV_180)
#define gltRadToDeg(x)	((x)*GLT_INV_PI_DIV_180)


/*! Constructor */
NetworkViewer::NetworkViewer(QWidget* parent) : QGLWidget(parent) {
    //Connect refresh to changes in the display of network or archive
    connect(Globals::getEventRouter(), SIGNAL(networkDisplayChangedSignal()), this, SLOT(refresh()), Qt::QueuedConnection);
    connect(Globals::getEventRouter(), SIGNAL(archiveTimeStepChangedSignal()), this, SLOT(refresh()), Qt::QueuedConnection);

    //Connect reset to changes in the network
    connect(Globals::getEventRouter(), SIGNAL(networkChangedSignal()), this, SLOT(reset()), Qt::QueuedConnection);

    //Initialize variables
    paintGLSkipped = false;
    resizeGLSkipped = false;
    useDisplayList = false;

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

    //Create a unique id for the main display list
    mainDisplayList = glGenLists(1);
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

    /* Use an existing display list if one has already been created
	Only use display lists when not in full render mode. They speed up
	the graphics considerably, but crash when they are too big. */
    if(useDisplayList){
		//Call the display list
		glCallList(mainDisplayList);
    }

    //Record a new display list
    else{
		//Lock network display whilst rendering is taking place
		//FIXME: MIGHT WANT TO LOCK NETWORK AS WELL
		Globals::getNetworkDisplay()->lockMutex();

		//Start recording new display list
		glNewList(mainDisplayList, GL_COMPILE_AND_EXECUTE);

	    //Draw the axes
	    drawAxes();

	    //Draw the neurons without names
	    drawNeurons();

	    //Draw the connections
	    drawConnections();

	    //Restore the original state of the matrix
	    glPopMatrix();

		//Finished recording display list
		glEndList();

		//Have now created the display list so record this fact for next render
		useDisplayList = true;

		//Unlock network display
		Globals::getNetworkDisplay()->unlockMutex();
    }

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


/*! Re-draws the network */
void NetworkViewer::refresh(){
    useDisplayList = false;
    updateGL();
}


/*! Resets the view completely typically after the network has changed */
void NetworkViewer::reset(){
    //Load up the volume enclosing the entire network
    loadDefaultClippingVolume();

    //Look at this network horizontally
    viewClippingVolume_Horizontal(defaultClippingVol);

    //Network has changed so need to re-render the display list
    useDisplayList = false;

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
	if(!Globals::isDrawAxes())
		return;

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
}


/*! Draws the connections */
void NetworkViewer::drawConnections(){
    //Nothing to do if no network is loaded
    if(!Globals::networkLoaded())
		return;

    //Local variables declared once here to save processing. These point to the points stored in the neurons
    Point3D* fromNeuronPoint;
    Point3D* toNeuronPoint;

    //Get pointer to network that is to be drawn and its associated display
    Network* network = Globals::getNetwork();
    NetworkDisplay* netDisplay = Globals::getNetworkDisplay();

    //Default neuron colour
    RGBColor positiveConnectionColor = *netDisplay->getPositiveConnectionColor();
    RGBColor negativeConnectionColor = *netDisplay->getNegativeConnectionColor();

    //Sort out the connection mode
    unsigned int singleNeuronID=0, toNeuronID=0;
    unsigned int connectionMode = netDisplay->getConnectionMode();
    if(connectionMode & CONNECTION_MODE_ENABLED){
		singleNeuronID = netDisplay->getSingleNeuronID();
		if(connectionMode & SHOW_BETWEEN_CONNECTIONS)
			toNeuronID = netDisplay->getToNeuronID();
    }
    bool drawConnection;

    //Start drawing lines
    glBegin(GL_LINES);

	//Work through the connection groups listed in the network display
	QList<unsigned int> conGrpIDs = Globals::getNetworkDisplay()->getVisibleConnectionGroupIDs();
	for(QList<unsigned int>::iterator conGrpIter = conGrpIDs.begin(); conGrpIter != conGrpIDs.end(); ++conGrpIter){

	    //Pointer to connection group
	    ConnectionGroup* conGrp = network->getConnectionGroup(*conGrpIter);

	    //Get neuron groups for extracting the position information
	    NeuronGroup* fromNeuronGroup = network->getNeuronGroup(conGrp->getFromNeuronGroupID());
	    NeuronGroup* toNeuronGroup = network->getNeuronGroup(conGrp->getToNeuronGroupID());

	    //Draw all the connections in the group
	    QList<Connection*>::const_iterator endConGrp = conGrp->end();
	    for(QList<Connection*>::const_iterator conIter = conGrp->begin(); conIter != endConGrp; ++ conIter){

			//Decide if connection should be drawn, depending on the connection mode and neuron id
			drawConnection = true;
			if(connectionMode & CONNECTION_MODE_ENABLED){
				//Single neuron mode
				if( !(connectionMode & SHOW_BETWEEN_CONNECTIONS) ){
					//Show only connections from a single neuron
					if(connectionMode & SHOW_FROM_CONNECTIONS){
						if( (*conIter)->fromNeuronID != singleNeuronID)
							drawConnection = false;
					}
					//Show only connections to a single neuron
					else if(connectionMode & SHOW_TO_CONNECTIONS){
						if( (*conIter)->toNeuronID != singleNeuronID)
							drawConnection = false;
					}
					//Show from and to connections to a single neuron
					else if( ((*conIter)->fromNeuronID != singleNeuronID) && ((*conIter)->toNeuronID != singleNeuronID) ){
						drawConnection = false;
					}
				}
				//Between neuron mode
				else{
					//Only show connections from first neuron to second
					if( (*conIter)->fromNeuronID != singleNeuronID || (*conIter)->toNeuronID != toNeuronID)
						drawConnection = false;
				}

				//Decide whether to draw connection based on its weight
				if( (*conIter)->weight < 0 && (connectionMode & SHOW_POSITIVE_CONNECTIONS) )
					drawConnection = false;
				if( (*conIter)->weight >= 0 && (connectionMode & SHOW_NEGATIVE_CONNECTIONS))
					drawConnection = false;
			}

			//Draw the connection
			if(drawConnection){
				//Get the position of the from and to neurons
				//FIXME: THIS COULD BE SPEEDED UP BY STORING THE POSITION IN Connection AT THE COST OF SOME LOADING COMPLEXITY
				fromNeuronPoint = &fromNeuronGroup->getNeuronLocation((*conIter)->fromNeuronID);
				toNeuronPoint = &toNeuronGroup->getNeuronLocation((*conIter)->toNeuronID);

				//Set the colour
				if((*conIter)->weight >= 0)
					glColor3f(positiveConnectionColor.red, positiveConnectionColor.green, positiveConnectionColor.blue);
				if((*conIter)->weight < 0)
					glColor3f(negativeConnectionColor.red, negativeConnectionColor.green, negativeConnectionColor.blue);

				//Draw the connection
				glVertex3f(fromNeuronPoint->getXPos(), fromNeuronPoint->getYPos(), fromNeuronPoint->getZPos());
				glVertex3f(toNeuronPoint->getXPos(), toNeuronPoint->getYPos(), toNeuronPoint->getZPos());
			}
	    }
	}

    //End of line drawing
    glEnd();
}



/*! Draws the visible neurons in the network */
void NetworkViewer::drawNeurons(){
    //Nothing to do if no network is loaded
    if(!Globals::networkLoaded())
		return;

    //Local variables
    RGBColor *tmpColor, tmpColor2;

    //Get pointer to network that is to be drawn and its associated display
    Network* network = Globals::getNetwork();
    NetworkDisplay* netDisplay = Globals::getNetworkDisplay();

    //Connection mode
    unsigned int connectionMode = netDisplay->getConnectionMode();

    //Default neuron colour
    RGBColor defaultNeuronColor = *netDisplay->getDefaultNeuronColor();

    //Get map with colours of neurons
    QHash<unsigned int, RGBColor*> neuronColorMap = netDisplay->getNeuronColorMap();

    //Set the size of points in OpenGL
	glPointSize(Globals::getVertexSize());

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
				if(netDisplay->getSingleNeuronID() == (*neurIter)->getID() ){
					tmpColor2 = netDisplay->getSingleNeuronColor();
					glColor3f(tmpColor2.red, tmpColor2.green, tmpColor2.blue);
				}
				else if(netDisplay->getToNeuronID() == (*neurIter)->getID() ){
					tmpColor2 = netDisplay->getToNeuronColor();
					glColor3f(tmpColor2.red, tmpColor2.green, tmpColor2.blue);
				}
				else{
					glColor3f(defaultNeuronColor.red, defaultNeuronColor.green, defaultNeuronColor.blue);
				}
			}
			else if(neuronColorMap.contains(neurIter.key())){
				tmpColor = neuronColorMap[neurIter.key()];
				glColor3f(tmpColor->red, tmpColor->green, tmpColor->blue);
			}
			else{
				glColor3f(defaultNeuronColor.red, defaultNeuronColor.green, defaultNeuronColor.blue);
			}

			//Draw the neuron
			glPushName((*neurIter)->getID());
			glBegin(GL_POINTS);
		    glVertex3f(neurIter.value()->getXPos(), neurIter.value()->getYPos(), neurIter.value()->getZPos());
			glEnd();
			glPopName();
	    }
	}
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


