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
#include "EditNeuronParametersDialog.h"
#include "Debug.h"

//Qt includes
#include <qlabel.h>
#include <qpushbutton.h>
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
EditNeuronParametersDialog::EditNeuronParametersDialog(QWidget *parent, map<const char*, double, charKeyCompare> descValueMap, map<const char*, double, charKeyCompare> *defValMap, unsigned int neuronGrpID) : QDialog(parent, "EdNeurParamDlg", true){
	//Set caption to dialog
	this->setCaption("Neuron group " + QString::number(neuronGrpID));

	//Store reference to default values
	defaultValueMap = defValMap;

	//Create a validator to control inputs
	paramValidator = new QDoubleValidator(-100000.0, 100000.0, 4, this);

	//Create a vertical box to organise layout 
	Q3VBoxLayout *verticalBox = new Q3VBoxLayout(this, 5, 10);
	verticalBox->addSpacing(5);

	//Work through the map linking descriptions and values
	for(map<const char*, double>::iterator iter = descValueMap.begin(); iter != descValueMap.end(); ++iter){
		//Create a line edit to display and edit this value
		QLineEdit *parameterText = new QLineEdit(this);
		parameterText->setText(QString::number(iter->second));

		//Store the link between the value description and the line edit
		descriptionLineEditMap[iter->first] = parameterText;

		//Add a label and text box for this parameter
		addNeuronParameter(QString(iter->first), parameterText, verticalBox);
	}

	verticalBox->addSpacing(5);

	//Add buttons to apply and reset the parameters
	Q3HBoxLayout *buttonBox = new Q3HBoxLayout();
	QPushButton *okButton = new QPushButton("Ok", this);
	connect (okButton, SIGNAL(clicked()), this, SLOT(okButtonPressed()));
	buttonBox->addWidget(okButton);

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
EditNeuronParametersDialog::~ EditNeuronParametersDialog(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING EDIT NEURON PARAMETERS DIALOG"<<endl;
	#endif//MEMORY_DEBUG
}


//---------------------------------------------------------------------------
//-----------------------    PUBLIC METHODS    ------------------------------
//---------------------------------------------------------------------------

/*! Returns a map linking the line edits with the description of the parameter. */
map<const char*, QLineEdit*, charKeyCompare>* EditNeuronParametersDialog::getDescriptionLineEditMap(){
	return &descriptionLineEditMap;
}


//--------------------------------------------------------------------------
//------------------------    SLOTS    -------------------------------------
//--------------------------------------------------------------------------

/*! Closes the dialog without changing anything. */
void EditNeuronParametersDialog::cancelButtonPressed(){
	reject();
}


/*! Loads the default values. */
void EditNeuronParametersDialog::loadDefaultsButtonPressed(){
	for(map<const char*, double, charKeyCompare>::iterator iter = defaultValueMap->begin(); iter != defaultValueMap->end(); ++iter){
		if(descriptionLineEditMap.count(iter->first))
			descriptionLineEditMap[iter->first]->setText(QString::number(iter->second));
		else
			cerr<<"EditNeuronParametersDialog: MAP KEY NOT FOUND LOADING DEFAULTS"<<endl;
	}
}


/*! Should store the current values as the default values. */
//FIXME NOT IMPLEMENTED YET.
void EditNeuronParametersDialog::makeDefaultsButtonPressed(){
}


/*! Closes the dialog. */
void EditNeuronParametersDialog::okButtonPressed(){
	accept();
}


//-----------------------------------------------------------------------------
//-----------------------        PRIVATE METHODS     --------------------------
//-----------------------------------------------------------------------------

/*! Adds widgets to gather information about a neuron parameter to the neuron 
	parameter box. */
void EditNeuronParametersDialog::addNeuronParameter(QString labelText, QLineEdit *lineEdit, Q3VBoxLayout *vBox){
	lineEdit->setMaximumSize(50, 20);
	lineEdit->setValidator(paramValidator);
	Q3HBoxLayout *hBoxLayout = new Q3HBoxLayout();
	hBoxLayout->addSpacing(10);
	hBoxLayout->addWidget(new QLabel(labelText, this));
	hBoxLayout->addWidget(lineEdit);
	hBoxLayout->addStretch(5);
	vBox->addLayout(hBoxLayout);
}



