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
#include "ArchiveStatisticsDialog.h"
#include "Utilities.h"

//Qt includes
#include <q3buttongroup.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qvalidator.h>
#include <qregexp.h>
#include <qlabel.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

//Other includes
#include <iostream>
using namespace std;


/*! Main Constructor. */
ArchiveStatisticsDialog::ArchiveStatisticsDialog(QWidget* parent, vector<NeuronGroup*> neurGrpVect) : QDialog(parent) {
	//Store neuron group vector
	neuronGrpVector = neurGrpVect;

	//Set up the graphical components of the dialog
	initialiseDialog();
}


/*! Constructor used when editing the archive statistics parameters. */
ArchiveStatisticsDialog::ArchiveStatisticsDialog(QWidget* parent, vector<NeuronGroup*> neurGrpVect, ArchiveStatisticsHolder* archStatsHold) : QDialog(parent) {
	//Store neuron group vector
	neuronGrpVector = neurGrpVect;

	//Set up the graphical components of the dialog
	initialiseDialog();

	//Set the name of the archive statistics
        archStatsNameText->setText(archStatsHold->description.data());
	
	//If the archive statistics is a neuron group, set up the dialog appropriately.
	if(archStatsHold->archStatVector.size() == 1 && archStatsHold->archStatVector[0]->getType() == ArchiveStatistic::NEURON_GROUP){
		//Top radio button should be on by default, so just have to set the correct location in the combo box.
		unsigned int tmpNeurGrpID = ((NeuronGrpArchiveStatistic*)archStatsHold->archStatVector[0])->getNeuronGrpID();
		for(int i=0; i < neuronGrpCombo->count(); ++i){
			try{
                                unsigned int comboNeurGrpID = Utilities::getNeuronGrpID(neuronGrpCombo->text(i).toStdString());
				if(comboNeurGrpID == tmpNeurGrpID){
					neuronGrpCombo->setCurrentItem(i);
					return;
				}
			}
			catch(std::exception& er){// Catch-all for all std exceptions
				cerr<<"ArchiveStatisticsDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
				QString errorString = "Exception thrown getting neuron group id: \"";
				errorString += er.what();
				errorString += "\"";
				QMessageBox::critical( 0, "NeuronGrpID Error", errorString);
				return;
			}
		}
		//There is an error if we have reached this point
		cerr<<"ArchiveStatisticsDialog: COULD NOT FIND NEURON GROUP ID "<<tmpNeurGrpID<<" IN COMBO BOX."<<endl;
		QMessageBox::critical( 0, "NeuronGrpID Error", "Could not find neuron group ID");
		return;
	}
	/* Otherwise, need to add the appropriate information to the text field, which can be 
		extracted automatically from each class.*/
	else{
		//Set the neuron id radio button on
		neurIDRadioButt->setChecked(true);
		neurIDText->setEnabled(true);
		neuronGrpCombo->setEnabled(false);

		//Add the strings to the text area.
		QString neurIDStr = "";
		for(vector<ArchiveStatistic*>::iterator iter = archStatsHold->archStatVector.begin(); iter != archStatsHold->archStatVector.end(); ++iter){
			neurIDStr += (*iter)->toQString();
			if((iter + 1) != archStatsHold->archStatVector.end())
				neurIDStr += ", ";
		}
		neurIDText->setText(neurIDStr);
	}
}


/*! Sets up the dialog. This is placed in a separate method because there are two
	different constructors. */
void ArchiveStatisticsDialog::initialiseDialog(){

	//Set title of the dialog
	this->setCaption("Statistics");

	//Create vertical layout to organise widget
	Q3VBoxLayout *mainVerticalBox = new Q3VBoxLayout(this, 5, 10, "vertical1");

	//Add a dialog to get a desription of the statistic
	QRegExp regExp( "([0-9]|[A-Z]|[a-z]|_|\\s){1,50}" );
	QValidator* archStatsNameValidator = new QRegExpValidator(regExp, this);
	Q3HBoxLayout *archStatsNameBox = new Q3HBoxLayout();
	archStatsNameBox->addSpacing(10);
	archStatsNameBox->addWidget(new QLabel("Name", this));
	archStatsNameText = new QLineEdit("UNTITLED", this);
	archStatsNameText->setValidator(archStatsNameValidator);
	archStatsNameBox->addWidget(archStatsNameText);
	archStatsNameBox->addSpacing(5);
	mainVerticalBox->addLayout(archStatsNameBox);

	//Set up radio buttons to choose between a neuron group or a list of neurons
	Q3ButtonGroup* buttonGroup = new Q3ButtonGroup();
	neurGrpRadioButt = new QRadioButton("Neuron group", this);
	neurGrpRadioButt->setChecked(true);
	connect (neurGrpRadioButt, SIGNAL(clicked()), this, SLOT(neurGrpRadioButtonClicked()));
	buttonGroup->insert(neurGrpRadioButt);
	mainVerticalBox->addWidget(neurGrpRadioButt);

	//Add combo to select neuron group
	Q3HBoxLayout* neurGrpBox = new Q3HBoxLayout();
	neuronGrpCombo = new QComboBox(this);
	for(vector<NeuronGroup*>::iterator iter = neuronGrpVector.begin(); iter != neuronGrpVector.end(); ++iter){
		QString tmpNeurGrpName = (*iter)->name;
		tmpNeurGrpName += " [";
		tmpNeurGrpName += QString::number((*iter)->neuronGrpID);
		tmpNeurGrpName += "]";
		neuronGrpCombo->insertItem(tmpNeurGrpName);
	}
	neurGrpBox->addSpacing(10);
	neurGrpBox->addWidget(neuronGrpCombo);
	neurGrpBox->addStretch(5);
	mainVerticalBox->addLayout(neurGrpBox);

	//Add radio button to insert a list of neurons
	neurIDRadioButt = new QRadioButton("Neuron IDs", this);
	neurIDRadioButt->setChecked(false);
	connect (neurIDRadioButt, SIGNAL(clicked()), this, SLOT(neurIDRadioButtonClicked()));
	buttonGroup->insert(neurIDRadioButt);
	mainVerticalBox->addWidget(neurIDRadioButt);
	
	//Add text area to hold lists of neurons
	neurIDText = new Q3TextEdit(this);
	neurIDText->setTextFormat(Qt::PlainText);
	neurIDText->setEnabled(false);
	mainVerticalBox->addWidget(neurIDText);

	//Set up ok and cancel buttons
	Q3HBoxLayout *buttonBox = new Q3HBoxLayout();
	QPushButton* okButton = new QPushButton("Ok", this, "okButton");
	QPushButton* cancelButton = new QPushButton("Cancel", this, "cancelButton");	
	buttonBox->addWidget(okButton);
	buttonBox->addWidget(cancelButton);
	mainVerticalBox->addLayout(buttonBox);
	
	connect (okButton, SIGNAL(clicked()), this, SLOT(okButtonPressed()));
	connect (cancelButton, SIGNAL(clicked()), this, SLOT(cancelButtonPressed()));

	//Set dialog size
	this->resize(600, 400);
}


/*! Destructor. */
ArchiveStatisticsDialog::~ArchiveStatisticsDialog(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING ARCHIVE STATISTICS DIALOG"<<endl;
	#endif//MEMORY_DEBUG
}


//-----------------------------------------------------------------------------
//----------------            PUBLIC METHODS            -----------------------
//-----------------------------------------------------------------------------

/*! Returns the archive statistics that have been created by the dialog. */
ArchiveStatisticsHolder* ArchiveStatisticsDialog::getArchiveStatistics(){
	return archStatsHolder;
}


//-----------------------------------------------------------------------------
//----------------             PRIVATE SLOTS            -----------------------
//-----------------------------------------------------------------------------

/*! Called when the cancel button is pressed and rejects the dialog. */
void ArchiveStatisticsDialog::cancelButtonPressed(){
	this->reject();
}


/*! Called when the neuron group option is selected. */
void ArchiveStatisticsDialog::neurGrpRadioButtonClicked(){
	neurIDText->setEnabled(false);
	neuronGrpCombo->setEnabled(true);
}


/*! Called when the neuron ID option is selected. */
void ArchiveStatisticsDialog::neurIDRadioButtonClicked(){
	neurIDText->setEnabled(true);
	neuronGrpCombo->setEnabled(false);
}


/*! Called when the ok button is pressed. Either creates a single
	archive statistic to monitor the neuron group or a number of archive
	statistics to monitor each of the comma separated entries in the text
	area. */
void ArchiveStatisticsDialog::okButtonPressed(){
	//Create an archive statistic holder, which will hold all of the archive statistics
	archStatsHolder = new ArchiveStatisticsHolder;
	archStatsHolder->firingNeuronCount = 0;
	archStatsHolder->firingNeuronTotal = 0;

	//Get the description of the statistical properties
	archStatsHolder->description = archStatsNameText->text().ascii();

	/* If the neuron group option has been selected, monitoring is simple and just
		add the neuron group struct to the statistics structure.*/
	if(neurGrpRadioButt->isOn()){
		try{
                        unsigned int selectedNeurGrpID = Utilities::getNeuronGrpID(neuronGrpCombo->currentText().toStdString());
			ArchiveStatistic* tmpArchStat = new NeuronGrpArchiveStatistic(&archStatsHolder->firingNeuronCount, &archStatsHolder->firingNeuronTotal, selectedNeurGrpID);
			archStatsHolder->archStatVector.push_back(tmpArchStat);
			this->accept();
			return;
		}
		catch(std::exception& er){// Catch-all for all std exceptions
			cerr<<"ArchiveStatisticsDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
			QString errorString = "Exception thrown getting neuron group id: \"";
			errorString += er.what();
			errorString += "\"";
			QMessageBox::critical( 0, "NeuronGrpID Error", errorString);
			this->reject();
			return;
		}
	}


	/* Otherwise need to validate the list of neuron ids that have been supplied.*/
	QString neurIDStr = neurIDText->text();

	//First get rid of all white space
	for(unsigned int i=0; i<neurIDStr.length(); ++i){
		if(neurIDStr.at(i).isSpace()){
			neurIDStr.remove(i, 1);
			--i;
		}
	}
        cout<<"STRIPPED STRING: "<<neurIDStr.toStdString()<<endl;

	//Convert to string list - the neuron id string should be comma separated.
	QStringList tempStringList = QStringList::split(",", neurIDStr);


	/* Need to check that each expression is valid and that each neuron id lies within
		the ranges of the neuron groups. */
	neuronIDError = false;
	for(unsigned int i=0; i<tempStringList.size(); ++i){
                if((tempStringList[i].contains('&'))){
			unsigned int firstNeuronID = checkNeuronID(tempStringList[i].section('&', 0, 0));
			unsigned int secondNeuronID = checkNeuronID(tempStringList[i].section('&', -1, -1));
			ArchiveStatistic* tmpArchStat = new AndArchiveStatistic(&archStatsHolder->firingNeuronCount, &archStatsHolder->firingNeuronTotal, firstNeuronID, secondNeuronID);
			archStatsHolder->archStatVector.push_back(tmpArchStat);
		}
                else if((tempStringList[i].contains('|'))){
			unsigned int firstNeuronID = checkNeuronID(tempStringList[i].section('|', 0, 0));
			unsigned int secondNeuronID = checkNeuronID(tempStringList[i].section('|', -1, -1));
			ArchiveStatistic* tmpArchStat = new OrArchiveStatistic(&archStatsHolder->firingNeuronCount, &archStatsHolder->firingNeuronTotal, firstNeuronID, secondNeuronID);
			archStatsHolder->archStatVector.push_back(tmpArchStat);
		}
                else if((tempStringList[i].contains('-'))){
			unsigned int firstNeuronID = checkNeuronID(tempStringList[i].section('-', 0, 0));
			unsigned int secondNeuronID = checkNeuronID(tempStringList[i].section('-', -1, -1));
			ArchiveStatistic* tmpArchStat = new RangeArchiveStatistic(&archStatsHolder->firingNeuronCount, &archStatsHolder->firingNeuronTotal, firstNeuronID, secondNeuronID);
			archStatsHolder->archStatVector.push_back(tmpArchStat);
		}
		else {
			unsigned int neuronID = checkNeuronID(tempStringList[i]);
			ArchiveStatistic* tmpArchStat = new NeuronIDArchiveStatistic(&archStatsHolder->firingNeuronCount, &archStatsHolder->firingNeuronTotal, neuronID);
			archStatsHolder->archStatVector.push_back(tmpArchStat);
		}
	}
	if(neuronIDError){
		QMessageBox::critical( 0, "Neuron ID Error", "Neuron ID information is incorrectly formatted or does not match the available neuron groups.");
		return;
	}

	this->accept();
}


/*! Checks to see that neuron id string is valid and lies within the range of one of the
	available neuron groups. */
unsigned int ArchiveStatisticsDialog::checkNeuronID(QString idStr){
	//Check that the string converts to an appropriate integer
	unsigned int neuronID = idStr.toUInt();
	if(neuronID == 0){
		neuronIDError = true;
                cerr<<"Malformed neuron ID string: "<<idStr.toStdString()<<endl;
		return 0;
	}

	//Check that the neuron id lies within the range of one of the neuron groups
	for(vector<NeuronGroup*>::iterator iter = neuronGrpVector.begin(); iter != neuronGrpVector.end(); ++iter){
		if( neuronID >= (*iter)->startNeuronID  &&  neuronID < ( (*iter)->startNeuronID  + ((*iter)->width * (*iter)->length) ) ){
			return neuronID;
		}
	}

	//If we have reached this point we have not found a neuron group with the appropriate range for the id
	neuronIDError = true;
	cerr<<"Neuron ID does not lie within the range of any of the available neuron groups: "<<neuronID<<endl;
	return 0;
}

