//SpikeStream includes
#include "DBConfigMainWindow.h"
using namespace spikestream;

//Qt includes
#include <QApplication>
#include <QDebug>
#include <QMessageBox>

//Other includes
#include <iostream>
#include <stdio.h>
using namespace std;


/*! Function to handle QDebug messages */
void logMessageOutput(QtMsgType type, const char *msg){
	switch (type) {
	case QtDebugMsg:
		cout<<"Debug: "<<msg<<endl;
	break;
	case QtWarningMsg:
		QMessageBox::warning( 0, "Warning", msg);
		fprintf(stderr, "Warning: %s\n", msg);
	break;
	case QtCriticalMsg:
		QMessageBox::critical( 0, "Critical Error", msg);
		fprintf(stderr, "Critical: %s\n", msg);
	break;
	case QtFatalMsg:
		QMessageBox::critical( 0, "Fatal Error", msg);
		fprintf(stderr, "Fatal: %s\n", msg);
		abort();
	}
}


//-------------------------- Main ------------------------------------------
/*! Main method that launches the application. */
//--------------------------------------------------------------------------

int main( int argc, char ** argv ) {
	//Install message handler for logging
	qInstallMsgHandler(logMessageOutput);

	//Create QApplication
	QApplication dbConfigApp(argc, argv);

	//Start up main window of application
	DBConfigMainWindow *dbConfigMainWin = new DBConfigMainWindow();
	dbConfigMainWin->setWindowTitle( "SpikeStream Database Configuration" );
	dbConfigMainWin->show();


//	//Listen for window closing events
//	dbConfigApp.connect( &dbConfigApp, SIGNAL(lastWindowClosed()), &dbConfigApp, SLOT(quit()) );

//	//Execute application
//	return dbConfigApp.exec();
}

