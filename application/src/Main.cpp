//SpikeStream includes
#include "SpikeStreamMainWindow.h"
#include "SpikeStreamApplication.h"

//Qt includes
#include <QApplication>
#include <QMessageBox>

 void logMessageOutput(QtMsgType type, const char *msg){
     switch (type) {
	 case QtDebugMsg:
	     cout<<"Debug: "<<msg<<endl;
	     break;
	 case QtWarningMsg:
	     fprintf(stderr, "Warning: %s\n", msg);
	     break;
	 case QtCriticalMsg:
	     QMessageBox::critical( 0, "SpikeStream Critical Error", msg);
	     fprintf(stderr, "Critical: %s\n", msg);
	     break;
	 case QtFatalMsg:
	     QMessageBox::critical( 0, "SpikeStream Fatal Error", msg);
	     fprintf(stderr, "Fatal: %s\n", msg);
	     abort();
     }
 }

//-------------------------- Main ------------------------------------------
/*! Main method for simulator that launches the application. */
//--------------------------------------------------------------------------

int main( int argc, char ** argv ) {

    //Install message handler for logging
    qInstallMsgHandler(logMessageOutput);


	//Create QApplication
	SpikeStreamApplication spikeStrApp(argc, argv);

	//Start up main window of application
	SpikeStreamMainWindow *spikeStrMainWin = new SpikeStreamMainWindow();
	spikeStrMainWin->setCaption( "SpikeStream" );
	spikeStrMainWin->show();

	//Listen for window closing events
	spikeStrApp.connect( &spikeStrApp, SIGNAL(lastWindowClosed()), &spikeStrApp, SLOT(quit()) );

	//Execute application
	return spikeStrApp.exec();
}

