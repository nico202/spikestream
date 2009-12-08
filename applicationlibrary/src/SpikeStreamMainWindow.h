#ifndef SPIKESTREAMMAINWINDOW_H
#define SPIKESTREAMMAINWINDOW_H

//SpikeStream includes
#include "BusyDialog.h"
#include "SpikeStreamApplication.h"

//Qt includes
#include <q3mainwindow.h>
#include <qtabwidget.h>
#include <qapplication.h>
#include <q3process.h>
#include <qtimer.h>
#include <q3accel.h>
//Added by qt3to4:
#include <QCloseEvent>


//-------------------------- SpikeStream Main Window -----------------------
/*! The main class of the application and the widget below the Spike Stream
	Application. Responsible for setting up the various widgets, coordinating
	communication between them and for loading and saving databases.*/
//--------------------------------------------------------------------------

class SpikeStreamMainWindow: public QMainWindow {
    Q_OBJECT

	public:
		SpikeStreamMainWindow(SpikeStreamApplication *ssApp);
		~SpikeStreamMainWindow();
		//NetworkViewer_V2* getNetworkViewer();
		void reloadConnectionDetails();
		void reloadConnections();
		void reloadNeuronGroups();
		void setFromNeuronID(unsigned int neurGrpID, unsigned int fromNeurID);
		void setToNeuronID(unsigned int toNeurID);
		void setSplashScreenMessage(QString);
		void simulationDestroyed();
		void simulationInitialised();


		//======================== VARIABLES ===================================
		/*! Static reference to the main application to invoke global methods
			to reload neuron groups etc. */
		static SpikeStreamMainWindow* spikeStrMainWin;

		/*! Static reference to the QApplication to control rendering, lock
			global mutexes, etc. */
		static SpikeStreamApplication* spikeStreamApplication;

		/*! Root directory of the application set once and used by other classes
			to load files etc.*/
		static QString workingDirectory;


	public slots:
		void reloadPatterns();

	signals:
		void reload();


	private slots:
		void about();
		void acceleratorKeyPressed(int acceleratorID);
		void clearDatabases();
		void closeEvent( QCloseEvent* );
		void importConnectionMatrix();
		void loadDatabases();
		void importNRMNetwork();
		void managePatterns();
		void manageProbes();
		void reloadDevices();
		void reloadEverything();
		void saveDatabases();


	private:
		//======================== VARIABLES ==================================
		//Database handling classes
		DBInterface* networkDBInterface;
		DBInterface* archiveDBInterface;
		DBInterface* patternDBInterface;
		DBInterface* deviceDBInterface;

		//Store references to classes that we need to access
		//SimulationManager* simulationManager;
		//SimulationWidget *simulationWidget;
		QTabWidget *tabWidget;
		Q3Accel* keyboardAccelerator;

		/*! Default location for looking for image files, scripts, etc.*/
		QString defaultFileLocation;


		//=========================== METHODS ===============================
		/*! Declare copy constructor private so it cannot be used inadvertently. */
		SpikeStreamMainWindow (const SpikeStreamMainWindow&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		SpikeStreamMainWindow operator = (const SpikeStreamMainWindow&);

};


#endif//SPIKESTREAMMAINWINDOW_H
