/***************************************************************************
 *   SpikeStream Application                                               *
 *   Copyright (C) 2007 by David Gamez                                     *
 *   david@davidgamez.eu                                                   *
 *   Version 0.1                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

//SpikeStream includes
#include "AnalysisLoaderWidget.h"
#include "SpikeStreamMainWindow.h"
#include "LogWriter.h"
#include "Debug.h"
#include "Utilities.h"
#include "Tester.h"
#include "GlobalVariables.h"
#include "DatabaseManager.h"
#include "PatternDialog.h"
#include "ProbeDialog.h"
#include "ConfigLoader.h"
#include "ConnectionMatrixLoader.h"
#include "Globals.h"
#include "NRMImportDialog.h"
#include "SpikeStreamException.h"
#include "NetworksWidget.h"
using namespace spikestream;

//Qt includes
#include <qsplitter.h>
#include <qsplashscreen.h>
#include <qpixmap.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <q3accel.h>
#include <qfile.h>
#include <q3scrollview.h>
#include <q3vbox.h>
#include <q3filedialog.h>
//Added by qt3to4:
#include <QCloseEvent>
#include <Q3PopupMenu>
#include <QtDebug>

//Other includes
#include <mysql++.h>
#include <string>
using namespace std;
using namespace mysqlpp;


//Declare static variables here for linking
QString SpikeStreamMainWindow::workingDirectory = "";
SpikeStreamMainWindow* SpikeStreamMainWindow::spikeStrMainWin;
SpikeStreamApplication* SpikeStreamMainWindow::spikeStreamApplication;


/*! Constructor. */
SpikeStreamMainWindow::SpikeStreamMainWindow(SpikeStreamApplication *ssApp) : Q3MainWindow( 0, "SpikeStream", Qt::WDestructiveClose ){
    //Store a static reference to itself so that it can be accessed from anywhere
	spikeStrMainWin = this;

	//Store reference to spike stream application
	spikeStreamApplication = ssApp;

        //FIXME: CARRY OUT HOUSEKEEPING TASKS ON DATABASE, SUCH AS SORTING OUT CRASHED SIMULATIONS ETC.

	//Get the working directory, which should be defined by the SPIKESTREAM_ROOT variable
	workingDirectory = getenv("SPIKESTREAM_ROOT");

	//Working directory is essential, so exit if it is not defined.
	if( workingDirectory == ""){
		cerr<<"CANNOT FIND SPIKESTREAM_ROOT VARIABLE. THIS MUST BE DEFINED FOR APPLICATION TO WORK, EXITING"<<endl;
		QMessageBox::critical( 0, "SpikeStream", "SPIKESTREAM_ROOT variable is not defined.\nApplication will now exit.");
		delete this;
		exit(1);
	}
	else if( !QFile::exists(workingDirectory) ){
                cerr<<"SPIKESTREAM_ROOT VARIABLE POINTS TO AN INVALID DIRECTORY: "<<workingDirectory.toStdString()<<endl;
		QMessageBox::critical( 0, "SpikeStream", "SPIKESTREAM_ROOT variable does not point to a valid directory.\nApplication will now exit.");
		delete this;
		exit(1);
	}
	else {
		#ifdef PATH_DEBUG
			cout<<"Working directory: "<<workingDirectory<<endl;
		#endif//PATH_DEBUG
	}
        //Store root directory in global scope
	Globals::setSpikeStreamRoot(workingDirectory);

	//Set up splash screen to give feedback whilst application is loading
	QSplashScreen *splashScreen = 0;
	QPixmap splashPixmap(workingDirectory + "/images/spikestream_splash.png" );
	splashScreen = new QSplashScreen( splashPixmap );
	splashScreen->show();


	//Set up config
	ConfigLoader* configLoader;
	if(QFile::exists(workingDirectory +  "/spikestream.config")){
                configLoader = new ConfigLoader(workingDirectory.toStdString() + "/spikestream.config");
	}
	else{
                cerr<<"Cannot find configuration file at "<<workingDirectory.toStdString()<<"/spikestream.config"<<endl;
		QMessageBox::critical( 0, "Config Error", "Cannot find configuration file\nApplication will now exit.");
		exit(1);	
	}
	

        //Set up the logging - FIXME USE PROPER LOGGER
	//Load the log level from the configuration file: 0 disables logging, 1 enables medium logging, 2 enables full logging
	int logLev = 0;
	try{
		logLev = Utilities::getInt(configLoader->getStringData("log_level"));
	}
	catch(std::exception& er){// Catch-all for std exceptions
		cerr<<"SpikeStreamMainWindow: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown getting log level: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Config Error", errorString);
	}

	if(logLev == 0)
		LogWriter::disableLogging();
	else{//Logging is enabled
		//Set the log level
		if(logLev == 1 || logLev == 2)
			LogWriter::setLogLevel(logLev);
		else{
			cerr<<"SpikeStreamMainWindow: LOG LEVEL SHOULD BE 0, 1 OR 2"<<endl;
			LogWriter::setLogLevel(1);
		}
		
		//Find the log file
		if(QFile::exists(workingDirectory + "/SpikeStream.log")){
			QString logPath = workingDirectory + "/SpikeStream.log";
			LogWriter::setLogPath(logPath.ascii());
		}
		else{
			cerr<<"Cannot find log file. Creating new file."<<endl;
			LogWriter::setLogPath(workingDirectory + "/SpikeStream.log");
		}
		
		//Check to see if it has reached its maximum size defined in config file. 
		//Disable logging if it is too big
		unsigned int maxLogSize = 100000;
		try{
			maxLogSize = Utilities::getUInt(configLoader->getStringData("Max_log_size"));
		}
		catch(std::exception& er){// Catch-all for std exceptions
			cerr<<"SpikeStreamMainWindow: STD EXCEPTION \""<<er.what()<<"\""<<endl;
			QString errorString = "Exception thrown geting max log size: \"";
			errorString += er.what();
			errorString += "\"";
			QMessageBox::critical( 0, "Config Error", errorString);
		}

		QFile logFile(workingDirectory + "/SpikeStream.log");
		if(logFile.size() > maxLogSize){
			cerr<<"Log file is full. Logging disabled. [ Max_log_size: "<<maxLogSize<<"; Current log size: "<<logFile.size()<<" ]"<<endl;
			LogWriter::disableLogging();	
		}
		
		//Write the date and time to the log.
		LogWriter::writeDate();
	}
	
	//Set up the Network database interface and connect to it
	//This will be used by many other classes, so keep reference to it
	networkDBInterface = new DBInterface(
		configLoader->getCharData("neuralNetworkHost"), 
		configLoader->getCharData("neuralNetworkUser"), 
		configLoader->getCharData("neuralNetworkPassword"), 
		configLoader->getCharData("neuralNetworkDatabase")
	);
	if(!networkDBInterface->connectToDatabase(true)){//Exceptions enabled
		QMessageBox::critical (this, "Initialisation failure", "Connection to NeuralNetwork database failed");
		exit(1);
	}

	//Set up the archive database interface and connect to it
	archiveDBInterface = new DBInterface(
		configLoader->getCharData("archiveHost"), 
		configLoader->getCharData("archiveUser"), 
		configLoader->getCharData("archivePassword"), 
		configLoader->getCharData("archiveDatabase")
	);
	if(!archiveDBInterface->connectToDatabase(true)){//Exceptions enabled
		QMessageBox::critical (this, "Initialisation failure", "Connection to NeuralArchive database failed");
		exit(1);
	}

	//Set up the pattern database interface and connect to it
	patternDBInterface = new DBInterface(
		configLoader->getCharData("patternHost"), 
		configLoader->getCharData("patternUser"), 
		configLoader->getCharData("patternPassword"), 
		configLoader->getCharData("patternDatabase")
	);
	if(!patternDBInterface->connectToDatabase(true)){//Exceptions enabled
		QMessageBox::critical (this, "Initialisation failure", "Connection to Patterns database failed");
		exit(1);
	}

	//Set up the device database interface and connect to it
	deviceDBInterface = new DBInterface(
		configLoader->getCharData("deviceHost"), 
		configLoader->getCharData("deviceUser"), 
		configLoader->getCharData("devicePassword"), 
		configLoader->getCharData("deviceDatabase")
	);
	if(!deviceDBInterface->connectToDatabase(true)){//Exceptions enabled
		QMessageBox::critical (this, "Initialisation failure", "Connection to Devices database failed");
		exit(1);
	}

	//Set up the data access objects wrapping the databases.
	DBInfo netDBInfo(
		configLoader->getCharData("spikeStreamNetworkHost"),
		configLoader->getCharData("spikeStreamNetworkUser"),
		configLoader->getCharData("spikeStreamNetworkPassword"),
		configLoader->getCharData("spikeStreamNetworkDatabase")
	);
	try{
	    NetworkDao* networkDao = new NetworkDao(netDBInfo);
	    Globals::setNetworkDao(networkDao);
	}
	catch(SpikeStreamException& ex){
	    qCritical()<<ex.getMessage();
	}

	//Get the default location for saving and loading databases
	defaultFileLocation = configLoader->getCharData("default_file_location");
	Globals::setWorkingDirectory(defaultFileLocation);

	//Set up the global event router and connect appropriate signals to this class
	connect(Globals::getEventRouter(), SIGNAL(reloadSignal()), this, SLOT(reloadEverything()), Qt::QueuedConnection);

	//Set up tester to run any code that needs testing
	//Tester *tester = new Tester();
	//delete tester;


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

/*	QAction* rotateLeftAction = new QAction(this);
	rotateLeftAction->setShortcuts(QKeySequence::Open);
     connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

		keyboardAccelerator->insertItem(Qt::Key_Up);
	keyboardAccelerator->insertItem(Qt::Key_Down);
	keyboardAccelerator->insertItem(Qt::Key_Left);
	keyboardAccelerator->insertItem(Qt::Key_Right);
	keyboardAccelerator->insertItem(Qt::CTRL + Qt::Key_Up);
	keyboardAccelerator->insertItem(Qt::CTRL + Qt::Key_Down);
	keyboardAccelerator->insertItem(Qt::CTRL + Qt::Key_Left);
	keyboardAccelerator->insertItem(Qt::CTRL + Qt::Key_Right);
	keyboardAccelerator->insertItem(Qt::SHIFT + Qt::Key_Up);
	keyboardAccelerator->insertItem(Qt::SHIFT + Qt::Key_Down);
	keyboardAccelerator->insertItem(Qt::SHIFT + Qt::Key_Left);
	keyboardAccelerator->insertItem(Qt::SHIFT + Qt::Key_Right);
	keyboardAccelerator->insertItem(Qt::ALT + Qt::Key_Up);
	keyboardAccelerator->insertItem(Qt::ALT + Qt::Key_Down);
	keyboardAccelerator->insertItem(Qt::ALT + Qt::Key_Left);
	keyboardAccelerator->insertItem(Qt::ALT + Qt::Key_Right);
	keyboardAccelerator->insertItem(Qt::CTRL + Qt::ALT + Qt::Key_Up);
	keyboardAccelerator->insertItem(Qt::CTRL + Qt::ALT + Qt::Key_Down);
	keyboardAccelerator->insertItem(Qt::CTRL + Qt::ALT + Qt::Key_Left);
	keyboardAccelerator->insertItem(Qt::CTRL + Qt::ALT + Qt::Key_Right);
	keyboardAccelerator->insertItem(Qt::CTRL + Qt::Key_R);
	keyboardAccelerator->insertItem(Qt::CTRL + Qt::Key_Equal);
	keyboardAccelerator->insertItem(Qt::CTRL + Qt::Key_Minus);



	*/







	//Set up menus. 
	//Add file menu.
	Q3PopupMenu * fileMenu = new Q3PopupMenu(this);
	menuBar()->insertItem("File", fileMenu);
	fileMenu->insertItem("Clear databases", this, SLOT(clearDatabases()), Qt::CTRL+Qt::Key_R);
	fileMenu->insertItem("Load databases", this, SLOT(loadDatabases()), Qt::CTRL+Qt::Key_L);
	fileMenu->insertItem("Save databases", this, SLOT(saveDatabases()), Qt::CTRL+Qt::Key_S);
	fileMenu->insertSeparator();
	fileMenu->insertItem("Import connection matrix", this, SLOT(importConnectionMatrix()), Qt::CTRL+Qt::Key_I);
	fileMenu->insertItem("Import NRM network", this, SLOT(importNRMNetwork()), Qt::CTRL+Qt::Key_M);
	fileMenu->insertSeparator();
	fileMenu->insertItem("Quit", qApp, SLOT(closeAllWindows()), Qt::CTRL+Qt::Key_Q);

	//Add refresh menu for when changes have been made in the database independently of the application
	Q3PopupMenu *viewMenu = new Q3PopupMenu(this);
	menuBar()->insertItem("View", viewMenu);
	viewMenu->insertItem("Reload devices", this, SLOT(reloadDevices()), Qt::CTRL+Qt::Key_D);
	viewMenu->insertItem("Reload patterns", this, SLOT(reloadPatterns()), Qt::CTRL+Qt::Key_P);

	//Reload everything is broadcast to all classes connected to the event router
	viewMenu->insertItem("Reload everything", Globals::getEventRouter(), SLOT(reloadSlot()), Qt::SHIFT+Qt::Key_F5);

	//Add tools menu for pattern manager and probe
	Q3PopupMenu *toolsMenu = new Q3PopupMenu(this);
	menuBar()->insertItem("Tools", toolsMenu);
	toolsMenu->insertItem("Pattern manager", this, SLOT(managePatterns()), Qt::Key_F6);
	toolsMenu->insertItem("Probe manager", this, SLOT(manageProbes()), Qt::Key_F7);

	//Add help menu
	Q3PopupMenu *helpMenu = new Q3PopupMenu(this);
	menuBar()->insertItem("Help", helpMenu);
	helpMenu->insertItem("About", this , SLOT(about()), Qt::Key_F12);
	
	
	//Set up panes
	//Set up main splitter, which will divide graphical view from editor/viewer/simulator windows
	QSplitter *mainSplitterWidget = new QSplitter(this);
	tabWidget = new QTabWidget(mainSplitterWidget);

	//Add networks tab and connect it to appropriate signals
	NetworksWidget* networksWidget = new NetworksWidget(this);
	tabWidget->addTab(networksWidget, "Networks");
	
	//Set up editor window and add to tab widget
	QSplitter *layerSplitterWidget = new QSplitter( tabWidget);
	layerWidget = new LayerWidget(layerSplitterWidget, networkDBInterface, deviceDBInterface);
	connectionWidget = new ConnectionWidget(layerSplitterWidget, networkDBInterface, deviceDBInterface);
	layerSplitterWidget->setOrientation(Qt::Vertical);
        tabWidget->addTab(layerSplitterWidget, "Editor");
	

	//Create network viewer for right half of screen
	unsigned int maxAutoLoadConnGrpSize = 1000000;
	try{
		maxAutoLoadConnGrpSize = Utilities::getUInt(configLoader->getStringData("max_autoload_conngrp_size"));
	}
	catch(std::exception& er){// Catch-all for std exceptions
		cerr<<"SpikeStreamMainWindow: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown getting maxAutoLoadConnGrpSize: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Config Error", errorString);
	}

	//networkViewer = new NetworkViewer(mainSplitterWidget, splashScreen, networkDBInterface, maxAutoLoadConnGrpSize);
	networkViewer2 = new NetworkViewer_V2(mainSplitterWidget);
	networkViewer2->setMinimumSize(200, 60);
	networkViewer2->setBaseSize(700, 60);


	//Set up viewer tab
	Q3ScrollView* nwViewerPropScrollView = new Q3ScrollView(this);
	Q3VBox* nwViewerPropViewBox = new Q3VBox(nwViewerPropScrollView->viewport());
	nwViewerPropViewBox->setMinimumSize(1000,800);
	nwViewerPropScrollView->addChild(nwViewerPropViewBox);
	networkViewerProperties = new NetworkViewerProperties(nwViewerPropViewBox, networkViewer2, networkDBInterface);
	networkViewerProperties->setMinimumSize(800, 600);
        tabWidget->addTab(nwViewerPropScrollView, "Viewer");


	//Set up simulator tab
	Q3ScrollView* simulationScrollView = new Q3ScrollView(this);
	Q3VBox* simulationViewBox = new Q3VBox(simulationScrollView->viewport());
	simulationViewBox->setMinimumSize(1000,800);
	simulationScrollView->addChild(simulationViewBox);
	QSplitter *simulationSplitterWidget = new QSplitter(simulationViewBox);
	simulationSplitterWidget->setOrientation(Qt::Vertical);
	simulationWidget = new SimulationWidget(simulationSplitterWidget, networkDBInterface, archiveDBInterface, patternDBInterface, deviceDBInterface);
	simulationManager = simulationWidget->getSimulationManager();

	//Set up the simulation manager with the maximum load time
	int maxLoadTime_sec = 200;
	try{
		maxLoadTime_sec = Utilities::getInt(configLoader->getStringData("max_neuron_data_load_time"));
	}
	catch(std::exception& er){// Catch-all for std exceptions
		cerr<<"SpikeStreamMainWindow: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown getting max neuron data load time: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Config Error", errorString);
	}
	simulationManager->setMaxNeurDataLoadTime_sec(maxLoadTime_sec);

	//Set up monitor area at bottom of simulation tab
	MonitorArea *simulationMonitorArea = new MonitorArea(networkDBInterface, simulationManager, simulationSplitterWidget);
	simulationWidget->setMonitorArea(simulationMonitorArea);
        tabWidget->addTab(simulationScrollView, "Simulation");


	//Set up archive tab
	Q3ScrollView* archiveScrollView = new Q3ScrollView(this);
	Q3VBox* archiveViewBox = new Q3VBox(archiveScrollView->viewport());
	archiveViewBox->setMinimumSize(1000, 800);
	archiveScrollView->addChild(archiveViewBox);
	QSplitter *archiveSplitterWidget = new QSplitter(archiveViewBox);
	archiveSplitterWidget->setOrientation(Qt::Vertical);
	archiveWidget = new ArchiveWidget(archiveSplitterWidget, archiveDBInterface, archiveManager);
	archiveManager = archiveWidget->getArchiveManager();

	MonitorArea *archiveMonitorArea = new MonitorArea(archiveManager, archiveSplitterWidget);
	archiveMonitorArea->setMinimumSize(60, 200);
	archiveWidget->setMonitorArea(archiveMonitorArea);
        tabWidget->addTab(archiveScrollView, "Archive");

        //Set up analysis tab
       // AnalysisLoaderWidget* analysisLoaderWidget = new AnalysisLoaderWidget(this);
	//tabWidget->addTab(analysisLoaderWidget, "Analysis");

	//Set up layer widget connections
	layerWidget->setConnectionWidget(connectionWidget);
	
	//Set up network viewer references
	//networkViewer2->setNetworkViewerProperties((QWidget*)networkViewerProperties);

	//Set up an accelerator to switch between the tabs
	keyboardAccelerator = new Q3Accel( this );
	
	//Add all  the key combinations that will be required.
	keyboardAccelerator->insertItem(Qt::Key_F1);
	keyboardAccelerator->insertItem(Qt::Key_F2);
	keyboardAccelerator->insertItem(Qt::Key_F3);
	keyboardAccelerator->insertItem(Qt::Key_F4);

	 //Connect up accelerator with the method that will process the key events
	 connect (keyboardAccelerator, SIGNAL(activated(int)), this, SLOT(acceleratorKeyPressed(int)));

	//Finish off
	QPixmap iconPixmap(workingDirectory + "/images/spikestream_icon_64.png" );
	setIcon(iconPixmap);
        setCentralWidget( mainSplitterWidget );
        //setWindowState(Qt::WindowMaximized);

	//Get rid of splash screen if it is showing
	if(splashScreen){
		splashScreen->finish( this );
		delete splashScreen;
	}
	
	//Delete config loader
	delete configLoader;
}


/*! Destructor. */
SpikeStreamMainWindow::~SpikeStreamMainWindow(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING SPIKE STREAM MAIN WINDOW"<<endl;
	#endif//MEMORY_DEBUG
	
	//First need to stop the simulation if it is running
	if(simulationManager->isInitialised()){
		simulationManager->destroySimulation();
		cout<<"Waiting for simulation to finish"<<endl;
		bool threadFinished = simulationManager->wait(10000);
		if(!threadFinished)
			cerr<<"Simulation manager cannot be shut down cleanly within 10 seconds. Killing thread"<<endl;
	}
	
	//Need to stop archive manager if it is running
	if(archiveManager->isRunning()){
		archiveManager->stopArchive();
		cout<<"Waiting for archive manager to finish"<<endl;
		bool threadFinished = archiveManager->wait(10000);
		if(!threadFinished)
			cerr<<"Archive manager cannot be shut down cleanly within 10 seconds. Killing thread"<<endl;
	}

	//Delete all the database handling classes
	delete networkDBInterface;
	delete archiveDBInterface;
	delete patternDBInterface;
	delete deviceDBInterface;

	//Clean up globals - everything stored in globals is cleaned up by globals
	Globals::cleanUp();
}


//--------------------------------------------------------------------------
//------------------------------ PUBLIC METHODS ----------------------------
//--------------------------------------------------------------------------

/*! Returns a reference to the network viewer. */
NetworkViewer_V2* SpikeStreamMainWindow::getNetworkViewer(){
	return networkViewer2;
}


/*! Reloads just the details about the connections assuming that the connection
	groups have remained the same. */
void SpikeStreamMainWindow::reloadConnectionDetails(){
	networkViewerProperties->reloadConnectionDetails();
}



/*! Reloads connections after a class has made changes to the database. */
void SpikeStreamMainWindow::reloadConnections(){
	networkViewerProperties->reloadConnections();
	simulationWidget->reloadConnectionGroups();
}


/*! Reloads neurons after a change has been made to the database. */
void SpikeStreamMainWindow::reloadNeuronGroups(){
	networkViewerProperties->reloadNeuronGroups();
	simulationWidget->reloadNeuronGroups();
	connectionWidget->reloadConnections();
}


/*! When viewing individual neurons this method sets the from
	neuron number in the Simulation Widget, which can be used to
	fire a neuron or select a neuron for monitoring during a 
	simulation. */
void SpikeStreamMainWindow::setFromNeuronID(unsigned int neurGrpID, unsigned int fromNeurNum){
	simulationWidget->setFromNeuronID(neurGrpID, fromNeurNum);
}


/*! When viewing individual neurons this method sets the to
	neuron number in the Simulation Widget, which can be used to
	select a synapse for monitoring during a simulation. */
void SpikeStreamMainWindow::setToNeuronID(unsigned int toNeurNum){
	simulationWidget->setToNeuronID(toNeurNum);
}


/*! When simulation is destroyed this passes this information
	on to other widgets. */
void SpikeStreamMainWindow::simulationDestroyed(){
	connectionWidget->simulationDestroyed();
	layerWidget->simulationDestroyed();
}


/*! When simulation is initialised this passes this information
	on to other widgets. */
void SpikeStreamMainWindow::simulationInitialised(){
	connectionWidget->simulationInitialised();
	layerWidget->simulationInitialised();
}


//-------------------------------------------------------------------------
//----------------------------------- SLOTS -------------------------------
//-------------------------------------------------------------------------

/*! Displays brief information about the application. */
void SpikeStreamMainWindow::about(){
    QMessageBox::about( this, "About", "SpikeStream\nVersion 0.1 beta\nCreated by David Gamez: david@davidgamez.eu");
}

void SpikeStreamMainWindow::acceleratorKeyPressed(int acceleratorID){
	//Get the key sequence
	int keySequence = keyboardAccelerator->key(acceleratorID);
	
	//Change to a different tab
	switch(keySequence){
                case (Qt::Key_F1)://Edit tab
			simulationWidget->hideOpenWindows();
			archiveWidget->hideOpenWindows();
			tabWidget->setCurrentPage(0);
		break;
                case (Qt::Key_F2)://Network viewer properties tab
			simulationWidget->hideOpenWindows();
			archiveWidget->hideOpenWindows();
			tabWidget->setCurrentPage(1);
		break;
                case (Qt::Key_F3)://Simulation tab
			simulationWidget->showOpenWindows();
			archiveWidget->hideOpenWindows();
			tabWidget->setCurrentPage(2);
		break;
                case(Qt::Key_F4)://Archive tab
			simulationWidget->hideOpenWindows();
			archiveWidget->showOpenWindows();
			tabWidget->setCurrentPage(3);
		break;
		default:
			cerr<<"SpikeStreamMainWindow: KEY SEQUENCE NOT RECOGNIZED: "<<keySequence<<endl;
	}
}


/*! Empties all databases except the Neuron, Synapse and Probe types. Useful when
	an error has led to out of date information in one of the tables. However all 
	data will be lost if this method is called. */
void SpikeStreamMainWindow::clearDatabases(){
	/* Check that we do not have a simulation initialised. */
	if(SimulationManager::isInitialised()){
		QMessageBox::warning(this, "Load Databases", "Databases cannot be loaded when a simulation is initialised.\nDestroy the simulation and try again.");
		return;
	}

	/* Check that playback of an archive is not currently in progress. */
	if(ArchiveManager::isRunning()){
		QMessageBox::warning(this, "Load Databases", "Databases cannot be loaded when an archive is being played back.\nStop archive playback and try again.");
		return;
	}


	//Check that user wants to overwrite current database
	int response = QMessageBox::warning(this, "Clear Databases", "This will overwrite all data in all databases except NeuronTypes, SynapseTypes and ProbeTypes.\nDo you want to continue?", QMessageBox::Yes, QMessageBox::No, 0);
	if(response != QMessageBox::Yes)
		return;

	//Unload archive if it is loaded. Database clearing will invalidate the query.
	archiveWidget->unloadArchive();

	//Execute queries to clear all databases
	try{

		//Clear NeuralNetwork database
		Query networkQuery = networkDBInterface->getQuery();

		//First do all the straightforward clearing
		networkQuery.reset();
		networkQuery<<"DELETE FROM Connections";
		networkQuery.execute();

		networkQuery.reset();
		networkQuery<<"DELETE FROM ConnectionGroups";
		networkQuery.execute();

		networkQuery.reset();
		networkQuery<<"DELETE FROM Neurons";
		networkQuery.execute();

		networkQuery.reset();
		networkQuery<<"DELETE FROM NeuronGroups";
		networkQuery.execute();

		networkQuery.reset();
		networkQuery<<"DELETE FROM Probes";
		networkQuery.execute();

		networkQuery.reset();
		networkQuery<<"DELETE FROM NoiseParameters";
		networkQuery.execute();

		networkQuery.reset();
		networkQuery<<"DELETE FROM GlobalParameters";
		networkQuery.execute();

		//Add single blank row to global parameters
		networkQuery.reset();
		networkQuery<<"INSERT INTO GlobalParameters () VALUES ()";
		networkQuery.execute();


		//Next clear the parameter tables. Need to find these by looking in the appropriate Types tables
		networkQuery.reset();
		networkQuery<<"SELECT ParameterTableName FROM NeuronTypes";
                StoreQueryResult neurTableNameRes = networkQuery.store();
                for(StoreQueryResult::iterator paramTableIter = neurTableNameRes.begin(); paramTableIter != neurTableNameRes.end(); ++paramTableIter){
			Row tableNameRow (*paramTableIter);
			networkQuery.reset();
			networkQuery<<"DELETE FROM "<<(std::string)tableNameRow["ParameterTableName"];
			networkQuery.execute();
		}

		networkQuery.reset();
		networkQuery<<"SELECT ParameterTableName FROM SynapseTypes";
                StoreQueryResult synapseTableNameRes = networkQuery.store();
                for(StoreQueryResult::iterator paramTableIter = synapseTableNameRes.begin(); paramTableIter != synapseTableNameRes.end(); ++paramTableIter){
			Row tableNameRow (*paramTableIter);
			networkQuery.reset();
			networkQuery<<"DELETE FROM "<<(std::string)tableNameRow["ParameterTableName"];
			networkQuery.execute();
		}

		networkQuery.reset();
		networkQuery<<"SELECT ParameterTableName FROM ProbeTypes";
                StoreQueryResult probeTableNameRes = networkQuery.store();
                for(StoreQueryResult::iterator paramTableIter = probeTableNameRes.begin(); paramTableIter != probeTableNameRes.end(); ++paramTableIter){
			Row tableNameRow (*paramTableIter);
			networkQuery.reset();
			networkQuery<<"DELETE FROM "<<(std::string)tableNameRow["ParameterTableName"];
			networkQuery.execute();
		}


		//Clear NeuralArchive database
		Query archiveQuery = archiveDBInterface->getQuery();

		archiveQuery.reset();
		archiveQuery<<"DELETE FROM NetworkData";
		archiveQuery.execute();

		archiveQuery.reset();
		archiveQuery<<"DELETE FROM NetworkModels";
		archiveQuery.execute();


		//Clear Patterns database
		Query patternsQuery = patternDBInterface->getQuery();

		patternsQuery.reset();
		patternsQuery<<"DELETE FROM PatternData";
		patternsQuery.execute();

		patternsQuery.reset();
		patternsQuery<<"DELETE FROM PatternDescriptions";
		patternsQuery.execute();


		//Clear Devices database
		Query devicesQuery = deviceDBInterface->getQuery();

		devicesQuery.reset();
		devicesQuery<<"DELETE FROM Devices";
		devicesQuery.execute();

		devicesQuery.reset();
		devicesQuery<<"DELETE FROM SIMNOSSpikeReceptors";
		devicesQuery.execute();

		devicesQuery.reset();
		devicesQuery<<"DELETE FROM SIMNOSComponents";
		devicesQuery.execute();

		devicesQuery.reset();
		devicesQuery<<"DELETE FROM SynchronizationDelay";
		devicesQuery.execute();

		//Refresh display
		reloadEverything();

	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"SpikeStreamMainWindow: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when clearing databases: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Clear Databases Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"SpikeStreamMainWindow: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown clearing databases: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Clear Databases Error", errorString);
	}
}


/*! Called when window is closed. 
	This is needed to invoke destructor of neuronapplication
	Could run a check on whether the user really wants to quit.
	However, this is not needed at present since everthing is stored in the database. */
void SpikeStreamMainWindow::closeEvent( QCloseEvent* ce ){
	//If simulation is initialised, check that user really wants to quit
	if(simulationManager->isInitialised()){
		int response = QMessageBox::warning(this, "Quit SpikeStream?", "Simulation is still initialised.\nDo you want to quit?", QMessageBox::Yes, QMessageBox::No, 0);
		if(response != QMessageBox::Yes){
			return;
		}
	}
	ce->accept();
}


/*! Loads a neuron group and associated connections from a file. 
	This should be a comma separated matrix of connection weights. */
void SpikeStreamMainWindow::importConnectionMatrix(){
	/* Check that we do not have a simulation initialised. */
	if(SimulationManager::isInitialised()){
		QMessageBox::warning(this, "Import Connection Matrix", "Matrix cannot be loaded when a simulation is initialised.\nDestroy the simulation and try again.");
		return;
	}

	/* Check that playback of an archive is not currently in progress. */
	if(ArchiveManager::isRunning()){
		QMessageBox::warning(this, "Import Connection Matrix", "Matrix cannot be loaded when an archive is being played back.\nStop archive playback and try again.");
		return;
	}

	//Warn user about limitations
	int response = QMessageBox::warning(this, "Import ConnectionMatrix", "This is a preliminary method that will try create a new layer at position (0,0,0) with the default neuron and synapse types\n Make sure there is enough space for the new layer.\nDo you want to continue?", QMessageBox::Yes, QMessageBox::No, 0);
	if(response != QMessageBox::Yes)
		return;

	//Get the file name
	QString fileName = Q3FileDialog::getOpenFileName(defaultFileLocation, "All files (*.*)", this, "Load matrix dialog", "Choose a file to load" );
        if(fileName.isNull())
		return;

	//Unload archive if it is loaded. Database loading will invalidate the query if it changes the archive database.
	archiveWidget->unloadArchive();

	//Load from the file
	#ifdef LOAD_MATRIX_DEBUG
		cout<<"Loading connection matrix from file: "<<fileName<<endl;
	#endif//LOAD_MATRIX_DEBUG

	ConnectionMatrixLoader* matrixLoader = new ConnectionMatrixLoader(networkDBInterface);
	matrixLoader->loadConnectionMatrix(fileName);
	
	//Clean up database manager
	delete matrixLoader;

	//Reload the graphics
	reloadEverything();
}


/*! Displays a dialog/wizard to import networks from NRM files into the SpikeStream database */
void SpikeStreamMainWindow::importNRMNetwork(){
    NRMImportDialog* nrmDialog = new NRMImportDialog(this);
    nrmDialog->exec();
    delete nrmDialog;
    	//PatternDialog* patternDialog = new PatternDialog(this, patternDBInterface);
	//patternDialog->exec();
	//delete patternDialog;
}


/*! Loads databases from a file selected by the user.
	Launches database manager, which allows user to select which database to load. */
void SpikeStreamMainWindow::loadDatabases(){
	/* Check that we do not have a simulation initialised. */
	if(SimulationManager::isInitialised()){
		QMessageBox::warning(this, "Load Databases", "Databases cannot be loaded when a simulation is initialised.\nDestroy the simulation and try again.");
		return;
	}

	/* Check that playback of an archive is not currently in progress. */
	if(ArchiveManager::isRunning()){
		QMessageBox::warning(this, "Load Databases", "Databases cannot be loaded when an archive is being played back.\nStop archive playback and try again.");
		return;
	}


	//Check that user wants to overwrite current database
	int response = QMessageBox::warning(this, "Load Databases", "This will overwrite all data in the databases that are loaded including Neuron, Synapse and Probe types.\nDo you want to continue?", QMessageBox::Yes, QMessageBox::No, 0);
	if(response != QMessageBox::Yes)
		return;

	//Get the file name
	QString fileName = Q3FileDialog::getOpenFileName(defaultFileLocation, "Zipped SQL (*.sql.tar.gz)", this, "load database dialog", "Choose a file to load" );
        if(fileName.isNull())
		return;

	//Unload archive if it is loaded. Database loading will invalidate the query if it changes the archive database.
	archiveWidget->unloadArchive();

	//Do some sorting out on the name. The user may have typed .sql.zip at the end
	if(!fileName.endsWith(NEURON_APPLICATION_FILE_EXTENSION))
		fileName += NEURON_APPLICATION_FILE_EXTENSION;

	//Load from the file
	#ifdef LOAD_DATABASES_DEBUG
		cout<<"Loading databases from file: "<<fileName<<endl;
	#endif//SAVE_DATABASES_DEBUG

	DatabaseManager* dbManager = new DatabaseManager(this, fileName, false, networkDBInterface, archiveDBInterface, patternDBInterface, deviceDBInterface);
	dbManager->exec();
	
	//Clean up database manager
	delete dbManager;

	//Reload the graphics
	reloadEverything();
}


/*! Launches the pattern manager, which is used to load and delete patterns from the database. */
void SpikeStreamMainWindow::managePatterns(){
	PatternDialog* patternDialog = new PatternDialog(this, patternDBInterface);
	patternDialog->exec();
	delete patternDialog;
}


/*! Launches a probe manager, which enables the user to add or remove probes. */
void SpikeStreamMainWindow::manageProbes(){
	ProbeDialog* probeDialog = new ProbeDialog(this, networkDBInterface);
	probeDialog->exec();
	delete probeDialog;
}


/*! Menu item triggers this, which reloads the device table in the simulation widget. */
void SpikeStreamMainWindow::reloadDevices(){
	simulationWidget->reloadDevices();

}


/*! Reloads neuron groups and connections from the database. */
void SpikeStreamMainWindow::reloadEverything(){
    //Emit a signal. NOTE: Better way of handling this and saves keeping endless references
    emit reload();

    //Manual reload of classes  not connected to the signal FIXME: REPLACE WITH SIGNAL MECHANISM
	try{
		layerWidget->reloadNeuronGroups();
		connectionWidget->reloadConnections();
		simulationWidget->reloadNeuronGroups();
		simulationWidget->reloadConnectionGroups();
		networkViewerProperties->reloadConnections();
		networkViewerProperties->reloadNeuronGroups();
		networkViewer2->reloadEverything();
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"SpikeStreamMainWindow: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query reloading everything: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Reload Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"SpikeStreamMainWindow: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown reloading everything: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Reload Error", errorString);
	}
	catch (std::exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"SpikeStreamMainWindow: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown reloading everything: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Reload Error", errorString);
	}
}


/*! Reloads patterns after they have been added or deleted. */
void SpikeStreamMainWindow::reloadPatterns(){
	simulationWidget->reloadPatterns();
}


/*! Saves databases 
	Allows user to choose file and then launches database manager so that user can choose
	which databases to save. */
void SpikeStreamMainWindow::saveDatabases(){
    QString fileName = Q3FileDialog::getSaveFileName(defaultFileLocation, "Zipped SQL (*.sql.tar.gz)", this, "save file dialog", "Choose a filename to save under" );
        if(fileName.isNull())
		return;

	//First do some sorting out on the name. The user may have typed .sql.gz at the end
	if(!fileName.endsWith(NEURON_APPLICATION_FILE_EXTENSION))
		fileName += NEURON_APPLICATION_FILE_EXTENSION;

	//See if the file already exists and check if user wants to save if it does
	if(QFile::exists(fileName)){
		int response = QMessageBox::question(this, "Overwrite File?", "This file already exists.\nDo you want to overwrite it?", "Yes", "No", QString::null, 1, 0);
		if(response == 1)
			return;
		else
			QFile::remove(fileName);
	}
		
	//Save the file
	#ifdef SAVE_DATABASES_DEBUG
		cout<<"Saving databases in file: "<<fileName<<endl;
	#endif//SAVE_DATABASES_DEBUG

	DatabaseManager* dbManager = new DatabaseManager(this, fileName, true, networkDBInterface, archiveDBInterface, patternDBInterface, deviceDBInterface);
	dbManager->exec();

	//Clean up dbManager
	delete dbManager;
}


