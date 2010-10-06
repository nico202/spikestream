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
		Application. Responsible for initializing the application and setting up the various widgets.*/
    class SpikeStreamMainWindow: public QMainWindow {
		Q_OBJECT

		public:
			SpikeStreamMainWindow();
			~SpikeStreamMainWindow();

		protected:
			void closeEvent (QCloseEvent* event);

		private slots:
			void about();
			void clearDatabases();
			void importConnectionMatrix();
			void importNRMNetwork();
			void loadDatabases();
			void saveDatabases();
			void showAnalysisWidget();
			void showArchiveWidget();
			void showEditorWidget();
			void showNetworkWidget();
			void showSimulationWidget();
			void showViewerWidget();
			void databaseManagerFinished();


		private:
			//======================  VARIABLES  ======================
			QTabWidget *tabWidget;

			/*! Carries out heavy database tasks */
			DatabaseManager* databaseManager;

			QProgressDialog* progressDialog;

			//=======================  METHODS  =======================
			void initializeApplication();

			/*! Declare copy constructor private so it cannot be used inadvertently. */
			SpikeStreamMainWindow (const SpikeStreamMainWindow&);

			/*! Declare assignment private so it cannot be used inadvertently.*/
			SpikeStreamMainWindow operator = (const SpikeStreamMainWindow&);

    };
}

#endif//SPIKESTREAMMAINWINDOW_H
