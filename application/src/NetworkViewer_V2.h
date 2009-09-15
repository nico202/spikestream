#ifndef NETWORKVIEWER_H
#define NETWORKVIEWER_H

namespace spikestream {

    /*! Draws the current network using OpenGL */
    class NetworkViewer_V2 : public QGLWidget  {
	Q_OBJECT

	public:
	    NetworkViewer_V2(QWidget* parent);
	    ~NetworkViewer_V2();

	protected:
	    //Methods inherited from QGLWidget
	    void initializeGL();
	    void paintGL();
	    void resizeGL(int width, int height);

	private slots:
	    void refresh();

	private:



    };

}

#endif//NETWORKVIEWER_H
