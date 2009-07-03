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
#include "NeuronParametersDialog.h"
#include "Debug.h"
#include "Utilities.h"
#include "SpikeStreamMainWindow.h"

//Qt includes
#include <qlayout.h>
#include <qfile.h>
#include <qlabel.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QPixmap>
#include <Q3VBoxLayout>

//Other includes
#include <mysql++.h>
#include <iostream>
using namespace std;
using namespace mysqlpp;


/*! Constructor. */
NeuronParametersDialog::NeuronParametersDialog(QWidget *parent, DBInterface* netDBInter, SimulationManager* simMan) : QDialog(parent, "NeurParamDlg", false){
	//Store reference to database
	networkDBInterface = netDBInter;

	//Store reference to simulation manager
	simulationManager = simMan;

	//Set caption
	this->setCaption("Neuron Parameters");	

	//Set up pixmap that user clicks on to edit parameters
	editPixmap = new QPixmap(SpikeStreamMainWindow::workingDirectory + "/images/edit_parameters.xpm");

	//Record the maximum width of the tables
	unsigned int maxTableWidth = 0;

	//Create box to organise dialog
	Q3VBoxLayout *verticalBox = new Q3VBoxLayout(this, 2, 2);

	//Work through each neuron type and create a parameter table for each type
	try{
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"SELECT TypeID, Description, ParameterTableName FROM NeuronTypes";
                StoreQueryResult neuronTypesResult = query.store();
                for(StoreQueryResult::iterator neurTypesIter = neuronTypesResult.begin(); neurTypesIter != neuronTypesResult.end(); ++neurTypesIter){
			Row neuronTypesRow(*neurTypesIter);
			unsigned short neuronTypeID = Utilities::getUShort((std::string) neuronTypesRow["TypeID"]);
	
			//Create table
			paramTableMap[neuronTypeID] = new ParameterTable(this, neuronTypeID, (std::string)neuronTypesRow["ParameterTableName"]);
			paramTableMap[neuronTypeID]->setShowGrid(false);
			paramTableMap[neuronTypeID]->setSorting(false);
			paramTableMap[neuronTypeID]->setSelectionMode(Q3Table::NoSelection);
			paramTableMap[neuronTypeID]->verticalHeader()->hide();
			paramTableMap[neuronTypeID]->setLeftMargin(0);
			paramTableHeaderMap[neuronTypeID] = paramTableMap[neuronTypeID]->horizontalHeader();
	
			//Add up the total width of the columns to set the minimum size
			unsigned int tempTotalColumnWidth = 0;
	
			//Add column to edit value
			unsigned int insertionPoint = paramTableMap[neuronTypeID]->numCols();
			paramTableMap[neuronTypeID]->insertColumns(insertionPoint, 1);
			paramTableHeaderMap[neuronTypeID]->setLabel( insertionPoint, "Edit" );
			paramTableMap[neuronTypeID]->setColumnWidth( insertionPoint, 30);
			editColumn = insertionPoint;
			tempTotalColumnWidth += 30;
	
			//Add column for neuron group id
			insertionPoint = paramTableMap[neuronTypeID]->numCols();
			paramTableMap[neuronTypeID]->insertColumns(insertionPoint, 1);
			paramTableHeaderMap[neuronTypeID]->setLabel(insertionPoint, "NeuronGrpID" );
			paramTableMap[neuronTypeID]->setColumnWidth(insertionPoint, 90);
			neurGrpIDColumn = insertionPoint;
			tempTotalColumnWidth += 90;
	
			//Add column for neuron group name
			insertionPoint = paramTableMap[neuronTypeID]->numCols();
			paramTableMap[neuronTypeID]->insertColumns(insertionPoint, 1);
			paramTableHeaderMap[neuronTypeID]->setLabel(insertionPoint, "Name");
			paramTableMap[neuronTypeID]->setColumnWidth(insertionPoint, 90);
			nameColumn = insertionPoint;
			tempTotalColumnWidth += 90;
	
			//Store the start position of the parameters
			paramStartColumn = paramTableMap[neuronTypeID]->numCols();
		
			//Add a column for each of the parameters in the parameters table
			//Also store the names of the parameters so that their values can be loaded later
			//Also need to store link between a description and a default value
		
			/* The descriptions and values are on separate lines, so store them in two temporary
				maps and combine them afterwards */
			map<const char*, double, charKeyCompare> tempValueMap;
	
			//Create a string lists to hold the field names and value names
			fieldNamesMap[neuronTypeID] = new QStringList();
			valueNamesMap[neuronTypeID] = new QStringList();
	
			//Get column information from database
			query.reset();
			query<<"SHOW COLUMNS FROM "<<((std::string)neuronTypesRow["ParameterTableName"]);
                        StoreQueryResult showResult = query.store();
                        for(StoreQueryResult::iterator iter = showResult.begin(); iter != showResult.end(); ++iter){
				Row showRow(*iter);
	
				//Get the column name
                                QString fieldName(((std::string)showRow["Field"]).data());
		
				//If it is a description of a parameter
				if(fieldName.contains("_desc")){
					//Store field name to query it later
					(*fieldNamesMap[neuronTypeID]) += fieldName.section("_", 0, 0);
	
					//Add column for parameter
					unsigned int insertionPoint = paramTableMap[neuronTypeID]->numCols();
					paramTableMap[neuronTypeID]->insertColumns(insertionPoint, 1);
                                        QString headerText(((std::string)showRow["Default"]).data());
					tempTotalColumnWidth += headerText.length() * 7;
					paramTableHeaderMap[neuronTypeID]->setLabel( insertionPoint, headerText );
					paramTableMap[neuronTypeID]->setColumnWidth( insertionPoint, headerText.length() * 7);
		
					//Store link between field name and columns
					descriptionColumnMap[neuronTypeID][headerText.ascii()] = insertionPoint;
	
					//Store link between parameter name and a description 
					paramDescMap[neuronTypeID][(*fieldNamesMap[neuronTypeID]).last().ascii()] = paramTableHeaderMap[neuronTypeID]->label(insertionPoint).ascii();
	
					//Stores link between description and parameter name
					descParamMap[neuronTypeID][paramTableHeaderMap[neuronTypeID]->label(insertionPoint).ascii()] = (*fieldNamesMap[neuronTypeID]).last().ascii();
	
				}
				else if(fieldName.contains("_val")){
					//Convert value name to value description
					QString valueName = fieldName.section("_", 0, 0);
				
					//Store keys for querying the database
					(*valueNamesMap[neuronTypeID]) += valueName;
		
					//If the value is a boolean, store this fact
                                        QString valueType(((std::string)showRow["Type"]).data());
					if(valueType == "tinyint(1)")
						checkBoxMap[neuronTypeID][(*valueNamesMap[neuronTypeID]).last().ascii()] = true;//This value will be loaded as a check box
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
				defaultValueMap[neuronTypeID][ paramDescMap[neuronTypeID][iter->first] ] = iter->second;
			}
	
			//See if the width of this table is greater than the maximum width
			tempTotalColumnWidth += 10;
			if(tempTotalColumnWidth > maxTableWidth)
				maxTableWidth = tempTotalColumnWidth;
	
			//Add label for table
			Q3HBoxLayout *labelBox = new Q3HBoxLayout();
                        QString labelString(((std::string)neuronTypesRow["Description"]).data());
			labelString += " Parameters";
			tableLabelMap[neuronTypeID] = new QLabel(labelString, this);
			tableLabelMap[neuronTypeID] ->setPaletteForegroundColor(QColor(0,86,126));
			labelBox->addSpacing(5);
			labelBox->addWidget(tableLabelMap[neuronTypeID]);
			labelBox->addStretch(5);
	
			verticalBox->addLayout(labelBox);
	
			//Add table to layout
			verticalBox->addWidget(paramTableMap[neuronTypeID]);
		
			//Set up table so that clicks on edit pixmap launch edit dialog
			connect (paramTableMap[neuronTypeID], SIGNAL(parameterTableClicked(int, int, unsigned short)), this, SLOT(parameterTableClicked(int, int, unsigned short)));
		
			//Listen for changes in the check boxes
			connect (paramTableMap[neuronTypeID], SIGNAL(parameterTableValueChanged(int, int, unsigned short)), this, SLOT(parameterTableValueChanged(int, int, unsigned short)));
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"NeuronParametersDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when  loading information about neuron parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Parameters Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"NeuronParametersDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading information about neuron parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Parameters Error", errorString);
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"NeuronParametersDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading information about neuron parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Parameters Error", errorString);
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
NeuronParametersDialog::~NeuronParametersDialog(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING NEURON PARAMETERS DIALOG"<<endl;
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

/*! Loads the neuron parameters into the table. */
bool NeuronParametersDialog::loadNeuronParameters(){
	//Clear the tables
	for(map<unsigned short, ParameterTable*>::iterator iter = paramTableMap.begin(); iter != paramTableMap.end(); ++iter)
		iter->second->setNumRows(0);

	//Parameters are fresh from the database so reset change monitor
	parameterValuesChanged = false;

	//Disable apply button - no changes to apply
	applyButton->setEnabled(false);

	//Work through each of the neuron types and fill up the appropriate table
	try{
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"SELECT TypeID, ParameterTableName FROM NeuronTypes";
                StoreQueryResult neuronTypesResult = query.store();
                for(StoreQueryResult::iterator neurTypesIter = neuronTypesResult.begin(); neurTypesIter != neuronTypesResult.end(); ++neurTypesIter){
			Row neuronTypesRow(*neurTypesIter);
			unsigned short neuronTypeID = Utilities::getUShort((std::string) neuronTypesRow["TypeID"]);

			//Generate and execute query for this neuron type 
			query.reset();
			if(fieldNamesMap[neuronTypeID]->size() > 0){
				query<<"SELECT NeuronGrpID, ";
                                for(int i=0; i<fieldNamesMap[neuronTypeID]->size() - 1; ++i){
                                        query<<(*fieldNamesMap[neuronTypeID])[i].toStdString()<<"_val, ";
				}
                                query<<(*fieldNamesMap[neuronTypeID])[fieldNamesMap[neuronTypeID]->size() - 1].toStdString()<<"_val FROM "<<((std::string)neuronTypesRow["ParameterTableName"]);
			}
			else
				query<<"SELECT NeuronGrpID	FROM "<<((std::string)neuronTypesRow["ParameterTableName"]);
                        StoreQueryResult valueResult = query.store();
	
			//Add a row of parameters for each neuron group
                        for(StoreQueryResult::iterator iter = valueResult.begin(); iter != valueResult.end(); ++iter){
				Row valueRow(*iter);
	
				//Add new row to the table
				int currentRowNumber = paramTableMap[neuronTypeID]->numRows();
				paramTableMap[neuronTypeID]->insertRows(currentRowNumber, 1);
		
				//Add pixmap to launch editing dialog
				paramTableMap[neuronTypeID]->setPixmap(currentRowNumber, editColumn, *editPixmap);
		
				//Add neuron group id
                                paramTableMap[neuronTypeID]->setItem(currentRowNumber, neurGrpIDColumn, new Q3TableItem(paramTableMap[neuronTypeID], Q3TableItem::Never, ((std::string)valueRow["NeuronGrpID"]).data()));
		
				//Get neuron group name from database and add it to the table
				query.reset();
				query<<"SELECT Name FROM NeuronGroups WHERE NeuronGrpID = "<<(std::string)valueRow["NeuronGrpID"];
                                StoreQueryResult nameResult = query.store();
				Row nameRow = *nameResult.begin();
                                paramTableMap[neuronTypeID]->setItem(currentRowNumber, nameColumn, new Q3TableItem(paramTableMap[neuronTypeID], Q3TableItem::Never, ((std::string)nameRow["Name"]).data()));
		
				//Work through parameters and load them into the table
                                for(int i=0; i<fieldNamesMap[neuronTypeID]->size(); ++i){
					QString valueQueryName = (*fieldNamesMap[neuronTypeID])[i] + "_val";
					
					if(checkBoxMap[neuronTypeID].count((*fieldNamesMap[neuronTypeID])[i])){//Add a check box for the value
						unsigned int booleanValue = Utilities::getInt((std::string)valueRow[valueQueryName.ascii()]);
						Q3CheckTableItem *tempCheckTableItem = new Q3CheckTableItem( paramTableMap[neuronTypeID], QString(""));
						
						if(booleanValue == 0)
							tempCheckTableItem->setChecked(false);
						else if(booleanValue == 1)
							tempCheckTableItem->setChecked(true);
						paramTableMap[neuronTypeID]->setItem(currentRowNumber, paramStartColumn + i, tempCheckTableItem);
					}
					else{//Add a regular number cell
						paramTableMap[neuronTypeID]->setItem(currentRowNumber, paramStartColumn + i, 
                                                                new Q3TableItem(paramTableMap[neuronTypeID], Q3TableItem::Never, ((std::string)valueRow[valueQueryName.ascii()]).data()));
					}
				}
			}
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"NeuronParametersDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when  loading neuron parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Parameters Error", errorString);
		return false;
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"NeuronParametersDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading neuron parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Parameters Error", errorString);
		return false;
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"NeuronParametersDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading neuron parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Parameters Error", errorString);
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

	//Everything ok up to this point
	return true;
}


//-------------------------------------------------------------------------------
//------------------------        SLOTS           -------------------------------
//-------------------------------------------------------------------------------

/*! Stores the parameters and instructs tasks to reload them without closing
	the dialog. */
void NeuronParametersDialog::applyButtonPressed(){
	//Store parameters in database
	storeNeuronParameters();

	//Instruct simulation manager to send message instructing tasks to load parameters
	simulationManager->setNeuronParameters();

	//Now changes have been applied, reset button and change monitor
	parameterValuesChanged = false;
	applyButton->setEnabled(false);
}


/*! Hides the dialog without storing parameters. */
void NeuronParametersDialog::cancelButtonPressed(){
	this->hide();
}


/*! Loads up the default neuron parameters. */
//FIXME NOT IMPLEMENTED YET!
void NeuronParametersDialog::defaultsButtonPressed(){
}


/*! Stores the parameters, instructs tasks to reload parameters
	and hides dialog. */
void NeuronParametersDialog::okButtonPressed(){
	if(parameterValuesChanged){
		//Store parameters in database
		storeNeuronParameters();
	
		//Instruct simulation manager to send message instructing tasks to load parameters
		simulationManager->setNeuronParameters();
	}
	this->hide();
}


/*! Called when the parameter table is clicked. Launches edit parameters dialog if
	it has been clicked in the edit column. */
void NeuronParametersDialog::parameterTableClicked(int row, int col, unsigned short typeID){
	if(col == editColumn){//Want a list of ConnectionGrpIDs to send to the NetworkViewer
		//Get Neuron Group ID - set value in try-catch block
		unsigned int neuronGrpID = 0;

		//Create a map linking the descriptions with the values
		map<const char*, double, charKeyCompare> descValueMap;

		try{
			neuronGrpID = Utilities::getUInt(paramTableMap[typeID]->item(row, neurGrpIDColumn)->text().ascii());
			for(int i=paramStartColumn; i<paramTableMap[typeID]->numCols(); ++i){
				//Get parameter value if it is not a check box column
				if(checkBoxMap[typeID].count(descParamMap[typeID][paramTableHeaderMap[typeID]->label(i).ascii()]) == 0 ){
					double tempVal = Utilities::getDouble(paramTableMap[typeID]->text(row, i).ascii());
					descValueMap[paramTableHeaderMap[typeID]->label(i).ascii()] = tempVal;
				}
			}
		}
		catch(std::exception& er){// Catch-all for any other exceptions
			cerr<<"NeuronParametersDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
			QString errorString = "Exception thrown loading information for Edit Parameters Dialog: \"";
			errorString += er.what();
			errorString += "\"";
			QMessageBox::critical( 0, "Neuron Parameters Error", errorString);
			return;
		}
		
		//Launch edit neuron parameters dialog to adjust the parameters of this row
		EditNeuronParametersDialog *edParamDlg = new EditNeuronParametersDialog(this, descValueMap, &defaultValueMap[typeID], neuronGrpID);
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


/*! Called when a value has been changed in the tables. Generally called when a check
	box has been checked or unchecked. Records that the table state has changed. */
void NeuronParametersDialog::parameterTableValueChanged(int, int, unsigned short){
	//Learning state may have been changed, so want to enable apply button and change monitor
	parameterValuesChanged = true;
	applyButton->setEnabled(true);
}


//----------------------------------------------------------------------------------
//-----------------------       PRIVATE METHODS     --------------------------------
//----------------------------------------------------------------------------------

/*! Stores the neuron parameters in the database. */
void NeuronParametersDialog::storeNeuronParameters(){
	try{
		Query query = networkDBInterface->getQuery();
	
		//Work through each of the tables of neuron parameters
		for(map<unsigned short, ParameterTable*>::iterator paramTableIter = paramTableMap.begin(); paramTableIter != paramTableMap.end(); ++paramTableIter){
		
			//Work through parameters and load them into the table
			for(int rowNum=0; rowNum< paramTableIter->second->numRows(); ++rowNum){
				QString neurGrpIDText = paramTableIter->second->text(rowNum, neurGrpIDColumn);
				unsigned short neurTypeID = paramTableIter->first;
		
				//Create the first part of the query
				QString queryString = "UPDATE ";
                                queryString += paramTableIter->second->getParameterTableName().data();
				queryString += " SET ";
		
                                for(int i = 0; i < fieldNamesMap[neurTypeID]->size(); ++i){
					QString valueName = (*fieldNamesMap[neurTypeID])[i] + "_val";
					QString valueText = "";
	
					//Determine whether it is a check box column
					if(checkBoxMap[neurTypeID].count((*valueNamesMap[neurTypeID])[i])){
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
				queryString += " WHERE NeuronGrpID = ";
				queryString += neurGrpIDText;
		
				//Execute query
				query.reset();
                                query<<queryString.data();
				query.execute();
			}
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"NeuronParametersDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when storing neuron parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Parameters Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"NeuronParametersDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown storing neuron parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Parameters Error", errorString);
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"NeuronParametersDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown storing neuron parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Parameters Error", errorString);
	}
}


