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
#include "ProbeDialog.h"
#include "NeuronGroupType.h"
#include "Utilities.h"
#include "Debug.h"

//Qt includes
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qvalidator.h>
#include <qstring.h>
#include <qregexp.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

//Other includes
#include <iostream>
#include "mysql++.h"
using namespace std;
using namespace mysqlpp;


/*! Constructor */
ProbeDialog::ProbeDialog(QWidget* parent, DBInterface* netDBInter) : QDialog(parent){
	//Store references to database
	networkDBInterface = netDBInter;

	//Create boxes to organise layout of dialog
	Q3VBoxLayout *vBox = new Q3VBoxLayout(this, 5, 5, "Main vertical Box");

	//Create validator for name field
	QRegExp regExp( "([0-9]|[A-Z]|[a-z]|_|\\s){1,50}" );
    QValidator* nameValidator = new QRegExpValidator(regExp, this);

	//Add field, combo and button to add new probes
	Q3HBoxLayout *newProbeBox = new Q3HBoxLayout();
	newProbeBox->addWidget(new QLabel("Name:", this));
	nameText = new QLineEdit("Untitled", this);
	nameText->setValidator(nameValidator);
	nameText->setMaximumSize(200,30);
	newProbeBox->addWidget(nameText);
	probeTypeCombo = new QComboBox(this, "Probe Type");
	probeTypeCombo->setMinimumSize(250,30);
	fillProbeTypeCombo();
	newProbeBox->addWidget(new QLabel("Type:", this));
	newProbeBox->addWidget(probeTypeCombo);
	QPushButton* addProbeButt = new QPushButton("Add Probe", this, "AddProbe");
	addProbeButt->setBaseSize(130, 30);
	addProbeButt->setMaximumSize(130,30);
	addProbeButt->setMinimumSize(130,30);
	addProbeButt->setAccel(QKeySequence(Qt::CTRL + Qt::Key_A));
	connect (addProbeButt, SIGNAL(clicked()), this, SLOT(addProbe()));
	newProbeBox->addWidget(addProbeButt);

	vBox->addLayout(newProbeBox);
	vBox->addSpacing(10);

	//Add delete button
	Q3HBoxLayout *buttonBox = new Q3HBoxLayout();
	QPushButton* deleteProbeButt = new QPushButton("Delete Probe(s)", this, "DeleteProbe");
	deleteProbeButt->setBaseSize(130, 30);
	deleteProbeButt->setMaximumSize(130,30);
	deleteProbeButt->setMinimumSize(130,30);
	deleteProbeButt->setAccel(QKeySequence(Qt::Key_Delete));
	connect (deleteProbeButt, SIGNAL(clicked()), this, SLOT(deleteProbes()));
	buttonBox->addWidget(deleteProbeButt);
	buttonBox->addStretch(5);
	
	vBox->addLayout(buttonBox);
	vBox->addSpacing(10);

	//Set up table to hold pattern information
	probeTable = new Q3Table(0, 5, this);
	probeTable->setShowGrid(false);
	probeTable->setSorting(false);
	probeTable->setSelectionMode(Q3Table::NoSelection);
	probeTable->verticalHeader()->hide();
	probeTable->setLeftMargin(0);
	Q3Header * probeTableHeader = probeTable->horizontalHeader();

	selectionCol = 0;//Remember to update this when changing the table
	probeTableHeader->setLabel(selectionCol, "");
	probeTable->setColumnWidth(selectionCol, 20);

	probeIDCol = 1;//Remember to update this when changing the table
	probeTableHeader->setLabel(probeIDCol, "ID");
	probeTable->setColumnWidth(probeIDCol, 50);
	
	nameCol = 2;
	probeTableHeader->setLabel(nameCol, "Name");
	probeTable->setColumnWidth(nameCol, 280);

	typeDescCol = 3;
	probeTableHeader->setLabel(typeDescCol, "Type");
	probeTable->setColumnWidth(typeDescCol, 280);

	typeCol = 4;
	probeTableHeader->setLabel(typeCol, "TypeID");
	probeTable->setColumnWidth(typeCol, 50);

	//Connect layer table header clicked() slot to select all
	connect (probeTableHeader, SIGNAL(clicked(int)), this, SLOT(probeTableHeaderClicked(int)));

	//Fill probe table with information about current probes.
	loadProbeTable();

	vBox->addWidget(probeTable);

	//Set up ok and cancel buttons
	Q3HBoxLayout *okCancelBox = new Q3HBoxLayout();
	QPushButton *okPushButton = new QPushButton("Ok", this, "okButton");
	QPushButton *cancelPushButton = new QPushButton("Cancel", this, "cancelButton");	
	okCancelBox->addWidget(okPushButton);
	okCancelBox->addWidget(cancelPushButton);
	vBox->addLayout(okCancelBox);
	
	connect (okPushButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect (cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));

	this->setMinimumSize(700, 500);

}


//!Destructor
ProbeDialog::~ProbeDialog(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING PROBE DIALOG"<<endl;
	#endif//MEMORY_DEBUG
}


//---------------------------------------------------------------------
//-----------------          PRIVATE SLOTS           ------------------
//---------------------------------------------------------------------

/*! Adds a new probe to the neuron groups database and hides the dialog */
void ProbeDialog::addProbe(){
	//Sort out probe name
	QString probeName = nameText->text();
	if(probeName == "")
		probeName = "Untitled";

	//Extract probe type
	unsigned int probeType;
	int comboIndex = probeTypeCombo->currentItem();
	if(comboIndex > 0)
		probeType =  typePositionMap[comboIndex];
	else{
		cout<<"ProbeDialog: No probes currently available."<<endl;
		return;
	}

	//Add a new probe of this type to the database
	try{
		Query query = networkDBInterface->getQuery();
		query.reset();
	
		/* Add to NeuronGroups
			For most purposes, a probe acts as another neuron group that sends 
			and receives messages. */
		query.reset();
                query<<"INSERT INTO Probes (Name, Type) VALUES (\""<<probeName.toStdString()<<"\", "<<probeType<<")";
		query.execute();
		
		//Now need to get the automatically generated NeuronGrpID so that it can be added to the neurons in the new layers
		query.reset();
		query<<"SELECT MAX(ProbeID) from Probes";
                StoreQueryResult probeIDResult = query.store();
		Row row(*(probeIDResult.begin()));
		unsigned int probeID = Utilities::getUInt((std::string)row["MAX(ProbeID)"]);
	
		//Get the name of the parameter table for this probe from the database
		query.reset();
		query<<"SELECT ParameterTableName FROM ProbeTypes WHERE TypeID = "<<probeType;
                StoreQueryResult tableNameRes = query.store();
		Row tableNameRow (*tableNameRes.begin());//Should only be 1 row
	
		//Add a row to this table for this probe
		query.reset();
		query<<"INSERT INTO "<<(std::string)tableNameRow["ParameterTableName"]<<" (ProbeID) VALUES ("<<probeID<<")";
		query.execute();
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"ProbeDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when adding probe: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Probe Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"ProbeDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown adding probe: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Probe Error", errorString);
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"ProbeDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown adding probe: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Probe Error", errorString);
	}

	//Reload probe table
	loadProbeTable();
}


/*! Deletes the selected probes from the database */
void ProbeDialog::deleteProbes(){
	QString confirmDeleteStr = "Do you want to delete the following probe(s)?\n";
	map<unsigned int, unsigned int> deleteProbesMap;

	//Get the probe id(s) for the probe(s) to be deleted
	for(int i=0; i<probeTable->numRows(); i++){
		Q3CheckTableItem * item = (Q3CheckTableItem*)probeTable->item(i, selectionCol);
		if(item->isChecked()){
			unsigned int pID = probeTable->item(i, probeIDCol)->text().toUInt();	
			unsigned int pType = probeTable->item(i, typeCol)->text().toUInt();	
			deleteProbesMap[pID] = pType;
			confirmDeleteStr += "   ";
			confirmDeleteStr += probeTable->item(i, nameCol)->text()+= " (ID: ";
			confirmDeleteStr += probeTable->item(i, probeIDCol)->text() += ")\n";
		}
	}

	//Check that user really wants to delete these layers
	if(deleteProbesMap.size() > 0){
		if(!( QMessageBox::warning( this, "Confirm Delete of Probe(s)", confirmDeleteStr, "Yes", "No", 0, 0, 1 ))) {
			//Delete probes from database and table
			Query query = networkDBInterface->getQuery();
			for(map<unsigned int, unsigned int>::iterator iter = deleteProbesMap.begin(); iter != deleteProbesMap.end(); ++iter){
				try{
					//Delete from Probes database
					query.reset();
					query<<"DELETE FROM Probes WHERE ProbeID = "<<iter->first;
					query.execute();
	
					//Delete from probes parameter database
					//Get the name of the parameter table for this probe from the database
					query.reset();
					query<<"SELECT ParameterTableName FROM ProbeTypes WHERE TypeID = "<<iter->second;
                                        StoreQueryResult tableNameRes = query.store();
	
					//Check we have found this type
					if(tableNameRes.size() != 1){
						QString errStr =  "Missing probe type: ";
						errStr += QString::number(iter->second);
						QMessageBox::critical( 0, "Probe Manager", errStr);
						return;
					}
		
					Row tableNameRow (*tableNameRes.begin());//Should only be 1 row
	
					//Delete this probe from its parameter table
					query.reset();
					query<<"DELETE FROM "<<(std::string)tableNameRow["ParameterTableName"]<<" WHERE ProbeID = "<<iter->first;
					query.execute();
				}
				catch (const BadQuery& er) {// Handle any query errors
					cerr<<"ProbeDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
					QString errorString = "Bad query when deleting probe: \"";
					errorString += er.what();
					errorString += "\"";
					QMessageBox::critical( 0, "Delete Probe Error", errorString);
				}
				catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
					cerr<<"ProbeDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
					QString errorString = "Exception thrown deleting probe: \"";
					errorString += er.what();
					errorString += "\"";
					QMessageBox::critical( 0, "Delete Probe Error", errorString);
				}

				//Remove entry from probe table
				removeProbeFromTable(iter->first);
			}
		}
	}
}


/*! Selects or deselects all of the check boxes */
void ProbeDialog::probeTableHeaderClicked(int colNumber){
	if(colNumber == selectionCol){//Header above check boxes has been clicked
		//First count the number of selected rows
		int selectedRowCount = 0;
		for(int i=0; i<probeTable->numRows(); i++){
			Q3CheckTableItem * item = (Q3CheckTableItem*)probeTable->item(i, selectionCol);
			if(item->isChecked())
				selectedRowCount++;
		}
		//If all rows are selected want to deselect rows
		if(selectedRowCount == probeTable->numRows()){
			for(int i=0; i<probeTable->numRows(); i++){//Deselect all rows
				Q3CheckTableItem * item = (Q3CheckTableItem*)probeTable->item(i, selectionCol);
				item->setChecked(false);
			}
		}
		else{//Select all rows
			for(int i=0; i<probeTable->numRows(); i++){
				Q3CheckTableItem * item = (Q3CheckTableItem*)probeTable->item(i, selectionCol);
				item->setChecked(true);
			}
		}
	}
}


//---------------------------------------------------------------------
//-----------------         PRIVATE METHODS          ------------------
//---------------------------------------------------------------------

/*! Fills the probe type combo box with a list of probe types from the 
	ProbeTypes table in the NeuralNetwork database. */
void ProbeDialog::fillProbeTypeCombo(){
	try{
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"SELECT TypeID, Description FROM ProbeTypes";
                StoreQueryResult typeResult = query.store();
		int counter = 0;
                for(StoreQueryResult::iterator iter = typeResult.begin(); iter != typeResult.end(); ++iter){
			Row typeRow(*iter);
			unsigned short typeID = Utilities::getUShort((std::string) typeRow["TypeID"]);
	
			//Store link between neuron type and combo index
			typePositionMap[counter] = typeID;
	
			//Add to combo box
                        string typeDescription = (std::string)typeRow["Description"];
                        typeDescription += " [" + (std::string)typeRow["TypeID"] + "]";
                        probeTypeCombo->insertItem(typeDescription.data());
	
			//Increase counter
			++counter;
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"ProbeDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when getting probe types: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Probe Type Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"ProbeDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown getting probe types: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Probe Type Error", errorString);
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"ProbeDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown getting probe types: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Probe Type Error", errorString);
	}
}


/*! Gets the description corresponding to a particular probe type. */
string ProbeDialog::getDescription(unsigned int pType){
	try{
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"SELECT Description FROM ProbeTypes WHERE TypeID = "<<pType;
                StoreQueryResult result = query.store();
		
		//Should be just one row for each type id
		Row row(*result.begin());
		return (std::string)row["Description"];
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"ProbeDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when getting probe description: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Probe Error", errorString);
		return string("Unknown");
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"ProbeDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown getting probe description: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Probe Error", errorString);
		return string("Unknown");
	}
}


/*! Loads a table with all the available patterns. 
	Enables a pattern to be assigned to a suitable neuron group before 
	the simulation starts*/
void ProbeDialog::loadProbeTable(){
	//Empty table
	probeTable->setNumRows(0);
	
	//Load up a row for each pattern in the database
	try{
		Query probeQuery = networkDBInterface->getQuery();
		probeQuery.reset();
		probeQuery<<"SELECT ProbeID, Name, Type FROM Probes";
                StoreQueryResult result = probeQuery.store();
                for(StoreQueryResult::iterator iter = result.begin(); iter != result.end(); ++iter){
			Row row(*iter);
	
			//Add new row to the table
			int currentRow = probeTable->numRows();
			probeTable->insertRows(currentRow, 1);
		
			//Populate row with pattern information
			Q3CheckTableItem *checkTableItem = new Q3CheckTableItem( probeTable, "");
			probeTable->setItem( currentRow, selectionCol, checkTableItem);
	
			probeTable->setItem(currentRow, probeIDCol,
                                new Q3TableItem(probeTable, Q3TableItem::Never, ((std::string)row["ProbeID"]).data()));//ProbeID
	
			probeTable->setItem(currentRow, nameCol,
                                new Q3TableItem(probeTable, Q3TableItem::Never, ((std::string)row["Name"]).data()));//Description
		
			unsigned int probeType = Utilities::getUInt((std::string)row["Type"]);
			probeTable->setItem(currentRow, typeDescCol,
                                new Q3TableItem(probeTable, Q3TableItem::Never, getDescription(probeType).data()));//Type description
	
			probeTable->setItem(currentRow, typeCol,
				new Q3TableItem(probeTable, Q3TableItem::Never, QString::number(probeType)));//Type ID
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"ProbeDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when loading probes: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Probe Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"ProbeDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading probes: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Probe Error", errorString);
	}
}


/*! Removes the indicated probe from the table. */
void ProbeDialog::removeProbeFromTable(unsigned int probeID){
	for(int i=0; i<probeTable->numRows(); i++){
		unsigned int tempID = probeTable->item(i, probeIDCol)->text().toUInt();
		if(tempID == probeID){
			probeTable->removeRow(i);
			break;
		}
	}
}



