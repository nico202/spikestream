//SpikeStream includes
#include "AnalysisLoaderWidget.h"
#include "SpikeStreamMainWindow.h"
#include "Utilities.h"
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
#include "ArchiveWidget_V2.h"
#include "NeuronGroupWidget.h"
#include "ConnectionWidget_V2.h"
#include "NetworkViewer_V2.h"
#include "NetworkViewerProperties_V2.h"
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



#include <QDebug>
#include <QScrollArea>

//Other includes
#include <string>
using namespace std;


/*! Constructor. */
SpikeStreamMainWindow::SpikeStreamMainWindow() : QMainWindow( 0, "SpikeStream - Analysis", Qt::WDestructiveClose ){
    //Get the working directory, which should be defined by the SPIKESTREAM_ROOT variable
    workingDirectory = getenv("SPIKESTREAM_ROOT");

    //Working directory is essential, so exit if it is not defined.
    if( workingDirectory == ""){
	qCritical()<<"SPIKESTREAM_ROOT variable is not defined.\nApplication will now exit.";
	exit(1);
    }
    else if( !QFile::exists(workingDirectory) ){
	qCritical()<< "SPIKESTREAM_ROOT variable does not point to a valid directory.\nApplication will now exit.";
	exit(1);
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
	qCritical()<<"Cannot find configuration file\nApplication will now exit.";
	exit(1);
    }

    //Set up the data access objects wrapping the network database
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
	exit(1);
    }

    //Set up the data access objects wrapping the archive database.
    DBInfo archiveDBInfo(
	    configLoader->getCharData("spikeStreamArchiveHost"),
	    configLoader->getCharData("spikeStreamArchiveUser"),
	    configLoader->getCharData("spikeStreamArchivePassword"),
	    configLoader->getCharData("spikeStreamArchiveDatabase")
	    );
    try{
	ArchiveDao* archiveDao = new ArchiveDao(archiveDBInfo);
	Globals::setArchiveDao(archiveDao);
    }
    catch(SpikeStreamException& ex){
	qCritical()<<ex.getMessage();
	exit(1);
    }

    //Set up the data access object wrapping the analysis database.
    DBInfo analysisDBInfo(
	    configLoader->getCharData("spikeStreamAnalysisHost"),
	    configLoader->getCharData("spikeStreamAnalysisUser"),
	    configLoader->getCharData("spikeStreamAnalysisPassword"),
	    configLoader->getCharData("spikeStreamAnalysisDatabase")
	    );
    try{
	AnalysisDao* analysisDao = new AnalysisDao(analysisDBInfo);
	Globals::setAnalysisDao(analysisDao);
    }
    catch(SpikeStreamException& ex){
	qCritical()<<ex.getMessage();
	exit(1);
    }

    //Get the default location for saving and loading databases
    defaultFileLocation = configLoader->getCharData("default_file_location");
    Globals::setWorkingDirectory(defaultFileLocation);

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
    Q3PopupMenu * fileMenu = new Q3PopupMenu(this);
    menuBar()->insertItem("File", fileMenu);
    fileMenu->insertItem("Clear databases", this, SLOT(clearDatabases()));
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

    //Reload everything is broadcast to all classes connected to the event router
    viewMenu->insertItem("Reload everything", Globals::getEventRouter(), SLOT(reloadSlot()), Qt::SHIFT+Qt::Key_F5);

    //Add help menu
    Q3PopupMenu *helpMenu = new Q3PopupMenu(this);
    menuBar()->insertItem("Help", helpMenu);
    helpMenu->insertItem("About", this , SLOT(about()), Qt::Key_F12);


    //Set up panes
    //Set up main splitter, which will divide graphical view from editor/viewer/simulator windows
    QSplitter *mainSplitterWidget = new QSplitter(this);
    tabWidget = new QTabWidget(mainSplitterWidget);

    //Add networks tab
    NetworksWidget* networksWidget = new NetworksWidget(this);
    QScrollArea* networksScrollArea = new QScrollArea(this);
    networksScrollArea->setWidget(networksWidget);
    tabWidget->addTab(networksScrollArea, "Networks");

    //Set up editor window
    QSplitter *layerSplitterWidget = new QSplitter( tabWidget);
    new NeuronGroupWidget(layerSplitterWidget);
    new ConnectionWidget_V2(layerSplitterWidget);
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
    NetworkViewer_V2* networkViewer2 = new NetworkViewer_V2(mainSplitterWidget);

    //Set up viewer tab
    Q3ScrollView* nwViewerPropScrollView = new Q3ScrollView(this);
    Q3VBox* nwViewerPropViewBox = new Q3VBox(nwViewerPropScrollView->viewport());
    nwViewerPropViewBox->setMinimumSize(1000,800);
    nwViewerPropScrollView->addChild(nwViewerPropViewBox);
    new NetworkViewerProperties_V2(nwViewerPropViewBox);
    tabWidget->addTab(nwViewerPropScrollView, "Viewer");

    //Set up archive tab
    ArchiveWidget_V2* archiveWidget = new ArchiveWidget_V2(this);
    QScrollArea* archiveScrollArea = new QScrollArea(this);
    archiveScrollArea->setWidget(archiveWidget);
    tabWidget->addTab(archiveScrollArea, "Archives");

    //Set up analysis tab
    AnalysisLoaderWidget* analysisLoaderWidget = new AnalysisLoaderWidget(this);
    QScrollArea* analysisScrollArea = new QScrollArea(this);
    analysisScrollArea->setWidget(analysisLoaderWidget);

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
    //	if(simulationManager->isInitialised()){
    //		simulationManager->destroySimulation();
    //		cout<<"Waiting for simulation to finish"<<endl;
    //		bool threadFinished = simulationManager->wait(10000);
    //		if(!threadFinished)
    //			cerr<<"Simulation manager cannot be shut down cleanly within 10 seconds. Killing thread"<<endl;
    //	}

    //Need to stop archive manager if it is running
    //	if(archiveManager->isRunning()){
    //		archiveManager->stopArchive();
    //		cout<<"Waiting for archive manager to finish"<<endl;
    //		bool threadFinished = archiveManager->wait(10000);
    //		if(!threadFinished)
    //			cerr<<"Archive manager cannot be shut down cleanly within 10 seconds. Killing thread"<<endl;
    //	}

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

///*! Returns a reference to the network viewer. */
//NetworkViewer_V2* SpikeStreamMainWindow::getNetworkViewer(){
//	return networkViewer2;
//}


/*! Reloads just the details about the connections assuming that the connection
	groups have remained the same. */
void SpikeStreamMainWindow::reloadConnectionDetails(){
    //networkViewerProperties->reloadConnectionDetails();
}



/*! Reloads connections after a class has made changes to the database. */
void SpikeStreamMainWindow::reloadConnections(){
    //networkViewerProperties->reloadConnections();
    //simulationWidget->reloadConnectionGroups();
}


/*! Reloads neurons after a change has been made to the database. */
void SpikeStreamMainWindow::reloadNeuronGroups(){
    //networkViewerProperties->reloadNeuronGroups();
    //simulationWidget->reloadNeuronGroups();
    //connectionWidget->reloadConnections();
}


/*! When viewing individual neurons this method sets the from
	neuron number in the Simulation Widget, which can be used to
	fire a neuron or select a neuron for monitoring during a
	simulation. */
void SpikeStreamMainWindow::setFromNeuronID(unsigned int neurGrpID, unsigned int fromNeurNum){
    //simulationWidget->setFromNeuronID(neurGrpID, fromNeurNum);
}


/*! When viewing individual neurons this method sets the to
	neuron number in the Simulation Widget, which can be used to
	select a synapse for monitoring during a simulation. */
void SpikeStreamMainWindow::setToNeuronID(unsigned int toNeurNum){
    //simulationWidget->setToNeuronID(toNeurNum);
}


/*! When simulation is destroyed this passes this information
	on to other widgets. */
void SpikeStreamMainWindow::simulationDestroyed(){
    //connectionWidget->simulationDestroyed();
    //layerWidget->simulationDestroyed();
}


/*! When simulation is initialised this passes this information
	on to other widgets. */
void SpikeStreamMainWindow::simulationInitialised(){
    //connectionWidget->simulationInitialised();
    //layerWidget->simulationInitialised();
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
		    //simulationWidget->hideOpenWindows();
		    //archiveWidget->hideOpenWindows();
		    tabWidget->setCurrentPage(0);
		    break;
		case (Qt::Key_F2)://Network viewer properties tab
		    //simulationWidget->hideOpenWindows();
		    //archiveWidget->hideOpenWindows();
		    tabWidget->setCurrentPage(1);
		    break;
		case (Qt::Key_F3)://Simulation tab
		    //simulationWidget->showOpenWindows();
		    //archiveWidget->hideOpenWindows();
		    tabWidget->setCurrentPage(2);
		    break;
		case(Qt::Key_F4)://Archive tab
		    //simulationWidget->hideOpenWindows();
		    //archiveWidget->showOpenWindows();
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
    //	if(SimulationManager::isInitialised()){
    //		QMessageBox::warning(this, "Load Databases", "Databases cannot be loaded when a simulation is initialised.\nDestroy the simulation and try again.");
    //		return;
    //	}

    /* Check that playback of an archive is not currently in progress. */
    //	if(ArchiveManager::isRunning()){
    //		QMessageBox::warning(this, "Load Databases", "Databases cannot be loaded when an archive is being played back.\nStop archive playback and try again.");
    //		return;
    //	}


    //Check that user wants to overwrite current database
    int response = QMessageBox::warning(this, "Clear Databases", "This will overwrite all data in all databases except NeuronTypes, SynapseTypes and ProbeTypes.\nDo you want to continue?", QMessageBox::Yes, QMessageBox::No, 0);
    if(response != QMessageBox::Yes)
	return;

    //Unload archive if it is loaded. Database clearing will invalidate the query.
    //	archiveWidget->unloadArchive();

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
    //	if(simulationManager->isInitialised()){
    //		int response = QMessageBox::warning(this, "Quit SpikeStream?", "Simulation is still initialised.\nDo you want to quit?", QMessageBox::Yes, QMessageBox::No, 0);
    //		if(response != QMessageBox::Yes){
    //			return;
    //		}
    //	}
    ce->accept();
}


/*! Loads a neuron group and associated connections from a file.
	This should be a comma separated matrix of connection weights. */
void SpikeStreamMainWindow::importConnectionMatrix(){
    /* Check that we do not have a simulation initialised. */
    //	if(SimulationManager::isInitialised()){
    //		QMessageBox::warning(this, "Import Connection Matrix", "Matrix cannot be loaded when a simulation is initialised.\nDestroy the simulation and try again.");
    //		return;
    //	}

    /* Check that playback of an archive is not currently in progress. */
    //	if(ArchiveManager::isRunning()){
    //		QMessageBox::warning(this, "Import Connection Matrix", "Matrix cannot be loaded when an archive is being played back.\nStop archive playback and try again.");
    //		return;
    //	}

    //Warn user about limitations
    int response = QMessageBox::warning(this, "Import ConnectionMatrix", "This is a preliminary method that will try create a new layer at position (0,0,0) with the default neuron and synapse types\n Make sure there is enough space for the new layer.\nDo you want to continue?", QMessageBox::Yes, QMessageBox::No, 0);
    if(response != QMessageBox::Yes)
	return;

    //Get the file name
    QString fileName = Q3FileDialog::getOpenFileName(defaultFileLocation, "All files (*.*)", this, "Load matrix dialog", "Choose a file to load" );
    if(fileName.isNull())
	return;

    //Unload archive if it is loaded. Database loading will invalidate the query if it changes the archive database.
    //	archiveWidget->unloadArchive();

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
    //	if(SimulationManager::isInitialised()){
    //		QMessageBox::warning(this, "Load Databases", "Databases cannot be loaded when a simulation is initialised.\nDestroy the simulation and try again.");
    //		return;
    //	}

    /* Check that playback of an archive is not currently in progress. */
    //	if(ArchiveManager::isRunning()){
    //		QMessageBox::warning(this, "Load Databases", "Databases cannot be loaded when an archive is being played back.\nStop archive playback and try again.");
    //		return;
    //	}


    //Check that user wants to overwrite current database
    int response = QMessageBox::warning(this, "Load Databases", "This will overwrite all data in the databases that are loaded including Neuron, Synapse and Probe types.\nDo you want to continue?", QMessageBox::Yes, QMessageBox::No, 0);
    if(response != QMessageBox::Yes)
	return;

    //Get the file name
    QString fileName = Q3FileDialog::getOpenFileName(defaultFileLocation, "Zipped SQL (*.sql.tar.gz)", this, "load database dialog", "Choose a file to load" );
    if(fileName.isNull())
	return;

    //Unload archive if it is loaded. Database loading will invalidate the query if it changes the archive database.
    //	archiveWidget->unloadArchive();

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
    //simulationWidget->reloadDevices();

}


/*! Reloads neuron groups and connections from the database. */
void SpikeStreamMainWindow::reloadEverything(){
    //Emit a signal. NOTE: Better way of handling this and saves keeping endless references
    emit reload();

    //Manual reload of classes  not connected to the signal FIXME: REPLACE WITH SIGNAL MECHANISM
    try{
	//layerWidget->reloadNeuronGroups();
	//connectionWidget->reloadConnections();
	//simulationWidget->reloadNeuronGroups();
	//simulationWidget->reloadConnectionGroups();
	//networkViewerProperties->reloadConnections();
	//networkViewerProperties->reloadNeuronGroups();
	//networkViewer2->reloadEverything();
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
    //simulationWidget->reloadPatterns();
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


