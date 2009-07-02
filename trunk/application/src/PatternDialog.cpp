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
#include "PatternDialog.h"
#include "Utilities.h"
#include "PatternTypes.h"
#include "Debug.h"
#include "BusyDialog.h"
#include "SpikeStreamMainWindow.h"

//Qt includes
#include <qpushbutton.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <q3filedialog.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
using namespace std;
using namespace mysqlpp;


/*! Constructor. */
PatternDialog::PatternDialog(QWidget* parent, DBInterface* pattDBInter) : QDialog(parent){
	//Store reference to database
	patternDBInterface = pattDBInter;

	//Create manager to load and delete patterns
	patternManager = new PatternManager(patternDBInterface);

	//Set caption
	this->setCaption("Pattern Manager");

	//Create box to hold layout
	Q3VBoxLayout *verticalBox = new Q3VBoxLayout(this, 10, 10, "verticalBox");

	//Set up buttons to add and delete patterns
	Q3HBoxLayout *buttonBox = new Q3HBoxLayout();
	QPushButton* addPatternButt = new QPushButton("Add Pattern(s)", this, "AddPattern");
	addPatternButt->setBaseSize(130, 30);
	addPatternButt->setMaximumSize(130,30);
	addPatternButt->setAccel(QKeySequence(Qt::CTRL + Qt::Key_A));
	connect (addPatternButt, SIGNAL(clicked()), this, SLOT(addPattern()));
	buttonBox->addWidget(addPatternButt);
	
	QPushButton* deletePatternButt = new QPushButton("Delete Pattern(s)", this, "DeletePattern");
	deletePatternButt->setBaseSize(130, 30);
	deletePatternButt->setMaximumSize(130,30);
	deletePatternButt->setAccel(QKeySequence(Qt::Key_Delete));
	connect (deletePatternButt, SIGNAL(clicked()), this, SLOT(deletePattern()));
	buttonBox->addWidget(deletePatternButt);
	buttonBox->addStretch(5);
	
	verticalBox->addLayout(buttonBox);

	//Set up table to hold pattern information
	patternTable = new Q3Table(0, 8, this);
	patternTable->setShowGrid(false);
	patternTable->setSorting(false);
	patternTable->setSelectionMode(Q3Table::NoSelection);
	patternTable->verticalHeader()->hide();
	patternTable->setLeftMargin(0);
	Q3Header * patternTableHeader = patternTable->horizontalHeader();
	selectionCol = 0;//Remember to update this when changing the table
	patternTableHeader->setLabel(selectionCol, "");
	patternTable->setColumnWidth(selectionCol, 20);

	patternIDCol = 1;//Remember to update this when changing the table
	patternTableHeader->setLabel(patternIDCol, "ID");
	patternTable->setColumnWidth(patternIDCol, 20);
	
	descriptionCol = 2;
	patternTableHeader->setLabel(descriptionCol, "Description");
	patternTable->setColumnWidth(descriptionCol, 200);

	patternTableHeader->setLabel(3, "Type");
	patternTable->setColumnWidth(3, 80);
	patternTableHeader->setLabel( 4, "Width" );
	patternTable->setColumnWidth( 4, 50);
	patternTableHeader->setLabel( 5, "Length" );
	patternTable->setColumnWidth( 5, 50);
	patternTableHeader->setLabel( 6, "Size" );
	patternTable->setColumnWidth( 6, 50);
	patternTableHeader->setLabel( 7, "File" );
	patternTable->setColumnWidth( 7, 200);

	//Connect layer table header clicked() slot to select all
	connect (patternTableHeader, SIGNAL(clicked(int)), this, SLOT(tableHeaderClicked(int)));

	loadPatternTable();

	verticalBox->addWidget(patternTable);

	//Set up ok and cancel buttons
	Q3HBoxLayout *okCancelBox = new Q3HBoxLayout();
	QPushButton *okPushButton = new QPushButton("Ok", this, "okButton");
	QPushButton *cancelPushButton = new QPushButton("Cancel", this, "cancelButton");	
	okCancelBox->addWidget(okPushButton);
	okCancelBox->addWidget(cancelPushButton);
	verticalBox->addLayout(okCancelBox);
	
	connect (okPushButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect (cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));

	this->setMinimumSize(700, 150);

}


/*! Destructor. */
PatternDialog::~PatternDialog(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING PATTERN DIALOG!"<<endl;
	#endif//MEMORY_DEBUG

	delete patternManager;
}


//----------------------------------------------------------------------------------------------
//-------------------------------------- PRIVATE SLOTS -----------------------------------------
//----------------------------------------------------------------------------------------------

/*! Adds a pattern selected by the user using a file dialog. */
void PatternDialog::addPattern(){
	//Set up default file location
	QString defaultFileLocation = SpikeStreamMainWindow::workingDirectory;
	defaultFileLocation += "/patterns";

	QStringList fileNames = Q3FileDialog::getOpenFileNames("Pattern (*.pat)", defaultFileLocation, this, "Add pattern dialog", "Choose pattern(s) to add to database");
	if(fileNames.size() == 0)
		return;

	try{
		for(unsigned int i=0; i<fileNames.size(); ++i){
			if(!patternManager->addPattern(fileNames[i])){
				QString errorString = "Error adding pattern to database. Check the format of the input file:\n";
				errorString += fileNames[i];
				QMessageBox::critical( 0, "Pattern Manager", errorString);
				break;
			}
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"PatternDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when adding patterns: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Pattern Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"PatternDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown adding patterns: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Pattern Error", errorString);
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"PatternDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown adding patterns: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Pattern Error", errorString);
	}

	//Reload patterns
	loadPatternTable();
	SpikeStreamMainWindow::spikeStrMainWin->reloadPatterns();
}


/*! Deletes a pattern. */
void PatternDialog::deletePattern(){
	QString confirmDeleteStr = "Do you want to delete the following pattern(s)?\n";
	vector<unsigned int> deleteLayerIDs;

	//First get the layer id(s) for the layer(s) to be deleted
	for(int i=0; i<patternTable->numRows(); i++){
		Q3CheckTableItem * item = (Q3CheckTableItem*)patternTable->item(i, selectionCol);
		if(item->isChecked()){
			unsigned int tempInt = patternTable->item(i, patternIDCol)->text().toUInt();
			deleteLayerIDs.push_back(tempInt);
			confirmDeleteStr += "   ";
			confirmDeleteStr += patternTable->item(i, descriptionCol)->text()+= " (ID: ";
			confirmDeleteStr += patternTable->item(i, patternIDCol)->text() += ")\n";
		}
	}

	//Check that user really wants to delete these layers
	if(deleteLayerIDs.size() > 0){
		if(!( QMessageBox::warning( this, "Confirm Delete of Pattern(s)", confirmDeleteStr, "Yes", "No", 0, 0, 1 ))) {
			//Show dialog to inform user that deletion is in progress
			BusyDialog* busyDialog = new BusyDialog(this, "Deleting Patterns");
			busyDialog->setModal(true);
			busyDialog->showDialog("Deleting patterns, please wait.");
			SpikeStreamMainWindow::spikeStreamApplication->processEvents();

			//Delete patterns from database and table
			try{
				vector<unsigned int>::iterator deleteIDsIter;
				for(deleteIDsIter = deleteLayerIDs.begin(); deleteIDsIter != deleteLayerIDs.end(); deleteIDsIter++){
					if(!patternManager->deletePattern(*deleteIDsIter)){
						QString errorString = "Error deleting pattern from database.\nPattern ID: ";
						errorString += QString::number(*deleteIDsIter);
						QMessageBox::critical( 0, "Pattern Manager", errorString);
						return;
					}
					removePatternFromTable(*deleteIDsIter);
				}
			}
			catch (const BadQuery& er) {// Handle any query errors
				cerr<<"PatternDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
				QString errorString = "Bad query when deleting patterns: \"";
				errorString += er.what();
				errorString += "\"";
				QMessageBox::critical( 0, "Pattern Error", errorString);
			}
			catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
				cerr<<"PatternDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
				QString errorString = "Exception thrown deleting patterns: \"";
				errorString += er.what();
				errorString += "\"";
				QMessageBox::critical( 0, "Pattern Error", errorString);
			}
			catch(std::exception& er){// Catch-all for any other exceptions
				cerr<<"PatternDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
				QString errorString = "Exception thrown deleting patterns: \"";
				errorString += er.what();
				errorString += "\"";
				QMessageBox::critical( 0, "Pattern Error", errorString);
			}
			busyDialog->hide();
		}
	}
	SpikeStreamMainWindow::spikeStrMainWin->reloadPatterns();
}


/*! Called when the table header is clicked and selects or deselects all patterns. */
void PatternDialog::tableHeaderClicked(int colNumber){
	if(colNumber == selectionCol){//Header above check boxes has been clicked
		//First count the number of selected rows
		int selectedRowCount = 0;
		for(int i=0; i<patternTable->numRows(); i++){
			Q3CheckTableItem * item = (Q3CheckTableItem*)patternTable->item(i, selectionCol);
			if(item->isChecked())
				selectedRowCount++;
		}
		//If all rows are selected want to deselect rows
		if(selectedRowCount == patternTable->numRows()){
			for(int i=0; i<patternTable->numRows(); i++){//Deselect all rows
				Q3CheckTableItem * item = (Q3CheckTableItem*)patternTable->item(i, selectionCol);
				item->setChecked(false);
			}
		}
		else{//Select all rows
			for(int i=0; i<patternTable->numRows(); i++){
				Q3CheckTableItem * item = (Q3CheckTableItem*)patternTable->item(i, selectionCol);
				item->setChecked(true);
			}
		}
	}
}


//----------------------------------------------------------------------------------------------
//------------------------------------ PRIVATE METHODS -----------------------------------------
//----------------------------------------------------------------------------------------------

/*! Loads a table with all the available patterns. 
	Enables a pattern to be assigned to a suitable neuron group before the simulation starts. */
void PatternDialog::loadPatternTable(){
	//Empty pattern table
	patternTable->setNumRows(0);
	
	//Load up a row for each pattern in the database
	try{
		Query patternQuery = patternDBInterface->getQuery();
		patternQuery.reset();
		patternQuery<<"SELECT PatternGrpID, Description, PatternType, Width, Length, NumberOfPatterns, File FROM PatternDescriptions";
                StoreQueryResult result = patternQuery.store();
                for(StoreQueryResult::iterator iter = result.begin(); iter != result.end(); ++iter){
			Row row(*iter);
			//Add new row to the table
			int currentRow = patternTable->numRows();
			patternTable->insertRows(currentRow, 1);
		
			//Populate row with pattern information
			Q3CheckTableItem *checkTableItem = new Q3CheckTableItem( patternTable, "");
			patternTable->setItem( currentRow, 0, checkTableItem);
	
			patternTable->setItem(currentRow, 1,
                                new Q3TableItem(patternTable, Q3TableItem::Never, ((std::string)row["PatternGrpID"]).data()));//PatternGrpID
	
			patternTable->setItem(currentRow, 2,
                                new Q3TableItem(patternTable, Q3TableItem::Never, ((std::string)row["Description"]).data()));//Description
		
			unsigned int patternType = Utilities::getUInt((std::string)row["PatternType"]);
			patternTable->setItem(currentRow, 3,
				new Q3TableItem(patternTable, Q3TableItem::Never, PatternTypes::getDescription(patternType)));//Pattern Type
		
			patternTable->setItem(currentRow, 4,
                                new Q3TableItem(patternTable, Q3TableItem::Never, ((std::string)row["Width"]).data()));//Width
		
			patternTable->setItem(currentRow, 5,
                                new Q3TableItem(patternTable, Q3TableItem::Never, ((std::string)row["Length"]).data()));//Length
		
			patternTable->setItem(currentRow, 6,
                                new Q3TableItem(patternTable, Q3TableItem::Never, ((std::string)row["NumberOfPatterns"]).data()));//Width
	
			patternTable->setItem(currentRow, 7,
                                new Q3TableItem(patternTable, Q3TableItem::Never, ((std::string)row["File"]).data()));//Width
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"PatternDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when loading patterns: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Pattern Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"PatternDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading patterns: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Pattern Error", errorString);
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"PatternDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading patterns: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Pattern Error", errorString);
	}
}


/*! Removes the indicated pattern from the table. */
void PatternDialog::removePatternFromTable(unsigned int patternID){
	//Remove from table
	for(int i=0; i<patternTable->numRows(); i++){
		unsigned int tempID = patternTable->item(i, patternIDCol)->text().toUInt();
		if(tempID == patternID){
			patternTable->removeRow(i);
			break;
		}
	}
}


