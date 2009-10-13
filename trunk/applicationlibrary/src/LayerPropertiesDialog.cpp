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
#include "LayerPropertiesDialog.h"
#include "LayerManager.h"
#include "Debug.h"
#include "Utilities.h"
#include "NeuronGroup.h"
#include "NeuronGroupType.h"

//Qt includes
#include <qlayout.h>
#include <qlabel.h>
#include <q3accel.h>
#include <qpushbutton.h>
#include <qvalidator.h>
#include <qmessagebox.h>
#include <qstring.h>
#include <qregexp.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

//Other includes
#include <string>
#include <sstream>
#include <iostream>
#include "mysql++.h"
using namespace std;
using namespace mysqlpp;


/*! Constructor. */
LayerPropertiesDialog::LayerPropertiesDialog(QWidget *parent, const char *name, bool edMode, NeuronGroup &neuronGrp, DBInterface* netDBInter, DBInterface* devDBInter) : QDialog(parent, name){
	//Store references to databases
	networkDBInterface = netDBInter;
	deviceDBInterface = devDBInter;

	/* In editMode, the dialog displays information about an existing layer for editing
		Otherwise an entirely new layer is created.	*/
	editMode = edMode;

	//Initialise unrecognized neuron type index
	unknownNeuronTypeIndex = -1;

	//Create box to organise vertical layout of dialog
	Q3VBoxLayout *vBox = new Q3VBoxLayout(this, 5, 5, "Main vertical Box");
	
	//Validators used to check for the correct input
	QValidator* coordinateIntValidator = new QIntValidator(-10000, 10000, this);
	QValidator * dimensionsValidator = new QIntValidator(2, 500, this);
	QValidator * neuronSpacingValidator = new QIntValidator(1, 1000, this);
	QRegExp regExp( "([0-9]|[A-Z]|[a-z]|_|\\s){1,50}" );
    QValidator* nameValidator = new QRegExpValidator(regExp, this);

	//Set up name field
	Q3HBoxLayout *nameBox = new Q3HBoxLayout();
	nameBox->addWidget(new QLabel("Name", this));
	if(editMode)
		nameText = new QLineEdit(neuronGrp.name, this);
	else
		nameText = new QLineEdit("Untitled", this);
	nameText->setValidator(nameValidator);
	nameBox->addWidget(nameText);
	vBox->addLayout(nameBox);

	//Set up combo to select the type of layer
	if(!editMode){//Can't change the type so not much point in displaying it in edit mode
		Q3HBoxLayout *neurGrpTypeBox = new Q3HBoxLayout();
		neurGrpTypeCombo = new QComboBox(this, "Neuron Group Type");
		neurGrpTypeCombo->setMinimumSize(250,30);
		fillNeurGrpTypeCombo();

		/* Connect combo up with slot so that changes in the selection
			alter the available widgets. */
		connect (neurGrpTypeCombo, SIGNAL(activated(int)), this, SLOT(neuronGrpTypeChanged(int)));
	
		//Add the neuron group type combo
		neurGrpTypeBox->addWidget(new QLabel("Neuron Group Type", this, "Neuron group type"));
		neurGrpTypeBox->addWidget(neurGrpTypeCombo);
		neurGrpTypeBox->addStretch(3);
		vBox->addLayout(neurGrpTypeBox);
	
		//Add the combo to select the input layer
		Q3HBoxLayout* inputLayerBox = new Q3HBoxLayout();
		inputLayerLabel = new QLabel("Device input/output layer", this);
		inputLayerLabel->hide();
		inputLayerBox->addWidget(inputLayerLabel);
		inputLayerCombo = new QComboBox(this);
		inputLayerCombo->hide();
		inputLayerCombo->setMinimumSize(250,30);
	
		/* Connect the input layer combo up with a slot so that for each
			input layer, the available components are displayed */
		connect (inputLayerCombo, SIGNAL(activated(int)), this, SLOT(inputLayerComboChanged(int)));
	
		//Add the input layer combo
		inputLayerBox->addWidget(inputLayerCombo);
		inputLayerBox->addStretch(5);
		vBox->addLayout(inputLayerBox);
	
		//Add the combo to select the component
		Q3HBoxLayout* componentBox = new Q3HBoxLayout();
		componentLabel = new QLabel("SIMNOS component", this);
		componentLabel->hide();
		componentBox->addWidget(componentLabel);
		componentCombo = new QComboBox(this);
		componentCombo->hide();
		componentCombo->setMinimumSize(250,30);
		componentBox->addWidget(componentCombo);
		componentBox->addStretch(5);
		vBox->addLayout(componentBox);
	}

	//Set up neuron type field
	Q3HBoxLayout *neuronTypeBox = new Q3HBoxLayout();
	neuronTypeCombo = new QComboBox(this, "Neuron Types");

	//Load up the neuron types from the database
	bool neuronTypeFound = false;
	int counter = 0, neuronTypeComboIndex = 0;
	try{
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"SELECT TypeID, Description FROM NeuronTypes";
                StoreQueryResult typeResult = query.store();
                for(StoreQueryResult::iterator iter = typeResult.begin(); iter != typeResult.end(); ++iter){
			Row neurTypeRow(*iter);
			unsigned short neuronTypeID = Utilities::getUShort((std::string) neurTypeRow["TypeID"]);
	
			//Store link between neuron type and combo index
			neuronTypePositionMap[counter] = neuronTypeID;
                        string typeDescription = (std::string) neurTypeRow["Description"];
			typeDescription += " [" + (std::string) neurTypeRow["TypeID"] + "]";
                        neuronTypeCombo->insertItem(typeDescription.data());
			if(editMode){
				if(neuronTypeID == neuronGrp.neuronType){
					neuronTypeComboIndex = counter;
					neuronTypeFound = true;
				}
			}
			counter++;
		}
	}
    catch (const BadQuery& er) {// Handle any query errors
		cerr<<"LayerPropertiesDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query loading neuron types: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Types Error", errorString);
    }
    catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
        cerr<<"LayerPropertiesDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading neuron types: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Types Error", errorString);
	}
	catch(std::exception& er){// Catch-all for std exceptions
		cerr<<"LayerPropertiesDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading neuron types: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Types Error", errorString);
	}

	if(editMode && !neuronTypeFound){
		neuronTypeCombo->insertItem("Unknown");
		neuronTypeCombo->setCurrentItem(counter);
		unknownNeuronTypeIndex = counter;
	}
	else{
		neuronTypeCombo->setCurrentItem(neuronTypeComboIndex);
	}
	neuronTypeBox->addWidget(new QLabel("Neuron Type", this, "Neuron type"));
	neuronTypeBox->addWidget(neuronTypeCombo);
	vBox->addLayout(neuronTypeBox);

	//Set up width in neurons field
	//This cannot be changed in editMode since the connections will be affected
	Q3HBoxLayout *widthBox = new Q3HBoxLayout();
	widthBox->addWidget(new QLabel("Width (neurons)", this, "Width neurons"));
	widthText = new QLineEdit(this);
	widthText->setMaximumSize(50,20);
	widthText->setValidator( dimensionsValidator );
	widthBox->addWidget(widthText);
	if(editMode){
		widthText->setText(QString::number(neuronGrp.width));
		widthText->setReadOnly(true);
	}
	widthBox->addStretch(5);
	vBox->addLayout(widthBox);
	
	//Set up length in neurons field
	Q3HBoxLayout *lengthBox = new Q3HBoxLayout();
	lengthBox->addWidget(new QLabel("Length (neurons)", this, "Length neurons"));
	lengthText = new QLineEdit(this);
	lengthText->setMaximumSize(50,20);
	lengthText->setValidator( dimensionsValidator );
	lengthBox->addWidget(lengthText);
	if(editMode){
		lengthText->setText(QString::number(neuronGrp.length));
		lengthText->setReadOnly(true);
	}
	lengthBox->addStretch(5);
	vBox->addLayout(lengthBox);
	
	//Set up neuron spacing field
	Q3HBoxLayout *neuronSpacingBox = new Q3HBoxLayout();
	neuronSpacingLabel = new QLabel("Neuron Spacing", this, "Neuron Spacing label");
	neuronSpacingBox->addWidget(neuronSpacingLabel);
	if(editMode)
		neuronSpacingText = new QLineEdit(QString::number(neuronGrp.spacing), this);
	else
		neuronSpacingText = new QLineEdit(DEFAULT_NEURON_SPACING, this, "Neuron spacing" );
	neuronSpacingText->setValidator( neuronSpacingValidator );
	neuronSpacingBox->addWidget(neuronSpacingText);
	vBox->addLayout(neuronSpacingBox);
	
	//Set up layer center position
	Q3HBoxLayout *layerPositionBox = new Q3HBoxLayout();
	layerPositionBox->addWidget(new QLabel("Location", this, "Layer centre"));
	layerPositionBox->addSpacing(5);
	layerPositionBox->addWidget(new QLabel("X:", this, "xpos"));
	if(editMode)
		xPosText = new QLineEdit(QString::number(neuronGrp.xPos), this);
	else
		xPosText = new QLineEdit(this);
	xPosText->setValidator( coordinateIntValidator );
	xPosText->setMaximumSize(50,20);
	layerPositionBox->addWidget(xPosText);
	layerPositionBox->addSpacing(5);
	layerPositionBox->addWidget(new QLabel("Y:", this, "ypos"));
	if(editMode)
		yPosText = new QLineEdit(QString::number(neuronGrp.yPos),this);
	else
		yPosText = new QLineEdit(this);
	yPosText->setValidator( coordinateIntValidator );
	yPosText->setMaximumSize(50,20);
	layerPositionBox->addWidget(yPosText);
	layerPositionBox->addSpacing(5);
	layerPositionBox->addWidget(new QLabel("Z:", this, "zpos"));
	if(editMode)
		zPosText = new QLineEdit(QString::number(neuronGrp.zPos), this);
	else
		zPosText = new QLineEdit(this);
	zPosText->setValidator( coordinateIntValidator );
	zPosText->setMaximumSize(50,20);
	layerPositionBox->addWidget(zPosText);
	layerPositionBox->addStretch(5);
	vBox->addLayout(layerPositionBox);

	//Set up ok and cancel buttons
	Q3HBoxLayout *buttonBox = new Q3HBoxLayout();
	QPushButton *okPushButton = new QPushButton("Ok", this, "okButton");
	QPushButton *cancelPushButton = new QPushButton("Cancel", this, "cancelButton");	
	buttonBox->addWidget(okPushButton);
	buttonBox->addWidget(cancelPushButton);
	vBox->addLayout(buttonBox);
	
	connect (okPushButton, SIGNAL(clicked()), this, SLOT(okButtonPressed()));
	connect (cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));
	
	//Set up accelerator for return button
	Q3Accel *returnAccel = new Q3Accel( this );
    returnAccel->connectItem( returnAccel->insertItem( Qt::Key_Enter ), this, SLOT(okButtonPressed()));

	/* Load up the combos - do this last because they depend on other 
		components, which need to be initialised. */
	if(!editMode)
		loadInputLayers();
}


/*! Destructor. Qt widgets should be deleted automatically since
	they are all created with a reference to this. */
LayerPropertiesDialog::~LayerPropertiesDialog(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING LAYER PROPERTIES DIALOG"<<endl;
	#endif//MEMORY_DEBUG
}


//---------------------------------------------------------------------
//------------------------ PUBLIC METHODS -----------------------------
//---------------------------------------------------------------------

/*! Returns the structure NeuronGrpHolder containing all the information about the layer. */
NeuronGroup LayerPropertiesDialog::getNeuronGroup(){
	//Create the neuron group holder
	NeuronGroup neuronGrp;

	//Add connection type to neuron group holder
	try{
		if(!editMode){
			string connectionText = neurGrpTypeCombo->currentText().ascii();
			neuronGrp.neuronGrpType = NeuronGroupType::getType(connectionText);
		
			//Add component id and deviceNeuronGrpID if applicable
			if(NeuronGroupType::getType(neurGrpTypeCombo->currentText()) == NeuronGroupType::SIMNOSComponentLayer){
				neuronGrp.componentID = Utilities::getNeuronGrpID(componentCombo->currentText());//Not a neuron group id, but extraction method is the same
				neuronGrp.deviceNeuronGrpID = Utilities::getNeuronGrpID(inputLayerCombo->currentText());
			}
		}
	
		//Need to convert from combo box selection to the unsigned short representing the neuron type
		int comboIndex = neuronTypeCombo->currentItem();
		neuronGrp.neuronType =  neuronTypePositionMap[comboIndex];
		
		//Extract the rest of the information
		neuronGrp.name = nameText->text();
		neuronGrp.width = Utilities::getUInt(widthText->text().ascii());
		neuronGrp.length = Utilities::getUInt(lengthText->text().ascii());
		neuronGrp.spacing = Utilities::getUInt(neuronSpacingText->text().ascii());
		neuronGrp.xPos = Utilities::getInt(xPosText->text().ascii());
		neuronGrp.yPos = Utilities::getInt(yPosText->text().ascii());
		neuronGrp.zPos = Utilities::getInt(zPosText->text().ascii());
	}
    catch (const std::exception& er) {// Catch-all for any other MySQL++ exceptions
        cerr<<"LayerPropertiesDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown getting neuron group: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Group Error", errorString);
    }
	return neuronGrp;
}


//-----------------------------------------------------------------------------------
//-------------------------------------- SLOTS --------------------------------------
//-----------------------------------------------------------------------------------

/*! Called when the component combo selection is changed. */
void LayerPropertiesDialog::componentComboChanged(int comboIndex){
	if(noComponents || NeuronGroupType::getType(neurGrpTypeCombo->currentText()) != NeuronGroupType::SIMNOSComponentLayer){
		widthText->clear();
		lengthText->clear();
	}
	else{
		widthText->setText(QString::number(componentWidthMap[comboIndex]));
		lengthText->setText(QString::number(componentLengthMap[comboIndex]));
	}
}


/*! Called when the input layer combo is changed. 
	This method sets the available SIMNOS components. */
void LayerPropertiesDialog::inputLayerComboChanged(int){
	//Clear component combo and associated maps
	componentCombo->clear();
	componentWidthMap.clear();
	componentLengthMap.clear();
	noComponents = true;

	//If there are no viable input layers, reset combo and return
	if(noInputLayers){
		componentCombo->insertItem("No matching components", -1);
		widthText->clear();
		lengthText->clear();
		return;
	}
	
	try{
		//Get current neuron group id
		unsigned int inputNeurGrpID = Utilities::getNeuronGrpID(inputLayerCombo->currentText());

		//Find the width and length of this neuron group
		Query networkQuery = networkDBInterface->getQuery();
		networkQuery.reset();
		networkQuery<<"SELECT Width, Length FROM NeuronGroups WHERE NeuronGrpID = "<<inputNeurGrpID;
                StoreQueryResult neurGrpResult= networkQuery.store();
		Row neurGrpRow (*neurGrpResult.begin());//NeuronGrpID should be unique
		unsigned int neurGrpWidth = Utilities::getUInt((std::string)neurGrpRow["Width"]);
		unsigned int neurGrpLength = Utilities::getUInt((std::string)neurGrpRow["Length"]);
	
		//Need all the devices that match this width and length
		QStringList deviceDescriptionList;
		Query deviceQuery = deviceDBInterface->getQuery();
		deviceQuery.reset();
		deviceQuery<<"SELECT Description FROM Devices WHERE TotalNumColumns = "<<neurGrpWidth<<" AND TotalNumRows = "<<neurGrpLength;
                StoreQueryResult deviceDescRes = deviceQuery.store();
                for(StoreQueryResult::iterator devDescIter = deviceDescRes.begin(); devDescIter != deviceDescRes.end(); ++devDescIter){
			Row deviceDescRow(*devDescIter);
                        QString deviceDescriptionString(((std::string)deviceDescRow["Description"]).data());
			deviceDescriptionList += deviceDescriptionString;
		}
	
		/* Work through the SIMNOS components. */
		deviceQuery.reset();
		deviceQuery<<"SELECT Name, ComponentID, ReceptorIDs, Width, Length FROM SIMNOSComponents";
                StoreQueryResult simnosRes = deviceQuery.store();
                for(StoreQueryResult::iterator simnosIter = simnosRes.begin(); simnosIter != simnosRes.end(); ++simnosIter){
			Row simnosRow(*simnosIter);
			
			//Extract the first receptor id in the list
			//NOTE THIS ASSUMES THAT ALL OF THE RECEPTORS ARE FROM THE SAME DEVICE
                        QString receptorIDString = ((std::string)simnosRow["ReceptorIDs"]).data();
			if(receptorIDString.isEmpty()){
				cerr<<"LayerPropertiesDialog: NO RECEPTOR IDS FOR THIS COMPONENT"<<endl;
				QMessageBox::critical (this, "SIMNOS Component Error", "No receptor list for this component");
				return;
			}
	
			//Get the receptor ID - converting to int is a good check
			unsigned int firstReceptorID = Utilities::getUInt(receptorIDString.section(',', 0, 0).ascii());
			
			deviceQuery.reset();
			deviceQuery<<"SELECT DeviceDescription FROM SIMNOSSpikeReceptors WHERE ReceptorID = "<<firstReceptorID;
                        StoreQueryResult receptorDevDesRes = deviceQuery.store();
			Row receptorDevDesRow(*receptorDevDesRes.begin());//ReceptorID should be unique
                        QString receptorDeviceDesc (((std::string) receptorDevDesRow["DeviceDescription"]).data());
	
			/* If this description is in the deviceDescriptionList, add this component
				to the componentCombo */
			for(unsigned int i=0; i<deviceDescriptionList.size(); ++i){
				if(deviceDescriptionList[i] == receptorDeviceDesc){
					unsigned int tmpInsertionPosition = componentCombo->count();
	
					//Add this SIMNOS component to the component combo
                                        QString tempComboName (((std::string)simnosRow["Name"] + " [" + (std::string)simnosRow["ComponentID"] + "]").data());
					componentCombo->insertItem(tempComboName, tmpInsertionPosition);
	
					//Store the width and length
					componentWidthMap[tmpInsertionPosition] = Utilities::getUInt((std::string)simnosRow["Width"]);
					componentLengthMap[tmpInsertionPosition] = Utilities::getUInt((std::string)simnosRow["Length"]);
	
					//Exit from for loop
					break;
				}
			}
		}
	}
    catch (const BadQuery& er) {// Handle any query errors
		cerr<<"LayerPropertiesDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query loading SIMNOS components: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Device Error", errorString);
    }
    catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
        cerr<<"LayerPropertiesDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading SIMNOS components: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Device Error", errorString);
    }
	catch(std::exception& er){// Catch-all for std exceptions
		cerr<<"LayerPropertiesDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading SIMNOS components: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Device Error", errorString);
	}

	//Indicate if no matching components have been found
	if(componentCombo->count() == 0){
		componentCombo->insertItem("No matching components", -1);
	}
	else{//Signal that content of component combo has changed
		noComponents = false;
		componentComboChanged(componentCombo->currentItem());
	}
}


/*! Called when the neuron group type is changed and hides and shows
	appropriate widgets. */
void LayerPropertiesDialog::neuronGrpTypeChanged(int){
	int tempType = NeuronGroupType::getType(neurGrpTypeCombo->currentText());
	if(tempType == NeuronGroupType::SIMNOSComponentLayer){
		//Hide neuron spacing widgets
		neuronSpacingText->hide();
		neuronSpacingLabel->hide();

		//Make width and length non editable
		widthText->setReadOnly(true);
		lengthText->setReadOnly(true);

		//Show combo with list of input layers to connect to
		inputLayerCombo->show();
		inputLayerLabel->show();

		//Show combo with list of simnos components
		componentCombo->show();
		componentLabel->show();

		//Call appropriate slots
		inputLayerComboChanged(inputLayerCombo->currentItem());
	}
	else{//Should be 2D or 3D rectangular layer
		//Show neuron spacing widgets
		neuronSpacingText->show();
		neuronSpacingLabel->show();

		//Make width and length editable
		widthText->setReadOnly(false);
		lengthText->setReadOnly(false);
		widthText->clear();
		lengthText->clear();

		//Hide combo with list of input layers to connect to
		inputLayerCombo->hide();
		inputLayerLabel->hide();

		//Hide combo with list of simnos components
		componentCombo->hide();
		componentLabel->hide();
	}
	this->adjustSize();
}


/*! This method validates the input from the fields.
	If everything is ok, accept() is called.
	Otherwise an alert is shown about the missing fields. */
void LayerPropertiesDialog::okButtonPressed(){
	QString errorString = "";
	try{
		unsigned int tempUInt;
		QString tempS = nameText->text();
		if(tempS == "")
			errorString += "No name specified.\n";
		if(!editMode){//Only check length and width if not in edit mode
			if(lengthText->text() == "")
				errorString += " No length specified for neuron group.\n";
			else{
				tempUInt = Utilities::getUInt(lengthText->text().ascii());
				if(tempUInt < 1)
					errorString += " Length cannot be less than 1.\n";
			}
			if(widthText->text() == "")
				errorString += " No width specified for neuron group.\n";
			else{
				tempUInt = Utilities::getUInt(widthText->text().ascii());
				if(tempUInt < 1)
					errorString += " Width cannot be less than 1.\n";
			}
		}
		if(neuronSpacingText->text() == "")
			neuronSpacingText->setText(DEFAULT_NEURON_SPACING);
		tempUInt = Utilities::getUInt(neuronSpacingText->text().ascii());
		if(tempUInt < 1)
			errorString += " Neuron spacing cannot be less than 1.\n";
		tempS = xPosText->text();
		if(tempS == "")
			errorString += " No x position specified.\n";
		tempS = yPosText->text();
		if(tempS == "")
			errorString += " No y position specified.\n";
		tempS = zPosText->text();
		if(tempS == "")
			errorString += " No z position specified.\n";
		
		if(neuronTypeCombo->currentItem() == unknownNeuronTypeIndex){
			errorString += " Cannot select an unrecognized neuron type.\n";
		}
	
		if(!editMode){
			if(NeuronGroupType::getType(neurGrpTypeCombo->currentText()) == NeuronGroupType::SIMNOSComponentLayer){
				if(noInputLayers)
					errorString += "No device input layer selected\n";
				if(noComponents)
					errorString += "No SIMNOS components selected\n";
			}
		}
    }
    catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
        cerr<<"LayerPropertiesDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		errorString += "Exception thrown: \"";
		errorString += er.what();
		errorString += "\"";
    }

	//Decide whether to accept or reject input
	if(errorString != "")
		QMessageBox::critical( this, "Errors", errorString);
	else{
		accept();
	}
}


//---------------------------------------------------------------------
//------------------------ PRIVATE METHODS ----------------------------
//---------------------------------------------------------------------

/*! Loads up all the available neuron types from the neuron type
	database. */
void LayerPropertiesDialog::fillNeurGrpTypeCombo(){
	neurGrpTypeCombo->clear();
	unsigned short* neurGrpTypeArray = NeuronGroupType::getTypes();
	for(int i=0; i<NeuronGroupType::NumberTypes; i++){
		neurGrpTypeCombo->insertItem(NeuronGroupType::getDescription(neurGrpTypeArray[i]));
	}
}


/*! Loads up a list of all the available input layers. These must match
	the width and length of at least one device. */
void LayerPropertiesDialog::loadInputLayers(){
	//Clear any existing entries
	inputLayerCombo->clear();
	noInputLayers = true;

	/* Get a list of neuron groups that match at least one of the width and lengths
		in the Device table*/
	try{
		Query networkQuery = networkDBInterface->getQuery();
		networkQuery.reset();
		networkQuery<<"SELECT Name, NeuronGrpID, Width, Length FROM NeuronGroups";
                StoreQueryResult neurGrpResult= networkQuery.store();
                for(StoreQueryResult::iterator resIter = neurGrpResult.begin(); resIter < neurGrpResult.end(); resIter++){
			Row neurGrpRow(*resIter);
			/* Check to see if the width and length of the neuron group
				match that of any of the rows in the device database */
			unsigned int neurGrpWidth = Utilities::getUInt((std::string)neurGrpRow["Width"]);
			unsigned int neurGrpLength = Utilities::getUInt((std::string)neurGrpRow["Length"]);
	
			Query deviceQuery = deviceDBInterface->getQuery();
			deviceQuery.reset();
			deviceQuery<<"SELECT * FROM Devices WHERE TotalNumColumns = "<<neurGrpWidth<<" AND TotalNumRows = "<<neurGrpLength;
                        StoreQueryResult deviceResult = deviceQuery.store();
			if(deviceResult.size() > 0){
                                QString neurGrpString(((std::string)neurGrpRow["Name"] + " [" += (std::string)neurGrpRow["NeuronGrpID"] += "]").data());
				inputLayerCombo->insertItem(neurGrpString, -1);
			}
		}
	}
    catch (const BadQuery& er) {// Handle any query errors
		cerr<<"LayerPropertiesDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query loading input layers: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Device Error", errorString);
    }
    catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
        cerr<<"LayerPropertiesDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading input layers: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Device Error", errorString);
    }
	catch(std::exception& er){// Catch-all for std exceptions
		cerr<<"LayerPropertiesDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading input layers: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Device Error", errorString);
	}

	//Add entry specifying that there are no layers matching available devices
	if(inputLayerCombo->count() == 0){
		inputLayerCombo->insertItem("No input layers match available devices", -1);
	}
	else{
		noInputLayers = false;
	}

	//Indicate that input layers has changed
	inputLayerComboChanged(inputLayerCombo->currentItem());
}



