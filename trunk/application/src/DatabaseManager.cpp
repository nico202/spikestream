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
#include "DatabaseManager.h"
#include "SpikeStreamMainWindow.h"
#include "Debug.h"

//Qt includes
#include <qlayout.h>
#include <qpushbutton.h>
#include <q3accel.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>


/*! Constructor. */
DatabaseManager::DatabaseManager(QWidget * parent, QString flNme, bool saveDB, DBInterface* netDBInter, DBInterface* archDBInter, DBInterface* pattDBInter, DBInterface* devDBInter) : QDialog(parent){
	//Store references to databases and file name
	netDBInterface = netDBInter;
	archDBInterface = archDBInter;
	pattDBInterface = pattDBInter;
	devDBInterface = devDBInter;
	fileName = flNme;
	saveMode= saveDB;

	//Set caption
	if(saveMode)
		this->setCaption("Save Databases");
	else
		this->setCaption("Load Databases");

	//Get rid of extension from filename
	fileNameNoExtension = fileName.section('.', 0, 0);

	//Set up QProcess for running the script
	process = new Q3Process(this);

	//Connect process to slots
	connect( process, SIGNAL(readyReadStdout()), this, SLOT(readFromStdout()) );
	connect( process, SIGNAL(readyReadStderr()), this, SLOT(readFromStderr()) );
	connect( process, SIGNAL(processExited()), this, SLOT(processExited()) );

	//Create vertical layout to organise widget
	Q3VBoxLayout *verticalBox = new Q3VBoxLayout(this, 10, 10);

	//Add table to enable the databases to be saved or loaded to be selected
	dbTable = new Q3Table(0, 3, this);
	dbTable->setShowGrid(false);
	dbTable->setSorting(false);
	dbTable->setSelectionMode(Q3Table::NoSelection);
	dbTable->verticalHeader()->hide();
	dbTable->setLeftMargin(0);
	Q3Header * dbTableHeader = dbTable->horizontalHeader();
	nameCol = 0;
	dbTableHeader->setLabel(nameCol, "Database");
	dbTable->setColumnWidth(nameCol, 200);
	hostCol = 1;
	dbTableHeader->setLabel(hostCol, "Host");
	dbTable->setColumnWidth(hostCol, 150);
	selectionCol = 2;
	dbTableHeader->setLabel(selectionCol, "");
	dbTable->setColumnWidth(selectionCol, 50);

	verticalBox->addWidget(dbTable);

	//In save mode fill table with information about the databases to save
	if(saveMode){
		netDBRow = addDatabaseToTable(netDBInterface);
		archDBRow = addDatabaseToTable(archDBInterface);
		pattDBRow = addDatabaseToTable(pattDBInterface);
		devDBRow = addDatabaseToTable(devDBInterface);
	}
	//In load mode need to get list of saved databases and add them to the table
	else{
		//Set row numbers of databases to -1
		netDBRow = -1;
		archDBRow = -1;
		pattDBRow = -1;
		devDBRow = -1;
		getAvailableDatabases();
	}

	//Set up ok and cancel buttons
	Q3HBoxLayout *buttonBox = new Q3HBoxLayout();
	QPushButton *okPushButton = new QPushButton("Ok", this);
	QPushButton *cancelPushButton = new QPushButton("Cancel", this);	
	buttonBox->addWidget(okPushButton);
	buttonBox->addWidget(cancelPushButton);

	verticalBox->addLayout(buttonBox);
	
	connect (okPushButton, SIGNAL(clicked()), this, SLOT(okButtonPressed()));
	connect (cancelPushButton, SIGNAL(clicked()), this, SLOT(cancelButtonPressed()));
	
	//Set up accelerator for return button
	Q3Accel *returnAccel = new Q3Accel( this );
    returnAccel->connectItem( returnAccel->insertItem( Qt::Key_Enter ), this, SLOT(okButtonPressed()));

	//Create busy dialog
	busyDialog = new BusyDialog(this, "Database Manager");
	busyDialog->setModal(true);

	//Set appropriate size
	this->resize(500, 300);
}


/*! Destructor. */
DatabaseManager::~DatabaseManager(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING DATABASE MANAGER"<<endl;
	#endif//MEMORY_DEBUG
}


//---------------------------------------------------------------------------
//-------------------------------- SLOTS ------------------------------------
//---------------------------------------------------------------------------

/*! Hides dialog when cancel button is pressed. */
void DatabaseManager::cancelButtonPressed(){
	this->hide();
}


/*! Checks to see which databases are to be saved or loaded and carries out the 
	appropriate actions. */
void DatabaseManager::okButtonPressed(){
	//Work through the table and fill stack with selected databases
	if(netDBRow != -1){
		if(((Q3CheckTableItem*)dbTable->item(netDBRow, selectionCol))->isChecked()){
			dbStack.push(netDBInterface);
		}
	}
	if(archDBRow != -1){
		if(((Q3CheckTableItem*)dbTable->item(archDBRow, selectionCol))->isChecked()){
			dbStack.push(archDBInterface);
		}
	}
	if(pattDBRow != -1){
		if(((Q3CheckTableItem*)dbTable->item(pattDBRow, selectionCol))->isChecked()){
			dbStack.push(pattDBInterface);
		}
	}
	if(devDBRow != -1){
		if(((Q3CheckTableItem*)dbTable->item(devDBRow, selectionCol))->isChecked()){
			dbStack.push(devDBInterface);
		}
	}

	//Check to see that at least one database is going to be saved
	if(dbStack.empty()){
		if(saveMode)
			QMessageBox::information(this, "Saving Databases", "No databases selected for saving.", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
		else
			QMessageBox::information(this, "Loading Databases", "No databases selected for loading.", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
		return;
	}

	//Save selected databases
	if(saveMode){
		//Show busy dialog
		busyDialog->showDialog("Saving databases, please wait...");

		if(dbStack.size() == 1){
			saveDatabase(dbStack.top(), true);
		}
		else{
			saveDatabase(dbStack.top(), false);
		}
	}
	//Load selected databases from the file
	else{
		//Show busy dialog
		busyDialog->showDialog("Loading databases, please wait...");

		//Load the database at the top of the stack
		loadDatabase(dbStack.top());
	}
}


/*! Slot called when QProcess exits. */
void DatabaseManager::processExited(){
	if(savingDatabase){//Script saving database should have finished
		#ifdef SAVE_DATABASES_DEBUG
			cout<<"DatabaseManager: Saving database script exited"<<endl;
		#endif//SAVE_DATABASES_DEBUG
		savingDatabase = false;

		//No more databases left to save
		if(dbStack.empty()){
			//Hide busy dialog
			busyDialog->hide();
	
			//Check file is there
			if(!QFile::exists(fileName)){
				QMessageBox::critical (this, "Save Error", "Cannot find new file: " + fileName, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
			}

			//Hide dialog
			this->hide();	
		}

		//One more database left to save, so apply compression
		else if(dbStack.size() == 1){
			saveDatabase(dbStack.top(), true);
		}

		//Several databases still to save, so just add to tar archive without compression
		else{
			saveDatabase(dbStack.top(), false);
		}
	}
	else if(loadingDatabase){
		#ifdef LOAD_DATABASES_DEBUG
			cout<<"LoadSpikeStreamDatabase script exited"<<endl;
		#endif//LOAD_DATABASES_DEBUG
		loadingDatabase = false;

		//No more databases left to save
		if(dbStack.empty()){
			//Hide busy dialog
			busyDialog->hide();

			//Hide dialog
			this->hide();	
		}
		//Load next database on stack
		else{
			loadDatabase(dbStack.top());
		}
	}
	else if(listingDatabases){
		listingDatabases = false;
		fillLoadTable();
	}
	else
		cerr<<"DatabaseManager: UNKNOWN PROCESS HAS FINISHED"<<endl;
}


/*! Reads output from stderr generated by process.
	Tries to terminate process. */
void DatabaseManager::readFromStderr(){
	busyDialog->hide();

	//Display error
	QMessageBox::critical (this, "Process Error", "Process has output an error.\nSee command line for more details.", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);

	//Print out current process arguments
	cerr<<"DatabaseManager: PROCESS ERROR: ";
	QStringList processArgs = process->arguments();
	for(unsigned int i=0; i<processArgs.size(); ++i)
                cerr<<processArgs[i].toStdString()<<" ";
	cerr<<endl;

	//Print out error from process
	cerr<<"------------------- Start error message ----------------------"<<endl;
	while(process->canReadLineStderr()){
		QString processString = process->readLineStderr();
                cerr<<processString.toStdString()<<endl;
	}
	cerr<<"-------------------  End error message ----------------------"<<endl;

	//Clean up
	process->tryTerminate();//Politely ask process to quit
	busyDialog->hide();
}


/*! Prints stdout.
	When listing databases this is used to get the list of databases
	available in a file. */
void DatabaseManager::readFromStdout(){
	#ifdef LOAD_DATABASES_DEBUG
		cout<<"DatabaseManager: Normal process output: "<<endl;
	#endif//LOAD_DATABASES_DEBUG
	#ifdef SAVE_DATABASES_DEBUG
		cout<<"DatabaseManager: Normal process output: "<<endl;
	#endif//SAVE_DATABASES_DEBUG
	while(process->canReadLineStdout()){
		QString processString = process->readLineStdout();
		#ifdef LOAD_DATABASES_DEBUG
			cout<<processString<<endl;
		#endif//LOAD_DATABASES_DEBUG
		#ifdef SAVE_DATABASES_DEBUG
			cout<<processString<<endl;
		#endif//SAVE_DATABASES_DEBUG

		if(listingDatabases){
			if(processString == "Start files"){
				storeList = true;
			}
			else if(processString == "End files"){
				storeList = false;
			}
			else if(storeList){
				loadDBList += processString;
			}
		}
	}
}


//----------------------------------------------------------------------------
//-------------------------- PRIVATE METHODS ---------------------------------
//----------------------------------------------------------------------------

/*! Adds the information in the database to the table. */
int DatabaseManager::addDatabaseToTable(DBInterface* dbInter){
	//Add new row to the table
	unsigned int currentRowNumber = dbTable->numRows();
	dbTable->insertRows(currentRowNumber, 1);

	//Add database name
	DBParameters dbParam = dbInter->getDBParameters();
	dbTable->setItem(currentRowNumber, nameCol, new Q3TableItem(dbTable, Q3TableItem::Never, dbParam.database));

	//Add host name
	dbTable->setItem(currentRowNumber, hostCol, new Q3TableItem(dbTable, Q3TableItem::Never, dbParam.host));

	//Add check box to select table
	Q3CheckTableItem *tempCheckTableItem = new Q3CheckTableItem(dbTable, QString(""));
	tempCheckTableItem->setChecked(false);
	dbTable->setItem(currentRowNumber, selectionCol, tempCheckTableItem);

	//Return row number
	return currentRowNumber;
}


/*! Looks at list of databases stored in the file and adds them to the table. */
void DatabaseManager::fillLoadTable(){
	//Get parameters from the databases
	DBParameters netDBParam = netDBInterface->getDBParameters();
	DBParameters archDBParam = archDBInterface->getDBParameters();
	DBParameters pattDBParam = pattDBInterface->getDBParameters();
	DBParameters devDBParam = devDBInterface->getDBParameters();

	//Add name to table if it matches the name of one of the existing databases
	for(unsigned int i=0; i<loadDBList.size(); ++i){
		if(loadDBList[i].section('.', 0, 0) == netDBParam.database)
			netDBRow = addDatabaseToTable(netDBInterface);
		else if(loadDBList[i].section('.', 0, 0) == archDBParam.database)
			archDBRow = addDatabaseToTable(archDBInterface);
		else if(loadDBList[i].section('.', 0, 0) == pattDBParam.database)
			pattDBRow = addDatabaseToTable(pattDBInterface);
		else if (loadDBList[i].section('.', 0, 0) == devDBParam.database)
			devDBRow = addDatabaseToTable(devDBInterface);
	}
}


/*! Uses the DBFileInspector script to list all of the databases in the file. */
void DatabaseManager::getAvailableDatabases(){
	//Add arguments to qprocess
	process->clearArguments();	
	process->addArgument(SpikeStreamMainWindow::workingDirectory + "/scripts/DBFileInspector");

	//File
	process->addArgument(fileName);

	//Start the process
	if ( process->start() ) {
		savingDatabase = false;
		loadingDatabase = false;
		listingDatabases = true;
	}
	else{
		//Error handling
		QString msgString("DBFileInspector failed to start when listing contents of ");
		msgString += fileName;
		QMessageBox::critical (this, "List Error", msgString, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
	}
}


/*! Loads a database from the file using LoadSpikeStreamDatabase script. */
void DatabaseManager::loadDatabase(DBInterface* dbInter){
	//Get information about database
	DBParameters dbParam = dbInter->getDBParameters();

	//Add arguments to qprocess
	process->clearArguments();	
	process->addArgument(SpikeStreamMainWindow::workingDirectory + "/scripts/LoadSpikeStreamDatabase");

	//User argument
	QString userArg("-u");
	userArg += dbParam.user;
	process->addArgument(userArg);

	//Password argument
	QString passwordArg("-p");
	passwordArg += dbParam.password;
	process->addArgument(passwordArg);

	//Host argument
	QString hostArg("-h");
	hostArg += dbParam.host;
	process->addArgument(hostArg);

	//Database
	process->addArgument(dbParam.database);

	//File
	process->addArgument(fileName);

	//Pop the reference to the database off the stack
	dbStack.pop();
	
	//Start the process
	if ( process->start() ) {
		savingDatabase = false;
		loadingDatabase = true;
		listingDatabases = false;
	}
	else{
		//Error handling
		QString msgString("LoadSpikeStreamDatabase script failed to start with database: ");
		msgString += dbParam.database;
		QMessageBox::critical (this, "Load Error", msgString, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
	}
}


/*! Saves a database into the file using SaveSpikeStreamDatabase script.
	If it is the last database to be saved, compress should be set to true
	so that the file is compressed. */
void DatabaseManager::saveDatabase(DBInterface* dbInter, bool compress){

	//Get information about database
	DBParameters dbParam = dbInter->getDBParameters();

	//Add arguments to qprocess
	process->clearArguments();	
	process->addArgument(SpikeStreamMainWindow::workingDirectory + "/scripts/SaveSpikeStreamDatabase");

	//User argument
	QString userArg("-u");
	userArg += dbParam.user;
	process->addArgument(userArg);

	//Password argument
	QString passwordArg("-p");
	passwordArg += dbParam.password;
	process->addArgument(passwordArg);

	//Host argument
	QString hostArg("-h");
	hostArg += dbParam.host;
	process->addArgument(hostArg);

	//Database
	process->addArgument(dbParam.database);

	//File
	process->addArgument(fileNameNoExtension);

	//Compress or not
	if(compress)
		process->addArgument("--compress");

	//Pop the reference to the database off the stack
	dbStack.pop();
	
	//Start the process
	if ( process->start() ) {
		savingDatabase = true;
		loadingDatabase = false;
		listingDatabases = false;
	}
	else{
		//Error handling
		QString msgString("mysqldump failed to start when saving database ");
		msgString += dbParam.database;
		QMessageBox::critical (this, "Save Error", msgString, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
	}
}


