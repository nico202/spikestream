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
#include "SynapseParametersDialog.h"
#include "Debug.h"
#include "Utilities.h"
#include "SpikeStreamMainWindow.h"

//Qt includes
#include <qlayout.h>
#include <qfile.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QPixmap>
#include <QLabel>
#include <Q3VBoxLayout>

//Other includes
#include <mysql++.h>
#include <iostream>
using namespace std;
using namespace mysqlpp;


/*! Constructor. */
SynapseParametersDialog::SynapseParametersDialog(QWidget *parent, DBInterface* netDBInter, SimulationManager* simMan) : QDialog(parent, "SynParamDlg", false){
	//Store reference to database
	networkDBInterface = netDBInter;

	//Store reference to simulation manager
	simulationManager = simMan;

	//Set caption
	this->setCaption("Synapse Parameters");	

	//Set up pixmap that user clicks on to edit parameters
	editPixmap = new QPixmap(SpikeStreamMainWindow::workingDirectory + "/images/edit_parameters.xpm");

	//Record the maximum width of the tables
	unsigned int maxTableWidth = 0;

	//Create box to organise dialog
	Q3VBoxLayout *verticalBox = new Q3VBoxLayout(this, 2, 2);

	//Work through each synapse type and create a parameter table for each type
	try{
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"SELECT TypeID, Description, ParameterTableName FROM SynapseTypes";
                StoreQueryResult synapseTypesResult = query.store();
                for(StoreQueryResult::iterator synTypesIter = synapseTypesResult.begin(); synTypesIter != synapseTypesResult.end(); ++synTypesIter){
			Row synapseTypesRow(*synTypesIter);
			unsigned short synapseTypeID = Utilities::getUShort((std::string) synapseTypesRow["TypeID"]);
	
			//Create table
			paramTableMap[synapseTypeID] = new ParameterTable(this, synapseTypeID, (std::string)synapseTypesRow["ParameterTableName"]);
			paramTableMap[synapseTypeID]->setShowGrid(false);
			paramTableMap[synapseTypeID]->setSorting(false);
			paramTableMap[synapseTypeID]->setSelectionMode(Q3Table::NoSelection);
			paramTableMap[synapseTypeID]->verticalHeader()->hide();
			paramTableMap[synapseTypeID]->setLeftMargin(0);
			paramTableHeaderMap[synapseTypeID] = paramTableMap[synapseTypeID]->horizontalHeader();
	
			//Add up the total width of the columns to set the minimum size
			unsigned int tempTotalColumnWidth = 0;
	
			//Add column to edit value
			unsigned int insertionPoint = paramTableMap[synapseTypeID]->numCols();
			paramTableMap[synapseTypeID]->insertColumns(insertionPoint, 1);
			paramTableHeaderMap[synapseTypeID]->setLabel( insertionPoint, "Edit" );
			paramTableMap[synapseTypeID]->setColumnWidth( insertionPoint, 30);
			editColumn = insertionPoint;
			tempTotalColumnWidth += 30;
	
			//Add column for connection group id
			insertionPoint = paramTableMap[synapseTypeID]->numCols();
			paramTableMap[synapseTypeID]->insertColumns(insertionPoint, 1);
			paramTableHeaderMap[synapseTypeID]->setLabel(insertionPoint, "ConnGrpID" );
			paramTableMap[synapseTypeID]->setColumnWidth(insertionPoint, 70);
			connGrpIDColumn = insertionPoint;
			tempTotalColumnWidth += 70;
	
			//Add column for from neuron group
			insertionPoint = paramTableMap[synapseTypeID]->numCols();
			paramTableMap[synapseTypeID]->insertColumns(insertionPoint, 1);
			paramTableHeaderMap[synapseTypeID]->setLabel(insertionPoint, "From");
			paramTableMap[synapseTypeID]->setColumnWidth(insertionPoint, 120);
			fromGrpColumn = insertionPoint;
			tempTotalColumnWidth += 120;
	
			//Add column for to neuron group
			insertionPoint = paramTableMap[synapseTypeID]->numCols();
			paramTableMap[synapseTypeID]->insertColumns(insertionPoint, 1);
			paramTableHeaderMap[synapseTypeID]->setLabel(insertionPoint, "To");
			paramTableMap[synapseTypeID]->setColumnWidth(insertionPoint, 120);
			toGrpColumn = insertionPoint;
			tempTotalColumnWidth += 120;
	
	
			//Store the start position of the parameters
			paramStartColumn = paramTableMap[synapseTypeID]->numCols();
		
			/* Add a column for each of the parameters in the parameters table
				Also store the names of the parameters so that their values can be loaded later
				Also need to store link between a description and a default value. */
		
			/* The descriptions and values are on separate lines, so store them in two temporary
				maps and combine them afterwards */
			map<const char*, double, charKeyCompare> tempValueMap;
	
			//Create a string lists to hold the field names and value names
			fieldNamesMap[synapseTypeID] = new QStringList();
			valueNamesMap[synapseTypeID] = new QStringList();
	
			//Do the querying to get the names
			query.reset();
			query<<"SHOW COLUMNS FROM "<<((std::string)synapseTypesRow["ParameterTableName"]);
                        StoreQueryResult showResult = query.store();
                        for(StoreQueryResult::iterator iter = showResult.begin(); iter != showResult.end(); ++iter){
				Row showRow(*iter);
	
				//Get the column name
                                QString fieldName(((std::string)showRow["Field"]).data());
		
				//If it is a description of a parameter
				if(fieldName.contains("_desc")){
					//Store field name to query it later
					(*fieldNamesMap[synapseTypeID]) += fieldName.section("_", 0, 0);
	
					//Add column for parameter
					unsigned int insertionPoint = paramTableMap[synapseTypeID]->numCols();
					paramTableMap[synapseTypeID]->insertColumns(insertionPoint, 1);
                                        QString headerText(((std::string)showRow["Default"]).data());
					tempTotalColumnWidth += headerText.length() * 7;
					paramTableHeaderMap[synapseTypeID]->setLabel( insertionPoint, headerText );
					paramTableMap[synapseTypeID]->setColumnWidth( insertionPoint, headerText.length() * 7);
		
					//Store link between field name and columns
					descriptionColumnMap[synapseTypeID][headerText.ascii()] = insertionPoint;
	
					//Store link between parameter name and a description 
					paramDescMap[synapseTypeID][(*fieldNamesMap[synapseTypeID]).last().ascii()] = paramTableHeaderMap[synapseTypeID]->label(insertionPoint).ascii();
	
					//Stores link between description and parameter name
					descParamMap[synapseTypeID][paramTableHeaderMap[synapseTypeID]->label(insertionPoint).ascii()] = (*fieldNamesMap[synapseTypeID]).last().ascii();
	
				}
				else if(fieldName.contains("_val")){
					//Convert value name to value description
					QString valueName = fieldName.section("_", 0, 0);
				
					//Store keys for querying the database
					(*valueNamesMap[synapseTypeID]) += valueName;
		
					//If the value is a boolean, store this fact
                                        QString valueType(((std::string)showRow["Type"]).data());
					if(valueType == "tinyint(1)")
						checkBoxMap[synapseTypeID][(*valueNamesMap[synapseTypeID]).last().ascii()] = true;//This value will be loaded as a check box
					else{
						//Get the default value for any other data types
						double defaultValue = Utilities::getDouble((std::string)showRow["Default"]);
		
						//Store link between description and default value
						tempValueMap[valueName.ascii()] = defaultValue;
					}
		
				}
			}
		
			/*Have now two maps, one linking parameter names with values and another
				linking parameter names and descriptions. Need to combine them into a
				single default value map */
			for(map<const char*, double>::iterator iter = tempValueMap.begin(); iter != tempValueMap.end(); ++iter){
				defaultValueMap[synapseTypeID][ paramDescMap[synapseTypeID][iter->first] ] = iter->second;
			}
	
			//See if the width of this table is greater than the maximum width
			tempTotalColumnWidth += 10;
			if(tempTotalColumnWidth > maxTableWidth)
				maxTableWidth = tempTotalColumnWidth;
	
			//Add label for table
			Q3HBoxLayout *labelBox = new Q3HBoxLayout();
                        QString labelString(((std::string)synapseTypesRow["Description"]).data());
			labelString += " Parameters";
			tableLabelMap[synapseTypeID] = new QLabel(labelString, this);
			tableLabelMap[synapseTypeID] ->setPaletteForegroundColor(QColor(0,86,126));
			labelBox->addSpacing(5);
			labelBox->addWidget(tableLabelMap[synapseTypeID]);
			labelBox->addStretch(5);
	
			verticalBox->addLayout(labelBox);
	
			//Add table to layout
			verticalBox->addWidget(paramTableMap[synapseTypeID]);
		
			//Set up table so that clicks on edit pixmap launch edit dialog
			connect (paramTableMap[synapseTypeID], SIGNAL(parameterTableClicked(int, int, unsigned short)), this, SLOT(parameterTableClicked(int, int, unsigned short)));
		
			//Listen for changes in the check boxes
			connect (paramTableMap[synapseTypeID], SIGNAL(parameterTableValueChanged(int, int, unsigned short)), this, SLOT(parameterTableValueChanged(int, int, unsigned short)));
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"SynapseParametersDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when  loading information about synapse parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Synapse Parameters Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"SynapseParametersDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading information about synapse parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Synapse Parameters Error", errorString);
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"SynapseParametersDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading information about synapse parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Synapse Parameters Error", errorString);
	}

	
	//Resize dialog to maximise the number of visible parameters
	if(maxTableWidth < 1200)
		this->resize(maxTableWidth, 200);
	else
		this->resize(1200, 200);
	

	//Set up buttons
	Q3HBoxLayout *buttonBox = new Q3HBoxLayout();
	QPushButton *okButton = new QPushButton("Ok", this);
	buttonBox->addWidget(okButton);
	connect (okButton, SIGNAL(clicked()), this, SLOT(okButtonPressed()));

	applyButton = new QPushButton("Apply", this);
	buttonBox->addWidget(applyButton);
	connect (applyButton, SIGNAL(clicked()), this, SLOT(applyButtonPressed()));

	QPushButton *defaultsButton = new QPushButton("Load Defaults", this);
	buttonBox->addWidget(defaultsButton);
	connect (defaultsButton, SIGNAL(clicked()), this, SLOT(defaultsButtonPressed()));

	QPushButton *cancelButton = new QPushButton("Cancel", this);	
	buttonBox->addWidget(cancelButton);
	connect (cancelButton, SIGNAL(clicked()), this, SLOT(cancelButtonPressed()));

	verticalBox->addLayout(buttonBox);
}


/*! Destructor. */
SynapseParametersDialog::~SynapseParametersDialog(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING SYNAPSE PARAMETERS DIALOG"<<endl;
	#endif//MEMORY_DEBUG

	//Delete any class members allocated on heap and not linked to Qt
	delete editPixmap;

	//Clean up the maps holding the field and value names
	for(map<unsigned short, QStringList*>::iterator iter = fieldNamesMap.begin(); iter != fieldNamesMap.end(); ++iter)
		delete iter->second;

	for(map<unsigned short, QStringList*>::iterator iter = valueNamesMap.begin(); iter != valueNamesMap.end(); ++iter)
		delete iter->second;
}


//--------------------------------------------------------------------------------
//----------------------      PUBLIC METHODS       -------------------------------
//--------------------------------------------------------------------------------

/*! Loads up the synapse parameters into the table. */
bool SynapseParametersDialog::loadSynapseParameters(){
	//Clear the tables
	for(map<unsigned short, ParameterTable*>::iterator iter = paramTableMap.begin(); iter != paramTableMap.end(); ++iter)
		iter->second->setNumRows(0);

	//Parameters are fresh from the database so reset change monitor
	parameterValuesChanged = false;

	//Disable apply button - no changes to apply
	applyButton->setEnabled(false);

	//Work through each of the synapse types and fill up the appropriate table
	try{
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"SELECT TypeID, ParameterTableName FROM SynapseTypes";
                StoreQueryResult synapseTypesResult = query.store();
                for(StoreQueryResult::iterator synTypesIter = synapseTypesResult.begin(); synTypesIter != synapseTypesResult.end(); ++synTypesIter){
			Row synapseTypesRow(*synTypesIter);
			unsigned short synapseTypeID = Utilities::getUShort((std::string) synapseTypesRow["TypeID"]);
	
			//Generate and execute query for this synapse type 
			query.reset();
			query<<"SELECT ConnGrpID, ";
			for(unsigned int i=0; i<fieldNamesMap[synapseTypeID]->size() - 1; ++i){
                                query<<(*fieldNamesMap[synapseTypeID])[i].toStdString()<<"_val, ";
			}
                        query<<(*fieldNamesMap[synapseTypeID])[fieldNamesMap[synapseTypeID]->size() - 1].toStdString()<<"_val FROM "<<((std::string)synapseTypesRow["ParameterTableName"]);
                        StoreQueryResult valueResult = query.store();
	
			//Add a row of parameters for each synapse group
                        for(StoreQueryResult::iterator iter = valueResult.begin(); iter != valueResult.end(); ++iter){
				Row valueRow(*iter);
	
				//Add new row to the table
				int currentRowNumber = paramTableMap[synapseTypeID]->numRows();
				paramTableMap[synapseTypeID]->insertRows(currentRowNumber, 1);
		
				//Add pixmap to launch editing dialog
				paramTableMap[synapseTypeID]->setPixmap(currentRowNumber, editColumn, *editPixmap);
		
				//Add synapse group id
                                paramTableMap[synapseTypeID]->setItem(currentRowNumber, connGrpIDColumn, new Q3TableItem(paramTableMap[synapseTypeID], Q3TableItem::Never, ((std::string)valueRow["ConnGrpID"]).data()));
	
				//Extract the from and to neuron group names for this connection group
				query.reset();
				query<<"SELECT FromNeuronGrpID, ToNeuronGrpID FROM ConnectionGroups WHERE ConnGrpID = "<<(std::string)valueRow["ConnGrpID"];
                                StoreQueryResult fromToIDRes = query.store();
				Row fromToIDRow(*fromToIDRes.begin());
		
				//Add from neuron group to table
                                paramTableMap[synapseTypeID]->setItem(currentRowNumber, fromGrpColumn, new Q3TableItem(paramTableMap[synapseTypeID], Q3TableItem::Never, getNeuronGroupName(QString(((std::string)fromToIDRow["FromNeuronGrpID"]).data()))));
		
				//Add to neuron group to table
                                paramTableMap[synapseTypeID]->setItem(currentRowNumber, toGrpColumn, new Q3TableItem(paramTableMap[synapseTypeID], Q3TableItem::Never, getNeuronGroupName(QString(((std::string)fromToIDRow["ToNeuronGrpID"]).data()))));
		
				//Work through parameters and load them into the table
				for(unsigned int i=0; i<fieldNamesMap[synapseTypeID]->size(); ++i){
					QString valueQueryName = (*fieldNamesMap[synapseTypeID])[i] + "_val";
					
					if(checkBoxMap[synapseTypeID].count((*fieldNamesMap[synapseTypeID])[i])){//Add a check box for the value
						unsigned int booleanValue = Utilities::getInt((std::string)valueRow[valueQueryName.ascii()]);
						Q3CheckTableItem *tempCheckTableItem = new Q3CheckTableItem( paramTableMap[synapseTypeID], QString(""));
						
						if(booleanValue == 0)
							tempCheckTableItem->setChecked(false);
						else if(booleanValue == 1)
							tempCheckTableItem->setChecked(true);
						paramTableMap[synapseTypeID]->setItem(currentRowNumber, paramStartColumn + i, tempCheckTableItem);
					}
					else{//Add a regular number cell
						paramTableMap[synapseTypeID]->setItem(currentRowNumber, paramStartColumn + i, 
                                                                new Q3TableItem(paramTableMap[synapseTypeID], Q3TableItem::Never, ((std::string)valueRow[valueQueryName.ascii()]).data()));
					}
				}
			}
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"SynapseParametersDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when  loading synapse parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Synapse Parameters Error", errorString);
		return false;
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"SynapseParametersDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading synapse parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Synapse Parameters Error", errorString);
		return false;
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"SynapseParametersDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading synapse parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Synapse Parameters Error", errorString);
		return false;
	}


	//Hide tables that have no entries in them
	for(map<unsigned short, ParameterTable*>::iterator iter = paramTableMap.begin(); iter != paramTableMap.end(); ++iter){
		if(iter->second->numRows() <= 0){
			iter->second->hide();
			tableLabelMap[iter->first]->hide();
		}
		else{
			iter->second->show();
			tableLabelMap[iter->first]->show();
		}
	}

	//Everything should be ok if we have reached this point
	return true;
}


//-------------------------------------------------------------------------------
//------------------------        SLOTS           -------------------------------
//-------------------------------------------------------------------------------

/*! Stores the parameters without closing the dialog. */
void SynapseParametersDialog::applyButtonPressed(){
	//Store parameters in database
	storeSynapseParameters();

	//Instruct simulation manager to send message instructing tasks to load parameters
	simulationManager->setSynapseParameters();

	//Now changes have been applied, reset button and change monitor
	parameterValuesChanged = false;
	applyButton->setEnabled(false);
}


/*! Hides the dialog when the cancel button is pressed. */
void SynapseParametersDialog::cancelButtonPressed(){
	this->hide();
}


/*! Loads the default parameters into the table. */
//FIXME NOT IMPLEMENTED YET
void SynapseParametersDialog::defaultsButtonPressed(){
}


/*! Stores the parameters and closes the dialog. */
void SynapseParametersDialog::okButtonPressed(){
	if(parameterValuesChanged){
		//Store parameters in database
		storeSynapseParameters();
	
		//Instruct simulation manager to send message instructing tasks to load parameters
		simulationManager->setSynapseParameters();
	}
	this->hide();
}


/*! Launches a dialog to edit the parameters.
	This changes the values in the table, but they are not updated in the database
	until ok or apply is clicked. */
void SynapseParametersDialog::parameterTableClicked(int row, int col, unsigned short typeID){
	if(col == editColumn){//Want a list of ConnectionGrpIDs to send to the NetworkViewer
		//Get Connection Group ID - set value in try-catch block
		unsigned int connGrpID = 0;

		//Create a map linking the descriptions with the values
		map<const char*, double, charKeyCompare> descValueMap;

		try{
			connGrpID = Utilities::getUInt(paramTableMap[typeID]->item(row, connGrpIDColumn)->text().ascii());
			for(int i=paramStartColumn; i<paramTableMap[typeID]->numCols(); ++i){
				//Get parameter value if it is not a check box column
				if(checkBoxMap[typeID].count(descParamMap[typeID][paramTableHeaderMap[typeID]->label(i).ascii()]) == 0 ){
					double tempVal = Utilities::getDouble(paramTableMap[typeID]->text(row, i).ascii());
					descValueMap[paramTableHeaderMap[typeID]->label(i).ascii()] = tempVal;
				}
			}
		}
		catch(std::exception& er){// Catch-all for any other exceptions
			cerr<<"SynapseParametersDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
			QString errorString = "Exception thrown loading information for Edit Parameters Dialog: \"";
			errorString += er.what();
			errorString += "\"";
			QMessageBox::critical( 0, "Synapse Parameters Error", errorString);
			return;
		}

		//Launch edit synapse parameters dialog to adjust the parameters of this row
		EditSynapseParametersDialog *edParamDlg = new EditSynapseParametersDialog(this, descValueMap, &defaultValueMap[typeID], connGrpID);
		if ( edParamDlg->exec() == QDialog::Accepted ) {
			//Get the map linking parameter descriptions to line edits
			map<const char*, QLineEdit*, charKeyCompare> *tempMap = edParamDlg->getDescriptionLineEditMap();

			//Work through map
			for(map<const char*, QLineEdit*>::iterator iter = tempMap->begin(); iter != tempMap->end(); ++iter){
				//Get the parameter text
				QString parameterText = iter->second->text();

				//Get the position of this column in the table
				int columnNumber = descriptionColumnMap[typeID][iter->first];

				//Set the value in the table
				paramTableMap[typeID]->setItem(row, columnNumber, new Q3TableItem(paramTableMap[typeID], Q3TableItem::Never, parameterText));
			}

			//Parameters have been changed
			parameterValuesChanged = true;
		
			//Enable apply button
			applyButton->setEnabled(true);
		}

		//Clean up
		delete edParamDlg;
	}
}


/*! Tracks whether a check box has been checked or cleared in the table. */
void SynapseParametersDialog::parameterTableValueChanged(int, int, unsigned short){
	//Learning state may have been changed, so want to enable apply button and change monitor
	parameterValuesChanged = true;
	applyButton->setEnabled(true);
}


//----------------------------------------------------------------------------------
//-----------------------       PRIVATE METHODS     --------------------------------
//----------------------------------------------------------------------------------

/*! Gets the name of the neuron group given the neuron group id. */
QString SynapseParametersDialog::getNeuronGroupName(QString neuronGrpID){
	QString neuronGrpName("");
	try{
		Query query = networkDBInterface->getQuery();
		query.reset();
                query<<"SELECT Name FROM NeuronGroups WHERE NeuronGrpID = "<<neuronGrpID.toStdString();
                StoreQueryResult tempResult = query.store();
		Row tempFromRow(*tempResult.begin());
                neuronGrpName = ( (std::string)tempFromRow["Name"] + " [").data();
		neuronGrpName += neuronGrpID + "]";
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"SynapseParametersDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when getting neuron group name: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Group Error", errorString);
		return QString("Unknown");
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"SynapseParametersDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown getting neuron group name: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Group Error", errorString);
		return QString("Unknown");
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"SynapseParametersDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown getting neuron group name: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Group Error", errorString);
		return QString("Unknown");
	}
	return neuronGrpName;
}


/*! Stores the synapse parameters in the database. */
void SynapseParametersDialog::storeSynapseParameters(){
	try{
		Query query = networkDBInterface->getQuery();
	
		//Work through each of the tables of synapse parameters
		for(map<unsigned short, ParameterTable*>::iterator paramTableIter = paramTableMap.begin(); paramTableIter != paramTableMap.end(); ++paramTableIter){
		
			//Work through parameters and load them into the table
			for(int rowNum=0; rowNum< paramTableIter->second->numRows(); ++rowNum){
				QString connGrpIDText = paramTableIter->second->text(rowNum, connGrpIDColumn);
				unsigned short synTypeID = paramTableIter->first;
		
				//Create the first part of the query
				QString queryString = "UPDATE ";
                                queryString += paramTableIter->second->getParameterTableName().data();
				queryString += " SET ";
		
				for(unsigned int i = 0; i < fieldNamesMap[synTypeID]->size(); ++i){
					QString valueName = (*fieldNamesMap[synTypeID])[i] + "_val";
					QString valueText = "";
	
					//Determine whether it is a check box column
					if(checkBoxMap[synTypeID].count((*valueNamesMap[synTypeID])[i])){
						Q3CheckTableItem * item = (Q3CheckTableItem*)paramTableIter->second->item(rowNum, i + paramStartColumn);
						if(item->isChecked())
							valueText = "1";
						else
							valueText = "0";
					}
					else {
						valueText = paramTableIter->second->text(rowNum, i+paramStartColumn);
					}
	
					queryString += valueName;
					queryString += " = ";
					queryString += valueText;
					queryString += ", ";
				}
	
				//Trim last comma and space
				queryString.remove(queryString.length() - 2, 2);
	
				//Finish off query string
				queryString += " WHERE ConnGrpID = ";
				queryString += connGrpIDText;
		
				//Execute query
				query.reset();
                                query<<queryString.toStdString();
				query.execute();
			}
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"SynapseParametersDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when storing synapse parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Synapse Parameters Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"SynapseParametersDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown storing synapse parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Synapse Parameters Error", errorString);
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"SynapseParametersDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown storing synapse parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Synapse Parameters Error", errorString);
	}
}



