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
#include "NetworkViewerProperties.h"
#include "Debug.h"
#include "Utilities.h"
#include "SimulationManager.h"
#include "SpikeStreamMainWindow.h"

//Qt includes
#include <qlayout.h>
#include <q3buttongroup.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QLabel>
#include <Q3VBoxLayout>

//Other includes
#include <mysql++.h>
using namespace std;
using namespace mysqlpp;


/*! Constructor. */
NetworkViewerProperties::NetworkViewerProperties(QWidget *parent, NetworkViewer *nwViewer, DBInterface *dbInter) : QWidget(parent, "Network Viewer Properties"){

	//Store reference to the networkViewer and dbInterface
	networkViewer = nwViewer;
	dbInterface = dbInter;
	
	//Initialise data variables
	firstSingleNeuronNumber = 0;
	secondSingleNeuronNumber = 0;
	showFromConns = true;
	showToConns = true;
	betweenMode = false;

	//Create a dialog to control the highlighting
	highlightDialog = new HighlightDialog(this, networkViewer);

	//Create main vertical box for layout
	Q3VBoxLayout *vBox = new Q3VBoxLayout(this, 5, 10, "Main vertical Box");
	vBox->addSpacing(5);

	//Add button to control highlighting
	Q3HBoxLayout* highlightBox = new Q3HBoxLayout();
	highlightButton = new QPushButton("Highlight", this);
	highlightButton->setBaseSize(150, 20);
	highlightButton->setMinimumSize(150, 20);
	highlightButton->setMaximumSize(150, 20);
	connect(highlightButton, SIGNAL(clicked()), this, SLOT(highlightButtonPressed()));
	highlightBox->addWidget(highlightButton);
	highlightBox->addStretch(5);
	vBox->addLayout(highlightBox);

	//Set up check box to toggle full rendering of scene
	fullRenderCheckBox = new QCheckBox("Render high quality view", this);
	vBox->addWidget(fullRenderCheckBox);
	
	//Set up label and combo box to control the delay before full scene rendering takes place
	Q3HBoxLayout *renderDelayBox = new Q3HBoxLayout();
	renderDelayBox->addSpacing(20);
	renderDelayLabel = new QLabel("Render delay", this);
	renderDelayLabel->setEnabled(false);
	renderDelayBox->addWidget(renderDelayLabel);
	renderDelayCombo = new QComboBox(this);
	renderDelayCombo->insertItem(QString::number(0.5) + " seconds");
	renderDelayCombo->insertItem(QString::number(1) + " seconds");
	renderDelayCombo->insertItem(QString::number(2) + " seconds");
	renderDelayCombo->insertItem(QString::number(5) + " seconds");
	renderDelayCombo->insertItem(QString::number(10) + " seconds");
	renderDelayCombo->setEnabled(false);
	renderDelayCombo->setCurrentItem(1);
	renderDelayBox->addWidget(renderDelayCombo);
	renderDelayBox->addStretch(5);
	vBox->addLayout(renderDelayBox);
	
	//Set up progress bar to display render progress
	Q3HBoxLayout *renderProgressBox = new Q3HBoxLayout();
	renderProgressBox->addSpacing(20);
	renderProgressLabel = new QLabel("Render progress", this);
	renderProgressLabel->setEnabled(false);
	renderProgressBox->addWidget(renderProgressLabel);
	progressBar = new Q3ProgressBar(10, this);
	progressBar->setEnabled(false);
	networkViewer->setRenderProgressBar(progressBar);//Set the progress bar in the network viewer to enable render updates
	renderProgressBox->addWidget(progressBar);
	renderCancelButton = new QPushButton("Cancel", this);
	renderCancelButton->setEnabled(false);
	renderProgressBox->addWidget(renderCancelButton);
	connect (renderCancelButton, SIGNAL(clicked()), this, SLOT(cancelRenderProgress()));
	renderProgressBox->addStretch(5);
	vBox->addLayout(renderProgressBox);
	
	vBox->addSpacing(20);
	
	//Add controls for connections
	connectionsCheckBox = new QCheckBox("Show connections", this);
	connectionsCheckBox->setChecked(true);//Start in connection mode
	vBox->addWidget(connectionsCheckBox);
	
	//Set up radio buttons to choose between connection displays
	Q3ButtonGroup* connButtonGroup = new Q3ButtonGroup();
	allConnRadioButt = new QRadioButton("All selected connections", this);
	allConnRadioButt->setChecked(true);
	allConnRadioButt->setEnabled(true);
	connButtonGroup->insert(allConnRadioButt);
	connsSingleNeurRadioButt = new QRadioButton("", this);
	connsSingleNeurRadioButt->setChecked(false);
	connsSingleNeurRadioButt->setEnabled(true);
	connButtonGroup->insert(connsSingleNeurRadioButt);
	
	//Add all connection radio button to viewer widget
	Q3HBoxLayout *allConnBox = new Q3HBoxLayout();
	allConnBox->addSpacing(20);
	allConnBox->addWidget(allConnRadioButt);
	vBox->addLayout(allConnBox);
	
	//Add neuron connection radio button
	Q3HBoxLayout *neuronConnsBox = new Q3HBoxLayout();
	neuronConnsBox->addSpacing(20);
	neuronConnsBox->addWidget(connsSingleNeurRadioButt);
	
	//Add filter to view positive or negative connections
	neurConnsFilterCombo = new QComboBox(this);
	neurConnsFilterCombo->insertItem("All connections");
	neurConnsFilterCombo->insertItem("Positive connections");
	neurConnsFilterCombo->insertItem("Negative connections");
	neurConnsFilterCombo->setEnabled(false);
	neuronConnsBox->addWidget(neurConnsFilterCombo);
	
	//Add filter to view from/to connections
	neurConnsFromToCombo = new QComboBox(this);
	neurConnsFromToCombo->insertItem("from/to");
	neurConnsFromToCombo->insertItem("from");
	neurConnsFromToCombo->insertItem("to");
	neurConnsFromToCombo->insertItem("between");
	neurConnsFromToCombo->setEnabled(false);
	neuronConnsBox->addWidget(neurConnsFromToCombo);
	
	//Add the rest of this line
	singleNeurNumLabel_1 = new QLabel(QString::number(firstSingleNeuronNumber), this);
	singleNeurNumLabel_1->setEnabled(false);
	neuronConnsBox->addWidget(singleNeurNumLabel_1);
	inLabel_1 = new QLabel("in", this);
	inLabel_1->setEnabled(false);
	neuronConnsBox->addWidget(inLabel_1);
	neurConnsNeurGrpCombo_1 = new QComboBox(this);
	neurConnsNeurGrpCombo_1->setEnabled(false);
	loadLayerNames(neurConnsNeurGrpCombo_1);
	neuronConnsBox->addWidget(neurConnsNeurGrpCombo_1);
	andLabel = new QLabel("and", this);
	andLabel->setEnabled(false);
	neuronConnsBox->addWidget(andLabel);
	singleNeurNumLabel_2 = new QLabel("0", this);
	singleNeurNumLabel_2->setEnabled(false);
	neuronConnsBox->addWidget(singleNeurNumLabel_2);
	inLabel_2 = new QLabel("in", this);
	inLabel_2->setEnabled(false);
	neuronConnsBox->addWidget(inLabel_2);
	neurConnsNeurGrpCombo_2 = new QComboBox(this);
	neurConnsNeurGrpCombo_2->setEnabled(false);
	loadLayerNames(neurConnsNeurGrpCombo_2);
	neuronConnsBox->addWidget(neurConnsNeurGrpCombo_2);
	neuronConnsBox->addStretch(5);
	vBox->addLayout(neuronConnsBox);
	
	//Add connection details check box
	Q3HBoxLayout *connDetailsCheckBoxBox = new Q3HBoxLayout();
	connDetailsCheckBoxBox->addSpacing(40);
	connectionDetailsCheckBox = new QCheckBox("Connection details", this);
	connectionDetailsCheckBox->setChecked(false);
	connectionDetailsCheckBox->setEnabled(false);
	connDetailsCheckBoxBox->addWidget(connectionDetailsCheckBox);
	vBox->addLayout(connDetailsCheckBoxBox);
	
	//Add widgets to display total number of connections
	Q3HBoxLayout *neurTotalConnsBox = new Q3HBoxLayout();
	neurTotalConnsBox->addSpacing(40);
	neurConnsLabel = new QLabel("Total number of connections: ", this);
	neurConnsLabel->setEnabled(false);
	neurTotalConnsBox->addWidget(neurConnsLabel);
	neurTotalConnsLabel = new QLabel("", this);
	neurTotalConnsLabel->setEnabled(false);
	neurTotalConnsBox->addWidget(neurTotalConnsLabel);
	neurTotalConnsBox->addStretch(5);
	vBox->addLayout(neurTotalConnsBox);
	
	//Add connection details table
	Q3HBoxLayout *connDetailsTableBox = new Q3HBoxLayout();
	connDetailsTableBox->addSpacing(40);
	connectionDetailsTable = new Q3Table(0, 6, this);
	connectionDetailsTable->setEnabled(false);
	connectionDetailsTable->setSelectionMode(Q3Table::NoSelection);
	connectionDetailsTable->verticalHeader()->hide();
	connectionDetailsTable->setLeftMargin(0);
	Q3Header * connTableHeader = connectionDetailsTable->horizontalHeader();
	connTableHeader->setLabel(0, "ConnGrpID" );
	connectionDetailsTable->setColumnWidth(0, 75);
	connTableHeader->setLabel(1, "PreConnNeurID" );
	connectionDetailsTable->setColumnWidth(1, 110);
	connTableHeader->setLabel(2, "PostConnNeurID" );
	connectionDetailsTable->setColumnWidth(2, 110);
	connTableHeader->setLabel(3, "Saved Weight" );
	connectionDetailsTable->setColumnWidth(3, 90);
	connTableHeader->setLabel(4, "Temp Weight" );
	connectionDetailsTable->setColumnWidth(4, 90);
	connTableHeader->setLabel(5, "Delay" );
	connectionDetailsTable->setColumnWidth(5, 60);
	connectionDetailsTable->setMinimumSize(600,300);
	connDetailsTableBox->addWidget(connectionDetailsTable);
	connDetailsTableBox->addStretch(5);
	vBox->addLayout(connDetailsTableBox);
	
	vBox->addStretch(5);

	//Connect signals to slots
	connect (fullRenderCheckBox, SIGNAL(clicked()), this, SLOT(renderStateChanged()));
	connect (renderDelayCombo, SIGNAL(activated(int)), this, SLOT(renderDelayChanged(int)));
	connect (connectionsCheckBox, SIGNAL(clicked()), this, SLOT(connCheckBoxChanged()));
	connect (allConnRadioButt, SIGNAL(clicked()), this, SLOT(showConnectionsChanged()));
	connect (connsSingleNeurRadioButt, SIGNAL(clicked()), this, SLOT(showConnectionsChanged()));
	connect (neurConnsNeurGrpCombo_1, SIGNAL(activated(int)), this, SLOT(firstNeuronComboChanged(int)));
	connect (neurConnsNeurGrpCombo_2, SIGNAL(activated(int)), this, SLOT(secondNeuronComboChanged(int)));
	connect (neurConnsFilterCombo, SIGNAL(activated(int)), this, SLOT(filterCombosChanged(int)));
	connect (neurConnsFromToCombo, SIGNAL(activated(int)), this, SLOT(filterCombosChanged(int)));
	connect (connectionDetailsCheckBox, SIGNAL(clicked()), this, SLOT(showConnectionDetailsChanged()));
	connect (connTableHeader, SIGNAL(clicked(int)), this, SLOT(sortRows(int)));
}


/*! Destructor. */
NetworkViewerProperties::~NetworkViewerProperties(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING NETWORK VIEWER PROPERTIES"<<endl;
	#endif//MEMORY_DEBUG
}


//--------------------------------------------------------------------------------------
//--------------------------------- PUBLIC METHODS -------------------------------------
//--------------------------------------------------------------------------------------

/*! Reloads details about the connections if they are visible. */
void NetworkViewerProperties::reloadConnectionDetails(){
	if(connectionDetailsCheckBox->isChecked()){
		loadConnectionDetails();
	}
}


/*! Updates the connection details table. */
void NetworkViewerProperties::reloadConnections(){
	if(connectionDetailsCheckBox->isChecked()){
		loadConnectionDetails();
	}
}


/*! Reloads the list of neuron group names and resets all information relevant to neuron groups. */
void NetworkViewerProperties::reloadNeuronGroups(){
	try{
		loadLayerNames(neurConnsNeurGrpCombo_1);
		loadLayerNames(neurConnsNeurGrpCombo_2);
		if(connsSingleNeurRadioButt->isOn() && (neurConnsNeurGrpCombo_1->count() > 0)){
			unsigned int layer1ID = Utilities::getNeuronGrpID(neurConnsNeurGrpCombo_1->currentText());
			if(betweenMode){
				unsigned int layer2ID = Utilities::getNeuronGrpID(neurConnsNeurGrpCombo_2->currentText());
				networkViewer->setNeuronConnectionMode(true, layer1ID, true, layer2ID);
			}
			else{
				networkViewer->setNeuronConnectionMode(true, layer1ID, false, 0);
			}
			if(connectionDetailsCheckBox->isChecked()){
				loadConnectionDetails();
			}
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"NetworkViewerProperties: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query reloading neuron groups: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Load Neuron Groups Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"NetworkViewerProperties: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown reloading neuron groups: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Load Neuron Groups Error", errorString);
	}
	catch (std::exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"NetworkViewerProperties: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown reloading neuron groups: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Load Neuron Groups Error", errorString);
	}
}


/*! Controls which single neuron is displayed in the connection details table. */
void NetworkViewerProperties::setFirstSingleNeuronNumber(unsigned int neuronNumber){
	firstSingleNeuronNumber = neuronNumber;
	singleNeurNumLabel_1->setText(QString::number(neuronNumber));
	if(connectionDetailsCheckBox->isChecked()){
		loadConnectionDetails();
	}
	//Update the fire neuron text box to the selected neuron
	if(neurConnsNeurGrpCombo_1->count() > 0){
		unsigned int neurGrpID = Utilities::getNeuronGrpID(neurConnsNeurGrpCombo_1->currentText());
		SpikeStreamMainWindow::spikeStrMainWin->setFromNeuronID(neurGrpID, neuronNumber);
	}	
}


/*! Sets the label displaying which connection or neuron group is being rendered. */
void NetworkViewerProperties::setRenderProgressLabel(const QString labelText){
	renderProgressLabel->setText(labelText);
}


/*! Controls which single neuron is displayed in the connection details table. */
void NetworkViewerProperties::setSecondSingleNeuronNumber(unsigned int neuronNumber){
	secondSingleNeuronNumber = neuronNumber;
	singleNeurNumLabel_2->setText(QString::number(neuronNumber));
	if(connectionDetailsCheckBox->isChecked()){
		loadConnectionDetails();
	}
	//Update any classes that depend on this number
	SpikeStreamMainWindow::spikeStrMainWin->setToNeuronID(neuronNumber);
}


//-----------------------------------------------------------------------------------------
//---------------------------------------- SLOTS ------------------------------------------
//-----------------------------------------------------------------------------------------

/*! Linked to the cancel render button and cancels the render. */
void NetworkViewerProperties::cancelRenderProgress(){
	networkViewer->cancelRenderProgress();
}


/*! Called when the check box controlling whether connections are displayed is clicked
	Hides or shows the connections and their associated GUI components. */
void NetworkViewerProperties::connCheckBoxChanged(){
	if(connectionsCheckBox->isChecked()){
		allConnRadioButt->setEnabled(true);
		connsSingleNeurRadioButt->setEnabled(true);
		if(connsSingleNeurRadioButt->isChecked()){
			neurConnsNeurGrpCombo_1->setEnabled(true);
			neurConnsFilterCombo->setEnabled(true);
			neurConnsFromToCombo->setEnabled(true);
			connectionDetailsCheckBox->setEnabled(true);
			inLabel_1->setEnabled(true);
			singleNeurNumLabel_1->setEnabled(true);
			if(connectionDetailsCheckBox->isChecked()){
				connectionDetailsTable->setEnabled(true);
				neurTotalConnsLabel->setEnabled(true);
				neurConnsLabel->setEnabled(true);
			}
			if(betweenMode){
				neurConnsNeurGrpCombo_2->setEnabled(true);
				andLabel->setEnabled(true);
				inLabel_2->setEnabled(true);
				singleNeurNumLabel_2->setEnabled(true);
			}
		}
		networkViewer->showConnections(true);
	}
	else{
		allConnRadioButt->setEnabled(false);
		connsSingleNeurRadioButt->setEnabled(false);
		neurConnsNeurGrpCombo_1->setEnabled(false);
		neurConnsNeurGrpCombo_2->setEnabled(false);
		neurConnsFilterCombo->setEnabled(false);
		neurConnsFromToCombo->setEnabled(false);
		neurConnsLabel->setEnabled(false);
		connectionDetailsCheckBox->setEnabled(false);
		connectionDetailsTable->setEnabled(false);
		neurTotalConnsLabel->setEnabled(false);
		singleNeurNumLabel_1->setEnabled(false);
		andLabel->setEnabled(false);
		inLabel_1->setEnabled(false);
		inLabel_2->setEnabled(false);
		singleNeurNumLabel_2->setEnabled(false);
		networkViewer->showConnections(false);
	}
}


/*! Called when the filters controlling the connections to a single neuron are changed
	Sets the filters that are applied to the connections in singel neuron mode. */
void NetworkViewerProperties::filterCombosChanged(int){
	//Extract the from/to settings
	switch(neurConnsFromToCombo->currentItem()){
		case 0:
			showFromConns = true;
			showToConns = true;
			betweenMode = false;
			//Enable relevant components
			neurConnsNeurGrpCombo_2->setEnabled(false);
			singleNeurNumLabel_2->setEnabled(false);
			andLabel->setEnabled(false);
			inLabel_2->setEnabled(false);
		break;
		case 1:
			showFromConns = true;
			showToConns = false;
			betweenMode = false;
			//Enable relevant components
			neurConnsNeurGrpCombo_2->setEnabled(false);
			singleNeurNumLabel_2->setEnabled(false);
			andLabel->setEnabled(false);
			inLabel_2->setEnabled(false);
		break;
		case 2:
			showFromConns = false;
			showToConns = true;
			betweenMode = false;
			//Enable relevant components
			neurConnsNeurGrpCombo_2->setEnabled(false);
			singleNeurNumLabel_2->setEnabled(false);
			andLabel->setEnabled(false);
			inLabel_2->setEnabled(false);
		break;
		case 3:
			showFromConns = true;
			showToConns = true;
			betweenMode = true;
			//Enable relevant components
			neurConnsNeurGrpCombo_2->setEnabled(true);
			singleNeurNumLabel_2->setEnabled(true);
			andLabel->setEnabled(true);
			inLabel_2->setEnabled(true);
		break;
		default:
			cerr<<"NetworkViewerProperties.cpp: CANNOT IDENTIFY FROM/TO COMBO INDEX - PROBABLY CHANGED AT SOME POINT WITHOUT UPDATING loadConnectionDetails()";
			QMessageBox::critical( 0, "Connection Database Error", "Cannot identify from/to combo index");
			return;
	}

	//Get the settings for the neuron filter in the network viewer properties
	short minWeight, maxWeight;
	switch(neurConnsFilterCombo->currentItem()){
		case 0://Show all connections to neuron
			minWeight = -128;
			maxWeight = 127;
		break;
		case 1://Show positive connection weights to this neuron
			minWeight = 0;
			maxWeight = 127;
		break;
		case 2://Show negative connection weights to this neuron
			minWeight = -128;
			maxWeight = 0;
		break;
		default://Cannot find index. Must have been changed at some point without updating this method
			cerr<<"NetworkViewerProperties: INDEX NOT FOUND"<<endl;
			QMessageBox::critical( 0, "Filter Error", "Cannot identify neuron connections filter index.");
			return;
	}

	//Set the filter mode in the network viewer
	//Check to see if filter mode is on or off. With filter mode off drawing is done more quickly
	if(neurConnsFromToCombo->currentItem() == 0 && neurConnsFilterCombo->currentItem() == 0)
		networkViewer->setNeuronFilterMode(false, -1, -1, true, true, false);//Switch off filter mode
	else
		networkViewer->setNeuronFilterMode(true, minWeight, maxWeight, showFromConns, showToConns, false);
	
	//Set the between mode in the network viewer
	if(betweenMode && neurConnsNeurGrpCombo_1->count() > 0){
		unsigned int layer1ID = Utilities::getNeuronGrpID(neurConnsNeurGrpCombo_1->currentText());//Get the first neuron id
		unsigned int layer2ID = Utilities::getNeuronGrpID(neurConnsNeurGrpCombo_2->currentText());//Get the second neuron id
		networkViewer->setNeuronConnectionMode(true, layer1ID, true, layer2ID);
	}
	else if(neurConnsNeurGrpCombo_1->count() > 0){
		unsigned int layer1ID = Utilities::getNeuronGrpID(neurConnsNeurGrpCombo_1->currentText());//Get the first neuron id
		networkViewer->setNeuronConnectionMode(true, layer1ID, false, 0);
	}
	
	if(connectionDetailsCheckBox->isChecked())
		loadConnectionDetails();
}


/*! Called when combo box controlling which layer is used for single neuron mode is activated. */
void NetworkViewerProperties::firstNeuronComboChanged(int){
	if(connsSingleNeurRadioButt->isOn() && neurConnsNeurGrpCombo_1->count() > 0){
		unsigned int layer1ID = Utilities::getNeuronGrpID(neurConnsNeurGrpCombo_1->currentText());
		if(betweenMode){//In between mode
			unsigned int layer2ID = Utilities::getNeuronGrpID(neurConnsNeurGrpCombo_2->currentText());//Get the second neuron id
			networkViewer->setNeuronConnectionMode(true, layer1ID, true, layer2ID);
		}
		else{
			networkViewer->setNeuronConnectionMode(true, layer1ID, false, 0);
		}
	}
}


/*! Launches a dialog to add or remove the highlighting of neurons in the network.*/
void NetworkViewerProperties::highlightButtonPressed(){
	highlightDialog->show();
}


/*! Controls the setting of the render delay. */
void NetworkViewerProperties::renderDelayChanged(int){
	QString comboText = renderDelayCombo->currentText();
	QString delayText = comboText.section( ' ', 0, 0 );
	double tempDelay = 0.0;
	try{
		tempDelay = Utilities::getDouble(delayText.ascii());
	}
	catch (std::exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"NetworkViewerProperties: EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown converting string to double: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Render Delay Error", errorString);
		return;
	}
	networkViewer->setRenderDelay(tempDelay);
}


/*! Controls full render mode
	Enables the relevant GUI components and sets network viewer into full render mode. */
void NetworkViewerProperties::renderStateChanged(){
	if(fullRenderCheckBox->isChecked()){
		renderDelayLabel->setEnabled(true);
		renderDelayCombo->setEnabled(true);
		renderProgressLabel->setEnabled(true);
		progressBar->setEnabled(true);
		networkViewer->setFullRenderMode(true);
		renderCancelButton->setEnabled(true);
	}
	else{
		renderDelayLabel->setEnabled(false);
		renderDelayCombo->setEnabled(false);
		renderProgressLabel->setEnabled(false);
		progressBar->setEnabled(false);
		networkViewer->setFullRenderMode(false);
		renderCancelButton->setEnabled(false);
	}
}


/*! Called when combo box controlling which layer is used for single neuron mode is activated.
	Identical to previous method, but keep separate for the moment. */
void NetworkViewerProperties::secondNeuronComboChanged(int){
	if(connsSingleNeurRadioButt->isOn() && neurConnsNeurGrpCombo_1->count() > 0){
		unsigned int layer1ID = Utilities::getNeuronGrpID(neurConnsNeurGrpCombo_1->currentText());
		if(betweenMode){//In between mode
			unsigned int layer2ID = Utilities::getNeuronGrpID(neurConnsNeurGrpCombo_2->currentText());//Get the second neuron id
			networkViewer->setNeuronConnectionMode(true, layer1ID, true, layer2ID);
		}
		else{
			networkViewer->setNeuronConnectionMode(true, layer1ID, false, 0);
		}
	}
}


/*! Shows or hides the table displaying the list of connections for a single neuron
	Single neuron number should be meaningful by this point. */
void NetworkViewerProperties::showConnectionDetailsChanged(){
	if(connectionDetailsCheckBox->isChecked()){
		connectionDetailsTable->setEnabled(true);
		neurConnsLabel->setEnabled(true);
		neurTotalConnsLabel->setEnabled(true);
		loadConnectionDetails();
	}
	else{
		connectionDetailsTable->setEnabled(false);
		neurConnsLabel->setEnabled(false);
		neurTotalConnsLabel->setEnabled(false);
		connectionDetailsTable->setNumRows(0);
	}
}


/*! Called in response to a change in the radio buttons controlling whether all connections
	are shown or just connections to a single neuron. */
void NetworkViewerProperties::showConnectionsChanged(){
	if(allConnRadioButt->isOn()){
		networkViewer->setNeuronConnectionMode(false, 0, false, 0);
		connectionDetailsCheckBox->setEnabled(false);
		connectionDetailsTable->setEnabled(false);
		neurConnsLabel->setEnabled(false);
		neurConnsNeurGrpCombo_1->setEnabled(false);
		neurConnsNeurGrpCombo_2->setEnabled(false);
		neurConnsFilterCombo->setEnabled(false);
		neurConnsFromToCombo->setEnabled(false);
		neurConnsLabel->setEnabled(false);
		connectionDetailsCheckBox->setEnabled(false);
		connectionDetailsTable->setEnabled(false);
		neurTotalConnsLabel->setEnabled(false);
		andLabel->setEnabled(false);
		inLabel_1->setEnabled(false);
		inLabel_2->setEnabled(false);
		singleNeurNumLabel_1->setEnabled(false);
		singleNeurNumLabel_2->setEnabled(false);
	}
	else if(connsSingleNeurRadioButt->isOn() && neurConnsNeurGrpCombo_1->count() > 0){
		unsigned int layer1ID = Utilities::getNeuronGrpID(neurConnsNeurGrpCombo_1->currentText());
		if(betweenMode){
			unsigned int layer2ID = Utilities::getNeuronGrpID(neurConnsNeurGrpCombo_2->currentText());
			networkViewer->setNeuronConnectionMode(true, layer1ID, true, layer2ID);
			neurConnsNeurGrpCombo_2->setEnabled(true);
			andLabel->setEnabled(true);
			inLabel_2->setEnabled(true);
			singleNeurNumLabel_2->setEnabled(true);
		}
		else{
			networkViewer->setNeuronConnectionMode(true, layer1ID, false, 0);
		}
		connectionDetailsCheckBox->setEnabled(true);
		if(connectionDetailsCheckBox->isChecked()){
			neurConnsLabel->setEnabled(true);
			neurTotalConnsLabel->setEnabled(true);
			connectionDetailsTable->setEnabled(true);
		}
		neurConnsNeurGrpCombo_1->setEnabled(true);
		neurConnsFilterCombo->setEnabled(true);
		neurConnsFromToCombo->setEnabled(true);
		singleNeurNumLabel_1->setEnabled(true);
		inLabel_1->setEnabled(true);
	}
}


/*! Sorts rows in response to a click on one of the column headers. */
void NetworkViewerProperties::sortRows(int col){
	connectionDetailsTable->sortColumn(col, false, true);
}


//-------------------------------------------------------------------------------
//---------------------------- PRIVATE METHODS ----------------------------------
//-------------------------------------------------------------------------------

/*! Fills the table with details about a set of connecions to a neuron from the database. */
void NetworkViewerProperties::loadConnectionDetails(){
	//First clear the table
	connectionDetailsTable->setNumRows(0);
	
	//Load all of the connection information from database
	try{
		vector<unsigned int> *viewVector = networkViewer->getConnectionViewVector();
		Query query = dbInterface->getQuery();
		query.reset();
		
		if(!viewVector->empty()){//Only want to display connections within connection view vector
			//Add neuron information to query
			query<<"SELECT ConnGrpID, PreSynapticNeuronID, PostSynapticNeuronID, Weight, TempWeight, Delay FROM Connections WHERE ";
			
			//Control whether from or to connections are shown
			if(betweenMode){
				query<<"((PreSynapticNeuronID = "<<firstSingleNeuronNumber<<" AND PostSynapticNeuronID = "<<secondSingleNeuronNumber<<") ";
				query<<"OR (PreSynapticNeuronID = "<<secondSingleNeuronNumber<<" AND PostSynapticNeuronID = "<<firstSingleNeuronNumber<<"))";
			}
			else{
				if(showFromConns && showToConns)
					query<<"(PreSynapticNeuronID = "<<firstSingleNeuronNumber<<" OR PostSynapticNeuronID = "<<firstSingleNeuronNumber<<")";
			
				else if(showFromConns && !showToConns)
					query<<"PreSynapticNeuronID = "<<firstSingleNeuronNumber;
					
				else if (!showFromConns && showToConns)
					query<<"PostSynapticNeuronID = "<<firstSingleNeuronNumber;
				
				else{//Error, neither from nor to connections are showing
					cerr<<"NetworkViewerProperties: PROBLEM WITH FROM/TO FILTERING"<<endl;
					QMessageBox::critical( 0, "Connection Database Error", "Problem with from/to filtering");
					return;
				}
			}
			//Add list of displayed connection groups to query
			query<<" AND (ConnGrpID = ";
			for(unsigned int i=0; i< viewVector->size(); i++){
				query<<viewVector->at(i);
				if(i != viewVector->size() - 1)
					query<<" OR ConnGrpID = ";
			
			}
			query<<")";
	
		
			//Add restrictions on weights to query
			switch(neurConnsFilterCombo->currentItem()){
				case 0://No weight filtering, do nothing
					;
				break;
				case 1://Only show positive weights
					query<<" AND Weight >= 0 AND Weight <= 127";
				break;
				case 2://Only show negative weights
					query<<" AND Weight < 0 AND Weight >= -128";
				break;
				default:
					cerr<<"NetworkViewerProperties: CANNOT IDENTIFY FILTER COMBO INDEX - PROBABLY CHANGED AT SOME POINT WITHOUT UPDATING loadConnectionDetails()";
					QMessageBox::critical( 0, "Connection Database Error", "Cannot find filter combo index");
					return;
			}
                        StoreQueryResult result = query.store();
			connectionDetailsTable->insertRows(0, result.size());//Add rows to table
			int rowCount = 0;
                        for(StoreQueryResult::iterator iter = result.begin(); iter != result.end(); iter++){
				Row row(*iter);
                                connectionDetailsTable->setItem(rowCount, 0, new Q3TableItem(connectionDetailsTable, Q3TableItem::Never, ((string)row["ConnGrpID"]).data()));
                                connectionDetailsTable->setItem(rowCount, 1, new Q3TableItem(connectionDetailsTable, Q3TableItem::Never, ((string)row["PreSynapticNeuronID"]).data()));
                                connectionDetailsTable->setItem(rowCount, 2, new Q3TableItem(connectionDetailsTable, Q3TableItem::Never, ((string)row["PostSynapticNeuronID"]).data()));
                                connectionDetailsTable->setItem(rowCount, 3, new Q3TableItem(connectionDetailsTable, Q3TableItem::Never, ((string)row["Weight"]).data()));
                                connectionDetailsTable->setItem(rowCount, 4, new Q3TableItem(connectionDetailsTable, Q3TableItem::Never, ((string)row["TempWeight"]).data()));
                                connectionDetailsTable->setItem(rowCount, 5, new Q3TableItem(connectionDetailsTable, Q3TableItem::Never, ((string)row["Delay"]).data()));
				rowCount++;
			}
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"NetworkViewerProperties: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query loading connection information: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connection Database Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"NetworkViewerProperties: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading connection information: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connection Database Error", errorString);
	}
	
	//Set Label indicating total number of connections
	neurTotalConnsLabel->setText(QString::number(connectionDetailsTable->numRows()));
}


/*! Loads a list of layer names and IDs into a combo box 
	Adapted from method in connection properties dialog. */
void NetworkViewerProperties::loadLayerNames(QComboBox *comboBox){
	try{
		comboBox->clear();
		Query query = dbInterface->getQuery();
		query.reset();
		query<<"SELECT Name, NeuronGrpID FROM NeuronGroups";
                StoreQueryResult layerResult= query.store();
                for(StoreQueryResult::iterator resIter = layerResult.begin(); resIter < layerResult.end(); resIter++){
			Row row(*resIter);
                        string layerString((std::string)row["Name"] + " [" += (string)row["NeuronGrpID"] += "]");
                        comboBox->insertItem(layerString.data());
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"NetworkViewerProperties: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query loading layer names: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Load Layer Names Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"NetworkViewerProperties: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading layer names: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Load Layer Names Error", errorString);
	}
}



