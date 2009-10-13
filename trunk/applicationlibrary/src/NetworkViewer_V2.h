#ifndef NETWORKVIEWER_H
#define NETWORKVIEWER_H

//SpikeStream includes
#include "Box.h"

//Qt includes
#include <qgl.h>

//FIXME: DELETE THESE INCLUDES WHEN THIS CLASS IS TIDIED UP!
#include "RGBColor.h"
#include <q3progressbar.h>
#include <vector>
using namespace std;

namespace spikestream {

    /*! Draws the current network using OpenGL */
    class NetworkViewer_V2 : public QGLWidget  {
	Q_OBJECT

	public:
	    NetworkViewer_V2(QWidget* parent);
	    ~NetworkViewer_V2();

	    //FIXME: TEMPORARILY PUBLIC
	    void loadDefaultClippingVolume();


	    //FIXME: EMPTY METHODS DECLARED FOR BACKWARDS COMPATIBILITY WITH OLD NETWORK VIEWER REMOVE ASAP
	    void addHighlight(unsigned int, RGBColor*) {}
	    void cancelRenderProgress(){}
	    void clearHighlights(){}
	    void deleteConnectionGroup(unsigned int){}
	    void deleteNeuronGroup(unsigned int){}
	    vector<unsigned int>* getConnectionViewVector(){ return &TEMP_VECTOR;}
	    void loadConnectionGroup(unsigned int, bool){}
	    void loadNeuronGroup(unsigned int, bool){}
	    void reloadEverything(){}
	    void setConnectionView(vector<unsigned int>){}
	    void setFullRenderMode(bool){}
	    void setLayerView(vector<unsigned int>){}
	    void setMaxAutoLoadConnGrpSize(unsigned int){}
	    void setNetworkViewerProperties(QWidget*){}
	    void setNeuronConnectionMode(bool, unsigned int, bool, unsigned int){}
	    void setNeuronFilterMode(bool, char, char, bool, bool, bool){}
	    void setRenderDelay(double){}
	    void setRenderProgressBar(Q3ProgressBar*){}
	    void showConnections(bool){}
	    void zoomAboveLayer(unsigned int){}
	    void zoomToLayer(unsigned int){}

	    vector<unsigned int> TEMP_VECTOR;

	public slots:
	    void refresh();
	    void reset();

	protected:
	    //Methods inherited from QGLWidget
	    void initializeGL();
	    void paintGL();
	    void resizeGL(int width, int height);

	    //Methods inherited from QWidget
	    void mouseDoubleClickEvent (QMouseEvent * event );

	private slots:
	    void moveBackward();
	    void moveForward();
	    void moveUp();
	    void moveDown();
	    void moveLeft();
	    void moveRight();
	    void resetView();
	    void rotateUp();
	    void rotateDown();
	    void rotateLeft();
	    void rotateRight();

	    void viewClippingVolume_Horizontal(Box& clipVolume);
	    void viewClippingVolume_Vertical(Box& clipVolume);
	    void zoomDefaultView();
	    void zoomAboveNeuronGroup(unsigned int neuronGroupID);
	    void zoomToNeuronGroup(unsigned int neuronGroupID);

	private:
	    //=======================  VARIABLES  ========================
	    /*! Reference to the display list for the graphics. This stores OpenGL
	    commands, which can then be played back as a block. */
	    GLuint mainDisplayList;

	    /*! Determines whether the stored display list should be used or a new one generated.
		A new one should be generated if the network or the network display is changed */
	    bool useDisplayList;

	    /*! Cancels render.*/
	    bool cancelRender;

	    /*! During full render, do not want to start render or resize, so record
		whether paintGL() or resizeGL() have been called and then call them
		again at the end of the render. newTempScreen width and height are the
		latest values of the screen width and height if resizeGL() has been called. */
	    bool paintGLSkipped;

	    /*! During render, do not want to start render or resize, so record
		whether paintGL() or resizeGL() have been called and then call them
		again at the end of the render. newTempScreen width and height are the
		latest values of the screen width and height if resizeGL() has been called. */
	    bool resizeGLSkipped;

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
	    Box defaultClippingVol;

	    /*! Screen width is stored here when resizeGL() is cancelled because
		render is in progress */
	    int newTempScreenWidth;

	    /*! Screen height is stored here when resizeGL() is cancelled because
		render is in progress */
	    int newTempScreenHeight;

	    //======================  METHODS  ===========================
	    void checkOpenGLErrors();
	    void drawAxes();
	    void drawConnections();
	    void drawNeurons();
	    void fillRotationMatrix(float angle, float x, float y, float z);
	    void initialiseCameraParameters();
	    void positionCamera();
	    void rotateVector(GLfloat x, GLfloat y, GLfloat z, GLfloat result[]);
	    void rotateXAxis(float angle);
	    void rotateZAxis(float angle);

    };

}

#endif//NETWORKVIEWER_H
