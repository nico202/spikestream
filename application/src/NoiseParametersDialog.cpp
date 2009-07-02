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
#include "NoiseParametersDialog.h"
#include "Debug.h"
#include "Utilities.h"
#include "SpikeStreamMainWindow.h"

//Qt includes
#include <qlayout.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

//Other includes
#include <math.h>
#include <mysql++.h>
#include <iostream>
using namespace std;
using namespace mysqlpp;


/*! Constructor. */
NoiseParametersDialog::NoiseParametersDialog(QWidget *parent, DBInterface* netDBInter, SimulationManager* simMan) : QDialog(parent, "NoiseParamDlg", false){
	//Store reference to database
	networkDBInterface = netDBInter;

	//Store reference to simulation manager
	simulationManager = simMan;

	//Set caption
	this->setCaption("Noise Parameters");	

	//Create box to organise dialog
	Q3VBoxLayout *verticalBox = new Q3VBoxLayout(this, 2, 2);

	//Create parameter table 
	paramTable = new Q3Table(0, 0, this);
	paramTable->setShowGrid(false);
	paramTable->setSorting(false);
	paramTable->setSelectionMode(Q3Table::NoSelection);
	paramTable->verticalHeader()->hide();
	paramTable->setLeftMargin(0);
	paramTableHeader = paramTable->horizontalHeader();

	//Add up the total width of the columns to set the minimum size
	unsigned int tempTotalColumnWidth = 0;

	//Add column for neuron group id
	unsigned int insertionPoint = paramTable->numCols();
	paramTable->insertColumns(insertionPoint, 1);
	paramTableHeader->setLabel( insertionPoint, "Neuron Group" );
	paramTable->setColumnWidth( insertionPoint, 200);
	neurGrpCol = insertionPoint;
	tempTotalColumnWidth += 200;

	//Add column to set noise mode
	insertionPoint = paramTable->numCols();
	paramTable->insertColumns(insertionPoint, 1);
	paramTableHeader->setLabel(insertionPoint, "Noise" );
	paramTable->setColumnWidth(insertionPoint, 60);
	noiseModeCol = insertionPoint;
	tempTotalColumnWidth += 60;

	//Add column for percent of neurons
	insertionPoint = paramTable->numCols();
	paramTable->insertColumns(insertionPoint, 1);
	paramTableHeader->setLabel(insertionPoint, "Percent of Neurons");
	paramTable->setColumnWidth(insertionPoint, 200);
	neuronPercentCol = insertionPoint;
	tempTotalColumnWidth += 200;

	//Add column to set direct or synaptic firing
	insertionPoint = paramTable->numCols();
	paramTable->insertColumns(insertionPoint, 1);
	paramTableHeader->setLabel(insertionPoint, "Direct / Synaptic Firing");
	paramTable->setColumnWidth(insertionPoint, 200);
	firingModeCol = insertionPoint;
	tempTotalColumnWidth += 200;

	//Add table to layout
	verticalBox->addWidget(paramTable);
	
	//Listen for changes in the check boxes
	connect (paramTable, SIGNAL(valueChanged(int, int)), this, SLOT(paramValueChanged(int, int)));

	//Resize dialog to maximise the number of visible parameters
	if((tempTotalColumnWidth + 10) < 1200)
		this->resize(tempTotalColumnWidth + 10, 300);
	else
		this->resize(1200, 300);

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
NoiseParametersDialog::~NoiseParametersDialog(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING NOISE PARAMETERS DIALOG"<<endl;
	#endif//MEMORY_DEBUG
}


//--------------------------------------------------------------------------------
//----------------------      PUBLIC METHODS       -------------------------------
//--------------------------------------------------------------------------------

/*! Loads up the noise parameters into the table. */
bool NoiseParametersDialog::loadParameters(){
	//Clear the table
	paramTable->setNumRows(0);

	//Parameters are fresh from the database so reset change monitor
	paramValuesChanged = false;

	//Disable apply button - no changes to apply
	applyButton->setEnabled(false);

	//Work through each of the neuron groups and fill up the table
	try{
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"SELECT NeuronGrpID, NoiseEnabled, PercentNeurons, DirectFiring, SynapticWeight FROM NoiseParameters";
                StoreQueryResult noiseParamsRes = query.store();
	
		//Work through results and load noise parameters
                for(StoreQueryResult::iterator noiseParamsIter = noiseParamsRes.begin(); noiseParamsIter != noiseParamsRes.end(); ++noiseParamsIter){
			Row noiseParamsRow(*noiseParamsIter);
	
			//Add new row to the table
			int currentRowNumber = paramTable->numRows();
			paramTable->insertRows(currentRowNumber, 1);
	
			//Get neuron group name from database and add it to the table
			query.reset();
			query<<"SELECT Name FROM NeuronGroups WHERE NeuronGrpID = "<<(std::string)noiseParamsRow["NeuronGrpID"];
                        StoreQueryResult nameResult = query.store();
			Row nameRow = *nameResult.begin();
                        string neurGrpName((std::string)nameRow["Name"] + " [" + (std::string)noiseParamsRow["NeuronGrpID"] + "]");
                        paramTable->setItem(currentRowNumber, neurGrpCol, new Q3TableItem(paramTable, Q3TableItem::Never, neurGrpName.data()));
		
			//Add a check box to enable or disable noise in neuron group
			unsigned int noiseEnabledVal = Utilities::getInt((std::string)noiseParamsRow["NoiseEnabled"]);
			Q3CheckTableItem *tempCheckTableItem = new Q3CheckTableItem(paramTable, QString(""));
			if(noiseEnabledVal == 0)
				tempCheckTableItem->setChecked(false);
			else if(noiseEnabledVal == 1)
				tempCheckTableItem->setChecked(true);
			paramTable->setItem(currentRowNumber, noiseModeCol, tempCheckTableItem);
	
			//Add drop down combo box for the percentage of neurons
			double percentNeurons = Utilities::getDouble((std::string)noiseParamsRow["PercentNeurons"]);
			unsigned int percentNeuronsRow = 0;
			QStringList percentNumbersStrList;
	
			//Add numbers from 0.01 to 5
			percentNumbersStrList += "0.01";
			if(percentNeurons == 0.01)
				percentNeuronsRow = 0;
			percentNumbersStrList += "0.05";
			if(percentNeurons == 0.05)
				percentNeuronsRow = 1;
			percentNumbersStrList += "0.1";
			if(percentNeurons == 0.1)
				percentNeuronsRow = 2;
			percentNumbersStrList += "0.5";
			if(percentNeurons == 0.5)
				percentNeuronsRow = 3;
			percentNumbersStrList += "1";
			if(percentNeurons == 1.0)
				percentNeuronsRow = 4;
			percentNumbersStrList += "5";
			if(percentNeurons == 5.0)
				percentNeuronsRow = 5;
	
			//Add numbers from 10 to 100
			unsigned int percentRowCount = 6;
			for(unsigned int i=10; i<=100; i += 10){
				if(i == percentNeurons)
					percentNeuronsRow = percentRowCount;
				percentNumbersStrList += QString::number(i);
				++percentRowCount;
			}

			//Add random percentage
			percentNumbersStrList += "Random";
			if(percentNeurons == RANDOM_PERCENT_NEURONS_NOISE)//
				percentNeuronsRow = percentRowCount;

			Q3ComboTableItem* tempPercentCombo = new Q3ComboTableItem(paramTable, percentNumbersStrList);
			tempPercentCombo->setCurrentItem(percentNeuronsRow);
			paramTable->setItem(currentRowNumber, neuronPercentCol, tempPercentCombo);
	
			/* Add drop down combo to select between direct firing of neurons or 
				firing of synapses with a particular weight */
			unsigned int directFiringVal = Utilities::getInt((std::string)noiseParamsRow["DirectFiring"]);
			double synapticWeight = Utilities::getDouble((std::string)noiseParamsRow["SynapticWeight"]);
			unsigned int firingModeRow = 0;
			QStringList firingModeStrList;
			firingModeStrList += "Direct";
			unsigned int indexCounter = 1;
			for(double i=-1.0; i<=1.0; i += 0.2){
				if(rint(i*10) == (synapticWeight*10)){//Small errors need to be removed 
					firingModeRow = indexCounter;
				}
				if(rint(i * 10) == 0.0)//QString::number gives a strange very small value for zero
					firingModeStrList += "Synaptic weight 0";
				else
					firingModeStrList += "Synaptic weight " + QString::number(i);
				++indexCounter;
			}
			Q3ComboTableItem* tempFiringModeCombo = new Q3ComboTableItem(paramTable, firingModeStrList);
			if(directFiringVal == 1)
				tempFiringModeCombo->setCurrentItem(0);
			else 
				tempFiringModeCombo->setCurrentItem(firingModeRow);
			paramTable->setItem(currentRowNumber, firingModeCol, tempFiringModeCombo);
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"NoiseParametersDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when loading noise parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Noise Parameters Error", errorString);
		return false;
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"NoiseParametersDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading noise parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Noise Parameters Error", errorString);
		return false;
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"NoiseParametersDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading noise parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Noise Parameters Error", errorString);
		return false;
	}

	//Everything ok up to this point.
	return true;
}


//-------------------------------------------------------------------------------
//------------------------        SLOTS           -------------------------------
//-------------------------------------------------------------------------------

/*! Stores the parameters and instructs tasks to reload them
	without closing the dialog. */
void NoiseParametersDialog::applyButtonPressed(){
	//Store parameters in database
	storeParameters();

	//Instruct simulation manager to send message instructing tasks to load parameters
	simulationManager->setNoiseParameters();

	//Now changes have been applied, reset button and change monitor
	paramValuesChanged = false;
	applyButton->setEnabled(false);
}


/*! Hides the dialog without storing the parameters. */
void NoiseParametersDialog::cancelButtonPressed(){
	this->hide();
}


/*! Loads default noise parameters. */
//FIXME NOT IMPLEMENTED YET!.
void NoiseParametersDialog::defaultsButtonPressed(){
}


/*! Stores the parameters, instructs tasks to reload them and hides the dialog. */
void NoiseParametersDialog::okButtonPressed(){
	if(paramValuesChanged){
		//Store parameters in database
		storeParameters();
	
		//Instruct simulation manager to send message instructing tasks to load parameters
		simulationManager->setNoiseParameters();
	}
	this->hide();
}


/*! Called when a check box or comobo box is altered. */
void NoiseParametersDialog::paramValueChanged(int, int){
	//Enable apply button and set change flag
	paramValuesChanged = true;
	applyButton->setEnabled(true);
}


//----------------------------------------------------------------------------------
//-----------------------       PRIVATE METHODS     --------------------------------
//----------------------------------------------------------------------------------

/*! Stores the noise parameters in the database. */
void NoiseParametersDialog::storeParameters(){
	try{
		Query query = networkDBInterface->getQuery();
	
		/* Declare variables to hold the extracted values
			Everything is an int because this is how MySQL treats bool*/
		unsigned int noiseEnabled, neuronGrpID, directFiring;
		double synapticWeight, percentNeurons;
	
		//Work through parameter table
		for(int rowNum = 0; rowNum < paramTable->numRows(); ++rowNum){
			neuronGrpID = Utilities::getNeuronGrpID(paramTable->item(rowNum, neurGrpCol)->text());
	
			//Extract whether noise mode is enabled
			Q3CheckTableItem * item = (Q3CheckTableItem*)paramTable->item(rowNum, noiseModeCol);
			if(item->isChecked())
				noiseEnabled = 1;
			else
				noiseEnabled = 0;
	
			//Extract the percentage of neurons
			if(paramTable->item(rowNum, neuronPercentCol)->text() == "Random")//Select percentage of neurons at random
				percentNeurons = RANDOM_PERCENT_NEURONS_NOISE;
			else
				percentNeurons = Utilities::getDouble(paramTable->item(rowNum, neuronPercentCol)->text().ascii());
	
			//Extract whether it is direct or synaptic firing of neurons
			QString directFiringStr = paramTable->item(rowNum, firingModeCol)->text();
			if(directFiringStr == "Direct"){
				directFiring = 1;
				synapticWeight = 0;
			}
			else{//Need to extract the synaptic weight
				directFiring = 0;
				QString synWeightStr = directFiringStr.section(" ", 2, 2);
				synapticWeight = Utilities::getDouble(synWeightStr.ascii());
			}
			
			//Write to database
			Query query = networkDBInterface->getQuery();
			query.reset();
			query<<"UPDATE NoiseParameters SET NoiseEnabled = "<<noiseEnabled<<", PercentNeurons = "<<percentNeurons<<", DirectFiring = "<<directFiring<<", SynapticWeight = "<<synapticWeight<<" WHERE NeuronGrpID = "<<neuronGrpID;
			query.execute();
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"NoiseParametersDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when storing noise parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Noise Parameters Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"NoiseParametersDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown storing noise parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Noise Parameters Error", errorString);
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"NoiseParametersDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown storing noise parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Noise Parameters Error", errorString);
	}
}


