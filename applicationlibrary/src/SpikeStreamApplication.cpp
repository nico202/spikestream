//SpikeStream includes
#include "SpikeStreamApplication.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

//Other includes
#ifdef LINUX32_SPIKESTREAM
	#include <X11/Xlib.h>
#endif
#include <iostream>
using namespace std;


/*! Constructor. */
SpikeStreamApplication::SpikeStreamApplication(int & argc, char ** argv) : QApplication(argc, argv){
	renderDuration_ms = 0;
	startRenderKeyEventTime = 0;
	rendering = false;

	//Set the application styles
	this->setStyleSheet("* { font-size: 12px; }");
}


/*! Destructor. */
SpikeStreamApplication::~SpikeStreamApplication(){
}


//Inherited from QApplication
bool SpikeStreamApplication::notify(QObject* receiver, QEvent* event){
	bool result = true;
	try{
		result = QApplication::notify(receiver, event);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<"Event handling exception: "<<ex.getMessage();
	}
	catch(...){
		qCritical()<<"Unknown event handling exception.";
	}
	return result;
}


//--------------------------------------------------------------------------
//-------------------------- PUBLIC METHODS --------------------------------
//--------------------------------------------------------------------------

/*! Called when the network viewer starts the render and records the time
	at this point and the time of the last key event. */
void SpikeStreamApplication::startRender(){
	rendering = true;
	gettimeofday(&startRenderTime, NULL);
	startRenderKeyEventTime = keyEventTime;
}


/*! Records the time at which the render completes and calculates the duration
	of the render. */
void SpikeStreamApplication::stopRender(){
	rendering = false;
	gettimeofday(&stopRenderTime, NULL);
	renderDuration_ms = 1000 * (stopRenderTime.tv_sec - startRenderTime.tv_sec) + (stopRenderTime.tv_usec - startRenderTime.tv_usec) / 1000;
}


//---------------------------------------------------------------------------
//------------------------- PROTECTED METHODS -------------------------------
//---------------------------------------------------------------------------

/*! Method inherited from QApplication that is called whenever an X11 XEvent is
	received by the application. Returning true filters the event, returning
	false passes the event on to the application.
	This method filters events that are received whilst the NetworkViewer is
	rendering. Since display lists are used, have no control during the render
	so have to look for events that were generated during the time window of
	the render. This method is specific to Linux. */
#ifdef LINUX32_SPIKESTREAM
	bool SpikeStreamApplication::x11EventFilter( XEvent * xEvent){

		//Look for events from the keyboard
		if(xEvent->type == KeyPress || xEvent->type == KeyRelease){
			//Record time of key event
			keyEventTime = xEvent->xkey.time;

			//If the key event arrived during the render, ignore it.
			if((keyEventTime - startRenderKeyEventTime) < renderDuration_ms){
				return true;
			}
		}
		return false;
	}
#endif//LINUX32_SPIKESTREAM



