//SpikeStream includes
#include "EventRouter.h"
#include "Globals.h"
#include "NetworkViewer_V2.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

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
NetworkViewer_V2::NetworkViewer_V2(QWidget* parent) : QGLWidget(parent) {
    //Refresh the display whenever network display has changed
    connect(Globals::getEventRouter(), SIGNAL(networkDisplayChangedSignal()), this, SLOT(refresh()), Qt::QueuedConnection);

    //Reset the display whenever the network changes
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

    //Set display to initial state
    reset();
}


/*! Destructor */
NetworkViewer_V2::~NetworkViewer_V2(){
}


/*----------------------------------------------------------*/
/*-----                PROTECTED METHODS               -----*/
/*----------------------------------------------------------*/

/*! Inherited from QGLWidget */
void NetworkViewer_V2::initializeGL(){
    //White background
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    //Create a unique id for the main display list
    mainDisplayList = glGenLists(1);
}


/*! Inherited from QGLWidget */
void NetworkViewer_V2::paintGL(){
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
	//Start recording new display list
	glNewList(mainDisplayList, GL_COMPILE_AND_EXECUTE);

	    //Draw the axes
	    drawAxes();

	    //Draw the neurons
	    drawNeurons();

	    //Draw the connections
	    drawConnections();

	    //Restore the original state of the matrix
	    glPopMatrix();

	//Finished recording display list
	glEndList();

	//Have now created the display list so record this fact for next render
	//useDisplayList = true;
    }

    //Check for OpenGL errors//
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
void NetworkViewer_V2::resizeGL(int screenWidth, int screenHeight){
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

void NetworkViewer_V2::mouseDoubleClickEvent (QMouseEvent * event ){
    cout<<"Mouse double click: "<<endl;
//    If you derive a class from QGLWidget you can reimplement
//mouseMoveEvent(QMouseEvent *event).
//Use something like this (stolen from the red book p152):
//
//  // Get the cursor and convert to model coordinates
//  QPoint cursor = event->pos();
//  GLint viewport[4];
//  GLdouble mvmatrix[16], projmatrix[16];
//  GLint realy;
//  GLdouble wx, wy, wz;
//
//  // Convert to model coordinates
//  glGetIntegerv(GL_VIEWPORT, viewport);
//  glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
//  glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
//
//  realy = viewport[3] - (GLint) cursor.y() - 1;
//  gluUnProject((GLdouble) cursor.x(), (GLdouble) realy, 0.0,
//    mvmatrix, projmatrix, viewport, &wx, &wy, &wz);
//
//Note the above assumes the point you're asking for is at 0 in
//the Z plane.

}


/*----------------------------------------------------------*/
/*-----                PRIVATE SLOTS                   -----*/
/*----------------------------------------------------------*/

void NetworkViewer_V2::moveUp(){
    cameraMatrix[12] += cameraMatrix[8];
    cameraMatrix[13] += cameraMatrix[9];
    cameraMatrix[14] += cameraMatrix[10];
    updateGL();
}

void NetworkViewer_V2::moveDown(){
    cameraMatrix[12] -= cameraMatrix[8];
    cameraMatrix[13] -= cameraMatrix[9];
    cameraMatrix[14] -= cameraMatrix[10];
    updateGL();
}

void NetworkViewer_V2::moveLeft(){
    cameraMatrix[12] -= cameraMatrix[0];
    cameraMatrix[13] -= cameraMatrix[1];
    cameraMatrix[14] -= cameraMatrix[2];
    updateGL();
}

void NetworkViewer_V2::moveRight(){
    cameraMatrix[12] += cameraMatrix[0];
    cameraMatrix[13] += cameraMatrix[1];
    cameraMatrix[14] += cameraMatrix[2];
    updateGL();
}

void NetworkViewer_V2::moveForward(){
    cameraMatrix[12] += cameraMatrix[4];
    cameraMatrix[13] += cameraMatrix[5];
    cameraMatrix[14] += cameraMatrix[6];
    updateGL();
}

void NetworkViewer_V2::moveBackward(){
    cameraMatrix[12] -= cameraMatrix[4];
    cameraMatrix[13] -= cameraMatrix[5];
    cameraMatrix[14] -= cameraMatrix[6];
    updateGL();
}

void NetworkViewer_V2::resetView(){
    viewClippingVolume_Horizontal(defaultClippingVol);
    updateGL();
}

void NetworkViewer_V2::rotateUp(){
    sceneRotateX += 2.5f;
    updateGL();
}

void NetworkViewer_V2::rotateDown(){
    sceneRotateX -= 2.5f;
    updateGL();
}

void NetworkViewer_V2::rotateLeft(){
    sceneRotateZ += 2.5f;
    updateGL();
}

void NetworkViewer_V2::rotateRight(){
    sceneRotateZ -= 2.5f;
    updateGL();
}


/*! Re-draws the network */
void NetworkViewer_V2::refresh(){
    useDisplayList = false;
    updateGL();
}


/*! Resets the view completely typically after the network has changed */
void NetworkViewer_V2::reset(){
    //Load up the volume enclosing the entire network
    loadDefaultClippingVolume();

    //Look at this network horizontally
    viewClippingVolume_Horizontal(defaultClippingVol);

    //Network has changed so need to re-render the display list
    useDisplayList = false;

    //Re-draw
    updateGL();
}


/*! Resets the view so all neural networks can be seen. */
void NetworkViewer_V2::zoomDefaultView(){
    viewClippingVolume_Horizontal(defaultClippingVol);
}


/*! Moves viewing position above selected layer and resizes it appropriately
	Don't need to set viewStateChanged here since the viewing angle is
	outside of the main list. */
void NetworkViewer_V2::zoomAboveNeuronGroup(unsigned int neuronGroupID){
    if(!Globals::networkLoaded())
	return;
    Box neurGrpBox = Globals::getNetwork()->getNeuronGroupBoundingBox(neuronGroupID);
    viewClippingVolume_Vertical(neurGrpBox);
    updateGL();
}


/*! Moves viewing position beside selected layer and resizes it appropriately
	Don't need to set viewStateChanged here since the viewing angle is
	outside of the main list. */
void NetworkViewer_V2::zoomToNeuronGroup(unsigned int neuronGroupID){
    if(!Globals::networkLoaded())
	return;
    Box neurGrpBox = Globals::getNetwork()->getNeuronGroupBoundingBox(neuronGroupID);
    viewClippingVolume_Horizontal(neurGrpBox);
    updateGL();
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Checks for errors in OpenGL. */
void NetworkViewer_V2::checkOpenGLErrors(){
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
void NetworkViewer_V2::drawAxes(void){
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
void NetworkViewer_V2::drawConnections(){
    //Nothing to do if no network is loaded
    if(!Globals::networkLoaded())
	return;

    //Local variables declared once here to save processing
    Point3D* fromNeurPoint;
    Point3D* toNeurPoint;

    //Get pointer to network that is to be drawn and its associated display
    Network* network = Globals::getNetwork();
    NetworkDisplay* netDisplay = Globals::getNetworkDisplay();

    //Default neuron colour
    RGBColor positiveConnectionColor = netDisplay->getPositiveConnectionColor();
    RGBColor negativeConnectionColor = netDisplay->getNegativeConnectionColor();

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
	    QList<Connection*>* conList = conGrp->getConnections();
	    QList<Connection*>::iterator endConList = conList->end();
	    for(QList<Connection*>::iterator conIter = conList->begin(); conIter != endConList; ++ conIter){

		//Get the position of the from and to neurons
		//FIXME: THIS COULD BE SPEEDED UP BY STORING THE POSITION IN Connection AT THE COST OF SOME LOADING COMPLEXITY
		fromNeurPoint = fromNeuronGroup->getNeuronLocation((*conIter)->fromNeuronID);
		toNeurPoint = toNeuronGroup->getNeuronLocation((*conIter)->toNeuronID);

		//Set the colour
		if((*conIter)->weight >= 0)
		    glColor3f(positiveConnectionColor.red, positiveConnectionColor.green, positiveConnectionColor.blue);
		else
		    glColor3f(negativeConnectionColor.red, negativeConnectionColor.green, negativeConnectionColor.blue);

		//Draw the neuron
		glVertex3f(fromNeurPoint->xPos, fromNeurPoint->yPos, fromNeurPoint->zPos);
		glVertex3f(toNeurPoint->xPos, toNeurPoint->yPos, toNeurPoint->zPos);

	    }
	}

    //End of line drawing
    glEnd();
}



/*! Draws the visible neurons in the network */
void NetworkViewer_V2::drawNeurons(){
    //Nothing to do if no network is loaded
    if(!Globals::networkLoaded())
	return;

    //Local variables
    RGBColor* tmpColor;

    //Get pointer to network that is to be drawn and its associated display
    Network* network = Globals::getNetwork();
    NetworkDisplay* netDisplay = Globals::getNetworkDisplay();

    //Default neuron colour
    RGBColor defaultNeuronColor = netDisplay->getDefaultNeuronColor();

    //Get map with colours of neurons
    QHash<unsigned int, RGBColor*> neuronColorMap = netDisplay->getNeuronColorMap();

    //Set the size of points in OpenGL
    glPointSize(5.0f);

    //Start point drawing of neurons in OpenGL
    glBegin(GL_POINTS);

	//Work through the neuron groups listed in the view vector
	QList<unsigned int> neuronGrpIDs = Globals::getNetworkDisplay()->getVisibleNeuronGroupIDs();
	for(QList<unsigned int>::iterator neurGrpIter = neuronGrpIDs.begin(); neurGrpIter != neuronGrpIDs.end(); ++neurGrpIter){
	    //Get the map of neurons associated with this group
	    NeuronMap* neuronMap = network->getNeuronGroup(*neurGrpIter)->getNeuronMap();

	    //Draw the neurons in the map
	    NeuronMap::iterator neurMapEnd = neuronMap->end();
	    for(NeuronMap::iterator neurIter = neuronMap->begin(); neurIter != neurMapEnd; ++neurIter){

		//Set the color of the neuron
		if(neuronColorMap.contains(neurIter.key())){
		    tmpColor = neuronColorMap[neurIter.key()];
		    glColor3f(tmpColor->red, tmpColor->green, tmpColor->blue);
		}
		else{
		    glColor3f(defaultNeuronColor.red, defaultNeuronColor.green, defaultNeuronColor.blue);
		}

		//Draw the neuron
		glVertex3f(neurIter.value()->xPos, neurIter.value()->yPos, neurIter.value()->zPos);
	    }
	}
    //End the drawing of points in OpenGL
    glEnd();
}



/*! Point camera towards position specified in struct for camera and rotate scene appropriately */
void NetworkViewer_V2::positionCamera(){    //
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
void NetworkViewer_V2::fillRotationMatrix(float angle, float x, float y, float z){
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


/*! Sets the camera parameters to their starting values. */
void NetworkViewer_V2::initialiseCameraParameters(){
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
void NetworkViewer_V2::loadDefaultClippingVolume(){
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
void NetworkViewer_V2::rotateVector(GLfloat x, GLfloat y, GLfloat z, GLfloat result[]){
    result[0] = rotationMatrix[0] * x + rotationMatrix[4] * y + rotationMatrix[8] *  z;
    result[1] = rotationMatrix[1] * x + rotationMatrix[5] * y + rotationMatrix[9] *  z;
    result[2] = rotationMatrix[2] * x + rotationMatrix[6] * y + rotationMatrix[10] * z;
}


/*! Rotates camera around its own X axis. */
void NetworkViewer_V2::rotateXAxis(float angle){
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
void NetworkViewer_V2::rotateZAxis(float angle){
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
void NetworkViewer_V2::viewClippingVolume_Horizontal(Box& clipVolume){
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
void NetworkViewer_V2::viewClippingVolume_Vertical(Box& clipVolume){
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

