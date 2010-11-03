//SpikeStream includes
#include "AnalysisLoaderWidget.h"
#include "SpikeStreamMainWindow.h"
#include "GlobalVariables.h"
#include "ConfigLoader.h"
#include "Globals.h"
#include "NRMImportDialog.h"
#include "SpikeStreamException.h"
#include "NetworksWidget.h"
#include "ArchiveWidget.h"
#include "NeuronGroupWidget.h"
#include "ConnectionWidget.h"
#include "NetworkViewer.h"
#include "NetworkViewerProperties.h"
#include "SimulationLoaderWidget.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <qsplitter.h>
#include <qsplashscreen.h>
#include <qpixmap.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qfile.h>
#include <QCloseEvent>
#include <QDebug>
#include <QScrollArea>

//Other includes
#include <string>
using namespace std;


/*! Constructor. */
SpikeStreamMainWindow::SpikeStreamMainWindow() : QMainWindow(){
	setWindowTitle("SpikeStream");

	try{
		//Get the working directory and store in global scope
		QString rootDir = QCoreApplication::applicationDirPath();
		rootDir.truncate(rootDir.size() - 4);//Trim the "/bin" off the end
		Globals::setSpikeStreamRoot(rootDir);

		//Set up splash screen to give feedback whilst application is loading
		QSplashScreen *splashScreen = 0;
		QPixmap splashPixmap(Globals::getSpikeStreamRoot() + "/images/spikestream_splash.png" );
		splashScreen = new QSplashScreen( splashPixmap );
		splashScreen->show();

		//Set up application and GUI
		initializeApplication();

		//Get rid of splash screen if it is showing
		if(splashScreen){
			splashScreen->finish( this );
			delete splashScreen;
		}
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
		exit(1);
	}
	catch(...){
		qCritical()<<"An unknown exception occurred setting up application.";
		exit(1);
	}
}


/*! Destructor. */
SpikeStreamMainWindow::~SpikeStreamMainWindow(){
	//Clean up globals - everything stored in globals is cleaned up by globals
	Globals::cleanUp();

	delete databaseManager;
}


/*----------------------------------------------------------*/
/*-----                PROTECTED METHODS               -----*/
/*----------------------------------------------------------*/

/*! Overridden close to check that network is saved and simulation or analysis is not running. */
void SpikeStreamMainWindow::closeEvent (QCloseEvent* event){
	if(databaseManager->isRunning()){
		qWarning()<<"Database manager is still running, it is recommended that you wait for it to complete";
		event->ignore();
		return;
	}

	//Nothing will be running or unsaved if no network is loaded
	if(!Globals::networkLoaded()) {
		event->accept();
		return;
	}

	//Check for simulation loaded
	if(Globals::isSimulationLoaded()){
		qWarning()<<"SpikeStream should not be quit with a simulation loaded.\nUnload the simulation and try again.";
		event->ignore();
		return;
	}

	//Check for analysis running
	if(Globals::isAnalysisRunning()){
		int response = QMessageBox::warning(this, "Quit SpikeStream?", "Analysis is running.\nAre you sure that you want to quit SpikeStream?", QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
		if(response != QMessageBox::Ok){
			event->ignore();
			return;
		}
	}

	//Check for network save
	if(!Globals::getNetwork()->isSaved()){
		int response = QMessageBox::warning(this, "Quit SpikeStream?", "Network has not been saved.\nAre you sure that you want to quit SpikeStream?", QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
		if(response != QMessageBox::Ok){
			event->ignore();
			return;
		}
	}

	//Accept event if checks have been passed.
	event->accept();
}


/*----------------------------------------------------------*/
/*-----                PRIVATE SLOTS                   -----*/
/*----------------------------------------------------------*/

/*! Displays brief information about the application. */
void SpikeStreamMainWindow::about(){
	QMessageBox::about( this, "About", "SpikeStream\nVersion 0.1 beta\nCreated by David Gamez: david@davidgamez.eu");
}


/*! Called when the database manager has finished its task */
void SpikeStreamMainWindow::databaseManagerFinished(){
	//Check for errors
	if(databaseManager->isError())
		qCritical()<<databaseManager->getErrorMessage();

	if(databaseManager->getTaskID() == DatabaseManager::CLEAR_DATABASES_TASK){
		//Inform other classes about the change
		Globals::setNetwork(NULL);
		Globals::getEventRouter()->reloadSlot();
		progressDialog->hide();
		delete progressDialog;
	}
}


/*! Displays the analysis widget */
void SpikeStreamMainWindow::showAnalysisWidget(){
	tabWidget->setCurrentIndex(5);
}


/*! Displays the archive widget */
void SpikeStreamMainWindow::showArchiveWidget(){
	tabWidget->setCurrentIndex(4);
}


/*! Displays the editor widget */
void SpikeStreamMainWindow::showEditorWidget(){
	tabWidget->setCurrentIndex(1);
}


/*! Displays the networks widget */
void SpikeStreamMainWindow::showNetworkWidget(){
	tabWidget->setCurrentIndex(0);
}


/*! Displays the simulation widget */
void SpikeStreamMainWindow::showSimulationWidget(){
	tabWidget->setCurrentIndex(3);
}


/*! Displays the viewer widget */
void SpikeStreamMainWindow::showViewerWidget(){
	tabWidget->setCurrentIndex(2);
}


/*! Empties all databases except the Neuron, Synapse and Probe types. Useful when
	an error has led to out of date information in one of the tables. However all
	data will be lost if this method is called. */
void SpikeStreamMainWindow::clearDatabases(){
	//Confirm that user wants to take this action.
	QMessageBox msgBox;
	msgBox.setText("Clearing Database");
	msgBox.setInformativeText("Are you sure that you want to clear the database completely?\nAll data will be lost and this step cannot be undone.");
	msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Cancel);
	int ret = msgBox.exec();
	if(ret != QMessageBox::Ok)
		return;

	//Instruct database manager to delete all networks
	progressDialog = new QProgressDialog("Clearing databases, please wait", "", 0, 0, this, Qt::CustomizeWindowHint);
	progressDialog->setWindowModality(Qt::WindowModal);
	progressDialog->setCancelButton(0);//Cannot implement cancel sensibly
	databaseManager->startClearDatabases();
	progressDialog->show();
}


/*! Loads a neuron group and associated connections from a file.
	This should be a comma separated matrix of connection weights. */
void SpikeStreamMainWindow::importConnectionMatrix(){
	qWarning()<<"Import connection matrix method not implemented";
	//    //Warn user about limitations
	//    int response = QMessageBox::warning(this, "Import ConnectionMatrix", "This is a preliminary method that will try create a new layer at position (0,0,0) with the default neuron and synapse types\n Make sure there is enough space for the new layer.\nDo you want to continue?", QMessageBox::Yes, QMessageBox::No, 0);
	//    if(response != QMessageBox::Yes)
	//	return;
	//
	//    //Get the file name
	//    QString fileName = Q3FileDialog::getOpenFileName(defaultFileLocation, "All files (*.*)", this, "Load matrix dialog", "Choose a file to load" );
	//    if(fileName.isNull())
	//	return;
	//
	//    ConnectionMatrixLoader* matrixLoader = new ConnectionMatrixLoader(networkDBInterface);
	//    matrixLoader->loadConnectionMatrix(fileName);
	//
	//    //Clean up matrix loader
	//    delete matrixLoader;

}


/*! Displays a dialog/wizard to import networks from NRM files into the SpikeStream database */
void SpikeStreamMainWindow::importNRMNetwork(){
	NRMImportDialog* nrmDialog = new NRMImportDialog(this);
	nrmDialog->exec();
	delete nrmDialog;
}


/*! Loads databases from a file selected by the user.
	Launches database manager, which allows user to select which database to load. */
void SpikeStreamMainWindow::loadDatabases(){
	qWarning()<<"Load database method not implemented";
}


/*! Saves databases
	Allows user to choose file and then launches database manager so that user can choose
	which databases to save. */
void SpikeStreamMainWindow::saveDatabases(){
	qWarning()<<"Save databases method not implementd";
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Sets up application including databases and GUI. */
void SpikeStreamMainWindow::initializeApplication(){
	//Set up config
	ConfigLoader* configLoader = new ConfigLoader();

	//Set up the data access objects wrapping the network database
	DBInfo netDBInfo(
			configLoader->getParameter("spikeStreamNetworkHost"),
			configLoader->getParameter("spikeStreamNetworkUser"),
			configLoader->getParameter("spikeStreamNetworkPassword"),
			"SpikeStreamNetwork"
	);

	//Create network data access object
	NetworkDao* networkDao = new NetworkDao(netDBInfo);
	Globals::setNetworkDao(networkDao);

	//Set up the data access objects wrapping the archive database.
	DBInfo archiveDBInfo(
			configLoader->getParameter("spikeStreamArchiveHost"),
			configLoader->getParameter("spikeStreamArchiveUser"),
			configLoader->getParameter("spikeStreamArchivePassword"),
			"SpikeStreamArchive"
	);

	//Create archive data access object
	ArchiveDao* archiveDao = new ArchiveDao(archiveDBInfo);
	Globals::setArchiveDao(archiveDao);

	//Set up the data access object wrapping the analysis database.
	DBInfo analysisDBInfo(
			configLoader->getParameter("spikeStreamAnalysisHost"),
			configLoader->getParameter("spikeStreamAnalysisUser"),
			configLoader->getParameter("spikeStreamAnalysisPassword"),
			"SpikeStreamAnalysis"
	);

	//Create analysis data access object
	AnalysisDao* analysisDao = new AnalysisDao(analysisDBInfo);
	Globals::setAnalysisDao(analysisDao);

	//Create Database manager
	databaseManager = new DatabaseManager(Globals::getNetworkDao()->getDBInfo(), Globals::getArchiveDao()->getDBInfo(), Globals::getAnalysisDao()->getDBInfo());
	connect(databaseManager, SIGNAL(finished()), this, SLOT(databaseManagerFinished()), Qt::UniqueConnection);
	progressDialog = NULL;

	//Get the default location for saving and loading databases
	Globals::setWorkingDirectory(configLoader->getParameter("default_file_location"));

	//Load display settings from configuration file
	Globals::getNetworkDisplay()->loadDisplaySettings(configLoader);

	//Actions
	//Add OpenGL actions
	QAction* moveUpAction = new QAction(this);
	moveUpAction->setShortcut(QKeySequence(Qt::Key_Up));
	connect(moveUpAction, SIGNAL(triggered()), Globals::getEventRouter(), SLOT(moveUpSlot()));
	this->addAction(moveUpAction);

	QAction* moveDownAction = new QAction(this);
	moveDownAction->setShortcut(QKeySequence(Qt::Key_Down));
	connect(moveDownAction, SIGNAL(triggered()), Globals::getEventRouter(), SLOT(moveDownSlot()));
	this->addAction(moveDownAction);

	QAction* moveLeftAction = new QAction(this);
	moveLeftAction->setShortcut(QKeySequence(Qt::Key_Left));
	connect(moveLeftAction, SIGNAL(triggered()), Globals::getEventRouter(), SLOT(moveLeftSlot()));
	this->addAction(moveLeftAction);

	QAction* moveRightAction = new QAction(this);
	moveRightAction->setShortcut(QKeySequence(Qt::Key_Right));
	connect(moveRightAction, SIGNAL(triggered()), Globals::getEventRouter(), SLOT(moveRightSlot()));
	this->addAction(moveRightAction);

	QAction* moveForwardAction = new QAction(this);
	moveForwardAction->setShortcut(QKeySequence(Qt::Key_PageUp));
	connect(moveForwardAction, SIGNAL(triggered()), Globals::getEventRouter(), SLOT(moveForwardSlot()));
	this->addAction(moveForwardAction);

	QAction* moveBackwardAction = new QAction(this);
	moveBackwardAction->setShortcut(QKeySequence(Qt::Key_PageDown));
	connect(moveBackwardAction, SIGNAL(triggered()), Globals::getEventRouter(), SLOT(moveBackwardSlot()));
	this->addAction(moveBackwardAction);

	QAction* resetViewAction = new QAction(this);
	resetViewAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
	connect(resetViewAction, SIGNAL(triggered()), Globals::getEventRouter(), SLOT(resetViewSlot()));
	this->addAction(resetViewAction);

	QAction* rotateUpAction = new QAction(this);
	rotateUpAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Up));
	connect(rotateUpAction, SIGNAL(triggered()), Globals::getEventRouter(), SLOT(rotateUpSlot()));
	this->addAction(rotateUpAction);

	QAction* rotateDownAction = new QAction(this);
	rotateDownAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Down));
	connect(rotateDownAction, SIGNAL(triggered()), Globals::getEventRouter(), SLOT(rotateDownSlot()));
	this->addAction(rotateDownAction);

	QAction* rotateLeftAction = new QAction(this);
	rotateLeftAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Left));
	connect(rotateLeftAction, SIGNAL(triggered()), Globals::getEventRouter(), SLOT(rotateLeftSlot()));
	this->addAction(rotateLeftAction);

	QAction* rotateRightAction = new QAction(this);
	rotateRightAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Right));
	connect(rotateRightAction, SIGNAL(triggered()), Globals::getEventRouter(), SLOT(rotateRightSlot()));
	this->addAction(rotateRightAction);

	//Set up menus.
	//Add file menu.
	QMenu * fileMenu = new QMenu("File", this);
	menuBar()->addMenu(fileMenu);
	fileMenu->addAction("Clear databases", this, SLOT(clearDatabases()));
	fileMenu->addSeparator();
	fileMenu->addAction("Import NRM network", this, SLOT(importNRMNetwork()), Qt::CTRL+Qt::Key_M);
	fileMenu->addSeparator();
	fileMenu->addAction("Quit", qApp, SLOT(closeAllWindows()), Qt::CTRL+Qt::Key_Q);

	//Add refresh menu for when changes have been made in the database independently of the application
	QMenu *viewMenu = new QMenu("View", this);
	menuBar()->addMenu(viewMenu);

	//Reload everything is broadcast to all classes connected to the event router
	viewMenu->addAction("Reload everything", Globals::getEventRouter(), SLOT(reloadSlot()), Qt::CTRL + Qt::Key_F5);

	//Add help menu
	QMenu *helpMenu = new QMenu("Help", this);
	menuBar()->addMenu(helpMenu);
	helpMenu->addAction("About", this , SLOT(about()), Qt::Key_F12);

	//Set up panes
	//Set up main splitter, which will divide graphical view from editor/viewer/simulator windows
	QSplitter *mainSplitterWidget = new QSplitter(this);
	tabWidget = new QTabWidget(mainSplitterWidget);

	//Add networks tab
	QScrollArea* networksScrollArea = new QScrollArea();
	NetworksWidget* networksWidget = new NetworksWidget(networksScrollArea);
	networksScrollArea->setWidget(networksWidget);
	tabWidget->addTab(networksScrollArea, "Networks");

	//Set up editor window
	QSplitter *layerSplitterWidget = new QSplitter(tabWidget);
	NeuronGroupWidget* neuronGrpWidget = new NeuronGroupWidget();
	ConnectionWidget* conWidget = new ConnectionWidget();
	layerSplitterWidget->addWidget(neuronGrpWidget);
	layerSplitterWidget->addWidget(conWidget);
	layerSplitterWidget->setOrientation(Qt::Vertical);
	tabWidget->addTab(layerSplitterWidget, "Editor");

	//Create network viewer for right half of screen
	new NetworkViewer(mainSplitterWidget);

	//Set up viewer tab
	QScrollArea* networkViewPropsScrollArea = new QScrollArea(tabWidget);
	NetworkViewerProperties* networkViewerProperties = new NetworkViewerProperties(networkViewPropsScrollArea);
	networkViewPropsScrollArea->setWidget(networkViewerProperties);
	tabWidget->addTab(networkViewPropsScrollArea, "Viewer");

	//Set up simulation tab
	QScrollArea* simulationScrollArea = new QScrollArea(tabWidget);
	SimulationLoaderWidget* simulationWidget = new SimulationLoaderWidget();
	simulationScrollArea->setWidget(simulationWidget);
	tabWidget->addTab(simulationScrollArea, "Simulation");

	//Set up archive tab
	QScrollArea* archiveScrollArea = new QScrollArea(tabWidget);
	ArchiveWidget* archiveWidget = new ArchiveWidget();
	archiveScrollArea->setWidget(archiveWidget);
	tabWidget->addTab(archiveScrollArea, "Archives");

	//Set up analysis tab
	QScrollArea* analysisScrollArea = new QScrollArea(tabWidget);
	AnalysisLoaderWidget* analysisLoaderWidget = new AnalysisLoaderWidget();
	analysisScrollArea->setWidget(analysisLoaderWidget);
	tabWidget->addTab(analysisScrollArea, "Analysis");

	//Add all  the key combinations that will be required.
	QAction* showNetworkWidgetAction = new QAction(this);
	showNetworkWidgetAction->setShortcut(QKeySequence(Qt::Key_F1));
	connect(showNetworkWidgetAction, SIGNAL(triggered()), this, SLOT(showNetworkWidget()));
	this->addAction(showNetworkWidgetAction);

	QAction* showEditorWidgetAction = new QAction(this);
	showEditorWidgetAction->setShortcut(QKeySequence(Qt::Key_F2));
	connect(showEditorWidgetAction, SIGNAL(triggered()), this, SLOT(showEditorWidget()));
	this->addAction(showEditorWidgetAction);

	QAction* showViewerWidgetAction = new QAction(this);
	showViewerWidgetAction->setShortcut(QKeySequence(Qt::Key_F3));
	connect(showViewerWidgetAction, SIGNAL(triggered()), this, SLOT(showViewerWidget()));
	this->addAction(showViewerWidgetAction);

	QAction* showSimulationWidgetAction = new QAction(this);
	showSimulationWidgetAction->setShortcut(QKeySequence(Qt::Key_F4));
	connect(showSimulationWidgetAction, SIGNAL(triggered()), this, SLOT(showSimulationWidget()));
	this->addAction(showSimulationWidgetAction);

	QAction* showArchiveWidgetAction = new QAction(this);
	showArchiveWidgetAction->setShortcut(QKeySequence(Qt::Key_F5));
	connect(showArchiveWidgetAction, SIGNAL(triggered()), this, SLOT(showArchiveWidget()));
	this->addAction(showArchiveWidgetAction);

	QAction* showAnalysisWidgetAction = new QAction(this);
	showAnalysisWidgetAction->setShortcut(QKeySequence(Qt::Key_F6));
	connect(showAnalysisWidgetAction, SIGNAL(triggered()), this, SLOT(showAnalysisWidget()));
	this->addAction(showAnalysisWidgetAction);

	//Finish off
	QPixmap iconPixmap(Globals::getSpikeStreamRoot() + "/images/spikestream_icon_64.png" );
	setWindowIcon(iconPixmap);
	setCentralWidget( mainSplitterWidget );
	if( Util::getBool(configLoader->getParameter("maximize_gui") ))
		setWindowState(Qt::WindowMaximized);

	//Delete config loader
	delete configLoader;
}


