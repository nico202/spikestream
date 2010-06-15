#ifndef SPIKESTREAMAPPLICATION_H
#define SPIKESTREAMAPPLICATION_H

//Qt includes
#include <QApplication>

//Other includes
#ifdef LINUX32_SPIKESTREAM
	#include <sys/time.h>
#endif//LINUX32_SPIKESTREAM

namespace spikestream {

    /*! Inherits from QApplication so that it can filter out XEvents during
		slow renders. Is the QApplication for the application. */
    class SpikeStreamApplication : public QApplication {
		Q_OBJECT

		public:
			SpikeStreamApplication(int & argc, char ** argv);
			~SpikeStreamApplication();

		protected:
			#ifdef LINUX32_SPIKESTREAM
				//Inherited from QApplication - Linux specific
				bool x11EventFilter( XEvent * );
			#endif//LINUX32_SPIKESTREAM

		private slots:
			void startRender();
			void stopRender();

		private:
			//============================ VARIABLES ==============================
			/*! Records the duration of each render.*/
			unsigned int renderDuration_ms;

			/*! Records the time of the last key press at the start of the render.*/
			unsigned int startRenderKeyEventTime;

			/*! Records the time of each key press event.*/
			unsigned int keyEventTime;

			#ifdef LINUX32_SPIKESTREAM
				/*! Time structure to record the start of the render.*/
				timeval startRenderTime;

				/*! Time structure to record the end of the render.*/
				timeval stopRenderTime;
			#endif//LINUX32_SPIKESTREAM

			/*! Records when rendering is in progress.*/
			bool rendering;


			//============================ METHODS ================================
			/*! Declare copy constructor private so it cannot be used inadvertently.*/
			SpikeStreamApplication(const SpikeStreamApplication&);

			/*! Declare assignment private so it cannot be used inadvertently.*/
			SpikeStreamApplication operator=(const SpikeStreamApplication&);

    };
}


#endif//SPIKESTREAMAPPLICATION_H


