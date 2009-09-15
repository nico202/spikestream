#include "EventRouter.h"
#include "Globals.h"
#include "NetworkViewer_V2.h"
using namespace spikestream;

/*! Constructor */
NetworkViewer_V2::NetworkViewer_V2(QWidget* parent) : QGLWidget(parent) {
    //Connect up to signal that is emitted whenever network display has changed
    connect(Globals::getEventRouter(), SIGNAL(networkDisplayChangedSignal()), this, SLOT(refresh()), Qt::QueuedConnection);

}


/*! Destructor */
NetworkViewer_V2::~NetworkViewer_V2(){
}


/*----------------------------------------------------------*/
/*-----                PROTECTED METHODS               -----*/
/*----------------------------------------------------------*/
void NetworkViewer_V2::initializeGL(){
}


void NetworkViewer_V2::paintGL(){
    //Local variables used in the render
    bool paintSkipped = false;
    bool resizeSkipped = false;

    //If we are already painting, skip further calls to this method triggered by accelerator keys
    if(paintingGL){
	paintSkipped = true;
	return;
    }


    //Draw the neurons
    drawNeurons();

    //Check for OpenGL errors
    checkOpenGLErrors();

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


void NetworkViewer_V2::resizeGL(int width, int height){
}



/*----------------------------------------------------------*/
/*-----                PRIVATE SLOTS                   -----*/
/*----------------------------------------------------------*/

/*! Re-draws the network */
void NetworkViewer_V2::refresh(){
    updateGL();
}


void NetworkViewer_V2::drawNeurons(){
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
