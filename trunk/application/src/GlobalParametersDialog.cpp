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
#include "GlobalParametersDialog.h"
#include "Utilities.h"
#include "Debug.h"

//Qt includes
#include <qlabel.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

//Other includes
#include <mysql++.h>
#include <map>
#include <iostream>
using namespace std;
using namespace mysqlpp;


/*! Constructor. */
GlobalParametersDialog::GlobalParametersDialog(QWidget *parent, DBInterface *netDBInter, SimulationManager *simMan) : QDialog(parent, "GlobParamDlg", false){
	//Store reference to database
	networkDBInterface = netDBInter;

	//Store reference to simulation manager
	simulationManager = simMan;

	//Set caption to dialog
	this->setCaption("Global Parameters");

	//Create a validator to control inputs
	paramValidator = new QDoubleValidator(-100000.0, 100000.0, 4, this);

	//Create a vertical box to organise layout 
	Q3VBoxLayout *verticalBox = new Q3VBoxLayout(this, 5, 10);
	verticalBox->addSpacing(5);

	/* Create a text box and label for each of the parameters in the Global Parameters 
		table that are not boolean */

	/* The descriptions and values are on separate lines, so store them in two temporary
		maps and combine them afterwards */
	map<const char*, const char*, charKeyCompare> tempDescParamMap;
	map<const char*, double, charKeyCompare> tempValueMap;

	try{
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"SHOW COLUMNS FROM GlobalParameters";
                StoreQueryResult showResult = query.store();
                for(StoreQueryResult::iterator iter = showResult.begin(); iter != showResult.end(); ++iter){
			Row showRow(*iter);
	
			//Get the column name
                        QString fieldName(((std::string)showRow["Field"]).data());
		
			//If it is a description of a parameter
			if(fieldName.contains("_desc")){
				//Store key so that it does not vanish from map
				mapKeyStrList += fieldName.section("_", 0, 0);
	
				//Store the description to set up the label later
                                labelMap[mapKeyStrList.last().ascii()] = new QString(((std::string)showRow["Default"]).data());
			}
			else if(fieldName.contains("_val")){
				//Store key so that it does not vanish from map
				mapKeyStrList += fieldName.section("_", 0, 0);
	
	
				//If the value is a boolean, store this fact
                                QString valueType(((std::string)showRow["Type"]).data());
				if(valueType == "tinyint(1)"){//This value will be loaded as a check box
					int defaultValue =  Utilities::getUInt((std::string)showRow["Default"]);
					if(defaultValue == 0)
						boolParameterMap[mapKeyStrList.last().ascii()] = false;
					else if(defaultValue == 1)
						boolParameterMap[mapKeyStrList.last().ascii()] = true;
					else
						cerr<<"GlobalParametersDialog: BOOLEAN VALUE NOT RECOGNIZED: "<<defaultValue<<endl;
					
					//Store in default value map for querying
					defaultValueMap[mapKeyStrList.last().ascii()] = (double)defaultValue;
					
				}
				else{
					//Get the default value for any other data types
					double defaultValue = Utilities::getDouble((std::string)showRow["Default"]);
	
					//Store link between description and default value
					defaultValueMap[mapKeyStrList.last().ascii()] = defaultValue;
				}
			}
		}
	}
    catch (const BadQuery& er) {// Handle any query errors
		cerr<<"GlobalParametersDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when loading information about global parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Global Parameters Error", errorString);
		exit(1);//Critical error
    }
    catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
        cerr<<"GlobalParametersDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown when loading information about global parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Global Parameters Error", errorString);
		exit(1);//Critical error
    }

	//Work through the parameters, and set up dialog
	for(map<const char *, QString*, charKeyCompare>::iterator iter = labelMap.begin(); iter != labelMap.end(); ++iter){

		if(boolParameterMap.count(iter->first)){
			//Create a check box
			QCheckBox *tempCheckBox = new QCheckBox(*iter->second, this);
			checkBoxMap[iter->first] = tempCheckBox;
			Q3HBoxLayout *hBoxLayout = new Q3HBoxLayout();
			hBoxLayout->addSpacing(10);
			hBoxLayout->addWidget(tempCheckBox);
			verticalBox->addLayout(hBoxLayout);
		}
		else {
			//Create line edit
			QLineEdit *parameterText = new QLineEdit(this);
			lineEditMap[iter->first] = parameterText;
	
			//Add a label and text box for this parameter
			addParameter(iter->second, parameterText, verticalBox);
		}
	}
	verticalBox->addSpacing(5);

	//Add buttons to apply and reset the parameters
	Q3HBoxLayout *buttonBox = new Q3HBoxLayout();
	QPushButton *okButton = new QPushButton("Ok", this);
	connect (okButton, SIGNAL(clicked()), this, SLOT(okButtonPressed()));
	buttonBox->addWidget(okButton);

	QPushButton *applyButton = new QPushButton("Apply", this);
	connect (applyButton, SIGNAL(clicked()), this, SLOT(applyButtonPressed()));
	buttonBox->addWidget(applyButton);

	QPushButton *loadDefaultsButton = new QPushButton("Load Defaults", this);
	connect (loadDefaultsButton, SIGNAL(clicked()), this, SLOT(loadDefaultsButtonPressed()));
	buttonBox->addWidget(loadDefaultsButton);

	QPushButton *makeDefaultsButton = new QPushButton("Make Defaults", this);
	connect (makeDefaultsButton, SIGNAL(clicked()), this, SLOT(makeDefaultsButtonPressed()));
	buttonBox->addWidget(makeDefaultsButton);
	
	QPushButton *cancelButton = new QPushButton("Cancel", this);
	connect (cancelButton, SIGNAL(clicked()), this, SLOT(cancelButtonPressed()));
	buttonBox->addWidget(cancelButton);
	verticalBox->addLayout(buttonBox);
}


/*! Destructor. */
GlobalParametersDialog::~ GlobalParametersDialog(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING GLOBAL PARAMETERS DIALOG"<<endl;
	#endif//MEMORY_DEBUG

	//Clean up dynamically allocated QStrings
	for(map<const char *, QString*, charKeyCompare>::iterator iter = labelMap.begin(); iter != labelMap.end(); ++iter)
		delete iter->second;
}


//---------------------------------------------------------------------------
//-----------------------    PUBLIC METHODS    ------------------------------
//---------------------------------------------------------------------------

/*! Extract the current values of the parameters from the database
 	NOTE This assumes that there is at least 1 parameter. */
bool GlobalParametersDialog::loadParameters(){
	try{
		//Generate and execute query
		Query query = networkDBInterface->getQuery();
		query.reset();
		QString queryString ("SELECT ");
		for(map<const char*, double, charKeyCompare>::iterator iter = defaultValueMap.begin(); iter != defaultValueMap.end(); ++iter){
			queryString += iter->first;
			queryString += "_val, ";
		}
		//Trim the last comma and finish off query
		queryString.remove(queryString.length() - 2, 2);
		queryString += " FROM GlobalParameters";
	
		//Execute query
                query<<queryString.toStdString();
                StoreQueryResult valueResult = query.store();
	
		//Check number of rows. Global parameters should only have one row
                if(valueResult.num_rows() == 0){
			cerr<<"Global Parameters table is empty and should have a single row"<<endl;
			QMessageBox::critical( 0, "Global Parameters Error", "Global Parameters table is empty and should have a single row");
			return false;
		}
                else if (valueResult.num_rows() > 1){
			cerr<<"Global Parameters table has more than one row and should only have a single row"<<endl;
			QMessageBox::critical( 0, "Global Parameters Error", "Global Parameters table has more than one row and should only have a single row");
			return false;
		}
		Row valueRow(*valueResult.begin());
	
		//Load up the double parameters
		for(map<const char*, QLineEdit*, charKeyCompare>::iterator iter = lineEditMap.begin(); iter != lineEditMap.end(); ++iter){
			//Get the name of the parameter value
			QString paramName(iter->first);
			paramName += "_val";
	
			//Get the parameter value from the database query
			double currentParamVal = Utilities::getDouble((std::string)valueRow[paramName.ascii()]);
			
			//Set the text on the line edit
			iter->second->setText(QString::number(currentParamVal));
		}
	
		//Load up the check box parameters
		//Load up the double parameters
		for(map<const char*, QCheckBox*, charKeyCompare>::iterator iter = checkBoxMap.begin(); iter != checkBoxMap.end(); ++iter){
			//Get the name of the parameter value
			QString paramName(iter->first);
			paramName += "_val";
	
			//Get the parameter value from the database query
			int currentParamVal = Utilities::getUInt((std::string)valueRow[paramName.ascii()]);
			
			//Set the check box appropriately
			if(currentParamVal == 1)
				iter->second->setChecked(true);
			else if(currentParamVal == 0)
				iter->second->setChecked(false);
			else{
				cerr<<"GlobalParametersDialog: CURRENT VALUE NOT RECOGNIZED"<<currentParamVal<<endl;
				QMessageBox::critical( 0, "Global Parameters Error", "Current parameter value not recognized");
				return false;
			}
		}
	}
    catch (const BadQuery& er) {// Handle any query errors
		cerr<<"GlobalParametersDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when loading global parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Global Parameters Error", errorString);
		return false;
    }
    catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
        cerr<<"GlobalParametersDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown when loading global parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Global Parameters Error", errorString);
		return false;
    }

	//Everything should be ok if we have reached this point.
	return true;
}


//--------------------------------------------------------------------------
//------------------------    SLOTS    -------------------------------------
//--------------------------------------------------------------------------

/*! Stores the global parameters when the apply button is pressed. */
void GlobalParametersDialog::applyButtonPressed(){
	storeParameters();
	simulationManager->setGlobalParameters();
}


/*! Slot connected to cancel button that closes dialog without storing values. */
void GlobalParametersDialog::cancelButtonPressed(){
	reject();
}


/*! Loads up the default parameters. */
void GlobalParametersDialog::loadDefaultsButtonPressed(){
	//Load default double values
	for(map<const char*, QLineEdit*, charKeyCompare>::iterator iter = lineEditMap.begin(); iter != lineEditMap.end(); ++iter){
		iter->second->setText(QString::number(defaultValueMap[iter->first]));
	}

	//Load default boolean values
	for(map<const char*, QCheckBox*, charKeyCompare>::iterator iter = checkBoxMap.begin(); iter != checkBoxMap.end(); ++iter){
		iter->second->setChecked((boolParameterMap[iter->first]));
	}
}


/*! Makes the currently entered parameters the defaults. */
void GlobalParametersDialog::makeDefaultsButtonPressed(){
//FIXME NOT IMPLEMENTED YET
}


/*! Stores the parameters and hides the dialog. */
void GlobalParametersDialog::okButtonPressed(){
	storeParameters();
	simulationManager->setGlobalParameters();
	accept();
}


//-----------------------------------------------------------------------------
//-----------------------        PRIVATE METHODS     --------------------------
//-----------------------------------------------------------------------------

/*! Adds a parameter to the dialog. */
void GlobalParametersDialog::addParameter(QString *labelText, QLineEdit *lineEdit, Q3VBoxLayout *vBox){
	lineEdit->setMaximumSize(50, 20);
	lineEdit->setValidator(paramValidator);
	Q3HBoxLayout *hBoxLayout = new Q3HBoxLayout();
	hBoxLayout->addSpacing(10);
	hBoxLayout->addWidget(new QLabel(*labelText, this));
	hBoxLayout->addWidget(lineEdit);
	hBoxLayout->addStretch(5);
	vBox->addLayout(hBoxLayout);
}


/*! Stores the parameters in the database. */
void GlobalParametersDialog::storeParameters(){
	//Create the first part of the query
	QString queryString = "UPDATE GlobalParameters SET ";
	
	//First add double parameters to query
	for(map<const char*, QLineEdit*, charKeyCompare>::iterator iter = lineEditMap.begin(); iter != lineEditMap.end(); ++iter){
		QString valueName(iter->first);
		valueName += "_val";
		QString valueText = iter->second->text();
		queryString += valueName;
		queryString += " = ";
		queryString += valueText;
		queryString += ", ";
	}

	//Now add any boolean values
	for(map<const char*, QCheckBox*, charKeyCompare>::iterator iter = checkBoxMap.begin(); iter != checkBoxMap.end(); ++iter){
		QString valueName(iter->first);
		valueName += "_val";
		queryString += valueName;
		queryString += " = ";
		if(iter->second->isChecked())
			queryString += "1";
		else
			queryString += "0";
		queryString += ", ";
	}

	//Remove last comma if values have been added
	if(!lineEditMap.empty() || !checkBoxMap.empty())
		queryString.remove(queryString.length() - 2, 2);

	//Execute query
	try{
		Query query = networkDBInterface->getQuery();
		query.reset();
                query<<queryString.toStdString();
		query.execute();
	}
    catch (const BadQuery& er) {// Handle any query errors
		cerr<<"ConnectionWidget: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when storing global parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Global Parameters Error", errorString);
    }
    catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
        cerr<<"ConnectionWidget: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown when storing global parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Global Parameters Error", errorString);
    }
}


