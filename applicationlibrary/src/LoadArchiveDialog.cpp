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
#include "LoadArchiveDialog.h"
#include "NameDialog.h"
#include "Debug.h"
#include "Utilities.h"
#include "ArchiveWidget.h"
#include "SimulationWidget.h"

//Qt includes
#include <qlayout.h>
#include <q3accel.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

//Other includes
#include <mysql++.h>
#include <iostream>
using namespace std;
using namespace mysqlpp;


/*! Constructor. */
LoadArchiveDialog::LoadArchiveDialog(QWidget *parent, DBInterface *archDBInter) : QDialog(parent){
	//Store reference to archive database
	archiveDBInterface = archDBInter;

	//Set caption
    setCaption( "Load Archive" );

	//Create box to organise vertical layout of dialog
	Q3VBoxLayout *mainVerticalBox = new Q3VBoxLayout(this, 5, 10, "Main vertical Box");
	
	//Set up table
	archiveList = new Q3ListBox(this);
	archiveList->setMinimumSize(350, 400);
	archiveList->setBaseSize(350, 400);

	loadArchiveInformation();

	mainVerticalBox->addWidget(archiveList);

	//Set up buttons to rename and delete archives
	Q3HBoxLayout *renameDeletebuttonBox = new Q3HBoxLayout();
	QPushButton *renamePushButton = new QPushButton("Rename", this, "renameButton");
	connect (renamePushButton, SIGNAL(clicked()), this, SLOT(renameButtonPressed()));
	renameDeletebuttonBox->addWidget(renamePushButton);

	QPushButton *deletePushButton = new QPushButton("Delete", this, "deleteButton");
	connect (deletePushButton, SIGNAL(clicked()), this, SLOT(deleteButtonPressed()));
	renameDeletebuttonBox->addWidget(deletePushButton);

	QPushButton *deleteAllPushButton = new QPushButton("Delete all", this, "deleteAllButton");
	if(SimulationManager::isInitialised())
		deleteAllPushButton->setEnabled(false);
	else
		connect (deleteAllPushButton, SIGNAL(clicked()), this, SLOT(deleteAllButtonPressed()));
	renameDeletebuttonBox->addWidget(deleteAllPushButton);
	mainVerticalBox->addLayout(renameDeletebuttonBox);

	//Set up ok and cancel buttons
	Q3HBoxLayout *okCanButtonBox = new Q3HBoxLayout();
	QPushButton *okPushButton = new QPushButton("Ok", this, "okButton");
	QPushButton *cancelPushButton = new QPushButton("Cancel", this, "cancelButton");	
	okCanButtonBox->addWidget(okPushButton);
	okCanButtonBox->addWidget(cancelPushButton);
	mainVerticalBox->addLayout(okCanButtonBox);
	
	connect (okPushButton, SIGNAL(clicked()), this, SLOT(okButtonPressed()));
	connect (cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));
	
	//Set up accelerator for return button
	Q3Accel *returnAccel = new Q3Accel( this );
    returnAccel->connectItem( returnAccel->insertItem( Qt::Key_Enter ), this, SLOT(okButtonPressed()));
}


/*! Destructor. */
LoadArchiveDialog::~ LoadArchiveDialog(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING LOAD ARCHIVE DIALOG"<<endl;
	#endif//MEMORY_DEBUG
}


//-----------------------------------------------------------------------
//--------------------------- PUBLIC METHODS ----------------------------
//-----------------------------------------------------------------------

/*! Returns the start time of the selected archive. */
QString LoadArchiveDialog::getArchiveString(){
	return archiveList->currentText();
}


//-----------------------------------------------------------------------
//---------------------------- SLOTS ------------------------------------
//-----------------------------------------------------------------------

/*! Deletes all the archives. */
void LoadArchiveDialog::deleteAllButtonPressed(){
	if(SimulationManager::isInitialised()){
		cerr<<"LoadArchiveDialog: THIS SHOULD NOT BE CALLED WHEN SIMULATION IS INITIALISED"<<endl;
		return;//This button should be disabled when simulation is initialised
	}

	//Confirm that user wants to delete archives
	int warningConfirmation = QMessageBox::warning (this, "Delete Archive?", "Are you sure that you want to delete *all* the archives?", QMessageBox::Ok,  QMessageBox::Cancel, QMessageBox::NoButton);
	
	//User wants to delete archive
	if(warningConfirmation == QMessageBox::Ok){//1 is returned for cancel and closing message box

		/* Inform other classes that we are deleting archives. 
			ArchiveWidget should be connected to this signal. */
		emit deleteArchiveStarted();

		try{
			//Delete the archive's network model
			Query query = archiveDBInterface->getQuery();
			query.reset();
			query<<"DELETE FROM NetworkModels";
			query.execute();

			//Delete the archive's network data
			query = archiveDBInterface->getQuery();
			query.reset();
			query<<"DELETE FROM NetworkData";
			query.execute();
		}
		catch (const BadQuery& er) {// Handle any query errors
			cerr<<"LoadArchiveDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
			QString errorString = "Bad query when deleting all archives: \"";
			errorString += er.what();
			errorString += "\"";
			QMessageBox::critical( 0, "Archive Error", errorString);
		}
		catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
			cerr<<"LoadArchiveDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
			QString errorString = "Exception thrown when deleting all archives: \"";
			errorString += er.what();
			errorString += "\"";
			QMessageBox::critical( 0, "Archive Error", errorString);
		}
		
		//Reload list of archives in case something went wrong with deletion
		loadArchiveInformation();

		//Inform other classes that we have finished deleting archives
		emit deleteArchiveFinished();
	}
}


/*! Deletes a single archive. */
void LoadArchiveDialog::deleteButtonPressed(){
	//Extract information about archive
	QString archiveText = archiveList->currentText();
	if(archiveText == ""){
		QMessageBox::critical(this, "Delete Archive", "No archive selected");
		return;
	}
	QString archiveStartTime = archiveText.section( "[ ", 1, 1 ); //string should equal "date ]"
	archiveStartTime = archiveStartTime.section( " ]", 0, 0); // string should equal "date"

	//First check that archive is not being accessed by simulation manager
	if(getUnixTime(archiveStartTime) == SimulationWidget::getSimulationStartTime()){
		QString messageText = "Archive \"" + archiveText + "\" is still be accessed\nby the simulation and cannot be deleted.";
		QMessageBox::critical (this, "Delete Archive", messageText, QMessageBox::Ok,  QMessageBox::NoButton, QMessageBox::NoButton);
		return;
	}

	//Confirm that user wants to delete archive
	QString messageText = "Are you sure that you want to delete \"" + archiveText;
	int warningConfirmation = QMessageBox::warning (this, "Delete Archive?", messageText, QMessageBox::Ok,  QMessageBox::Cancel, QMessageBox::NoButton);
	
	//User wants to delete archive
	if(warningConfirmation == QMessageBox::Ok){//1 is returned for cancel and closing message box

		/* Deleting an archive affects the query if there is an archive already loaded
			So have to delete and then re-initialise the query */
		emit deleteArchiveStarted();

		try{
			//Delete the archive's network model
			Query query = archiveDBInterface->getQuery();
			query.reset();
                        query<<"DELETE FROM NetworkModels WHERE StartTime = UNIX_TIMESTAMP(\""<<archiveStartTime.toStdString()<<"\")";
			query.execute();

			//Delete the archive's network data
			query = archiveDBInterface->getQuery();
			query.reset();
                        query<<"DELETE FROM NetworkData WHERE SimulationStartTime = UNIX_TIMESTAMP(\""<<archiveStartTime.toStdString()<<"\")";
			query.execute();
		}
		catch (const BadQuery& er) {// Handle any query errors
			cerr<<"LoadArchiveDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
			QString errorString = "Bad query when deleting archive: \"";
			errorString += er.what();
			errorString += "\"";
			QMessageBox::critical( 0, "Archive Error", errorString);
		}
		catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
			cerr<<"LoadArchiveDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
			QString errorString = "Exception thrown when deleting archive: \"";
			errorString += er.what();
			errorString += "\"";
			QMessageBox::critical( 0, "Archive Error", errorString);
		}
		
		//Reload list of archives
		loadArchiveInformation();

		emit deleteArchiveFinished();
	}
}


/*! Hides the dialog. */
void LoadArchiveDialog::okButtonPressed(){
	accept();
}


/*! Renames an archive. */
void LoadArchiveDialog::renameButtonPressed(){
	//Extract information about archive
	QString archiveText = archiveList->currentText();
	if(archiveText == ""){
		QMessageBox::critical(this, "Rename Archive", "No archive selected");
		return;
	}
	QString archiveName = archiveText.section(" [", 0, 0);
	QString archiveStartTime = archiveText.section( "[ ", 1, 1 ); //string should equal "date ]"
	archiveStartTime = archiveStartTime.section( " ]", 0, 0); // string should equal "date"

	//First check that archive is not being accessed by simulation manager
	if(getUnixTime(archiveStartTime) == SimulationWidget::getSimulationStartTime()){
		QString messageText = "Archive \"" + archiveText + "\" is still be accessed\nby the simulation and cannot be renamed.";
		QMessageBox::critical (this, "Delete Archive", messageText, QMessageBox::Ok,  QMessageBox::NoButton, QMessageBox::NoButton);
		return;
	}

	//Show dialog with current name
	NameDialog *nameDialog = new NameDialog(archiveName, this);
	if (nameDialog->exec() == QDialog::Accepted ) {//Ok button has been pressed
		QString newName = nameDialog->getName();

		//Change name in the database if user wants it changed
		//NOTE THIS USES UNIX_TIMESTAMP, WHICH MAY BE LOSSY AROUND CERTAIN DATES
		if(newName != archiveName){
			try{
				Query query = archiveDBInterface->getQuery();
				query.reset();
                                query<<"UPDATE NetworkModels SET Name = \""<<newName.toStdString()<<"\" WHERE StartTime = UNIX_TIMESTAMP(\'"<<archiveStartTime.toStdString()<<"\')";
				query.execute();
			}
			catch (const BadQuery& er) {// Handle any query errors
				cerr<<"LoadArchiveDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
				QString errorString = "Bad query when renaming archive: \"";
				errorString += er.what();
				errorString += "\"";
				QMessageBox::critical( 0, "Archive Error", errorString);
			}
			catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
				cerr<<"LoadArchiveDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
				QString errorString = "Exception thrown when renaming archive: \"";
				errorString += er.what();
				errorString += "\"";
				QMessageBox::critical( 0, "Archive Error", errorString);
			}

			//Reload list of archives
			loadArchiveInformation();

			/* Let any classes that might be using the archive that its name has changed. */
			emit archiveNameChanged(archiveStartTime, newName);
		}
	}

	//Clean up
	delete nameDialog;
}


//--------------------------------------------------------------------------------
//----------------------------- PRIVATE METHODS ----------------------------------
//--------------------------------------------------------------------------------

/*! Returns the current UNIX time in seconds. */
unsigned int LoadArchiveDialog::getUnixTime(QString &timeString){
	QDateTime dateTime = QDateTime::fromString(timeString, Qt::ISODate);
	return dateTime.toTime_t();
}


/*! Loads up the details about the archives into the archiveTable. */
void LoadArchiveDialog::loadArchiveInformation(){
	//Empty archive list
	archiveList->clear();

	/* Load up information about archives. */
	try{
		Query query = archiveDBInterface->getQuery();
		query.reset();
		query<<"SELECT StartTime, FROM_UNIXTIME(StartTime), Name FROM NetworkModels ORDER BY StartTime";
                StoreQueryResult modelResult = query.store();
                StoreQueryResult::iterator resIter;
		for (resIter = modelResult.begin(); resIter != modelResult.end(); ++resIter) {
			Row row(*resIter);
	
			/* Filter out any network models that do not have data
				Most likely these are here because the simulation is running
				and they will be deleted when the simulation is destroyed if
				no data is added. Also end up with network models that were
				not deleted because of a crash */
			query.reset();
			query<<"SELECT COUNT(*) FROM NetworkData WHERE SimulationStartTime = "<<row["StartTime"];
                        StoreQueryResult dataResult= query.store();
			Row dataRow(*dataResult.begin());//Should only be one row
			unsigned int numberOfRows = Utilities::getUInt((std::string)dataRow["COUNT(*)"]);
			if(numberOfRows > 0){
				//Add new row to the table
				int currentRow = archiveList->count();
                                QString insertionText(((std::string)row["Name"]).data());
				insertionText += "   [ ";
                                insertionText += ((std::string)row["FROM_UNIXTIME(StartTime)"]).data();
				insertionText += " ]";
				archiveList->insertItem(insertionText, currentRow);
			}
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"LoadArchiveDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when loading archive information: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Archive Error", errorString);
		return;
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"LoadArchiveDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading archive information: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Archive Error", errorString);
		return;
	}
	catch(std::exception& er){// Catch-all for std exceptions
		cerr<<"LoadArchiveDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading archive information: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Archive Error", errorString);
		return;
	}

	//Select the most recent item
	archiveList->setSelected(archiveList->count() -1, true);
}


