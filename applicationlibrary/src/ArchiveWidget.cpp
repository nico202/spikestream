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
#include "ArchiveWidget.h"
#include "Debug.h"
#include "GlobalVariables.h"
#include "Utilities.h"
#include "LoadArchiveDialog.h"
#include "NetworkModelXmlHandler.h"
#include "SpikeStreamMainWindow.h"
#include "ArchiveStatisticsDialog.h"

//Qt includes
#include <qlayout.h>
#include <qfile.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qstring.h>
#include <q3groupbox.h>
#include <qxml.h>
#include <qapplication.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

//Other includes
#include <iostream>
using namespace std;
using namespace mysqlpp;


/*! Constructor. */
ArchiveWidget::ArchiveWidget(QWidget *parent, DBInterface *archDBInter, ArchiveManager* archMan) : QWidget(parent, "Archive Widget"){
	//Store references
	archiveManager = archMan;
	archiveDBInterface = archDBInter;
	spikeStrApp = SpikeStreamMainWindow::spikeStreamApplication;//Set up a short version of this reference

	//Initialise variables
	ignoreButton = false;
	archiveStatisticsIDCount = 0;

	//Set up pixmap that user clicks on to edit statistics
	editPixmap = new QPixmap(SpikeStreamMainWindow::workingDirectory + "/images/edit_parameters.xpm");

	//Set up pixmaps to control play and stop
	QPixmap playPixmap(SpikeStreamMainWindow::workingDirectory + "/images/play.xpm");
	QPixmap stepPixmap(SpikeStreamMainWindow::workingDirectory + "/images/step.xpm");
	QPixmap stopPixmap(SpikeStreamMainWindow::workingDirectory + "/images/stop.xpm");
	QPixmap rewindPixmap(SpikeStreamMainWindow::workingDirectory + "/images/rewind.xpm");
	QPixmap fastForwardPixmap(SpikeStreamMainWindow::workingDirectory + "/images/fast_forward.xpm");

	//Create archive manager to read from database and play back patterns
	archiveManager = new ArchiveManager(archiveDBInterface, this);
	connect (archiveManager, SIGNAL(stopped()), this, SLOT(archiveStopped()));
	connect (archiveManager, SIGNAL(archiveError(const QString&)), this, SLOT(showArchiveError(const QString&)));

	//Create vertical layout to organise widget
	Q3VBoxLayout *mainVerticalBox = new Q3VBoxLayout(this, 5, 10, "vertical1");


	//--------------------- ARCHIVE CONTROLS ---------------------------
	//Create group box for the simulation controls
	Q3GroupBox *archiveGrpBox = new Q3GroupBox("Controls", this);

	//Create a vertical box to organise layout in group box
	Q3VBoxLayout *archVerticalBox = new Q3VBoxLayout(archiveGrpBox, 5, 10, "vertical3");
	archVerticalBox->addSpacing(20);

	//Set up load section
	Q3HBoxLayout *archLoadBox = new Q3HBoxLayout();
	loadButton = new QPushButton("Load", archiveGrpBox);
	loadButton->setBaseSize(120, 30);
	loadButton->setMaximumSize(120, 30);
	loadButton->setMinimumSize(120, 30);
	connect (loadButton, SIGNAL(clicked()), this, SLOT(loadButtonPressed()));
	archLoadBox->addSpacing(10);
	archLoadBox->addWidget(loadButton);

	unloadButton = new QPushButton("Unload", archiveGrpBox);
	unloadButton->setBaseSize(120, 30);
	unloadButton->setMaximumSize(120, 30);
	unloadButton->setMinimumSize(120, 30);
	connect (unloadButton, SIGNAL(clicked()), this, SLOT(unloadArchive()));
	archLoadBox->addStretch(5);
	archLoadBox->addWidget(unloadButton);
	archLoadBox->addSpacing(10);

	archVerticalBox->addLayout(archLoadBox);

	//Set up display for name and date of the archive
	Q3HBoxLayout *archInfoBox = new Q3HBoxLayout();
	archInfoText = new QLineEdit("", archiveGrpBox);
	archInfoText->setReadOnly(true);
	archInfoText->setEnabled(false);
	archInfoBox->addSpacing(10);
	archInfoBox->addWidget(archInfoText);
	archInfoBox->addSpacing(10);

	archVerticalBox->addLayout(archInfoBox);
	archVerticalBox->addSpacing(5);

	//Set up transport buttons
	Q3HBoxLayout *archTransportButtonBox = new Q3HBoxLayout();
	rewindButton = new QPushButton(QIcon(rewindPixmap), "", archiveGrpBox);
	rewindButton->setEnabled(false);
	rewindButton->setBaseSize(30, 30);
	rewindButton->setMaximumSize(30, 30);
	rewindButton->setMinimumSize(30, 30);
	connect (rewindButton, SIGNAL(clicked()), this, SLOT(rewindButtonPressed()));
	archTransportButtonBox->addSpacing(10);
	archTransportButtonBox->addWidget(rewindButton);

	playButton = new QPushButton(QIcon(playPixmap), "", archiveGrpBox);
	playButton->setToggleButton(true);
	playButton->setEnabled(false);
	playButton->setBaseSize(100, 30);
	playButton->setMaximumSize(100, 30);
	playButton->setMinimumSize(100, 30);
	connect (playButton, SIGNAL(toggled(bool)), this, SLOT(playButtonToggled(bool)));
	archTransportButtonBox->addWidget(playButton);

	stepButton = new QPushButton(QIcon(stepPixmap), "", archiveGrpBox);
	stepButton->setEnabled(false);
	stepButton->setBaseSize(50, 30);
	stepButton->setMaximumSize(50, 30);
	stepButton->setMinimumSize(50, 30);
	connect (stepButton, SIGNAL(clicked()), this, SLOT(stepButtonPressed()));
	archTransportButtonBox->addWidget(stepButton);

	fastForwardButton = new QPushButton(QIcon(fastForwardPixmap), "", archiveGrpBox);
	fastForwardButton->setToggleButton(true);
	fastForwardButton->setEnabled(false);
	fastForwardButton->setBaseSize(30, 30);
	fastForwardButton->setMaximumSize(30, 30);
	fastForwardButton->setMinimumSize(30, 30);
	connect (fastForwardButton, SIGNAL(toggled(bool)), this, SLOT(fastForwardButtonToggled(bool)));
	archTransportButtonBox->addWidget(fastForwardButton);
	archTransportButtonBox->addStretch(5);

	stopButton = new QPushButton(QIcon(stopPixmap), "", archiveGrpBox);
	stopButton->setEnabled(false);
	stopButton->setBaseSize(50, 30);
	stopButton->setMaximumSize(50, 30);
	stopButton->setMinimumSize(50, 30);
	connect (stopButton, SIGNAL(clicked()), this, SLOT(stopButtonPressed()));
	archTransportButtonBox->addWidget(stopButton);
	archTransportButtonBox->addSpacing(10);

	archVerticalBox->addLayout(archTransportButtonBox);
	archVerticalBox->addSpacing(5);

	Q3HBoxLayout *archFrameRateBox = new Q3HBoxLayout();
	archFrameRateBox->addSpacing(10);
	frameRateLabel = new QLabel("Frames per second", archiveGrpBox);
	frameRateLabel->setEnabled(false);
	archFrameRateBox->addWidget(frameRateLabel);
	frameRateCombo = new QComboBox(archiveGrpBox);
	frameRateCombo->setEnabled(false);
	frameRateCombo->insertItem("1");	
	frameRateCombo->insertItem("5");
	frameRateCombo->insertItem("10");
	frameRateCombo->insertItem("15");
	frameRateCombo->insertItem("20");
	frameRateCombo->insertItem("25");
	connect(frameRateCombo, SIGNAL(activated(int)), this, SLOT(frameRateComboChanged(int)));
	archFrameRateBox->addWidget(frameRateCombo);
	archFrameRateBox->addStretch(5);

	archVerticalBox->addLayout(archFrameRateBox);
	archVerticalBox->addSpacing(20);

	//Add Archive box to layout. Need an extra box to make it fit nicely
	Q3HBoxLayout *archHBox = new Q3HBoxLayout();
	archHBox->addWidget(archiveGrpBox);
	archHBox->addStretch(5);
	mainVerticalBox->addLayout(archHBox);
	mainVerticalBox->addSpacing(5);

	//----------------------- ARCHIVE STATISTICS -----------------------------
	//Create group box for the simulation controls
	statsGrpBox = new Q3GroupBox("Statistics", this);

	//Create a vertical box to organise layout in group box
	Q3VBoxLayout *statsVerticalBox = new Q3VBoxLayout(statsGrpBox, 5, 10, "vertical3");
	statsVerticalBox->addSpacing(20);

	//Add button for creating a statistical monitor
	Q3HBoxLayout* statsButtBox = new Q3HBoxLayout();
	QPushButton *addStatsButton = new QPushButton("Add Statistics Monitor", statsGrpBox);
	addStatsButton->setBaseSize(170, 30);
	addStatsButton->setMaximumSize(170, 30);
	addStatsButton->setMinimumSize(170, 30);
	connect(addStatsButton, SIGNAL(clicked()), this, SLOT(addStatsButtonClicked()));
	statsButtBox->addSpacing(10);
	statsButtBox->addWidget(addStatsButton);

	//Add button for deleting statistical monitor(s)
	deleteStatsButton = new QPushButton("Delete Statistics Monitor", statsGrpBox);
	deleteStatsButton->setBaseSize(170, 30);
	deleteStatsButton->setMaximumSize(170, 30);
	deleteStatsButton->setMinimumSize(170, 30);
	connect(deleteStatsButton, SIGNAL(clicked()), this, SLOT(deleteStatsButtonClicked()));
	statsButtBox->addWidget(deleteStatsButton);

	QPushButton* viewModelButton = new QPushButton("View Network Model", statsGrpBox);
	viewModelButton->setBaseSize(170, 30);
	viewModelButton->setMaximumSize(170, 30);
	viewModelButton->setMinimumSize(170, 30);
	connect(viewModelButton, SIGNAL(clicked()), this, SLOT(viewModelButtonClicked()));
	statsButtBox->addWidget(viewModelButton);

	statsButtBox->addStretch(5);
	statsVerticalBox->addLayout(statsButtBox);

	//Set up table
	Q3HBoxLayout* statsTableBox = new Q3HBoxLayout();
	statsTableBox->addSpacing(10);
	statsTable = new Q3Table(0, 6, statsGrpBox);
	statsTable->setShowGrid(false);
	statsTable->setSorting(false);
	statsTable->setSelectionMode(Q3Table::NoSelection);
	statsTable->verticalHeader()->hide();
	statsTable->setLeftMargin(0);
	Q3Header* statsTableHeader = statsTable->horizontalHeader();
	checkBxCol = 0;
	statsTableHeader->setLabel( checkBxCol, "" );
	statsTable->setColumnWidth( checkBxCol, 20);
	editCol = 1;
	statsTableHeader->setLabel( editCol, "Edit" );
	statsTable->setColumnWidth( editCol, 30);
	descCol = 2;
	statsTableHeader->setLabel( descCol, "Description");
	statsTable->setColumnWidth( descCol, 150);
	neursPerTimeStepCol = 3;
	statsTableHeader->setLabel( neursPerTimeStepCol, "Neurons Firing Per Time Step");
	statsTable->setColumnWidth( neursPerTimeStepCol, 200);
	totalNeursCol = 4;
	statsTableHeader->setLabel( totalNeursCol, "Neurons Fired Total");
	statsTable->setColumnWidth( totalNeursCol, 130);
	idCol = 5;
	statsTableHeader->setLabel( idCol, "ID");
	statsTable->setColumnWidth( idCol, 20);
	statsTable->setMinimumSize(600, 200);
	statsTableBox->addWidget(statsTable);
	statsTableBox->addSpacing(10);
	statsTableBox->addStretch(5);
	statsVerticalBox->addLayout(statsTableBox);

	//Listen for clicks on the edit pixmap
	connect (statsTable, SIGNAL(clicked(int, int, int, const QPoint &)), this, SLOT(statsTableClicked(int, int, int, const QPoint &)));

	//Add number of spikes/ neurons information
	Q3HBoxLayout *spikeNeurCountLayout = new Q3HBoxLayout();
	spikeNeurCountLayout->addSpacing(10);
	spikeNeurCountLabel_1 = new QLabel("Number of firing neurons ", statsGrpBox);
	spikeNeurCountLayout->addWidget(spikeNeurCountLabel_1);
	spikeNeurCountLabel_2 = new QLabel("", statsGrpBox);
	spikeNeurCountLayout->addWidget(spikeNeurCountLabel_2);
	spikeNeurCountLayout->addStretch(5);
	statsVerticalBox->addLayout(spikeNeurCountLayout);

	//Add total number of spikes information
	Q3HBoxLayout *spikeNeurTotalLayout = new Q3HBoxLayout();
	spikeNeurTotalLayout->addSpacing(10);
	spikeNeurTotalLabel_1 = new QLabel("Total firing neuron count ", statsGrpBox);
	spikeNeurTotalLayout->addWidget(spikeNeurTotalLabel_1);
	spikeNeurTotalLabel_2 = new QLabel("", statsGrpBox);
	spikeNeurTotalLayout->addWidget(spikeNeurTotalLabel_2);
	spikeNeurTotalLayout->addStretch(5);
	statsVerticalBox->addLayout(spikeNeurTotalLayout);
	statsVerticalBox->addSpacing(10);

	statsVerticalBox->addStretch(5);

	//Add Statistics box to layout. Need an extra box to make it fit nicely
	statsGrpBox->setEnabled(false);
	Q3HBoxLayout *statsHBox = new Q3HBoxLayout();
	statsHBox->addWidget(statsGrpBox);
	statsHBox->addStretch(5);
	mainVerticalBox->addLayout(statsHBox);
	mainVerticalBox->addStretch(5);

	//Set up a dialog to view the model
	viewModelDialog = new ViewModelDialog(this, &networkModelString);


	//----------------------- DOCK BUTTONS ------------------------------------
	Q3HBoxLayout *dockButtonBox = new Q3HBoxLayout();
	dockAllButton = new QPushButton("Dock All", this);
	dockAllButton->setEnabled(false);
	connect(dockAllButton, SIGNAL(clicked()), this, SLOT(dockAllButtonClicked()));
	dockButtonBox->addWidget(dockAllButton);
	undockAllButton = new QPushButton("Undock All", this);
	undockAllButton->setEnabled(false);
	connect(undockAllButton, SIGNAL(clicked()), this, SLOT(undockAllButtonClicked()));
	dockButtonBox->addWidget(undockAllButton);
	dockButtonBox->addStretch(5);

	mainVerticalBox->addLayout(dockButtonBox);
}


/*! Destructor. */
ArchiveWidget::~ArchiveWidget(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING ARCHIVE WIDGET"<<endl;
	#endif//MEMORY_DEBUG
	
	//Clean up any memory on the heap if we have an archive loaded.
	if(archiveManager->archiveIsLoaded())
		for(vector<NeuronGroup*>::iterator iter = neuronGrpVector.begin(); iter != neuronGrpVector.end(); ++iter){
			delete *iter;
	}
	delete archiveManager;
}


//-----------------------------------------------------------------------------
//------------------------- PUBLIC METHODS ------------------------------------
//-----------------------------------------------------------------------------

/*! Called to set controls appropriately when the archive has stopped playing. */
void ArchiveWidget::archiveStopped(){
	if(playButton->isOn())
		playButton->toggle();
	if(fastForwardButton->isOn())
		fastForwardButton->toggle();
}


/*! Used when deleting an archive. This interferes with the resUse of the
	archiveManager, which has to be deleted and reinitialised after the deletion
	of the archive is complete. */
void ArchiveWidget::freezeArchive(){
	archiveManager->freezeArchive();
}


/*! Returns a reference to the archive manager that handles the archive data. */
ArchiveManager* ArchiveWidget::getArchiveManager(){
	return archiveManager;
}


/*! Hides the windows that are currently visible.
FIXME NEED TO TRACK WHICH ARE CURRENTLY OPEN. */
void ArchiveWidget::hideOpenWindows(){
	monitorArea->dockMonitorWindows();
	viewModelDialog->hide();
}


/*! Passes a reference to the monitor area used to display archive. */
void ArchiveWidget::setMonitorArea(MonitorArea* monArea){
	monitorArea = monArea;
}

/*! Shows the windows that are currently visible.
FIXME NEED TO TRACK WHICH ARE CURRENTLY OPEN. */
void ArchiveWidget::showOpenWindows(){

}


/*! Used for displaying statistics about archive as it plays back. */
void ArchiveWidget::setSpikeCount(unsigned int spikeCount){
	spikeStrApp->lock();
	spikeNeurCountLabel_2->setText(QString::number(spikeCount));
	spikeStrApp->unlock();
}


/*! Used for displaying statistics about archive as it is played back. */
void ArchiveWidget::setSpikeTotal(unsigned int spikeTotal){
	spikeStrApp->lock();
	spikeNeurTotalLabel_2->setText(QString::number(spikeTotal));
	spikeStrApp->unlock();
}


/*! Re-initialises the archive from the values stored in the archive manager. 
	Only does this if the archive has not been deleted, otherwise the archive
	is unloaded. */
void ArchiveWidget::unfreezeArchive(){
	//If an archive has been loaded check to see if it still exists
	if(archiveManager->archiveIsLoaded()){
		try{
			Query query = archiveDBInterface->getQuery();
			query.reset();
                        query<<"SELECT * FROM NetworkModels WHERE StartTime = UNIX_TIMESTAMP(\'"<<archiveManager->getSimulationStartDateTime().toStdString()<<"\')";
                        StoreQueryResult results = query.store();
			if(results.size() != 1){//Current archive has been deleted
				//Unload archive
				unloadArchive();
			}
			else{//Current archive still exists, so unfreeze archive
				archiveManager->unfreezeArchive();
			}
		}
		catch (const BadQuery& er) {// Handle any query errors
			cerr<<"ArchiveWidget: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
			QString errorString = "Bad query when unfreezing archive: \"";
			errorString += er.what();
			errorString += "\"";
			QMessageBox::critical( 0, "Archive Error", errorString);
			return;
		}
		catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
			cerr<<"ArchiveWidget: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
			QString errorString = "Exception thrown when unfreezing archive: \"";
			errorString += er.what();
			errorString += "\"";
			QMessageBox::critical( 0, "Archive Error", errorString);
			return;
    	}
	}
}


/*! Unloads the current archive. */
void ArchiveWidget::unloadArchive(){
	if(archiveManager->isRunning()){
		QMessageBox::critical( 0, "Archive Error", "Archive cannot be unloaded whilst it is being played back.");
		return;
	}

	//Instruct archive manager to unload the archive
	archiveManager->unloadArchive();

	//Reset monitor area
	monitorArea->resetMonitorWindows();

	//Disable the transport controls
	enableArchiveControls(false);

	//Reset statistics
	spikeNeurCountLabel_2->setText("0");
	spikeNeurTotalLabel_2->setText("0");
	statsTable->setNumRows(0);
	archiveStatisticsVector.clear();

	//Reset and hide the view model dialog
	networkModelString = "";
	viewModelDialog->reloadText();
	viewModelDialog->hide();

	//Set the text descrbing the archive
	archInfoText->setText("");

	//Delete neuron groups stored in neuron group vector
	for(vector<NeuronGroup*>::iterator iter = neuronGrpVector.begin(); iter != neuronGrpVector.end(); ++iter){
		delete *iter;
	}
	neuronGrpVector.clear();
}


//-----------------------------------------------------------------------------
//---------------------------- SLOTS ------------------------------------------
//-----------------------------------------------------------------------------

/*! Adds a statistical monitor to the statistics table. */
void ArchiveWidget::addStatsButtonClicked(){
	if(!archiveManager->archiveIsLoaded()){//Double check that the neuron group vector has sensible values
		cerr<<"ArchiveWidget: NO ARCHIVE LOADED"<<endl;
		return;
	}

	//Use archive statistics dialog to get information about the statistics
	ArchiveStatisticsDialog statsMonDlg(this, neuronGrpVector);
	if(statsMonDlg.exec() == QDialog::Accepted){
		//Extract the information about the statistical properties to be monitored
		ArchiveStatisticsHolder* archStatHolder = statsMonDlg.getArchiveStatistics();
		addArchiveStatistics(archStatHolder);
	}
}


/*! Called when an  archive name is changed.
	If the currently loaded archive's name has been changed, it is updated. */
void ArchiveWidget::archiveNameChanged(QString timeStamp, QString newName){
        cout<<"time stamp = \""<<timeStamp.toStdString()<<"\" new name = "<<newName.toStdString()<<" get date stuff \""<<getDate(archInfoText->text()).toStdString()<<"\""<<endl;
	if(archiveManager->archiveIsLoaded()){
		if(getDate(archInfoText->text()) == timeStamp)
			archInfoText->setText(newName + " [ " + timeStamp + " ]");
	}
}


/*! Removes statistical monitor(s) from the statistics table. */
void ArchiveWidget::deleteStatsButtonClicked(){
	//Keep a record of whether we have deleted any
	bool archStatDeleted = false;

	//Work through table
	for(int i=0; i<statsTable->numRows(); ++i){
		Q3CheckTableItem * item = (Q3CheckTableItem*)statsTable->item(i, checkBxCol);
			if(item->isChecked()){
				//Get the ID of the selected statistic
				unsigned int archStatID;
				try{
					archStatID = Utilities::getUInt(statsTable->item(i, idCol)->text().ascii());
				}
				catch(std::exception& er){// Catch-all for all std exceptions
					cerr<<"ArchiveWidget: STD EXCEPTION \""<<er.what()<<"\""<<endl;
					QString errorString = "Exception thrown getting neuron group id: \"";
					errorString += er.what();
					errorString += "\"";
					QMessageBox::critical( 0, "NeuronGrpID Error", errorString);
					return;
				}
				
				//Delete the archive statistics
				deleteArchiveStatistics(archStatID);
				archStatDeleted = true;
			}
	}

	//Reload the archive statistics table if we have deleted anything
	if(archStatDeleted)
		loadStatisticsTable();
}


/*! Docks all monitor windows into the Monitor Area. */
void ArchiveWidget::dockAllButtonClicked(){
	monitorArea->dockMonitorWindows();
}


/*! Sets fast forwarding on or off. */
void ArchiveWidget::fastForwardButtonToggled(bool on){
	if(ignoreButton){
		ignoreButton = false;
		return;
	}
	if(on){
		if(playButton->isOn()){
			ignoreButton = true;
			playButton->toggle();
		}
		loadButton->setEnabled(false);
		deleteStatsButton->setEnabled(false);//Not a good idea to delete statistics whilst it is playing
		unloadButton->setEnabled(false);
		archiveManager->fastForwardArchive();
	}
	else{
		archiveManager->stopArchive();
		loadButton->setEnabled(true);
		deleteStatsButton->setEnabled(true);
		unloadButton->setEnabled(true);
	}
}


/*! Changes the frame rate at which the archive is played back. */
void ArchiveWidget::frameRateComboChanged(int){
	try{
		unsigned int frameRate = Utilities::getUInt(frameRateCombo->currentText().ascii());
		archiveManager->setFrameRate(frameRate);
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"ArchiveWidget: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown setting frame rate: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Frame Rate Error", errorString);
	}
}


/*! Displays load archive dialog. */
void ArchiveWidget::loadButtonPressed(){
	//Create load archive dialog
	LoadArchiveDialog *loadArchiveDialog = new LoadArchiveDialog(this, archiveDBInterface);
	connect (loadArchiveDialog, SIGNAL (deleteArchiveStarted()), this, SLOT (freezeArchive()));
	connect (loadArchiveDialog, SIGNAL (deleteArchiveFinished()), this, SLOT (unfreezeArchive()));
	connect (loadArchiveDialog, SIGNAL (archiveNameChanged(QString, QString)), this, SLOT (archiveNameChanged(QString, QString)));

	//Show load archive dialog
	if (loadArchiveDialog->exec() == QDialog::Accepted ) {//Load the archive
		QString archiveString = loadArchiveDialog->getArchiveString();
		
		if(archiveString != ""){//Check to see something has been selected
			/* Load up archive into monitor area, which 
				passes the necessary classes and data to the archive manager*/
			loadArchive(archiveString);
		}
	}
	//Free memory
	delete loadArchiveDialog;
}


/*! Plays or stops archive. */
void ArchiveWidget::playButtonToggled(bool on){
	if(ignoreButton){
		ignoreButton = false;
		return;
	}
	if(on){
		archiveManager->playArchive();
		loadButton->setEnabled(false);//To prevent conflict with load dialog
		deleteStatsButton->setEnabled(false);//Not a good idea to delete statistics whilst it is playing
		unloadButton->setEnabled(false);
		if(fastForwardButton->isOn()){
			ignoreButton = true;
			fastForwardButton->toggle();
		}
	}
	else{
		archiveManager->stopArchive();
		loadButton->setEnabled(true);
		deleteStatsButton->setEnabled(true);
		unloadButton->setEnabled(true);
	}
}


/*! Rewinds archive back to beginning. */
void ArchiveWidget::rewindButtonPressed(){
	try{
		//Rewind archive manager
		archiveManager->rewindArchive();

		//Rewind network monitors
		monitorArea->rewindNetworkMonitors();

		//Reload statistics table
		updateStatisticsTable();

		//Reset statistics totals
		spikeNeurCountLabel_2->setText("0");
		spikeNeurTotalLabel_2->setText("0");
	}
    catch (const BadQuery& er) {// Handle any query errors
		cerr<<"ArchiveWidget: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when rewinding archive: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Archive Error", errorString);
		return;
    }
    catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
        cerr<<"ArchiveWidget: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown when rewinding archive: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Archive Error", errorString);
		return;
    }
	if(playButton->isOn())
		playButton->toggle();
}


/*! Steps the archive forward one time step. */
void ArchiveWidget::stepButtonPressed(){
	if(playButton->isOn()){
		ignoreButton = true;
		playButton->toggle();
		loadButton->setEnabled(true);
		unloadButton->setEnabled(true);
	}
	if(fastForwardButton->isOn()){
		fastForwardButton->toggle();
	}
	archiveManager->stepArchive();
}


/*! Displays an error generated by the archive manager and stops playback. */
void ArchiveWidget::showArchiveError(const QString& message){
	archiveStopped();
	QMessageBox::critical( 0, "Archive Error", message);
}


void ArchiveWidget::statsTableClicked(int row, int col, int, const QPoint &){
	if(col == editCol){
		//Get the id of this statistic. The rows match the positions in the vector
		unsigned int archStatID = archiveStatisticsVector[row]->ID;

		//Launch a dialog with the information from this archive statistic
		ArchiveStatisticsDialog statsMonDlg(this, neuronGrpVector, archiveStatisticsVector[row]);

		/* If the dialog is accepted delete the old archive statistics and create
			a new one with the same id. */
		if(statsMonDlg.exec() == QDialog::Accepted){
			//Extract the information about the statistical properties to be monitored
			ArchiveStatisticsHolder* archStatHolder = statsMonDlg.getArchiveStatistics();

			//Delete the old archive statistics with this id
			deleteArchiveStatistics(archStatID);

			//Add the new one with the same id as before
			addArchiveStatistics(archStatHolder, archStatID);
		}
	}
}

/*! Stops the archive if it is running. */
void ArchiveWidget::stopButtonPressed(){
	archiveManager->stopArchive();
	loadButton->setEnabled(true);
	unloadButton->setEnabled(true);
	if(playButton->isOn()){
		playButton->toggle();
	}
	if(fastForwardButton->isOn()){
		fastForwardButton->toggle();
	}
}


/*! Undocks all monitor windows. */
void ArchiveWidget::undockAllButtonClicked(){
	monitorArea->undockMonitorWindows();
}


/*! Called when the archive statistics counts have been changed. */
void ArchiveWidget::updateStatisticsTable(){
	//Reload the firing neuron count and firing neuron total for each statistic
	int rowCount = 0;
	for(vector<ArchiveStatisticsHolder*>::iterator iter = archiveStatisticsVector.begin(); iter != archiveStatisticsVector.end(); ++iter){
		statsTable->setItem(rowCount, neursPerTimeStepCol, new Q3TableItem(statsTable, Q3TableItem::Never, QString::number((*iter)->firingNeuronCount)));
		statsTable->setItem(rowCount, totalNeursCol, new Q3TableItem(statsTable, Q3TableItem::Never, QString::number((*iter)->firingNeuronTotal)));
		++rowCount;
	}
}


/*! Called when the view model button is clicked and displays a non-modal dialog
	containing the view model. */
void ArchiveWidget::viewModelButtonClicked(){
	viewModelDialog->show();
}


//----------------------------------------------------------------------
//----------------------- PRIVATE METHODS ------------------------------
//----------------------------------------------------------------------

/*! Adds archive statistics to this widget and elsewhere. */
void ArchiveWidget::addArchiveStatistics(ArchiveStatisticsHolder* archStatHolder){
	/* Give each archive statistic and the archive statistics holder a unique 
		id to make it easier to delete. The archive statistics are stored as 
		a group in an archive statistics holder, so give the same id
		to each archive statistic in the group. */
	archStatHolder->ID = archiveStatisticsIDCount;
	for(vector<ArchiveStatistic*>::iterator iter = archStatHolder->archStatVector.begin(); iter != archStatHolder->archStatVector.end(); ++iter){
		(*iter)->setID(archiveStatisticsIDCount);
	}

	//Increase the id that will be allocated to the next group
	++archiveStatisticsIDCount;

	//Store the archStatHolder
	archiveStatisticsVector.push_back(archStatHolder);
	archiveManager->addArchiveStatistics(archStatHolder);

	//Reload the table
	loadStatisticsTable();
}


/*! Adds archive statistics to this widget and elsewhere using the supplied id. */
void ArchiveWidget::addArchiveStatistics(ArchiveStatisticsHolder* archStatHolder, unsigned int archStatID){
	/* Give each archive statistic and the archive statistics holder a unique 
		id to make it easier to delete. The archive statistics are stored as 
		a group in an archive statistics holder, so give the same id
		to each archive statistic in the group. */
	archStatHolder->ID = archStatID;
	for(vector<ArchiveStatistic*>::iterator iter = archStatHolder->archStatVector.begin(); iter != archStatHolder->archStatVector.end(); ++iter){
		(*iter)->setID(archStatID);
	}

	//Store the archStatHolder
	archiveStatisticsVector.push_back(archStatHolder);
	archiveManager->addArchiveStatistics(archStatHolder);

	//Reload the table
	loadStatisticsTable();
}


/*! Adds archive statistics to this widget and elsewhere. */
void ArchiveWidget::deleteArchiveStatistics(unsigned int archStatID){
	//Remove the reference to this ArchiveStatistic from the other classes
	archiveManager->deleteArchiveStatistics(archStatID);

	//Work through the vector and delete the ArchiveStatistic with this ID
	for(vector<ArchiveStatisticsHolder*>::iterator iter = archiveStatisticsVector.begin(); iter != archiveStatisticsVector.end(); ++iter){
		if((*iter)->ID == archStatID){
			iter = archiveStatisticsVector.erase(iter);//iter now points to the element after the erased element
			if(iter == archiveStatisticsVector.end())//Don't want to increase iter again if we are at the end of the vector.
				break;
		}
	}
}


/*! Enables or disables the archive controls. */
void ArchiveWidget::enableArchiveControls(bool enable){
	if(enable){
		archInfoText->setEnabled(true);
		rewindButton->setEnabled(true);
		playButton->setEnabled(true);
		stepButton->setEnabled(true);
		fastForwardButton->setEnabled(true);
		stopButton->setEnabled(true);
		frameRateLabel->setEnabled(true);
		frameRateCombo->setEnabled(true);
		dockAllButton->setEnabled(true);
		undockAllButton->setEnabled(true);
		statsGrpBox->setEnabled(true);
	}
	else{
		archInfoText->setEnabled(false);
		rewindButton->setEnabled(false);
		playButton->setEnabled(false);
		stepButton->setEnabled(false);
		fastForwardButton->setEnabled(false);
		stopButton->setEnabled(false);
		frameRateLabel->setEnabled(false);
		frameRateCombo->setEnabled(false);
		dockAllButton->setEnabled(false);
		undockAllButton->setEnabled(false);
		statsGrpBox->setEnabled(false);
	}
}


/*! Extracts the date of the archive from the string. */
QString ArchiveWidget::getDate(QString archiveString){
	archiveString = archiveString.section('[', 1, 1, QString::SectionSkipEmpty);
	archiveString = archiveString.section(']', 0, 0, QString::SectionSkipEmpty);
	return archiveString.stripWhiteSpace();
}


/*! Sets up the GUI to play back an archive. */
void ArchiveWidget::loadArchive(QString archiveString){
	try{
		//Set up the archive manager
		archiveManager->loadArchive(getDate(archiveString));
		archiveManager->setFrameRate(Utilities::getDouble(frameRateCombo->currentText().ascii()));
	
		//Reset monitor area
		monitorArea->resetMonitorWindows();
	
		//Get XML string from database with appropriate parameters
		Query archiveQuery = archiveDBInterface->getQuery();
		archiveQuery.reset();
                archiveQuery<<"SELECT NeuronGroups, ArchiveType FROM NetworkModels WHERE StartTime = UNIX_TIMESTAMP(\""<<getDate(archiveString).toStdString()<<"\")";
                StoreQueryResult results = archiveQuery.store();
		Row row(*results.begin());//Should only be one row

		//Set up labels for count and total spikes
		unsigned int archiveType = Utilities::getUInt((std::string)row["ArchiveType"]);
		if(archiveType == FIRING_NEURON_ARCHIVE){
			spikeNeurCountLabel_1->setText("Number of firing neurons ");
			spikeNeurTotalLabel_1->setText("Total firing neuron count ");
		}
		else if(archiveType == SPIKE_ARCHIVE){
			spikeNeurCountLabel_1->setText("Number of spikes ");
			spikeNeurTotalLabel_1->setText("Total spike count ");
		}
		else{
			cerr<<"ArchiveWidget: ARCHIVE TYPE NOT RECOGNIZED!"<<endl;
			QMessageBox::critical( 0, "Archive Error", "Archive type not recognized");
			archiveManager->unloadArchive();
			monitorArea->resetMonitorWindows();
			return;
		}
	
		//Process XML
                QString tmpNwModelStr(((std::string)row["NeuronGroups"]).data());
	
		/* Parse XML file. 
			The nwModelXmlHandler will receive events from the parsing and use 
			the information to set up the monitor area */
		QXmlInputSource xmlInput;
		xmlInput.setData(tmpNwModelStr);
		NetworkModelXmlHandler nwModelXmlHandler(monitorArea);
		QXmlSimpleReader reader;
		reader.setContentHandler(&nwModelXmlHandler);
		reader.setErrorHandler(&nwModelXmlHandler);
		reader.parse(xmlInput);
		if(nwModelXmlHandler.getParseError()){
                        cerr<<"ArchiveWidget: ERROR OCCURRED DURING PARSING \""<<nwModelXmlHandler.getParseErrorString().toStdString()<<"\""<<endl;
			QString errorString = "Error encountered whilst parsing XML Network Model: \"";
			errorString += nwModelXmlHandler.getParseErrorString();
			errorString += "\"";
			QMessageBox::critical( 0, "Archive Error", errorString);
			archiveManager->unloadArchive();
			monitorArea->resetMonitorWindows();
			return;
		}

		/* Store the neuron groups in the model. The neuron groups are created on the heap so this
			operation should copy across the addresses. We will have to delete these when the archive
			is unloaded.*/
		neuronGrpVector = nwModelXmlHandler.getNeuronGrpVector();

		/* Extract the formatted XML string of the network model and reload this into 
			the view model dialog. */
		networkModelString = nwModelXmlHandler.getNetworkModelString();
		viewModelDialog->reloadText();
	
		/* Connect the network monitors that have been created with the
			archive manager and networkDataXmlHandler that are going to 
			update them */
		monitorArea->connectToManager();
	
		/* Undock the monitor windows. */
		monitorArea->undockMonitorWindows();
	
		// Enable the transport controls
		enableArchiveControls(true);
	
		//Reset statistics
		spikeNeurCountLabel_2->setText("0");
		spikeNeurTotalLabel_2->setText("0");
		statsTable->setNumRows(0);
	
		//Set the text descrbing the archive
		archInfoText->setText(archiveString);
	}
    catch (const BadQuery& er) {// Handle any query errors
		cerr<<"ArchiveWidget: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when loading archive: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Archive Error", errorString);
		archiveManager->unloadArchive();
		monitorArea->resetMonitorWindows();
    }
    catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
        cerr<<"ArchiveWidget: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown when loading archive: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Archive Error", errorString);
		archiveManager->unloadArchive();
		monitorArea->resetMonitorWindows();
    }
    catch (const std::exception& er) {// Catch-all for any std exceptions
        cerr<<"ArchiveWidget: EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown when loading archive: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Archive Error", errorString);
		archiveManager->unloadArchive();
		monitorArea->resetMonitorWindows();
    }
}


/*! Loads up the statistics table from the archiveStatisticsVector. */
void ArchiveWidget::loadStatisticsTable(){
	//Reset the table
	statsTable->setNumRows(0);
	int currentRowNumber = 0;

	//Load up the archive statistics
	for(vector<ArchiveStatisticsHolder*>::iterator iter = archiveStatisticsVector.begin(); iter != archiveStatisticsVector.end(); ++iter){
		statsTable->insertRows(currentRowNumber, 1);
		statsTable->setItem(currentRowNumber, checkBxCol, new Q3CheckTableItem( statsTable, QString("")));
		statsTable->setPixmap(currentRowNumber, editCol, *editPixmap);
                statsTable->setItem(currentRowNumber, descCol, new Q3TableItem(statsTable, Q3TableItem::Never, (*iter)->description.data()));
		statsTable->setItem(currentRowNumber, neursPerTimeStepCol, new Q3TableItem(statsTable, Q3TableItem::Never, QString::number((*iter)->firingNeuronCount)));
		statsTable->setItem(currentRowNumber, totalNeursCol, new Q3TableItem(statsTable, Q3TableItem::Never, QString::number((*iter)->firingNeuronTotal)));
		statsTable->setItem(currentRowNumber, idCol, new Q3TableItem(statsTable, Q3TableItem::Never, QString::number((*iter)->ID)));
		++currentRowNumber;
	}
}
