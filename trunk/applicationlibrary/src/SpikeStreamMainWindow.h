#ifndef SPIKESTREAMMAINWINDOW_H
#define SPIKESTREAMMAINWINDOW_H

//SpikeStream includes
#include "DatabaseManager.h"
#include "SpikeStreamApplication.h"
using namespace spikestream;

//Qt includes
#include <QProgressDialog>
#include <QMainWindow>
#include <QTabWidget>
#include <QCloseEvent>

namespace spikestream {

    /*! The main class of the application and the widget below the Spike Stream
	Application. Responsible for setting up the various widgets and for loading and saving databases.*/
    class SpikeStreamMainWindow: public QMainWindow {
		Q_OBJECT

		public:
			SpikeStreamMainWindow();
			~SpikeStreamMainWindow();

		private slots:
			void about();
			void clearDatabases();
			void closeEvent( QCloseEvent* );
			void importConnectionMatrix();
			void importNRMNetwork();
			void loadDatabases();
			void saveDatabases();
			void showAnalysisWidget();
			void showArchiveWidget();
			void showEditorWidget();
			void showNetworkWidget();
			void databaseManagerFinished();


		private:
			//======================  VARIABLES  ======================
			QTabWidget *tabWidget;

			/*! Carries out heavy database tasks */
			DatabaseManager* databaseManager;

			QProgressDialog* progressDialog;

			//=======================  METHODS  =======================
			/*! Declare copy constructor private so it cannot be used inadvertently. */
			SpikeStreamMainWindow (const SpikeStreamMainWindow&);

			/*! Declare assignment private so it cannot be used inadvertently.*/
			SpikeStreamMainWindow operator = (const SpikeStreamMainWindow&);

    };
}

#endif//SPIKESTREAMMAINWINDOW_H
