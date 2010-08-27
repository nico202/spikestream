#ifndef NETWORKVIEWER_H
#define NETWORKVIEWER_H

//SpikeStream includes
#include "Box.h"

//Qt includes
#include <qgl.h>
#include <QHash>

namespace spikestream {

	/*! Draws the current network in 3D using OpenGL. */
	class NetworkViewer : public QGLWidget  {
		Q_OBJECT

		public:
			NetworkViewer(QWidget* parent);
			~NetworkViewer();


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

			/*! Object used to draw a sphere */
			GLUquadricObj* gluSphereObj;

			/*! Object used to draw a cone */
			GLUquadricObj* gluConeObj;

			/*! Factor by which weight is multiplied to obtain thickness */
			float weightRadiusFactor;

			/*! Minimum thickness of a connection */
			float minimumConnectionRadius;

			/*! Quality of the cones used to render a weighted connection */
			unsigned connectionQuality;

			/*! Map holding IDs of all neurons with a connection to them.
				Used when connection mode is enabled */
			QHash<unsigned, float> connectedNeuronMap;


			//======================  METHODS  ===========================
			void checkOpenGLErrors();
			void disableFullRender();
			void drawAxes();
			void drawConnections();
			void drawNeurons();
			void drawSphere(float xPos, float yPos, float zPos, float radius, unsigned quality);
			void drawWeightedConnection(float x1, float y1, float z1, float x2, float y2, float z2, float weight);
			void fillRotationMatrix(float angle, float x, float y, float z);
			unsigned int getSelectedNeuron(GLuint selectBuffer[], int hitCount, int bufferSize);
			void initialiseCameraParameters();
			void initialiseFullRender();
			void loadDefaultClippingVolume();
			void positionCamera();
			void rotateVector(GLfloat x, GLfloat y, GLfloat z, GLfloat result[]);
			void rotateXAxis(float angle);
			void rotateZAxis(float angle);
			void setZoomLevel();
			void zoomDefaultView();
			void zoomAboveNeuronGroup(unsigned int neuronGroupID);
			void zoomToNeuronGroup(unsigned int neuronGroupID);
	};

}

#endif//NETWORKVIEWER_H
