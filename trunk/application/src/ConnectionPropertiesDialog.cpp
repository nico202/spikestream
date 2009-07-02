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
#include "ConnectionPropertiesDialog.h"
#include "Debug.h"
#include "Utilities.h"

//Qt includes
#include <qlayout.h>
#include <q3accel.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <q3buttongroup.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QLabel>
#include <Q3VBoxLayout>

//Other includes
#include <mysql++.h>
#include <map>
#include <string>
#include <sstream>
#include <iostream>
using namespace std;
using namespace mysqlpp;


/*! Constructor used for standard connections that allows user to select the connection type */
ConnectionPropertiesDialog::ConnectionPropertiesDialog(QWidget *parent, const char *name, DBInterface *dbInter) : QDialog(parent, name){
	//Store reference to dbInterface
	dbInterface = dbInter;

	//Initialise variables
	editMode = false;
	connectionType = -1;
	fromNeurGrpID = -1;
	toNeurGrpID = -1;
	parameterError = false;

	//Call method to assemble dialog
	loadError = false;
	try{
		constructDialog();
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"ConnectionPropertiesDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when constructing dialog: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connection Properties Error", errorString);
		loadError = true;
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"ConnectionPropertiesDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown constructing dialog: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connection Properties Error", errorString);
		loadError = true;
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"ConnectionPropertiesDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown constructing dialog: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connection Properties Error", errorString);
		loadError = true;
	}
}


/*! Constructor used when creating SIMNOS component connections. In this case the
	from and to neuron group id are known along with the connection type. In this
	case this dialog is used to gather remaining information about the connection */
ConnectionPropertiesDialog::ConnectionPropertiesDialog(QWidget *parent, const char *name, DBInterface *dbInter, unsigned int fNeurGrpID, unsigned int tNeurGrpID, unsigned short connType) : QDialog(parent, name){
	//Store reference to dbInterface
	dbInterface = dbInter;

	//Initialise variables
	editMode = true;
	connectionType = connType;
	fromNeurGrpID = fNeurGrpID;
	toNeurGrpID = tNeurGrpID;

	//Call method to assemble dialog
	try{
		constructDialog();
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"ConnectionPropertiesDialog: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when constructing dialog: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connection Properties Error", errorString);
		loadError = true;
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"ConnectionPropertiesDialog: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown constructing dialog: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connection Properties Error", errorString);
		loadError = true;
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"ConnectionPropertiesDialog: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown constructing dialog: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connection Properties Error", errorString);
		loadError = true;
	}
}


/*! Creates the dialog, either using the supplied neuron group ids or allowing
	the user to select the neuron group ids and connection type */
void ConnectionPropertiesDialog::constructDialog(){

	/* Validators that are used to check for the correct input
		Parameter validator assumes that the parameters will be between -100,000 and 100,000
		Delay validator assumes that delay will be between 0 and 250 ms. */
	paramValidator = new QDoubleValidator(-100000.0, 100000.0, 4, this);
	delayValidator = new QIntValidator(DEFAULT_MIN_DELAY, DEFAULT_MAX_DELAY, this);
	
	//Create vertical box to arrane dialog widget
	Q3VBoxLayout *vBox = new Q3VBoxLayout(this, 5, 10, "Main vertical Box");
	
	//Set up radio buttons to choose between inter and intra layer connections
	if(!editMode){
		Q3ButtonGroup *buttonGroup = new Q3ButtonGroup();
		intraRadioButt = new QRadioButton("Connections within a single layer", this, "intraRadio");
		intraRadioButt->setChecked(true);
		interRadioButt = new QRadioButton("Connections between layers", this, "interRadio");
		interRadioButt->setChecked(false);
		buttonGroup->insert(intraRadioButt);
		buttonGroup->insert(interRadioButt);
	
		//Set up labels for combo boxes
		intraLayerLabel = new QLabel("Layer:", this, "Intra layer choice");
		fromLayerLabel = new QLabel("From Layer:", this, "Inter layer from");
		fromLayerLabel->setEnabled(false);
		toLayerLabel = new QLabel("To Layer:", this, "Inter layer to");
		toLayerLabel->setEnabled(false);
	}
	
	//Initialise parameter table before intitializing combo boxes. Otherwise get segmentation fault
	connParamTable = new ConnectionParameterTable(3, 2, this);
	
	if(editMode){//Fill table with the parameters for the connection type 
		loadParameterTable(connectionType);
	}
	else{//Set up combo boxes
		intraLayerCombo = new QComboBox(this, "Intra layer combo");
		fromLayerCombo = new QComboBox(this, "from layer combo");
		fromLayerCombo->setEnabled(false);
		toLayerCombo = new QComboBox(this, "to layer combo");
		toLayerCombo->setEnabled(false);
		loadLayerNames();
	
		//Arrange all the components in the connection widget
		//First the intra layer components
		vBox->addWidget(intraRadioButt);
		Q3HBoxLayout *intraBox = new Q3HBoxLayout();
		intraBox->addSpacing(20);
		intraBox->addWidget(intraLayerLabel);
		intraBox->addWidget(intraLayerCombo);
		intraBox->addStretch(5);
		vBox->addLayout(intraBox);
	
		//Add inter layer components
		vBox->addWidget(interRadioButt);
	
		Q3HBoxLayout *interBox = new Q3HBoxLayout();
		interBox->addSpacing(20);
		interBox->addWidget(fromLayerLabel);
		interBox->addWidget(fromLayerCombo);
		interBox->addWidget(toLayerLabel);
		interBox->addWidget(toLayerCombo);
		interBox->addStretch(5);
		vBox->addLayout(interBox);
		
		vBox->addSpacing(15);
		
		//Add combo box for connection type. Initially set to values for intra layer connection
		Q3HBoxLayout *connTypeBox = new Q3HBoxLayout();
		connectionTypeCombo = new QComboBox(this, "Connection Type");
		connectionTypeCombo->setMinimumSize(250,30);
		
		//Initially fill eith intra layer connection types
		fillConnectionTypeCombo_INTRA();
		connTypeBox->addWidget(new QLabel("Connection Type", this, "Connection type"));
		connTypeBox->addWidget(connectionTypeCombo);
		connTypeBox->addStretch(3);
		vBox->addLayout(connTypeBox);
	}

	//Finish table to show connection parameters
	vBox->addWidget(new QLabel("Connection Parameters", this, "Connection params"));
	connParamTable->setSorting(false);
	connParamTable->verticalHeader()->hide();
	connParamTable->setLeftMargin(0);
	connParamTable->horizontalHeader()->hide();
	connParamTable->setTopMargin(0);
	connParamTable->setColumnWidth( 0, 300);
	connParamTable->setColumnWidth( 1, 100);
	vBox->addWidget(connParamTable);
	
	vBox->addSpacing(5);

	//Add combo box for synapse type
	Q3HBoxLayout *synapseTypeBox = new Q3HBoxLayout();
	synapseTypeCombo = new QComboBox(this, "SynapseType");
	synapseTypeCombo->setMinimumSize(250, 30);

	//Load up the synapse types from the database
	Query query = dbInterface->getQuery();
	query.reset();
	query<<"SELECT TypeID, Description FROM SynapseTypes";
        StoreQueryResult typeResult = query.store();
	int counter = 0;
        for(StoreQueryResult::iterator iter = typeResult.begin(); iter != typeResult.end(); ++iter){
		Row synTypeRow(*iter);
		unsigned short synapseTypeID = Utilities::getUShort((std::string) synTypeRow["TypeID"]);

		//Store link between neuron type and combo index
		synapseTypePositionMap[counter] = synapseTypeID;

                string typeDescription = (std::string) synTypeRow["Description"];
                typeDescription += " [" + (std::string) synTypeRow["TypeID"] + "]";
                synapseTypeCombo->insertItem(typeDescription.data());
		counter++;
	}

	//Add combo to dialog
	synapseTypeBox->addWidget(new QLabel("Synapse type", this, "Synapse type"));
	synapseTypeBox->addWidget(synapseTypeCombo);
	synapseTypeBox->addStretch(5);
	vBox->addLayout(synapseTypeBox);
	
	vBox->addSpacing(5);
	
	//Set up labels and fields for delay 
	Q3HBoxLayout *delayBox = new Q3HBoxLayout();
	delayBox->addWidget(new QLabel("Delay Range:", this, "delay range"));
	delayBox->addSpacing(10);
	delayBox->addWidget(new QLabel("Min", this, "min delay"));
	minDelayText = new QLineEdit(QString::number(DEFAULT_MIN_DELAY), this);
	minDelayText->setValidator( delayValidator );
	minDelayText->setMaximumSize(50,20);
	delayBox->addWidget(minDelayText);
	delayBox->addSpacing(5);
	delayBox->addWidget(new QLabel("Max", this, "max delay"));
	maxDelayText = new QLineEdit(QString::number(DEFAULT_MAX_DELAY), this);
	maxDelayText->setValidator(delayValidator);
	maxDelayText->setMaximumSize(50, 20);
	delayBox->addWidget(maxDelayText);
	delayBox->addStretch(5);
	vBox->addLayout(delayBox);
	
	vBox->addSpacing(15);
	
	//Set up ok and cancel buttons
	Q3HBoxLayout *buttonBox = new Q3HBoxLayout();
	QPushButton *okPushButton = new QPushButton("Ok", this, "okButton");
	QPushButton *cancelPushButton = new QPushButton("Cancel", this, "cancelButton");	
	buttonBox->addWidget(okPushButton);
	buttonBox->addWidget(cancelPushButton);
	vBox->addLayout(buttonBox);
	
	//Connect signals to slots for event handling
	if(!editMode){
		connect (intraRadioButt, SIGNAL(clicked()), this, SLOT(layerRadioStateChanged()));
		connect (interRadioButt, SIGNAL(clicked()), this, SLOT(layerRadioStateChanged()));
		connect (connectionTypeCombo, SIGNAL(activated(int)), this, SLOT(paramSelectionChanged()));
	}

	connect (okPushButton, SIGNAL(clicked()), this, SLOT(okButtonPressed()));
	connect (cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect (connParamTable, SIGNAL(valueChanged(int, int)), this, SLOT(paramValueChanged(int, int)));	
	
	//Set up accelerator for return button
	Q3Accel *returnAccel = new Q3Accel( this );
        returnAccel->connectItem( returnAccel->insertItem( Qt::Key_Enter ), this, SLOT(okButtonPressed()));
}


/*! Destructor
	Qt widgets created with this as a reference should be deleted automatically. */
ConnectionPropertiesDialog::~ConnectionPropertiesDialog(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING CONNECTION PROPERTIES DIALOG"<<endl;
	#endif//MEMORY_DEBUG
}


//--------------------------------------------------------------------------------
//----------------------------- PUBLIC METHODS -----------------------------------
//--------------------------------------------------------------------------------

/*! Returns a connection holder containing all the critical information about a connection group. */
ConnectionHolder ConnectionPropertiesDialog::getConnectionHolder(){
	ConnectionHolder connHolder;

	//Set to and from layer id
	if(editMode){
		connHolder.fromLayerID = fromNeurGrpID;
		connHolder.toLayerID = toNeurGrpID;
		connHolder.connectionType = connectionType;
	}
	else{
		//Need to adjust response depending on whether the intra or inter layer button is selected
		if(intraRadioButt->isOn()){//Connections within a single layer
			connHolder.fromLayerID = Utilities::getNeuronGrpID(intraLayerCombo->currentText());
                        connHolder.toLayerID =  Utilities::getNeuronGrpID(intraLayerCombo->currentText());
		}
		else {//Inter radio button should be on in this case
                        connHolder.fromLayerID = Utilities::getNeuronGrpID(fromLayerCombo->currentText());
			connHolder.toLayerID = Utilities::getNeuronGrpID(toLayerCombo->currentText());
		}

		//Fill connection holder with connection type
		string connectionText = connectionTypeCombo->currentText().ascii();
		connHolder.connectionType = ConnectionType::getType(connectionText);
	}
	
	//Next set the delay values in the connection holder
	connHolder.minDelay = (unsigned short)minDelayText->text().toInt();
	connHolder.maxDelay = (unsigned short)maxDelayText->text().toInt();

	//Set the synapse type
	//Need to convert from combo box selection to the unsigned short representing the neuron type
	int comboIndex = synapseTypeCombo->currentItem();
	connHolder.synapseType =  synapseTypePositionMap[comboIndex];

	//Get the connection parameters
	connHolder.paramMap = getConnectionParameters();
	
	return connHolder;
}


/*! Returns a map containing all the connection parameters. */
map<string, double> ConnectionPropertiesDialog::getConnectionParameters(){
	map<string, double> connParamMap;
	map<string, string> connParamNames;
	unsigned short connType;
	if(editMode){
		connType = connectionType;
	}
	else{
		connType = ConnectionType::getType(connectionTypeCombo->currentText());
	}
	ConnectionType::getParameters(connParamNames, connType);//This returns a map filled with default values, so will ignore these and replace them with the values from the parameter table
	for(map<string, string>::iterator iter = connParamNames.begin(); iter != connParamNames.end(); iter++){
		string tempParamName = (*iter).first;
                connParamMap[tempParamName] = getParameterValue(QString(tempParamName.data()));
	}
	return connParamMap;
}


//-------------------------------------------------------------------------
//----------------------- SLOTS -------------------------------------------
//-------------------------------------------------------------------------

/*! Sets up dialog depending on whether it is for intra or inter layer connections. */
void ConnectionPropertiesDialog::layerRadioStateChanged(){
	if(intraRadioButt->isChecked()){
		intraLayerLabel->setEnabled(true);
		intraLayerCombo->setEnabled(true);
		fromLayerLabel->setEnabled(false);
		fromLayerCombo->setEnabled(false);
		toLayerLabel->setEnabled(false);
		toLayerCombo->setEnabled(false);
		fillConnectionTypeCombo_INTRA();
	}
	else if(interRadioButt->isChecked()){
		intraLayerLabel->setEnabled(false);
		intraLayerCombo->setEnabled(false);
		fromLayerLabel->setEnabled(true);
		fromLayerCombo->setEnabled(true);
		toLayerLabel->setEnabled(true);
		toLayerCombo->setEnabled(true);
		fillConnectionTypeCombo_INTER();	
	}
}


/*! Runs checks on parameters and delay, informing the user about any errors and changes. */
void ConnectionPropertiesDialog::okButtonPressed(){
	if(loadError){
		QMessageBox::critical( 0, "Connection Properties Error", "Error encountered loading dialog.\nConnection properties may be corrupted.\nPress cancel instead.");
		return;
	}

	//Set parameter error to false
	parameterError = false;

	//End editing on any cells that are being changed
	connParamTable->endAllEditing();

	//If the ending of editing has created a parameter error want to return
	if(parameterError)
		return;

	//In edit mode just do checks on the connection parameters and return
	if(editMode){
		if(!checkConnection_INTER(fromNeurGrpID, toNeurGrpID, connectionType))
			return;
	}
	else{//Run full checks
		//If interRadioButt is selected, check that from layer and to layer are different - otherwise it is not an inter layer connection
		if(interRadioButt->isOn()){
			if(fromLayerCombo->currentText() == toLayerCombo->currentText()){
				QMessageBox::warning(this, "Inter Layer Connection Error", "For an *inter* layer connection, 'from' and 'to' layers must be different");
				return;
			}
		}
		//Check connection parameters
		if(intraRadioButt->isOn()){//Connection within a single layer
			unsigned short connType = ConnectionType::getType(connectionTypeCombo->currentText());
			if(!checkConnection_INTRA(connType))
				return;
		}
		else{//Connections between layers
			unsigned int fromLayerID = Utilities::getNeuronGrpID(fromLayerCombo->currentText());
			unsigned int toLayerID = Utilities::getNeuronGrpID(toLayerCombo->currentText());
			unsigned short connType = ConnectionType::getType(connectionTypeCombo->currentText());
			if(!checkConnection_INTER(fromLayerID, toLayerID, connType))
				return;
		}
	}
	
	//Run checks on min delay
	QString errorString = "";
	int minDelay = 0;
	if(minDelayText->text() == ""){
		errorString += "No minimum delay specified.\n";
		minDelay = -1;
	}
	else{
		minDelay = minDelayText->text().toInt();
		if(minDelay < 0)
			errorString += "Minimum delay cannot be less than 0.\n";
		if(minDelay > 250)
			errorString += "Minimum delay cannot be greater than 250 ms.\n";
	} 
	int maxDelay = 0;
	if(maxDelayText->text() == ""){
		errorString += "No maximum delay specified.\n";
		maxDelay = -1;
	}
	else{
		maxDelay = maxDelayText->text().toInt();
		if(maxDelay < 0)
			errorString += "Maximum delay cannot be less than 0.\n";
		if(maxDelay > 250)
			errorString += "Maximum delay cannot be greater than 250 ms.\n";
	}
	//Check that min delay is less than max delay
	if(minDelay != -1 && maxDelay != -1)//Only run this check if they have both been set.
		if(minDelay > maxDelay)
			errorString += "Minimum delay cannot be greater than maximum delay.\n";
	if(errorString != ""){
		QMessageBox::warning( this, "Information Errors", errorString);
		return;
	}
	
	//If method has got this far everything should be ok
	accept();
}


/*! Changes the parameters displayed in the parameter table depending on the connection 
	type selected. */
void ConnectionPropertiesDialog::paramSelectionChanged(){
	unsigned short connType = ConnectionType::getType(connectionTypeCombo->currentText());
	loadParameterTable(connType);
}


/*! When the user edits a parameter in the parameter table this method is called, which
	checks the new value for errors. */
void ConnectionPropertiesDialog::paramValueChanged(int row, int col){
	int pos = 0;
	QString paramText = connParamTable->text(row, col);
	if(paramValidator->validate(paramText, pos) != QValidator::Acceptable){
		QString errorText = "Parameter: " + connParamTable->text(row, 0) + "\nPlease enter a number between -100000 and 100000";
		QMessageBox::warning(this, "Incorrect Parameter Value!", errorText);
		loadParameterTable(ConnectionType::getType(connectionTypeCombo->currentText()));
	}

	parameterError = true;
}


//-----------------------------------------------------------------------------------
//------------------------------ PRIVATE METHODS ------------------------------------
//-----------------------------------------------------------------------------------

/*! Checks the parameters of the connection for inter layer connection types
	Only need to check this for certain connection topologies. */
//FIXME MUST HANDLE ROTATION PARAMETER BEING SET BY USER
bool ConnectionPropertiesDialog::checkConnection_INTER(unsigned int fromLayerID, unsigned int toLayerID, unsigned short connectionType){
	//OnCentreOffSurround and OffCentreOnSurround
	//These two types will produce a connection field of the same size so can check them together
	if((connectionType == ConnectionType::OnCentreOffSurround) || (connectionType == ConnectionType::OffCentreOnSurround)){
		//Get the width and length of the to layer in neurons
		Query query = dbInterface->getQuery();
		query.reset();
		query<<"SELECT Width, Length FROM NeuronGroups WHERE NeuronGrpID = "<<toLayerID;
                StoreQueryResult toLayerRes= query.store();
		Row toRow(*toLayerRes.begin());
		int toWidth_Neur = Utilities::getInt((std::string)toRow["Width"]);
		int toLength_Neur = Utilities::getInt((std::string)toRow["Length"]);
		
		//Next get the size of the from layer in absolute values
		query.reset();
		query<<"SELECT Width, Length, Spacing FROM NeuronGroups WHERE NeuronGrpID = "<<fromLayerID;
                StoreQueryResult fromLayerRes= query.store();
		Row fromRow(*fromLayerRes.begin());
		int fromWidth_Neur = Utilities::getInt((std::string)fromRow["Width"]);
		int fromLength_Neur = Utilities::getInt((std::string)fromRow["Length"]);
		int fromSpacing_Pts = Utilities::getInt((std::string)fromRow["Spacing"]);
		double fromWidth_Pts = (double)((fromWidth_Neur - 1) * fromSpacing_Pts);
		double fromLength_Pts= (double)((fromLength_Neur -1) * fromSpacing_Pts);
	
		//Extract values for relevant parameters from parameter table
		double outerWidth = getParameterValue("Outer width");
		double outerLength = getParameterValue("Outer length");
		double innerWidth = getParameterValue("Inner width");
		double innerLength = getParameterValue("Inner length");
		int rotate = (int)getParameterValue("Rotate?");
		double overlap = getParameterValue("Overlap");
		double excitatoryWeight = getParameterValue("Excitation weight");
		double inhibitoryWeight = getParameterValue("Inhibition weight");
		int normalDistribution = (int)getParameterValue("Normal weight distribution?");
		double weightRange = getParameterValue("Weight range");
		
		//Do some basic sanity checks
		QString errorString = "";
		if(overlap >= outerWidth)//Overlap has to be less than OuterWidth. For the moment allow it to be negative, although this may be a bad idea.
			errorString += "Overlap has to be less than OuterWidth.\n";
			
		if(outerWidth < 1)//Outer width has to be greater than one
			errorString += "Outer width must be greater than 1. Otherwise it will not cover more than one neuron.\n";

		if(innerWidth >= outerWidth || innerLength >= outerLength)
			errorString += "Inner width and length have to be smaller than Outer width and length.";

		if(!(rotate == 1 || rotate == 0))
			errorString += "Rotate should be 0 (false) or 1 (true).\n";
		
		if(!(normalDistribution == 1 || normalDistribution ==0))
			errorString += "Normal distribution should be 0 (false) or 1 (true).\n";
		
		if(excitatoryWeight < 0.0 || excitatoryWeight > 1.0)
			errorString += "Excitation weight should be between 0.0 and 1.0.\n";
		
		if(inhibitoryWeight > 0.0 || inhibitoryWeight < -1.0)
			errorString += "Inhibition weight should be between -1.0 and 0.0.\n";
		
		if(weightRange > 2.0)
			errorString += "Cannot set weight range greater than range of possible weights.\n";
			
		//Display error messages
		if(errorString != ""){
			QMessageBox::warning(this, "Parameter Error(s)", errorString);
			return false;
		}
		
		//Determine the best fit orientation between the two layers. This will reduce the amount of adjustment needed
		bool rotateLayer, toWidthLongest = false, toLengthLongest = false, fromWidthLongest = false, fromLengthLongest = false;
		if(toWidth_Neur > toLength_Neur)
			toWidthLongest = true;
		if(toLength_Neur > toWidth_Neur)//Need to do this in case they are equal
			toLengthLongest = true;
		if(fromWidth_Neur > fromLength_Neur)
			fromWidthLongest = true;
		if(fromLength_Neur > fromWidth_Neur)
			fromLengthLongest = true;
		if((toWidthLongest && fromLengthLongest) || (toLengthLongest && fromWidthLongest)){
			rotateLayer = true;
			setParameterValue("Rotate?", 1);
		}
		else{
			rotateLayer = false;
			setParameterValue("Rotate?", 0);
		}
		
		//Now calculate the area required in the from layer if it is to be connected to the to layer with this type of connection
		double requiredConnAreaWidth, requiredConnAreaLength;
		if(rotateLayer){
			requiredConnAreaWidth = ((double)toLength_Neur * outerWidth) - (((double)toLength_Neur - 1.0) * overlap);
			requiredConnAreaLength = ((double)toWidth_Neur * outerLength) - (((double)toWidth_Neur - 1.0) * overlap);
		}
		else{
			requiredConnAreaWidth = ((double)toWidth_Neur * outerWidth) - (((double)toWidth_Neur - 1.0) * overlap);
			requiredConnAreaLength = ((double)toLength_Neur * outerLength) - (((double)toLength_Neur - 1.0) * overlap);
		}
		Utilities::roundTwoDecimalPlaces(requiredConnAreaWidth);//To reduce the precision of the comparison and hopefully avoid trivial errors 
		Utilities::roundTwoDecimalPlaces(requiredConnAreaLength);//To reduce the precision of the comparison and hopefully avoid trivial errors
		
		//Compare the two areas
		if(!((requiredConnAreaWidth == fromWidth_Pts) && (requiredConnAreaLength == fromLength_Pts))){//The two layers do not match
			//Adjust the outer width and length automatically, keeping the overlap constant
			double newOuterWidth, newOuterLength;
			if(rotateLayer){
				newOuterWidth = (fromWidth_Pts + ((double)toLength_Neur - 1.0)*overlap)/(double)toLength_Neur;
				newOuterLength = (fromLength_Pts + ((double)toWidth_Neur - 1.0)*overlap)/(double)toWidth_Neur;
			}
			else{
				newOuterWidth = (fromWidth_Pts + ((double)toWidth_Neur - 1.0)*overlap)/(double)toWidth_Neur;
				newOuterLength = (fromLength_Pts + ((double)toLength_Neur - 1.0)*overlap)/(double)toLength_Neur;
			}
			
			//If outerWidth or length are less than 1, a simple adjustment is not going to sort the connection out 
			//Probably the to layer is bigger than the from layer, which makes this type of connection senseless
			if(newOuterWidth <= 1.0 || newOuterLength <= 1.0){
				QMessageBox::warning(this, "Layer Size Mismatch!", "To make these two layers connect the adjusted outer width and length become less than 1.\nThis means that the connections will only cover one neuron.\nChoose different layers or a different type of connection.");
				return false;
			}
			
			//If this point is reached, new values should be ok.
			setParameterValue("Outer width", newOuterWidth);
			setParameterValue("Outer length", newOuterLength);
			setParameterValue("Inner width", innerWidth * (newOuterWidth/outerWidth));//Need to change this otherwise it could be larger than outerWidth
			setParameterValue("Inner length",  innerLength * (newOuterLength/outerLength));
			
			//Inform user that values have been changed.
			if(rotateLayer)
				QMessageBox::warning(this, "Layer Size Mismatch!", "There was a mismatch between the two layers you are trying to connect.\n Outer and Inner width and length have been adjusted.\n Layers will be rotated.\n Overlap has been left constant.");
			else
				QMessageBox::warning(this, "Layer Size Mismatch!", "There was a mismatch between the two layers you are trying to connect.\n Outer and Inner width and length have been adjusted.\n Overlap has been left constant.");
			return false;
		}
		else //The layers match so connection should be ok
			return true;
	}

	//Handle errors in the unstructured type
	else if (connectionType == ConnectionType::Unstructured){
		double averageWeight = getParameterValue("Average weight");
		double weightRange = getParameterValue("Weight range");
		double connectionDensity = getParameterValue("Connection density");

		//Do some basic sanity checks
		QString errorString = "";
		if(averageWeight < -1.0 || averageWeight > 1.0)
			errorString += "Weight should be between -1.0 and 1.0.\n";
		
		if(weightRange > 2.0)
			errorString += "Cannot set weight range greater than range of possible weights.\n";

		if(connectionDensity > 1.0 || connectionDensity < 0.0001)
			errorString += "Connection density must be between 1.0 and 0.0001.\n";
			
		//Display error messages
		if(errorString != ""){
			QMessageBox::warning(this, "Parameter Error(s)", errorString);
			return false;
		}
		return true;
	}

	//Check parameters for Unstructured excitatory inhibitory
	else if(connectionType == ConnectionType::UnstructuredExInhibInter){
		double excitatoryPercentage = getParameterValue("Excitatory percentage");
		double excitationWeight = getParameterValue("Excitation weight");
		double excitationWeightRange = getParameterValue("Excitation weight range");
		double inhibitionWeight = getParameterValue("Inhibition weight");
		double inhibitionWeightRange = getParameterValue("Inhibition weight range");
		double excitationConnProb = getParameterValue("Excitation connection prob");
		double inhibitionConnProb = getParameterValue("Inhibition connection prob");

		//Do some basic sanity checks
		QString errorString = "";
		if(excitatoryPercentage < 0.0 || excitatoryPercentage > 100.0)
			errorString += "Excitatory percentage must be between 0 and 100.\n";
		
		if(excitationWeight < 0.0 || excitationWeight > 1.0)
			errorString += "Excitation weight must be between 0.0 and 1.0.\n";
		
		if(excitationWeightRange < 0.0 || excitationWeightRange > 1.0)
			errorString += "Excitation weight range must be between 0 and 1.\n";
				
		if(inhibitionWeight < -1.0 || inhibitionWeight > 0.0)
			errorString += "Inhibition weight must be between -1.0 and 0.0.\n";
		
		if(inhibitionWeightRange < 0.0 || inhibitionWeightRange > 1.0)
			errorString += "Inhibition weight range must be between 0 and 1.\n";

		if(excitationConnProb < 0.0 || excitationConnProb > 1.0)
			errorString += "Excitation connection probability must be between 0 and 1.\n";
		
		if(inhibitionConnProb < 0.0 || inhibitionConnProb > 1.0)
			errorString += "Inhibition connection probability must be between 0 and 1.\n";
			
		//Display error messages
		if(errorString != ""){
			QMessageBox::warning(this, "Parameter Error(s)", errorString);
			return false;
		}
		return true;
	}

	//Handle errors in the topographic type
	else if (connectionType == ConnectionType::Topographic){
		double averageWeight = getParameterValue("Average weight");
		double weightRange = getParameterValue("Weight range");
		double overlap = getParameterValue("Overlap");

		//Do some basic sanity checks
		QString errorString = "";
		if(averageWeight < -1.0 || averageWeight > 1.0)
			errorString += "Weight should be between -1.0 and 1.0.\n";
		
		if(weightRange > 2.0)
			errorString += "Cannot set weight range greater than range of possible weights.\n";
		
		if(overlap < 0.0)
			errorString += "The overlap must be zero or more.\n";

		//Display error messages
		if(errorString != ""){
			QMessageBox::warning(this, "Parameter Error(s)", errorString);
			return false;
		}
		return true;
	}

	//Handle errors for the Virtual type of connection
	else if(connectionType == ConnectionType::Virtual){
		/*Need to check whether there is a non-virtual connection between the layers. 
			No point in creating a virtual connection when there is a non-virtual connection */
		unsigned int fromNeurGrpID = Utilities::getNeuronGrpID(fromLayerCombo->currentText());
		unsigned int toNeurGrpID = Utilities::getNeuronGrpID(toLayerCombo->currentText());
		try{
			Query query = dbInterface->getQuery();
			query.reset();
			query<<"SELECT COUNT(*) FROM ConnectionGroups WHERE FromNeuronGrpID = "<<fromNeurGrpID<<" AND ToNeuronGrpID = "<<toNeurGrpID;
                        StoreQueryResult countRes= query.store();
			Row countRow(*countRes.begin());
			if(Utilities::getUInt((std::string) countRow["COUNT(*)"]) > 0 ){//Already a connection between these neuron groups
				QMessageBox::warning(this, "Connection Error", "Virtual connections are unnecessary when there are real connections between neuron groups");
				return false;
			}
		}
		catch(Exception &exception){
			cerr<<"ConnectionPropertiesDialog: Exception: "<<exception.what();
			return false;
		}
		return true;
	}

	//Handle errors for the SIMNOS component connection type
	else if (connectionType == ConnectionType::SIMNOSComponent){
		double averageWeight = getParameterValue("Average weight");
		double weightRange = getParameterValue("Weight range");

		//Do some basic sanity checks
		QString errorString = "";
		if(averageWeight < -1.0 || averageWeight > 1.0)
			errorString += "Weight should be between -1.0 and 1.0.\n";
		
		if(weightRange > 2.0)
			errorString += "Cannot set weight range greater than range of possible weights.\n";
		
		//Check that layers are the same width
		//Get the dimensions of the from group
		Query query = dbInterface->getQuery();
		query<<"SELECT Width FROM NeuronGroups WHERE NeuronGrpID = "<<fromLayerID;
                StoreQueryResult fromLayerRes= query.store();
		Row fromRow(*fromLayerRes.begin());
		int fromWidth = Utilities::getInt((std::string)fromRow["Width"]);
		
		//Get the dimensions of the to group
		query.reset();
		query<<"SELECT Width FROM NeuronGroups WHERE NeuronGrpID = "<<toLayerID;
                StoreQueryResult toLayerRes= query.store();
		Row toRow(*toLayerRes.begin());
		int toWidth = Utilities::getInt((std::string)toRow["Width"]);

		//Check lengths are the same
		if(fromWidth != toWidth)
			errorString += "From and to neuron groups must be the same width for SIMNOS Component connections.\n";

		//Display error messages
		if(errorString != ""){
			QMessageBox::warning(this, "Parameter Error(s)", errorString);
			return false;
		}
		return true;
	}

	//Inter layer connection type has not been found
	else{
		cerr<<"ConnectionPropertiesDialog: Connection type has not been found in parameter error checking method: "<<connectionType<<endl;
		QMessageBox::critical( 0, "Connection Type Error", "Connection type has not been found.");
		return false;
	}
}


/*! Checks the parametes for intra layer connections. */
bool ConnectionPropertiesDialog::checkConnection_INTRA(unsigned short connectionType){

	//Check parameters for simple cortex connections
	if(connectionType == ConnectionType::SimpleCortex){
		double inhibitionRadius = getParameterValue("Inhibition radius");
		double excitationRadius = getParameterValue("Excitation radius");
		double overlap = getParameterValue("Overlap");
		double excitationConnDensity = getParameterValue("Excitation connection density");
		double inhibitionConnDensity = getParameterValue("Inhibition connection density");
		double normalDistribution = getParameterValue("Normal weight distribution?");
		double weightRange = getParameterValue("Weight range");
		double excitatoryWeight = getParameterValue("Excitation weight");
		double inhibitoryWeight = getParameterValue("Inhibition weight");
		
		//Do some basic sanity checks
		QString errorString = "";
		if(inhibitionRadius <= excitationRadius)
			errorString += "For this type of connection inhibition radius should be greater than the excitation radius.\n";
		
		if(overlap >= excitationRadius)
			errorString += "Overlap cannot be greater than excitation radius.\n";
		
		if(overlap >= inhibitionRadius)
			errorString += "Overlap cannot be greater than inhibition radius.\n";
		
		if(overlap < 0)
			errorString += "Overlap cannot be a negative number.\n";
		
		if(excitationConnDensity < 0.0 )
			errorString += "Connection density must be greater than 0.\n";

		if(inhibitionConnDensity < 0.0 )
			errorString += "Inhibition density must be greater than 0.\n";
		
		if(!(normalDistribution == 1 || normalDistribution ==0))
			errorString += "Normal distribution should be 0 (false) or 1 (true).\n";
		
		if(excitatoryWeight < 0.0 || excitatoryWeight > 1.0)
			errorString += "Excitation weight should be between 0.0 and 1.0.\n";
		
		if(inhibitoryWeight > 0.0 || inhibitoryWeight < -1.0)
			errorString += "Inhibition weight should be between -1.0 and 0.0.\n";
		
		if(weightRange > 2.0)
			errorString += "Cannot set weight range greater than range of possible weights.\n";
			
		//Display error messages
		if(errorString != ""){
			QMessageBox::warning(this, "Parameter Error(s)", errorString);
			return false;
		}
		return true;	
	}

	//Check parameters for Unstructured excitatory inhibitory
	else if(connectionType == ConnectionType::UnstructuredExInhibIntra){
		double excitatoryPercentage = getParameterValue("Excitatory percentage");
		double excitationWeight = getParameterValue("Excitation weight");
		double excitationWeightRange = getParameterValue("Excitation weight range");
		double inhibitionWeight = getParameterValue("Inhibition weight");
		double inhibitionWeightRange = getParameterValue("Inhibition weight range");
		double excitationConnProb = getParameterValue("Excitation connection prob");
		double inhibitionConnProb = getParameterValue("Inhibition connection prob");

		//Do some basic sanity checks
		QString errorString = "";
		if(excitatoryPercentage < 0.0 || excitatoryPercentage > 100.0)
			errorString += "Excitatory percentage must be between 0 and 100.\n";
		
		if(excitationWeight < 0.0 || excitationWeight > 1.0)
			errorString += "Excitation weight must be between 0.0 and 1.0.\n";
		
		if(excitationWeightRange < 0.0 || excitationWeightRange > 1.0)
			errorString += "Excitation weight range must be between 0 and 1.\n";
				
		if(inhibitionWeight < -1.0 || inhibitionWeight > 0.0)
			errorString += "Inhibition weight must be between -1.0 and 0.0.\n";
		
		if(inhibitionWeightRange < 0.0 || inhibitionWeightRange > 1.0)
			errorString += "Inhibition weight range must be between 0 and 1.\n";

		if(excitationConnProb < 0.0 || excitationConnProb > 1.0)
			errorString += "Excitation connection probability must be between 0 and 1.\n";
		
		if(inhibitionConnProb < 0.0 || inhibitionConnProb > 1.0)
			errorString += "Inhibition connection probability must be between 0 and 1.\n";
			
		//Display error messages
		if(errorString != ""){
			QMessageBox::warning(this, "Parameter Error(s)", errorString);
			return false;
		}
		return true;
	}

	//Have not found this intra layer connection type
	else{
		cerr<<"ConnectionPropertiesDialog: Connection Type not recognized in parameter checking method: "<<connectionType<<endl;
		QMessageBox::critical( 0, "Connection Type Error", "Connection type has not been found.");
		return false;
	}
}


/*! Fills connection type combo with the connection types appropriate to inter layer 
	connections and sets up the parameter table appropriately. */
void ConnectionPropertiesDialog::fillConnectionTypeCombo_INTER(){
	connectionTypeCombo->clear();
	unsigned short* interTypeArray = ConnectionType::getInterLayerTypes();
	for(int i=0; i<ConnectionType::NumberInterTypes; i++){
		if(interTypeArray[i] != ConnectionType::TempVirtual && interTypeArray[i] != ConnectionType::SIMNOSComponent)//Filter out temporary virtual connections and SIMNOS connections
			connectionTypeCombo->insertItem(ConnectionType::getDescription(interTypeArray[i]));
	}
	//If there is at least one inter connection type
	if(ConnectionType::NumberInterTypes > 0){
		loadParameterTable(interTypeArray[0]);
	}
	//If there are no inter connection types defined, reset table to zero rows.
	else
		connParamTable->setNumRows(0);

	//Clean up dynamically allocated array
	delete [] interTypeArray;
}



/*! Fills the connection type combo with the connection types appropriate 
	to intra layer connections. */
void ConnectionPropertiesDialog::fillConnectionTypeCombo_INTRA(){
	connectionTypeCombo->clear();
	unsigned short* intraTypeArray = ConnectionType::getIntraLayerTypes();
	for(int i=0; i<ConnectionType::NumberIntraTypes; i++){
		connectionTypeCombo->insertItem(ConnectionType::getDescription(intraTypeArray[i]));
	}

	//If there is at least one intra connection type
	if(ConnectionType::NumberIntraTypes > 0){
		loadParameterTable(intraTypeArray[0]);
	}
	//If there are no intra connection types defined, reset table to zero rows.
	else
		connParamTable->setNumRows(0);
	
	//Clean up dynamically allocated array
	delete [] intraTypeArray;
}


/*! Searches through the parameter table to get the latest value of a parameter. */
double ConnectionPropertiesDialog::getParameterValue(QString paramName){
	for(int i=0; i<connParamTable->numRows(); i++){
		if(connParamTable->text(i, 0) == paramName){//Found parameter, need to convert to double
			bool ok = true;
			double tempDouble = connParamTable->text(i, 1).toDouble(&ok);
			if(ok)
				return tempDouble;
			else{
                                cerr<<"ConnectionPropertiesDialog: PARAMETER TO DOUBLE CONVERSION ERROR: "<<paramName.toStdString()<<endl;
				QMessageBox::critical( 0, "Conversion Error", "Parameter to double conversion error.");
				exit(1);
			}
		}
	}
        cerr<<"ConnectionPropertiesDialog: PARAMETER NOT FOUND: "<<paramName.toStdString()<<endl;
	QMessageBox::critical( 0, "Parameter Error", "Parameter not found.");
	exit(1);
}


/*! Loads up the layer names into the combo boxes. */
void ConnectionPropertiesDialog::loadLayerNames(){
	Query query = dbInterface->getQuery();
	query.reset();

	if(editMode){
		//Sort out from layer
		query<<"SELECT Name FROM NeuronGroups WHERE NeuronGrpID = "<<fromNeurGrpID;
                StoreQueryResult fromLayerResult= query.store();
		Row fromLayerRow(*fromLayerResult.begin());//Neuron grp id is unique
                string fromLayerString((std::string)fromLayerRow["Name"] + " [" + QString::number(fromNeurGrpID).toStdString() + "]");
                fromLayerCombo->insertItem(fromLayerString.data());

		//Sort out to layer
		query.reset();
		query<<"SELECT Name FROM NeuronGroups WHERE NeuronGrpID = "<<toNeurGrpID;
                StoreQueryResult toLayerResult= query.store();
		Row toLayerRow(*toLayerResult.begin());//Neuron grp id is unique
                string toLayerString((std::string)toLayerRow["Name"] + " [" + QString::number(toNeurGrpID).toStdString() + "]");
                toLayerCombo->insertItem(toLayerString.data());
	}
	else{//Add all neuron groups to combos
		query<<"SELECT Name, NeuronGrpID FROM NeuronGroups";
                StoreQueryResult layerResult= query.store();
                for(StoreQueryResult::iterator resIter = layerResult.begin(); resIter < layerResult.end(); resIter++){
			Row row(*resIter);
			string layerString((std::string)row.at(0) + " [" += (std::string)row[1] += "]");
			intraLayerCombo->insertItem(layerString.data());
			fromLayerCombo->insertItem(layerString.data());
			toLayerCombo->insertItem(layerString.data());
		}
	}
}


/*! Loads parameter table from parameters specified in ConnectionType
	These should all be doubles. */
void ConnectionPropertiesDialog::loadParameterTable(unsigned short connType){
	//First create map and fill it with parameters
	map<string, string> tempParamMap;
	ConnectionType::getParameters(tempParamMap, connType);
	
	//Now fill parameter table with parameters
	int rowCount = 0;
	map<string, string>::iterator iter = tempParamMap.begin();
	connParamTable->setNumRows(0);
	while(iter != tempParamMap.end()){
		connParamTable->insertRows(rowCount, 1);
		connParamTable->setItem( rowCount, 0, new Q3TableItem( connParamTable, Q3TableItem::Never, iter->first.data()));
		connParamTable->setItem( rowCount, 1, new Q3TableItem(connParamTable, Q3TableItem::WhenCurrent, iter->second.data()));
		rowCount++;
		iter++;
	}
}


/*! Sets the value of a parameter. */
void ConnectionPropertiesDialog::setParameterValue(QString paramName, double value){
	for(int i=0; i<connParamTable->numRows(); i++){
		if(connParamTable->text(i, 0) == paramName){//Found parameter
			QString tempStr;
			connParamTable->setText(i, 1, tempStr.setNum(value));
			return;
		}
	}
        cerr<<"ConnectionPropertiesDialog: PARAMETER NOT FOUND: "<<paramName.toStdString()<<endl;
	QMessageBox::critical( 0, "Parameter Error", "Parameter not found.");
	//This error is non critical because it only prevents program from changing parameter values
}



