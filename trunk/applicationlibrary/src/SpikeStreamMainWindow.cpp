//SpikeStream includes
#include "AnalysisLoaderWidget.h"
#include "SpikeStreamMainWindow.h"
#include "GlobalVariables.h"
#include "ConfigLoader.h"
#include "Globals.h"
#include "NRMImportDialog.h"
#include "SpikeStreamException.h"
#include "NetworksWidget.h"
#include "ArchiveWidget_V2.h"
#include "NeuronGroupWidget.h"
#include "ConnectionWidget_V2.h"
#include "NetworkViewer_V2.h"
#include "NetworkViewerProperties_V2.h"
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
//Added by qt3to4:
#include <QCloseEvent>
#include <Q3PopupMenu>
#include <QDebug>
#include <QScrollArea>
#include <Q3ScrollView>
#include <Q3VBox>

//Other includes
#include <string>
using namespace std;


/*! Constructor. */
SpikeStreamMainWindow::SpikeStreamMainWindow() : QMainWindow( 0, "SpikeStream - Analysis", Qt::WDestructiveClose ){
	//Get the working directory and store in global scope
	QString rootDir = QCoreApplication::applicationDirPath();
	rootDir.truncate(rootDir.size() - 4);//Trim the "/bin" off the end
	Globals::setSpikeStreamRoot(rootDir);

	//Set up splash screen to give feedback whilst application is loading
	QSplashScreen *splashScreen = 0;
	QPixmap splashPixmap(Globals::getSpikeStreamRoot() + "/images/spikestream_splash.png" );
	splashScreen = new QSplashScreen( splashPixmap );
	splashScreen->show();

	//Set up config
	ConfigLoader* configLoader;
	if(QFile::exists(Globals::getSpikeStreamRoot() +  "/spikestream.config")){
		configLoader = new ConfigLoader(Globals::getSpikeStreamRoot().toStdString() + "/spikestream.config");
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
			"SpikeStreamNetwork"
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
			"SpikeStreamArchive"
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
			"SpikeStreamAnalysis"
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
	Globals::setWorkingDirectory(configLoader->getCharData("default_file_location"));

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
	//fileMenu->insertItem("Load databases", this, SLOT(loadDatabases()), Qt::CTRL+Qt::Key_L);
	//fileMenu->insertItem("Save databases", this, SLOT(saveDatabases()), Qt::CTRL+Qt::Key_S);
	fileMenu->insertSeparator();
	//fileMenu->insertItem("Import connection matrix", this, SLOT(importConnectionMatrix()), Qt::CTRL+Qt::Key_I);
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
	QScrollArea* networksScrollArea = new QScrollArea();
	NetworksWidget* networksWidget = new NetworksWidget(networksScrollArea);
	networksScrollArea->setWidget(networksWidget);
	tabWidget->addTab(networksScrollArea, "Networks");

	//Set up editor window
	QSplitter *layerSplitterWidget = new QSplitter(tabWidget);
	NeuronGroupWidget* neuronGrpWidget = new NeuronGroupWidget();
	ConnectionWidget_V2* conWidget = new ConnectionWidget_V2(this);
	layerSplitterWidget->addWidget(neuronGrpWidget);
	layerSplitterWidget->addWidget(conWidget);
	layerSplitterWidget->setOrientation(Qt::Vertical);
	tabWidget->addTab(layerSplitterWidget, "Editor");


	//Create network viewer for right half of screen
	unsigned int maxAutoLoadConnGrpSize = 1000000;
	try{
		maxAutoLoadConnGrpSize = Util::getUInt(configLoader->getStringData("max_autoload_conngrp_size").data());
	}
	catch(std::exception& er){// Catch-all for std exceptions
		cerr<<"SpikeStreamMainWindow: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown getting maxAutoLoadConnGrpSize: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Config Error", errorString);
	}
	new NetworkViewer_V2(mainSplitterWidget);

	//Set up viewer tab
	QScrollArea* networkViewPropsScrollArea = new QScrollArea(tabWidget);
	NetworkViewerProperties_V2* networkViewerProperties_V2 = new NetworkViewerProperties_V2(networkViewPropsScrollArea);
	networkViewPropsScrollArea->setWidget(networkViewerProperties_V2);
	tabWidget->addTab(networkViewPropsScrollArea, "Viewer");

	//Set up archive tab
	QScrollArea* archiveScrollArea = new QScrollArea(tabWidget);
	ArchiveWidget_V2* archiveWidget = new ArchiveWidget_V2();
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

	QAction* showArchiveWidgetAction = new QAction(this);
	showArchiveWidgetAction->setShortcut(QKeySequence(Qt::Key_F3));
	connect(showArchiveWidgetAction, SIGNAL(triggered()), this, SLOT(showArchiveWidget()));
	this->addAction(showArchiveWidgetAction);

	QAction* showAnalysisWidgetAction = new QAction(this);
	showAnalysisWidgetAction->setShortcut(QKeySequence(Qt::Key_F4));
	connect(showAnalysisWidgetAction, SIGNAL(triggered()), this, SLOT(showAnalysisWidget()));
	this->addAction(showAnalysisWidgetAction);

	//Finish off
	QPixmap iconPixmap(Globals::getSpikeStreamRoot() + "/images/spikestream_icon_64.png" );
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
	//Clean up globals - everything stored in globals is cleaned up by globals
	Globals::cleanUp();
}


/*----------------------------------------------------------*/
/*-----                PRIVATE SLOTS                   -----*/
/*----------------------------------------------------------*/

/*! Displays brief information about the application. */
void SpikeStreamMainWindow::about(){
	QMessageBox::about( this, "About", "SpikeStream\nVersion 0.1 beta\nCreated by David Gamez: david@davidgamez.eu");
}


/*! Displays the analysis widget */
void SpikeStreamMainWindow::showAnalysisWidget(){
	tabWidget->setCurrentPage(3);
}


/*! Displays the archive widget */
void SpikeStreamMainWindow::showArchiveWidget(){
	tabWidget->setCurrentPage(2);
}


/*! Displays the editor widget */
void SpikeStreamMainWindow::showEditorWidget(){
	tabWidget->setCurrentPage(1);
}


/*! Displays the networks widget */
void SpikeStreamMainWindow::showNetworkWidget(){
	tabWidget->setCurrentPage(0);
}


/*! Empties all databases except the Neuron, Synapse and Probe types. Useful when
	an error has led to out of date information in one of the tables. However all
	data will be lost if this method is called. */
void SpikeStreamMainWindow::clearDatabases(){
	//Confirm that user wants to take this action.
	QMessageBox msgBox;
	msgBox.setText("Deleting Network");
	msgBox.setInformativeText("Are you sure that you want to clear the database completely?\nAll data will be lost and this step cannot be undone.");
	msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Cancel);
	int ret = msgBox.exec();
	if(ret != QMessageBox::Ok)
		return;

	//Delete all networks
	Globals::getNetworkDao()->deleteAllNetworks();

	//Inform other classes about the change
	Globals::setNetwork(NULL);
	Globals::getEventRouter()->networkChangedSlot();
}


/*! Called when window is closed.
	This is needed to invoke destructor of neuronapplication
	Could run a check on whether the user really wants to quit.
	However, this is not needed at present since everthing is stored in the database. */
void SpikeStreamMainWindow::closeEvent( QCloseEvent* ce ){
	ce->accept();
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


