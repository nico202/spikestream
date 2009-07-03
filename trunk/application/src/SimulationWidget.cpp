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
#include "SimulationWidget.h"
#include "Debug.h"
#include "Utilities.h"
#include "SimulationTypes.h"
#include "DeviceTypes.h"
#include "PatternTypes.h"
#include "GlobalVariables.h"
#include "SpikeStreamMainWindow.h"

//Qt includes
#include <qfile.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <q3groupbox.h>
#include <qvalidator.h>
#include <qregexp.h>
#include <qmessagebox.h>
#include <q3buttongroup.h>
#include <qtimer.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

//Other includes
#include <iostream>
using namespace std;
using namespace mysqlpp;


//Declare and initialise static variables
unsigned int SimulationWidget::simulationStartTime = 0;


/*! Constructor. */
SimulationWidget::SimulationWidget(QWidget *parent, DBInterface *netDBInter, DBInterface *archDBInter, DBInterface *pattDBInter, DBInterface *devDBInter) : QWidget(parent, "Simulation Widget"){
	
	//Store a reference to the database interfaces
	archiveDBInterface = archDBInter;
	networkDBInterface = netDBInter;
	patternDBInterface = pattDBInter;
	deviceDBInterface = devDBInter;

	//Set up a short version of spike stream application reference
	spikeStrApp = SpikeStreamMainWindow::spikeStreamApplication;

	//Set up variables
	ignoreInitialiseButton = false;
	ignoreRecordButton = false;

	//Create a validators
	QRegExp regExp( "([0-9]|[A-Z]|[a-z]|_|\\s){1,50}" );
	QValidator* archiveNameValidator = new QRegExpValidator(regExp, this);
	QValidator* paramIntValidator = new QIntValidator(0, 1000, this);

	//Set up pixmaps to control play and stop
	QPixmap playPixmap(SpikeStreamMainWindow::workingDirectory + "/images/play.xpm");
	QPixmap stepPixmap(SpikeStreamMainWindow::workingDirectory + "/images/step.xpm");
	QPixmap stopPixmap(SpikeStreamMainWindow::workingDirectory + "/images/stop.xpm");
	QPixmap recordPixmap(SpikeStreamMainWindow::workingDirectory + "/images/record.xpm");
	
	//Create a simulation manager to handle all the simulation work
	simulationManager = new SimulationManager(networkDBInterface, archiveDBInterface, patternDBInterface, deviceDBInterface, this);
	connect(simulationManager, SIGNAL(simulationStartTimeChanged(unsigned int)), this, SLOT(setSimulationStartTime(unsigned int)));
	
	//Pass references to simulation manager
	simulationManager->setNeuronMonitors(&neuronMonitorMap);
	simulationManager->setSynapseMonitors(&synapseMonitorMap);

	//Create dialogs to edit the simulation parameters.
	neuronParametersDialog = new NeuronParametersDialog(this, networkDBInterface, simulationManager);
	synapseParametersDialog = new SynapseParametersDialog(this, networkDBInterface, simulationManager);
	globalParametersDialog = new GlobalParametersDialog(this, networkDBInterface, simulationManager);
	noiseParametersDialog = new NoiseParametersDialog(this, networkDBInterface, simulationManager);

	//Create vertical layout to organise widget
	Q3VBoxLayout *mainVerticalBox = new Q3VBoxLayout(this, 5, 10, "vertical1");


	//=================== SIMULATION SETTINGS =====================
	//Create a group box for the simulation settings
        settingsGrpBox = new Q3GroupBox("Initialisation Settings", this);

	//Create a vertical box to organise layout in group box
        Q3VBoxLayout *settingsVerticalBox = new Q3VBoxLayout(settingsGrpBox, 5, 5, "vertical2");

	//Add box to take archive name
	Q3HBoxLayout *archiveNameBox = new Q3HBoxLayout();
	archiveNameBox->addWidget(new QLabel("Archive name", settingsGrpBox));
	archNameText = new QLineEdit("UNTITLED", settingsGrpBox);
	archNameText->setValidator(archiveNameValidator);
	archiveNameBox->addWidget(archNameText);
	archMonitorTypeCombo = new QComboBox(settingsGrpBox);
	archMonitorTypeCombo->insertItem("Archive firing neurons");
	archMonitorTypeCombo->insertItem("Archive spikes");
	archiveNameBox->addWidget(archMonitorTypeCombo);
	archiveNameBox->addStretch(5);
	settingsVerticalBox->addLayout(archiveNameBox);

	//Add a tabbed pane to let the user select between pattern and live input for the layers
	inputTabWidget = new QTabWidget(settingsGrpBox);

	//Set up the pattern input tab
	QWidget *patternWidgetTab = new QWidget(settingsGrpBox);
        Q3VBoxLayout *patternWidgetVerticalBox = new Q3VBoxLayout(patternWidgetTab, 5, 5, "vertical2");

	//Set up table to hold pattern information
	patternTable = new Q3Table(0, 7, patternWidgetTab);
	patternTable->setShowGrid(false);
	patternTable->setSorting(false);
	patternTable->setSelectionMode(Q3Table::NoSelection);
	patternTable->verticalHeader()->hide();
	patternTable->setLeftMargin(0);
	Q3Header * patternTableHeader = patternTable->horizontalHeader();
	patternIDCol = 0;//Remember to update this when changing the table
	patternTableHeader->setLabel(patternIDCol, "ID");
	patternTable->setColumnWidth(patternIDCol, 20);
	patternTableHeader->setLabel(1, "Description");
	patternTable->setColumnWidth(1, 130);
	patternTableHeader->setLabel(2, "Type");
	patternTable->setColumnWidth(2, 100);
	patternTableHeader->setLabel( 3, "Width" );
	patternTable->setColumnWidth( 3, 50);
	patternTableHeader->setLabel( 4, "Length" );
	patternTable->setColumnWidth( 4, 50);
	patternTableHeader->setLabel( 5, "Size" );
	patternTable->setColumnWidth( 5, 50);
	patternNeurGrpCol = 6;//Remember to update this when changing the table
	patternTableHeader->setLabel( patternNeurGrpCol, "Neuron Group" );
	patternTable->setColumnWidth( patternNeurGrpCol, 100);
	loadPatternTable();
	patternWidgetVerticalBox->addWidget(patternTable);

	//Set up option to choose the number of timesteps per pattern
	Q3HBoxLayout *timeStepsPatternBox = new Q3HBoxLayout();
	timeStepsPatternBox ->addWidget(new QLabel("Number of time steps per pattern", patternWidgetTab));
	timeStepsPatternText = new QLineEdit("1", patternWidgetTab);
	timeStepsPatternText->setValidator(paramIntValidator);
	timeStepsPatternBox->addWidget(timeStepsPatternText);
	timeStepsPatternBox->addStretch(5);
	patternWidgetVerticalBox->addLayout(timeStepsPatternBox);

	//Add the patternWidget tab to the input tab widget
	inputTabWidget->addTab(patternWidgetTab, "Pattern Input");

	//Set up tab to hold live input settings
	QWidget *liveWidgetTab = new QWidget(settingsGrpBox);
	Q3VBoxLayout *liveWidgetVerticalBox = new Q3VBoxLayout(liveWidgetTab, 5, 10);	

	//Add table to enable devices to be connected to neuron groups
	deviceTable = new Q3Table(0, 9, liveWidgetTab);
	deviceTable->setShowGrid(false);
	deviceTable->setSorting(false);
	deviceTable->setSelectionMode(Q3Table::NoSelection);
	deviceTable->verticalHeader()->hide();
	deviceTable->setLeftMargin(0);
	Q3Header * deviceTableHeader = deviceTable->horizontalHeader();
	deviceIDCol = 0;//Remember to update this when changing the table
	deviceTableHeader->setLabel(deviceIDCol, "ID");
	deviceTable->setColumnWidth(deviceIDCol, 20);
	deviceTableHeader->setLabel(1, "Description");
	deviceTable->setColumnWidth(1, 180);
	deviceTableHeader->setLabel(2, "Type");
	deviceTable->setColumnWidth(2, 150);
	deviceTableHeader->setLabel( 3, "IP Address" );
	deviceTable->setColumnWidth( 3, 80);
	deviceTableHeader->setLabel( 4, "Port" );
	deviceTable->setColumnWidth( 4, 40);
	deviceTableHeader->setLabel( 5, "Width" );
	deviceTable->setColumnWidth( 5, 50);
	deviceTableHeader->setLabel( 6, "Length" );
	deviceTable->setColumnWidth( 6, 50);
	deviceNeurGrpCol = 7;//Remember to update this when changing the table
	deviceTableHeader->setLabel( deviceNeurGrpCol, "Neuron Group" );
	deviceTable->setColumnWidth( deviceNeurGrpCol, 150);
	deviceFiringModeCol = 8;
	deviceTableHeader->setLabel( deviceFiringModeCol, "Firing Mode" );
	deviceTable->setColumnWidth( deviceFiringModeCol, 150);
	loadDeviceTable();
	liveWidgetVerticalBox->addWidget(deviceTable);

	inputTabWidget->addTab(liveWidgetTab, "Device Input/Output");
	settingsVerticalBox->addWidget(inputTabWidget);

	//Add settings box to layout. Need an extra box to make it fit nicely
        settingsGrpBox->setMinimumWidth(700);
	Q3HBoxLayout *settingsHBox = new Q3HBoxLayout();
	settingsHBox->addWidget(settingsGrpBox);
	settingsHBox->addStretch(5);
	mainVerticalBox->addLayout(settingsHBox);


	//====================   PARAMETERS   ==========================
	//Create a group box for the parameters
	Q3GroupBox *parameterGrpBox = new Q3GroupBox("Parameters", this);

        //Create a horizontal box to organise layout in group box
        Q3HBoxLayout *parameterHBox = new Q3HBoxLayout();

	//Add button to display neuron parameters
	QPushButton *neurParamButton = new QPushButton("Neuron Parameters", parameterGrpBox);
	connect (neurParamButton, SIGNAL(clicked()), this, SLOT(neuronParamButtonPressed()));
	parameterHBox->addWidget(neurParamButton);

	//Add button to display synapse parameters
	QPushButton *synapseParamButton = new QPushButton("Synapse Parameters", parameterGrpBox);
	connect (synapseParamButton, SIGNAL(clicked()), this, SLOT(synapseParamButtonPressed()));
	parameterHBox->addWidget(synapseParamButton);

	//Add button to display global parameters
	QPushButton *globalParamButton = new QPushButton("Global Parameters", parameterGrpBox);
	connect (globalParamButton, SIGNAL(clicked()), this, SLOT(globalParamButtonPressed()));
	parameterHBox->addWidget(globalParamButton);

	//Add button to display global parameters
	QPushButton *noiseParamButton = new QPushButton("Noise", parameterGrpBox);
	connect (noiseParamButton, SIGNAL(clicked()), this, SLOT(noiseParamButtonPressed()));
	parameterHBox->addWidget(noiseParamButton);
	parameterHBox->addSpacing(10);

        //Add parameter box containing buttons to layout of parameter box
	Q3VBoxLayout *parameterVBox = new Q3VBoxLayout(parameterGrpBox, 5, 10);
	parameterVBox->addLayout(parameterHBox);
        parameterVBox->addSpacing(10);

        parameterGrpBox->setMinimumWidth(700);
        Q3HBoxLayout *paramHolderHBox = new Q3HBoxLayout();
        paramHolderHBox->addWidget(parameterGrpBox);
        paramHolderHBox->addStretch(5);
        mainVerticalBox->addLayout(paramHolderHBox);


	//=================== SIMULATION CONTROLS =========================
	//Create group box for the simulation controls
	Q3GroupBox *controlGrpBox = new Q3GroupBox("Simulation Controls", this);
        controlGrpBox->setMinimumSize(700, 300);

	//Create a vertical box to organise layout in group box
	Q3VBoxLayout *controlVerticalBox = new Q3VBoxLayout(controlGrpBox, 5, 10, "vertical3");

	//Set up initialise section
	Q3HBoxLayout *initialiseBox = new Q3HBoxLayout();
	initialiseButton = new QPushButton("Initialise", controlGrpBox);
	initialiseButton->setToggleButton(true);
	connect (initialiseButton, SIGNAL(toggled(bool)), this, SLOT(initialiseButtonToggled(bool)));
	initialiseBox->addWidget(initialiseButton);

	reloadWeightsButton = new QPushButton("Reload weights", controlGrpBox);
	reloadWeightsButton->setEnabled(false);
	connect(reloadWeightsButton, SIGNAL(clicked()), this, SLOT(reloadWeightsButtonPressed()));
	initialiseBox->addWidget(reloadWeightsButton);

	saveWeightsButton = new QPushButton("Save weights", controlGrpBox);
	saveWeightsButton->setEnabled(false);
	connect(saveWeightsButton, SIGNAL(clicked()), this, SLOT(saveWeightsButtonPressed()));
	initialiseBox->addWidget(saveWeightsButton);

	viewWeightsButton = new QPushButton("View weights", controlGrpBox);
	viewWeightsButton->setEnabled(false);
	connect(viewWeightsButton, SIGNAL(clicked()), this, SLOT(viewWeightsButtonPressed()));
	initialiseBox->addWidget(viewWeightsButton);
	initialiseBox->addSpacing(10);

	controlVerticalBox->addLayout(initialiseBox);
	controlVerticalBox->addSpacing(5);

	//Set up transport buttons
	Q3HBoxLayout *simTransportButtonBox = new Q3HBoxLayout();
	simStartButton = new QPushButton(QIcon(playPixmap), "", controlGrpBox);
	simStartButton->setToggleButton(true);
	simStartButton->setEnabled(false);
	connect (simStartButton, SIGNAL(toggled(bool)), this, SLOT(simStartButtonToggled(bool)));
	simTransportButtonBox->addSpacing(10);
	simTransportButtonBox->addWidget(simStartButton);

	simStepButton = new QPushButton(QIcon(stepPixmap), "", controlGrpBox);
	simStepButton->setEnabled(false);
	connect (simStepButton, SIGNAL(clicked()), this, SLOT(simStepButtonPressed()));
	simTransportButtonBox->addWidget(simStepButton);

	recordButton = new QPushButton(QIcon(recordPixmap), "", controlGrpBox);
	recordButton->setEnabled(false);
	recordButton->setToggleButton(true);
	connect (recordButton, SIGNAL(toggled(bool)), this, SLOT(recordButtonToggled(bool)));
	simTransportButtonBox->addWidget(recordButton);

	simStopButton = new QPushButton(QIcon(stopPixmap), "", controlGrpBox);
	simStopButton->setEnabled(false);
	connect (simStopButton, SIGNAL(clicked()), this, SLOT(simStopButtonPressed()));
	simTransportButtonBox->addWidget(simStopButton);

	frameRateCombo = new QComboBox(controlGrpBox);
	frameRateCombo->setEnabled(false);
	frameRateCombo->insertItem("Max speed");
	frameRateCombo->insertItem("1 fps");	
	frameRateCombo->insertItem("5 fps");
	frameRateCombo->insertItem("10 fps");
	frameRateCombo->insertItem("15 fps");
	frameRateCombo->insertItem("20 fps");
	frameRateCombo->insertItem("25 fps");
	frameRateCombo->setCurrentItem(0);
	connect(frameRateCombo, SIGNAL(activated(int)), this, SLOT(frameRateComboChanged(int)));
	simTransportButtonBox->addWidget(frameRateCombo);

	simModeCombo = new QComboBox(controlGrpBox);
	simModeCombo->setEnabled(false);
	simModeCombo->insertItem("Event driven");
	simModeCombo->insertItem("Update all neurons");	
	simModeCombo->insertItem("Update all synapses");
	simModeCombo->insertItem("Update everything");
	simModeCombo->setCurrentItem(0);
	connect(simModeCombo, SIGNAL(activated(int)), this, SLOT(simModeComboChanged(int)));
	simTransportButtonBox->addWidget(simModeCombo);

	simTransportButtonBox->addSpacing(10);
	simTransportButtonBox->addStretch(5);

	controlVerticalBox->addLayout(simTransportButtonBox);
	controlVerticalBox->addSpacing(10);

	//Set up option to monitor the activity in a neuron group
	Q3HBoxLayout *monitorBox = new Q3HBoxLayout();
	neuronGrpMonitorCombo = new QComboBox(controlGrpBox);
	neuronGrpMonitorCombo->setEnabled(false);
	monitorBox->addSpacing(10);
	monitorBox->addWidget(neuronGrpMonitorCombo);
	liveMonitorTypeCombo = new QComboBox(controlGrpBox);
	liveMonitorTypeCombo->insertItem("Neurons");
	liveMonitorTypeCombo->insertItem("Spikes");
	liveMonitorTypeCombo->setEnabled(false);
	monitorBox->addWidget(liveMonitorTypeCombo);
	monitorButton = new QPushButton("Live Monitor", controlGrpBox);
	monitorButton->setEnabled(false);
	connect(monitorButton, SIGNAL(clicked()), this, SLOT(monitorButtonPressed()));
	monitorBox->addWidget(monitorButton);
	monitorBox->addStretch(5);
	controlVerticalBox->addLayout(monitorBox);

	//Controls to monitor the data of a single specified neuron
	Q3HBoxLayout *monitorNeuronBox = new Q3HBoxLayout();
	monitorNeuronCombo = new QComboBox(controlGrpBox);
	connect(monitorNeuronCombo, SIGNAL(activated(const QString &)), this, SLOT(monitorNeuronComboActivated(const QString &)));
	monitorNeuronCombo->setEnabled(false);
	monitorNeuronBox->addSpacing(10);
	monitorNeuronBox->addWidget(monitorNeuronCombo);
	monitorNeuronText = new QLineEdit(controlGrpBox);
	monitorNeuronValidator = new QIntValidator(this);
	monitorNeuronText->setValidator(monitorNeuronValidator);
	monitorNeuronText->setEnabled(false);
	monitorNeuronBox->addWidget(monitorNeuronText);
	monitorNeuronButton = new QPushButton("Monitor Neuron", controlGrpBox);
	connect(monitorNeuronButton, SIGNAL(clicked()), this, SLOT(monitorNeuronButtonPressed()));
	monitorNeuronButton->setEnabled(false);
	monitorNeuronBox->addWidget(monitorNeuronButton);
	monitorNeuronBox->addStretch(5);
	controlVerticalBox->addLayout(monitorNeuronBox);

	//Controls to monitor the data of a single specified synapse
	Q3HBoxLayout *monitorSynapseBox = new Q3HBoxLayout();
	monitorSynapseBox->addSpacing(10);
	monitorSynapseFromLabel = new QLabel("From: ", controlGrpBox);
	monitorSynapseFromLabel->setEnabled(false);
	monitorSynapseBox->addWidget(monitorSynapseFromLabel);
	monitorSynapseFromNumLabel = new QLabel("0", controlGrpBox);
	monitorSynapseFromNumLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	monitorSynapseFromNumLabel->setEnabled(false);
	monitorSynapseBox->addWidget(monitorSynapseFromNumLabel);
	monitorSynapseToLabel = new QLabel("To: ", controlGrpBox);
	monitorSynapseToLabel->setEnabled(false);
	monitorSynapseBox->addWidget(monitorSynapseToLabel);
	monitorSynapseToNumLabel = new QLabel("0", controlGrpBox);
	monitorSynapseToNumLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	monitorSynapseToNumLabel->setEnabled(false);
	monitorSynapseBox->addWidget(monitorSynapseToNumLabel);
	monitorSynapseButton = new QPushButton("Monitor Synapse", controlGrpBox);
        connect(monitorSynapseButton, SIGNAL(clicked()), this, SLOT(monitorSynapseButtonPressed()));
	monitorSynapseButton->setEnabled(false);
	monitorSynapseBox->addWidget(monitorSynapseButton);
	monitorSynapseBox->addStretch(5);
	controlVerticalBox->addLayout(monitorSynapseBox);
	controlVerticalBox->addSpacing(10);
	
	//Set up option to inject random noise into the neuron group
	Q3HBoxLayout *injectNoiseBox = new Q3HBoxLayout();

	//Combo to choose neuron group
	injectNoiseNeurGrpCombo = new QComboBox(controlGrpBox);
	injectNoiseNeurGrpCombo->setEnabled(false);
	injectNoiseBox->addSpacing(10);
	injectNoiseBox->addWidget(injectNoiseNeurGrpCombo);

	//Combo to choose amount of noise
	injectNoiseAmntCombo = new QComboBox(controlGrpBox);
	injectNoiseAmntCombo->insertItem("1 Neuron");
	injectNoiseAmntCombo->insertItem("5%");
	injectNoiseAmntCombo->insertItem("25%");
	injectNoiseAmntCombo->insertItem("50%");
	injectNoiseAmntCombo->insertItem("75%");
	injectNoiseAmntCombo->insertItem("100%");
	injectNoiseAmntCombo->setEnabled(false);
	injectNoiseBox->addWidget(injectNoiseAmntCombo);

	//Button to control noise
	injectNoiseButton = new QPushButton("Inject Noise", controlGrpBox);
	injectNoiseButton->setEnabled(false);
	connect(injectNoiseButton, SIGNAL(clicked()), this, SLOT(injectNoiseButtonPressed()));
	injectNoiseBox->addWidget(injectNoiseButton);
	injectNoiseBox->addStretch(5);
	
	controlVerticalBox->addLayout(injectNoiseBox);

	//Controls to fire a single specified neuron
	Q3HBoxLayout *fireNeuronBox = new Q3HBoxLayout();
	fireNeuronCombo = new QComboBox(controlGrpBox);
	connect(fireNeuronCombo, SIGNAL(activated(const QString &)), this, SLOT(fireNeuronComboActivated(const QString &)));
	fireNeuronCombo->setEnabled(false);
	fireNeuronBox->addSpacing(10);
	fireNeuronBox->addWidget(fireNeuronCombo);
	fireNeuronText = new QLineEdit(controlGrpBox);
	fireNeuronValidator = new QIntValidator(this);
	fireNeuronText->setValidator(fireNeuronValidator);
	fireNeuronText->setEnabled(false);
	fireNeuronBox->addWidget(fireNeuronText);
	fireNeuronButton = new QPushButton("Fire Neuron", controlGrpBox);
	connect(fireNeuronButton, SIGNAL(clicked()), this, SLOT(fireNeuronButtonPressed()));
	fireNeuronButton->setEnabled(false);
	fireNeuronBox->addWidget(fireNeuronButton);
	fireNeuronBox->addStretch(5);

	controlVerticalBox->addLayout(fireNeuronBox);
	controlVerticalBox->addSpacing(10);

	//Add controls box to layout. Need an extra box to make it fit nicely
	Q3HBoxLayout *conHBox = new Q3HBoxLayout();
	conHBox->addWidget(controlGrpBox);
	conHBox->addStretch(5);
	mainVerticalBox->addLayout(conHBox);
	mainVerticalBox->addSpacing(10);


	//========================== DOCKING AND GRAPH VISIBILITY CONTROLS ==============================
        Q3HBoxLayout *dockButtonBox = new Q3HBoxLayout();
	dockAllButton = new QPushButton("Dock All", this);
	dockAllButton->setEnabled(false);
	connect(dockAllButton, SIGNAL(clicked()), this, SLOT(dockAllButtonClicked()));
	dockButtonBox->addWidget(dockAllButton);
	undockAllButton = new QPushButton("Undock All", this);
	undockAllButton->setEnabled(false);
	connect(undockAllButton, SIGNAL(clicked()), this, SLOT(undockAllButtonClicked()));
	dockButtonBox->addWidget(undockAllButton);
	hideGraphsButton = new QPushButton("Hide Graphs", this);
	hideGraphsButton->setEnabled(false);
	connect (hideGraphsButton, SIGNAL(clicked()), this, SLOT(hideGraphsButtonClicked()));
	dockButtonBox->addWidget(hideGraphsButton);
	showGraphsButton = new QPushButton("Show Graphs", this);
	showGraphsButton->setEnabled(false);
	connect (showGraphsButton, SIGNAL(clicked()), this, SLOT(showGraphsButtonClicked()));
	dockButtonBox->addWidget(showGraphsButton);
	dockButtonBox->addStretch(5);


        mainVerticalBox->addLayout(dockButtonBox);

	simErrorCheckTimer = new QTimer(this);
	connect( simErrorCheckTimer, SIGNAL(timeout()), this, SLOT(checkSimManagerForErrors()));
}


/*! Destructor. */
SimulationWidget::~SimulationWidget(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING SIMULATION WIDGET"<<endl;
	#endif//MEMORY_DEBUG

	delete simulationManager;
}


//--------------------------------------------------------------------
//------------------------- PUBLIC METHODS ---------------------------
//--------------------------------------------------------------------

/*! Returns a reference to the simulation manager.
	Used so that the neuron application can delete classes in the right order. */
SimulationManager* SimulationWidget::getSimulationManager(){
	return simulationManager;
}


/*! Returns the simulation start time to prevent the deletion of live
	archives. */
unsigned int SimulationWidget::getSimulationStartTime(){
	return simulationStartTime;
}


/*! Hides the windows that are currently visible.
FIXME NEED TO KEEP A RECORD SO THAT THE SAME WINDOWS CAN BE SHOWN AGAIN. */
void SimulationWidget::hideOpenWindows(){
	monitorArea->dockMonitorWindows();
	hideGraphsButtonClicked();
	neuronParametersDialog->hide();
	synapseParametersDialog->hide();
	globalParametersDialog->hide();
	noiseParametersDialog->hide();
}


/*! Reloads the connection groups. */
void SimulationWidget::reloadConnectionGroups(){
	synapseParametersDialog->loadSynapseParameters();
}


/*! Reloads the devices. */
void SimulationWidget::reloadDevices(){
	loadDeviceTable();
}


/*! vCalled whenver the list of neuron groups changes. */
void SimulationWidget::reloadNeuronGroups(){
	loadPatternTable();
	loadDeviceTable();
	neuronParametersDialog->loadNeuronParameters();
}


/*! Called when changes are made to the patterns in the database. */
void SimulationWidget::reloadPatterns(){
	loadPatternTable();
}


/*! Sets the currently active neuron so that the user can choose to fire it 
	for debugging etc. */
void SimulationWidget::setFromNeuronID(unsigned int neurGrpID, unsigned int fromNeurID){
	//Work through fireNeuronCombo to find correct neuron group
	for(int i=0; i<fireNeuronCombo->count(); ++i){
		QString neurGrpString = "[" + QString::number(neurGrpID) + "]";
		if(fireNeuronCombo->text(i).contains(neurGrpString)){
			fireNeuronCombo->setCurrentItem(i);
			fireNeuronComboActivated(fireNeuronCombo->currentText());
			break;
		}
	}
	fireNeuronText->setText(QString::number(fromNeurID));

	//Work through monitorNeuronCombo to find correct neuron group
	//FIXME COULD GET RID OF THIS IF BOTH COMBOS ARE KEPT STRICTLY IN SYNC
	for(int i=0; i<monitorNeuronCombo->count(); ++i){
		QString neurGrpString = "[" + QString::number(neurGrpID) + "]";
		if(monitorNeuronCombo->text(i).contains(neurGrpString)){
			monitorNeuronCombo->setCurrentItem(i);
			monitorNeuronComboActivated(monitorNeuronCombo->currentText());
			break;
		}
	}
	monitorNeuronText->setText(QString::number(fromNeurID));
	monitorSynapseFromNumLabel->setText(QString::number(fromNeurID));
}


/*! Sets the reference to the monitor area that will be used to create monitor windows. */
void SimulationWidget::setMonitorArea(MonitorArea *monAr){
	monitorArea = monAr;
}


/*! Sets the active to neuron. */
void SimulationWidget::setToNeuronID(unsigned int toNeurID){
	monitorSynapseToNumLabel->setText(QString::number(toNeurID));
}


/*! Shows the windows that are currently visible.
FIXME NEED TO TRACK WHICH ARE CURRENTLY OPEN. */
void SimulationWidget::showOpenWindows(){

}


//------------------------------------------------------------------------
//--------------------------------- SLOTS --------------------------------
//------------------------------------------------------------------------

/*! Checks the simulation manager to see if there have been any errors in 
	the simulation. */
void SimulationWidget::checkSimManagerForErrors(){
	if(simulationManager->simulationError()){
		//Stop the simulation if it is running
		simStopButtonPressed();

		//Show the error if we have not shown it already
		QString* tempQStrErr = new QString(simulationManager->getSimulationErrorMsg());
		if(!(simulationErrorMap.count(tempQStrErr))){
			simulationErrorMap[tempQStrErr] = true;
			QMessageBox::critical (this, "Simulation Error", *tempQStrErr);
		}
		else{
			cout<<"SimulationWidget: Error filter working"<<endl;
		}

		//Clear the error
		simulationManager->clearSimulationError();
	}
}


/*! Checks with the simulation manager to see if the weights have been saved for viewing 
	or if there has been an error. */
void SimulationWidget::checkViewWeightsSaved(){
	if(simulationManager->getViewWeightsSaved()){
		SpikeStreamMainWindow::spikeStrMainWin->reloadConnectionDetails();
		delete busyDialog;
	}
	else if(simulationManager->simulationError()){
		delete busyDialog;
		QMessageBox::critical (this, "View Weight Save Error!", "An error occurred whilst saving synapse weights for viewing.");
	}
	else{
        QTimer *timer = new QTimer(this);
        connect( timer, SIGNAL(timeout()), this, SLOT(checkViewWeightsSaved()));
        timer->start( 500, TRUE ); // 1/2 second single-shot timer
	}
}


/*! Called by timer to check whether weights have been loaded or not. The timer is 
	restarted if they have not finished loading. */
void SimulationWidget::checkWeightsLoadState(){
	if(simulationManager->getWeightsLoaded()){
		delete busyDialog;
	}
	else if (simulationManager->simulationError()){
		delete busyDialog;
		QMessageBox::critical (this, "Weight Loading Error!", "An error occurred whilst reloading synapse weights.");
	}
	else{
        QTimer *timer = new QTimer(this);
        connect( timer, SIGNAL(timeout()), this, SLOT(checkWeightsLoadState()) );
        timer->start( 1000, TRUE ); // 1 second single-shot timer
	}
}


/*! Checks with the simulation manager to see if the weights have been saved or 
	if there has been an error. */
void SimulationWidget::checkWeightsSaveState(){
	if(simulationManager->getWeightsSaved()){
		SpikeStreamMainWindow::spikeStrMainWin->reloadConnectionDetails();
		delete busyDialog;
	}
	else if(simulationManager->simulationError()){
		delete busyDialog;
		QMessageBox::critical (this, "Weight Save Error!", "An error occurred whilst saving synapse weights.");
	}
	else{
        QTimer *timer = new QTimer(this);
        connect( timer, SIGNAL(timeout()), this, SLOT(checkWeightsSaveState()) );
        timer->start( 500, TRUE ); // 1/2 second single-shot timer
	}
}


/*! Instructs monitor area to dock all monitor windows. */
void SimulationWidget::dockAllButtonClicked(){
	monitorArea->dockMonitorWindows();
}


/*! Fires a single neuron. */
void SimulationWidget::fireNeuronButtonPressed(){
	if(fireNeuronCombo->count() <=0)
		return;

	unsigned int neuronID = 0, neuronGrpID = 0;
	try{
		neuronID = Utilities::getUInt(fireNeuronText->text().ascii());
		neuronGrpID = Utilities::getNeuronGrpID(fireNeuronCombo->currentText());
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"SimulationWidget: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown firing neuron: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Fire Neuron Error", errorString);
		return;
	}

	if(neuronID < minFireNeuronID || neuronID > maxFireNeuronID){
		QMessageBox::critical(this, "Fire Neuron Error", "The neuron ID is out of range for this neuron group");
		return;
	}
	simulationManager->fireNeuron(neuronGrpID, neuronID);
}


/*! Gets the first neuron id in the neuron group when the selection
	is changed in the fire neuron combo. */
void SimulationWidget::fireNeuronComboActivated(const QString &string){
	if(string == "")
		return;

	try{
		unsigned int neuronGrpID = Utilities::getNeuronGrpID(string);
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"SELECT MIN(NeuronID), MAX(NeuronID) FROM Neurons WHERE NeuronGrpID = "<<neuronGrpID;
                StoreQueryResult result = query.store();
		Row row(*result.begin());//Should only be 1 row
		minFireNeuronID = Utilities::getUInt((std::string)row["MIN(NeuronID)"]);
		maxFireNeuronID = Utilities::getUInt((std::string)row["MAX(NeuronID)"]);
		fireNeuronValidator->setRange(minFireNeuronID, maxFireNeuronID);
		fireNeuronText->setText(QString::number(minFireNeuronID));
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"SimulationWidget: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when finding neuron ID range: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Range Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"SimulationWidget: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown finding neuron ID range: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Range Error", errorString);
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"SimulationWidget: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown finding neuron ID range: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Range Error", errorString);
	}
}


/*! Gets the first neuron id in the neuron group when the selection
	is changed in the monitor neuron combo. */
void SimulationWidget::monitorNeuronComboActivated(const QString &string){
	if(string == "")
		return;

	try{
		unsigned int neuronGrpID = Utilities::getNeuronGrpID(string);
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"SELECT MIN(NeuronID), MAX(NeuronID) FROM Neurons WHERE NeuronGrpID = "<<neuronGrpID;
                StoreQueryResult result = query.store();
		Row row(*result.begin());//Should only be 1 row
		minMonitorNeuronID = Utilities::getUInt((std::string)row["MIN(NeuronID)"]);
		maxMonitorNeuronID = Utilities::getUInt((std::string)row["MAX(NeuronID)"]);
		monitorNeuronValidator->setRange(minMonitorNeuronID, maxMonitorNeuronID);
		monitorNeuronText->setText(QString::number(minMonitorNeuronID));
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"SimulationWidget: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when finding neuron ID range: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Range Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"SimulationWidget: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown finding neuron ID range: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Range Error", errorString);
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"SimulationWidget: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown finding neuron ID range: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Range Error", errorString);
	}
}


/*! Called when the frame rate is changed for live simulation. */
void SimulationWidget::frameRateComboChanged(int){
	if(frameRateCombo->currentItem() == 0){
		simulationManager->setFrameRate(-1);//Max frame rate
	}
	else{
		try{
			int frameRate = Utilities::getUInt(frameRateCombo->currentText().section(" ", 0, 0).ascii());
			simulationManager->setFrameRate(frameRate);
		}
		catch(std::exception& er){// Catch-all for any other exceptions
			cerr<<"SimulationWidget: STD EXCEPTION \""<<er.what()<<"\""<<endl;
			QString errorString = "Exception thrown converting frame rate from combo: \"";
			errorString += er.what();
			errorString += "\"";
			QMessageBox::critical( 0, "Frame Rate Error", errorString);
		}
	}
}


/*! Shows the global parameters dialog. */
void SimulationWidget::globalParamButtonPressed(){
	if(globalParametersDialog->loadParameters())
		globalParametersDialog->show();
}


/*! Hides the graphs plotting data from neurons or synapses. */
void SimulationWidget::hideGraphsButtonClicked(){
	//Work through all of the neuron monitors and hide them
	for(map<unsigned int, MonitorDataPlotter*>::iterator iter = neuronMonitorMap.begin(); iter != neuronMonitorMap.end(); ++iter){
		if(iter->second->isShown()){
			iter->second->closeDialog(true);
		}
	}

	//Work through all the synapse monitors and hide them
	for (map<unsigned int*, MonitorDataPlotter*, synapseKeyCompare>::iterator iter = synapseMonitorMap.begin(); iter != synapseMonitorMap.end(); ++iter){
		if(iter->second->isShown()){
			iter->second->closeDialog(true);
		}
	}
}


/*! Launches or destroys the simulation and enables or disables
	the relevant components. */
void SimulationWidget::initialiseButtonToggled(bool on){
	/* Used when we want to alter the button's state without
		doing anything. */
	if(ignoreInitialiseButton){
		ignoreInitialiseButton = false;
		return;
	}

	if(on){
		//Check for isolated neuron groups that are not connected to anything
		if(neuronGroupsIsolated()){
			int response = QMessageBox::warning(this, "Run Simulation", "Some of the neuron groups in your network model are not connected to another neuron group.\n These will not be updated and may prevent the simulation from running.\nDo you want to continue?",QMessageBox::Yes, QMessageBox::No, 0);
			cout<<"RESPONSE IS "<<response<<endl;
			if(response != QMessageBox::Yes){
				//Reset button
				ignoreInitialiseButton = true;
				initialiseButton->toggle();
				return;
			}
		}

		//Reset simulation error map
		simulationErrorMap.clear();

		//Check that archive name is sensible and it is the right length
		QString archiveName = archNameText->text();
		if(archiveName.length() == 0)
			archiveName = "Untitled";
		else if(archiveName.length() > MAX_DATABASE_NAME_LENGTH)
			archiveName.truncate(MAX_DATABASE_NAME_LENGTH);
		archNameText->setText(archiveName);

		//Sort out whether spikes or firing neurons are being archived
		map<const char*, unsigned int> parameterMap;
		if(archMonitorTypeCombo->currentText() == "Archive firing neurons")//Archive firing neurons
			parameterMap["ArchiveFiringNeurons"] = 1;
		else if(archMonitorTypeCombo->currentText() == "Archive spikes")//Archive spikes
			parameterMap["ArchiveSpikes"] = 1;
		else
			cerr<<"SimulationWidget. CANNOT IDENTIFY WHETHER ARCHIVING FIRING NEURONS OR SPIKES"<<endl;

		//Simulation can be a combination of patterns and live mode
		map<unsigned int, unsigned int> patternInputMap;//Key is the neuron group ID; data is the pattern group id
		map<unsigned int, unsigned int> deviceInOutMap;//Key is the neuron group ID; data is the device ID
		map<unsigned int, double> deviceFiringModeMap;//Key is the neuron group ID; data is the firing mode of the device

		//Fill pattern input map with links between neuron groups and pattern data
		try{
			for(int i=0; i<patternTable->numRows(); ++i){
				QString comboText = ((Q3ComboTableItem*)patternTable->item(i, patternNeurGrpCol))->currentText();
				if(comboText != "None"){
					unsigned int patternGrpID = Utilities::getUInt(patternTable->text(i, patternIDCol).ascii());//Get patternID
					unsigned int neuronGrpID = Utilities::getNeuronGrpID(comboText);
					if(patternInputMap.count(neuronGrpID) > 0){//Check neuron group has not been entered twice
						QMessageBox::critical (this, "Simulation Error", "Two patterns cannot be applied to a single neuron group", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
						//Reset button
						ignoreInitialiseButton = true;
						initialiseButton->toggle();
						return;
					}
					else
						patternInputMap[neuronGrpID] = patternGrpID;
				}
			}
			//Fill parameter map with relevant simulation parameters
			parameterMap["TimeStepsPerPattern"] = Utilities::getUInt(timeStepsPatternText->text().ascii());
	
			//Fill device in out map with links between neuron groups and devices
			for(int i=0; i<deviceTable->numRows(); ++i){
				QString comboText = ((Q3ComboTableItem*)deviceTable->item(i, deviceNeurGrpCol))->currentText();
				if(comboText != "None"){//Device has been set
					unsigned int deviceID = Utilities::getUInt(deviceTable->text(i, deviceIDCol).ascii());//Get device ID
                                        unsigned int neuronGrpID = Utilities::getNeuronGrpID(comboText);
					if(deviceInOutMap.count(neuronGrpID) > 0){//Check neuron group has not been entered twice
						QMessageBox::critical (this, "Simulation Error", "Two devices cannot be connected to a single neuron group", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
						//Reset button
						ignoreInitialiseButton = true;
						initialiseButton->toggle();
						return;
					}
					else{
						//Store link between neuron group and device
						deviceInOutMap[neuronGrpID] = deviceID;

						//Extract information about the firing mode of the device
						QString directFiringStr = deviceTable->item(i, deviceFiringModeCol)->text();
						if(directFiringStr == "Direct"){
							deviceFiringModeMap[neuronGrpID] = INPUT_DIRECT_FIRING_MODE;
						}
						else if(directFiringStr == "N/A"){
							deviceFiringModeMap[neuronGrpID] = OUTPUT_FIRING_MODE;
						}
						else{//Need to extract the synaptic weight
							QString synWeightStr = directFiringStr.section(" ", 2, 2);
							deviceFiringModeMap[neuronGrpID] = Utilities::getDouble(synWeightStr.ascii());
						}
					}
				}
			}
		}
		catch(std::exception& er){// Catch-all for any other exceptions
			cerr<<"SimulationWidget: STD EXCEPTION \""<<er.what()<<"\""<<endl;
			QString errorString = "Exception thrown initialising simulation: \"";
			errorString += er.what();
			errorString += "\"";
			QMessageBox::critical( 0, "Initialise Simulation Error", errorString);
			ignoreInitialiseButton = true;//Reset button
			initialiseButton->toggle();
			return;
		}

		//Run a quick check that user has not entered a neuron group in both tables
		for(map<unsigned int, unsigned int>::iterator iter = patternInputMap.begin(); iter != patternInputMap.end(); ++iter){
			if(deviceInOutMap.count(iter->first)){
				QMessageBox::critical (this, "Simulation Error", "A pattern and a device cannot be connected to a single neuron group", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
				//Reset button
				ignoreInitialiseButton = true;
				initialiseButton->toggle();
				return;
			}
		}

		/*Reset simulationStartTime. 
			This should be set by the simulation manager if everything launches successfully */
		simulationStartTime = 0;

		//Start the simulation
		StartSimRes startSimRes = simulationManager->initialiseSimulation(archiveName, patternInputMap, deviceInOutMap, deviceFiringModeMap, parameterMap);

		if(startSimRes.started) {//If the simulation initialises successfully
			/*Inform simulation manager that graphics loading is in progress
				This is to avoid visibility changes being sent to neuron simulation tasks whilst loading */
			simulationManager->setGraphicsLoading(true);

			//Set button text
			initialiseButton->setText("Destroy");

			//Load up dock windows ready for live monitoring of the simulation
			monitorArea->loadSimulation();

			//Pass references to the network monitors to the simulation manager
			monitorArea->connectToManager();
			
			// Enable/disable relevant components
			settingsGrpBox->setEnabled(false);
			simStartButton->setEnabled(true);
			simStepButton->setEnabled(true);
			simStopButton->setEnabled(true);
			recordButton->setEnabled(true);
			liveMonitorTypeCombo->setEnabled(true);
			monitorButton->setEnabled(true);
			dockAllButton->setEnabled(true);
			undockAllButton->setEnabled(true);
			reloadWeightsButton->setEnabled(true);
			saveWeightsButton->setEnabled(true);
			viewWeightsButton->setEnabled(true);
			frameRateCombo->setEnabled(true);
			simModeCombo->setEnabled(true);

			//Load up combos with appropriate names and initialise them
			loadNeuronGrpNames(neuronGrpMonitorCombo);
			neuronGrpMonitorCombo->insertItem("All layers");
			neuronGrpMonitorCombo->setCurrentItem(neuronGrpMonitorCombo->count() - 1);
			neuronGrpMonitorCombo->setEnabled(true);
			loadNeuronGrpNames(injectNoiseNeurGrpCombo);
			injectNoiseNeurGrpCombo->setEnabled(true);
			injectNoiseAmntCombo->setEnabled(true);
			injectNoiseButton->setEnabled(true);
			loadNeuronGrpNames(fireNeuronCombo);
			fireNeuronComboActivated(fireNeuronCombo->currentText());//Sets the ranges for the text field
			fireNeuronCombo->setEnabled(true);
			fireNeuronText->setEnabled(true);
			fireNeuronButton->setEnabled(true);
			loadNeuronGrpNames(monitorNeuronCombo);
			monitorNeuronComboActivated(monitorNeuronCombo->currentText());//Sets the ranges for the text field
			monitorNeuronCombo->setEnabled(true);
			monitorNeuronText->setEnabled(true);
			monitorNeuronButton->setEnabled(true);
			monitorSynapseFromLabel->setEnabled(true);
			monitorSynapseToLabel->setEnabled(true);
			monitorSynapseFromNumLabel->setEnabled(true);
			monitorSynapseToNumLabel->setEnabled(true);
			monitorSynapseButton->setEnabled(true);
			hideGraphsButton->setEnabled(true);
			showGraphsButton->setEnabled(true);

			/* Trigger event for frame rate combo. If this has been set during a previous
				simulation want to set the simulation to this previous value */
			frameRateComboChanged(0);

			/* Trigger event for simulatoin mode combo. If this has been set during a previous
				simulation run, want to set simulatin to this previous value. */
			simModeComboChanged(simModeCombo->currentItem());

			//Start timer to check for errors from the simulation manager
        	simErrorCheckTimer->start( 500, FALSE ); // 1/2 second repeating timer

			/* Inform neuron application that simulation is running to enable/ disable
				any other components */
			simulationManager->setGraphicsLoading(false);
			SpikeStreamMainWindow::spikeStrMainWin->simulationInitialised();
		}
		
		//Simulation failed to initialse
		else{
			//Inform user about error if simulation was not cancelled by user or if there was an error during clean up
			if((!startSimRes.canceled) || simulationManager->getCleanUpError()){
				QString initErrorMessage("Error occurred whilst initialising simulation:\n");
				initErrorMessage += simulationManager->getInitErrorMsg();
				initErrorMessage += "Would you like to run the CleanPVM script?";
				if(QMessageBox::critical (this, "Simulation Error", initErrorMessage, QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel) == QMessageBox::Yes){
					ScriptRunner scriptRunner (this, "CleanPVM", SpikeStreamMainWindow::workingDirectory);
					scriptRunner.exec();
				}
			}

			//Reset button
			ignoreInitialiseButton = true;
			initialiseButton->toggle();
			return;
		}
	}

	//Simulation is to be destroyed
	else{
		//Confirm that the user wants to end the simulation
		int warningConfirmation = QMessageBox::warning (this, "Destroy Simulation?", "Are you sure that you want to terminate the simulation?", QMessageBox::Ok,  QMessageBox::Cancel, QMessageBox::NoButton);
		if(warningConfirmation != QMessageBox::Ok){//1 is returned for cancel and closing message box
			ignoreInitialiseButton = true;
			initialiseButton->toggle();
			return;
		}

		//User has confirmed, so destroy simulation
		//Stop recording if it is recording
		if(recordButton->isOn()){
			simulationManager->stopRecording();
			ignoreRecordButton = true;
			recordButton->toggle();
		}

		//Destroy simulation
		initialiseButton->setText("Initialise");
		if(simulationManager->isInitialised()){
			//Stop the timer checking for errors - clean up errors will be picked up at the end
			simErrorCheckTimer->stop();	

			//Destroy the simulation
			simulationManager->destroySimulation();

			//Reset simulation start time
			simulationStartTime = 0;
			
			//Enable/disable relevant graphical componentes
			settingsGrpBox->setEnabled(true);
			simStartButton->setEnabled(false);
			if(simStartButton->isOn())
				simStartButton->toggle();
			simStepButton->setEnabled(false);
			simStopButton->setEnabled(false);
			recordButton->setEnabled(false);
			liveMonitorTypeCombo->setEnabled(false);
			monitorButton->setEnabled(false);
			neuronGrpMonitorCombo->setEnabled(false);
			injectNoiseNeurGrpCombo->setEnabled(false);
			injectNoiseAmntCombo->setEnabled(false);
			injectNoiseButton->setEnabled(false);
			fireNeuronCombo->setEnabled(false);
			fireNeuronText->setEnabled(false);
			fireNeuronButton->setEnabled(false);
			monitorNeuronCombo->setEnabled(false);
			monitorNeuronText->setEnabled(false);
			monitorNeuronButton->setEnabled(false);
			monitorArea->resetMonitorWindows();
			dockAllButton->setEnabled(false);
			undockAllButton->setEnabled(false);
			saveWeightsButton->setEnabled(false);
			reloadWeightsButton->setEnabled(false);
			viewWeightsButton->setEnabled(false);
			frameRateCombo->setEnabled(false);
			simModeCombo->setEnabled(false);
			monitorSynapseFromLabel->setEnabled(false);
			monitorSynapseToLabel->setEnabled(false);
			monitorSynapseFromNumLabel->setEnabled(false);
			monitorSynapseToNumLabel->setEnabled(false);
			monitorSynapseButton->setEnabled(false);
			hideGraphsButton->setEnabled(false);
			showGraphsButton->setEnabled(false);

			//Wait for simulation manager to stop
			bool threadFinished = false;
			int cycleCount = 0;
			while(cycleCount < 300 && !threadFinished){
				threadFinished = simulationManager->wait(100);
				++cycleCount;
				spikeStrApp->processEvents();
			}
			if(!threadFinished){//Problem shutting down simulation manager
				cerr<<"Simulation manager cannot be shut down cleanly within 30 seconds."<<endl;
				QMessageBox::critical(this, "Simulation Error", "Simulation manager cannot be shut down cleanly within 30 seconds.");
			}

			//Check for clean up error in simulation manager
			if(simulationManager->getCleanUpError()){
				QString cleanUpErrorMessage("Error occurred whilst cleaning up simulation:\n");
				cleanUpErrorMessage += simulationManager->getCleanUpErrorMsg();
				cleanUpErrorMessage += "Would you like to run the CleanPVM script?";
				if(QMessageBox::critical (this, "Simulation Error", cleanUpErrorMessage, QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel) == QMessageBox::Yes){
					ScriptRunner scriptRunner (this, "CleanPVM", SpikeStreamMainWindow::workingDirectory);
					scriptRunner.exec();
				}
			}

			//Delete any remaining neuron monitor widgets
			for(map<unsigned int, MonitorDataPlotter*>::iterator iter = neuronMonitorMap.begin(); iter != neuronMonitorMap.end(); ++iter){
				iter->second->closeDialog(false);
				delete iter->second;
			}
			neuronMonitorMap.clear();

			//Delete any remaining synapse monitor widgets
			for(map<unsigned int*, MonitorDataPlotter*, synapseKeyCompare>::iterator iter = synapseMonitorMap.begin(); iter != synapseMonitorMap.end(); ++iter){
				iter->second->closeDialog(false);//Close dialog window
				delete iter->second;//Delete monitor data plotter
				delete [] iter->first;//Delete key to map
			}
			synapseMonitorMap.clear();//Remove pointers from map

			/* Inform neuron application that simulation has been destroyed to 
				enable or disable any other components */
			SpikeStreamMainWindow::spikeStrMainWin->simulationDestroyed();
		}
	}
}


/*! Injects the specified amount of noise into a neuron group. */
void SimulationWidget::injectNoiseButtonPressed(){
	if(injectNoiseNeurGrpCombo->count() <=0)//Check for empty combo box
		return;

	QString name = injectNoiseNeurGrpCombo->currentText();
	unsigned int neuronGrpID = Utilities::getNeuronGrpID(name);
	int noiseAmount;
	switch(injectNoiseAmntCombo->currentItem()){
		case(0): noiseAmount = -1; break;//-1 means fire a single neuron
		case(1): noiseAmount = 5; break;
		case(2): noiseAmount = 25; break;
		case(3): noiseAmount = 50; break;
		case(4): noiseAmount = 75; break;
		case(5): noiseAmount = 100; break;
		default: cerr<<"Noise amount not recognised: "; return;	
	}
	simulationManager->injectNoise(neuronGrpID, noiseAmount);
}


/*! Make the associated network monitor visible in the dock area. */
void SimulationWidget::monitorButtonPressed(){
	//Get the name of the neuron group to be monitored
	QString name = neuronGrpMonitorCombo->currentText();

	//Find out what type of monitoring
	bool monitorNeurons = true;
	if(liveMonitorTypeCombo->currentText() == "Neurons")
		monitorNeurons = true;
	else if(liveMonitorTypeCombo->currentText() == "Spikes")
		monitorNeurons = false;
	else{
		cerr<<"SimulationWidget: CANNOT IDENTIFY MONITORING TYPE"<<endl;
		return;
	}

	//Set the monitoring appropriately
	if(name == "All layers"){
		monitorArea->showAllMonitorWindows(monitorNeurons);
	}
	else{
		unsigned int neuronGrpID = Utilities::getNeuronGrpID(name);	
		monitorArea->showMonitorWindow(neuronGrpID, monitorNeurons);
	}

	//Undock the monitoring windows
	monitorArea->undockMonitorWindows();
}


/*! Monitors a single neuron. */
void SimulationWidget::monitorNeuronButtonPressed(){
	//Return if no layers are present
	if(monitorNeuronCombo->count() <=0)
		return;

	//Get the details about the neuron group
	unsigned int neuronID = 0, neuronGrpID = 0;
	try{
		neuronID = Utilities::getUInt(monitorNeuronText->text().ascii());
		neuronGrpID = Utilities::getNeuronGrpID(monitorNeuronCombo->currentText());
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"SimulationWidget: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown getting neuron or neuron group ID: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Monitor Neuron Error", errorString);
		return;
	}

	if(neuronID < minMonitorNeuronID || neuronID > maxMonitorNeuronID){
		QMessageBox::critical(this, "Monitor Neuron Error", "The neuron ID is out of range for this neuron group");
		return;
	}

	//Are we already monitoring this neuron group?
	if(neuronMonitorMap.count(neuronID)){
		#ifdef MONITOR_NEURON_DATA_DEBUG
			cout<<"SimulationWidget: Neuron "<<neuronID<<" is already being monitored."<<endl;
		#endif//MONITOR_NEURON_DATA_DEBUG

		//Return if dialog is open and presumably active.
		if(neuronMonitorMap[neuronID]->isShown()){
			#ifdef MONITOR_NEURON_DATA_DEBUG
				cout<<"SimulationWidget: Neuron data graph is already being shown."<<endl;
			#endif//MONITOR_NEURON_DATA_DEBUG
			return;
		}

		//Neuron group is not being monitored
		else{
			#ifdef MONITOR_NEURON_DATA_DEBUG
				cout<<"SimulationWidget: Showing neuron data graph."<<endl;
			#endif//MONITOR_NEURON_DATA_DEBUG

			if(!neuronMonitorMap[neuronID]->showDialog()){
				QMessageBox::critical(this, "Monitor Neuron Error", "Error restarting neuron monitoring.");
				//Remove neuron monitor
				neuronMonitorMap[neuronID]->closeDialog(false);//Do not try to stop neuron monitoring - if we are here something is broken with this
				delete neuronMonitorMap[neuronID];
				neuronMonitorMap.erase(neuronID);
			}
			return;
		}
	}

	#ifdef MONITOR_NEURON_DATA_DEBUG
		cout<<"SimulationWidget: Constructing neuron data graph."<<endl;
	#endif//MONITOR_NEURON_DATA_DEBUG

	//Construct a window. When the data about the window is received, the window will be populated with the appropriate graphs
	neuronMonitorMap[neuronID] = new MonitorDataPlotter(this, monitorNeuronCombo->currentText(), neuronGrpID, neuronID);

	//Request meta data about which variables are being monitored and check for errors
	if(!simulationManager->startNeuronMonitoring(neuronGrpID, neuronID, false)){
		QMessageBox::critical(this, "Monitor Neuron Error", "Error requesting monitor neuron information for this neuron group.");
		
		//Remove neuron monitor and return
		neuronMonitorMap[neuronID]->closeDialog(false);//Do not try to stop monitoring because something is already broken.
		delete neuronMonitorMap[neuronID];
		neuronMonitorMap.erase(neuronID);
		return;
	}
}


/*! Called when the button has been pressed to start synapse monitoring. */
void SimulationWidget::monitorSynapseButtonPressed(){
	//Declare variables to use in the try-catch block.
	unsigned int fromNeuronID = 0, toNeuronID = 0, neuronGrpID = 0;
	unsigned int* synapseMonKey;

	try{
		//First check that the from and to neurons are present and there is a connection between them.
		fromNeuronID = Utilities::getUInt(monitorSynapseFromNumLabel->text().ascii());
		toNeuronID = Utilities::getUInt(monitorSynapseToNumLabel->text().ascii());
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"SELECT COUNT(*) FROM Connections WHERE PreSynapticNeuronID="<<fromNeuronID<<" AND PostSynapticNeuronID="<<toNeuronID;
                StoreQueryResult connRes = query.store();
		Row connRow(*connRes.begin());//Should only be 1 row
		unsigned int numberOfConns = Utilities::getUInt((std::string)connRow["COUNT(*)"]);
		if(numberOfConns != 1){
			QMessageBox::critical(this, "Monitor Synapse", "No synapse selected.\nUse the view tab to select a unique synapse for monitoring.\nCheck the direction of the connection if you are in between mode.");
			return;
		}
	
		//Next find the neuron group that the synapse is in
		query.reset();
		query<<"SELECT NeuronGrpID FROM Neurons WHERE NeuronID="<<toNeuronID;
                StoreQueryResult neurGrpRes = query.store();
		Row neurGrpRow(*neurGrpRes.begin());//Should only be 1 row
		neuronGrpID = Utilities::getUInt((std::string)neurGrpRow["NeuronGrpID"]);
	
		//Turn neuron ids into a key to the map
		synapseMonKey = new unsigned int[2];
		synapseMonKey[0] = fromNeuronID;
		synapseMonKey[1] = toNeuronID;
	
		//Are we already monitoring this neuron group?
		if(synapseMonitorMap.count(synapseMonKey)){
			cout<<"SimulationWidget: Synapse from "<<fromNeuronID<<" to "<<toNeuronID<<" is already being monitored."<<endl;
			//Return if dialog is open and presumably active.
			if(synapseMonitorMap[synapseMonKey]->isShown()){
				//Get rid of the key, which is identical to the one already in the map
				delete [] synapseMonKey;
				return;
			}
			else{
				if(!synapseMonitorMap[synapseMonKey]->showDialog()){
					QMessageBox::critical(this, "Monitor Synapse Error", "Error restarting synapse monitoring.");
	
					//Remove synapse monitor
					synapseMonitorMap[synapseMonKey]->closeDialog(false);//Hide the dialog and do not try to stop monitoring because something is already broken
					delete synapseMonitorMap[synapseMonKey];//Delete the MonitorDataPlotter
					synapseMonitorMap.erase(synapseMonKey);//Delete the address of the key in the map
				}
				//Get rid of the key, which is identical to the one already in the map
				delete [] synapseMonKey;
				return;
			}
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"SimulationWidget: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when starting monitoring of synapse: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "MonitorSynapse Error", errorString);
		return;
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"SimulationWidget: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown starting monitoring of synapse: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Monitor Synapse Error", errorString);
		return;
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"SimulationWidget: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown starting monitoring of synapse: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Monitor Synapse Error", errorString);
		return;
	}

	/* Construct a window. When the data about the window is received, the window will be 
		populated with the appropriate graphs. In this case leave synapseMonKey on the heap
		so that it can be found by the map for comparison. */
	synapseMonitorMap[synapseMonKey] = new MonitorDataPlotter(this, neuronGrpID, fromNeuronID, toNeuronID);

	//Request meta data about which variables are being monitored and check for errors
	if(!simulationManager->startSynapseMonitoring(neuronGrpID, fromNeuronID, toNeuronID, false)){
		QMessageBox::critical(this, "Monitor Synapse Error", "Error requesting monitor information for this synapse.");
		
		//Remove neuron monitor and return
		synapseMonitorMap[synapseMonKey]->closeDialog(false);//Hide the dialog. Do not try to stop monitoring because something is already broken
		delete synapseMonitorMap[synapseMonKey];//Delete the MonitorDataPlotter
		synapseMonitorMap.erase(synapseMonKey);//Delete the address of the key in the map
		delete [] synapseMonKey;//Delete the array of length two that is the key
		return;
	}
}


/*! Shows the neuron parameter dialog. */
void SimulationWidget::neuronParamButtonPressed(){
	if(neuronParametersDialog->loadNeuronParameters())
		neuronParametersDialog->show();
}


/*! Shows the noise parameter dialog. */
void SimulationWidget::noiseParamButtonPressed(){
	if(noiseParametersDialog->loadParameters())
		noiseParametersDialog->show();
}


/*! Starts or stops the recording of neuron data to the archive. */
void SimulationWidget::recordButtonToggled(bool on){
	//Ignore events generated by deliberately toggling button
	if(ignoreRecordButton){
		ignoreRecordButton = false;
		return;
	}

	if(on){//Start recording
		if(!simulationManager->startRecording()){
			ignoreRecordButton = true;//Recording has failed to start so ignore event generated by toggling button
			recordButton->toggle();
		}
	}
	else{//Stop recording
		if(!simulationManager->stopRecording()){
			ignoreRecordButton = true;//Recording has failed to stop so ignore event generated by toggling button
			recordButton->toggle();
		}
	}
}


/*! Reloads weights from the database into the neuron groups. This resets the weights
	in the simulation without restarting it. */
void SimulationWidget::reloadWeightsButtonPressed(){
	int warningConfirmation = QMessageBox::warning (this, "Reload weights", "Are you sure that you want to reload the synapse weights?\nThis will overwrite any weights that have been learnt.", QMessageBox::Ok,  QMessageBox::Cancel, QMessageBox::NoButton);
	if(warningConfirmation == QMessageBox::Ok){//1 is returned for cancel and closing message box
		/* Instruct simulation manager to send save weights message. 
			This should be almost instantaneous */
		if(!simulationManager->loadWeights())
			 QMessageBox::critical (this, "Weight Loading Error!", "An error occurred whilst reloading synapse weights.", QMessageBox::Ok,  QMessageBox::NoButton, QMessageBox::NoButton);

		// Show busy dialog box
		busyDialog = new BusyDialog(this, QString("Simulation"));
		busyDialog->showDialog("Loading weights, please wait");

		/* Start timer to check to see if weights have been loaded, if not refresh
			busy dialog and restart timer */
        QTimer *timer = new QTimer(this);
        connect( timer, SIGNAL(timeout()), this, SLOT(checkWeightsLoadState()) );
        timer->start( 1000, TRUE ); // 1 second single-shot timer
	}
}


/*! Instructs the tasks to save their weights to the database. */
void SimulationWidget::saveWeightsButtonPressed(){
	int warningConfirmation = QMessageBox::warning (this, "Save weights", "Are you sure that you want to save the current synapse weights?\nThis will overwrite all the weights in the database.", QMessageBox::Ok,  QMessageBox::Cancel, QMessageBox::NoButton);
	if(warningConfirmation == QMessageBox::Ok){//1 is returned for cancel and closing message box
		/* Instruct simulation manager to send save weights message. 
			This should be almost instantaneous */
		if(!simulationManager->saveWeights())
			 QMessageBox::critical (this, "Weight Saving Error!", "An error occurred whilst saving the synapse weights.", QMessageBox::Ok,  QMessageBox::NoButton, QMessageBox::NoButton);

		// Show busy dialog box
		busyDialog = new BusyDialog(this, QString("Simulation"));
		busyDialog->showDialog("Saving weights, please wait");

		/* Start timer to check to see if weights have been saved, if not refresh
			busy dialog and restart timer */
        QTimer *timer = new QTimer(this);
        connect( timer, SIGNAL(timeout()), this, SLOT(checkWeightsSaveState()) );
        timer->start( 500, TRUE ); // 1 second single-shot timer
	}
}


/*! Sets the simulation start time - used to prevent the deletion of
	active archives. */
void SimulationWidget::setSimulationStartTime(unsigned int simStartTime){
	SimulationWidget::simulationStartTime = simStartTime;
}


/*! Shows all of the graphs monitoring data. */
void SimulationWidget::showGraphsButtonClicked(){
	for(map<unsigned int, MonitorDataPlotter*>::iterator iter = neuronMonitorMap.begin(); iter != neuronMonitorMap.end(); ++iter){
		if(!iter->second->isShown()){
			if(!iter->second->showDialog()){
				QMessageBox::critical(this, "Monitor Error", "Error restarting monitoring.");
			}
		}
	}
	//Work through all the synapse monitors and show them
	for (map<unsigned int*, MonitorDataPlotter*, synapseKeyCompare>::iterator iter = synapseMonitorMap.begin(); iter != synapseMonitorMap.end(); ++iter){
		if(!iter->second->isShown()){
			if(!iter->second->showDialog()){
				QMessageBox::critical(this, "Monitor Error", "Error restarting monitoring.");
			}
		}
	}
}


/*! Called when the combo setting the simulation mode is changed.
	Instructs the simulation manager to set the simulation mode appropriately. */
void SimulationWidget::simModeComboChanged(int currentIndex){
	switch(currentIndex){
		case 0: simulationManager->setUpdateMode(false, false); break; //Event driven updates
		case 1: simulationManager->setUpdateMode(true, false); break;//Update all neurons at each time step
		case 2: simulationManager->setUpdateMode(false, true); break;//Update all synapses at each time step
		case 3: simulationManager->setUpdateMode(true, true); break;//Update all neurons and symapses at each time step
		default: cerr<<"SimulationWidget: SIMULATION MODE COMBO OPTION NOT RECOGNIZED: "<<currentIndex<<endl;
	}
}


/*! Starts or stops the simulation playing. */
void SimulationWidget::simStartButtonToggled(bool on){
	if(on){
		if(simulationManager->startSimulation()){//If the simulation start successfully
			cout<<"Simulation started"<<endl;
			reloadWeightsButton->setEnabled(false);
			saveWeightsButton->setEnabled(false);
			viewWeightsButton->setEnabled(false);
			initialiseButton->setEnabled(false);
		}
	}
	else{
		simStopButtonPressed();
	}
}


/*! Steps through the simulation one time step at a time.
	NOTE this method is pretty slow, probably because of messaging
	lags in PVM. */
void SimulationWidget::simStepButtonPressed(){
	if(simStartButton->isOn())
		simStartButton->toggle();
	simulationManager->stepSimulation();
}


/*! Stops the simulation without destroying it. */
void SimulationWidget::simStopButtonPressed(){
	if(simulationManager->isRunning()){
		if(simulationManager->stopSimulation()){
			initialiseButton->setEnabled(true);
			if(simStartButton->isOn())
				simStartButton->toggle();
			saveWeightsButton->setEnabled(true);
			viewWeightsButton->setEnabled(true);
			reloadWeightsButton->setEnabled(true);
		}
	}
}


/*! Shows the synapse parameters dialog. */
void SimulationWidget::synapseParamButtonPressed(){
	if(synapseParametersDialog->loadSynapseParameters())
		synapseParametersDialog->show();
}


/*! Undocks all monitor windows. */
void SimulationWidget::undockAllButtonClicked(){
	monitorArea->undockMonitorWindows();
}


/*! Instructs the tasks to save their weights to the database. */
void SimulationWidget::viewWeightsButtonPressed(){
/* Instruct simulation manager to send view weights message. 
	This should be almost instantaneous. */
	if(!simulationManager->saveViewWeights())
			QMessageBox::critical (this, "View Weight Saving Error!", "An error occurred whilst saving the synapse weights for viewing.", QMessageBox::Ok,  QMessageBox::NoButton, QMessageBox::NoButton);

	// Show busy dialog box
	busyDialog = new BusyDialog(this, QString("Simulation"));
	busyDialog->showDialog("Saving weights for viewing, please wait");

	/* Start timer to check to see if weights have been saved, if not refresh
		busy dialog and restart timer */
	QTimer *timer = new QTimer(this);
	connect( timer, SIGNAL(timeout()), this, SLOT(checkViewWeightsSaved()) );
	timer->start( 500, TRUE ); // 1/2 second single-shot timer
}


//----------------------------------------------------------------------
//----------------------- PRIVATE METHODS ------------------------------
//----------------------------------------------------------------------

/*! Loads a list of layer names and IDs into a qstringlist
	Adapted from method in connection properties dialog. */
void SimulationWidget::getNeuronGrpNames(QStringList &stringList, unsigned int patternWidth, unsigned int patternLength){
	try{
		stringList.clear();
		stringList += "None";
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"SELECT Name, NeuronGrpID, Width, Length FROM NeuronGroups";
                StoreQueryResult neurGrpResult= query.store();
                for(StoreQueryResult::iterator resIter = neurGrpResult.begin(); resIter < neurGrpResult.end(); resIter++){
			Row row(*resIter);
			/* Check to see if the width and length of the neuron group
				match that of the pattern */
			unsigned int neurGrpWidth = Utilities::getUInt((std::string)row["Width"]);
			unsigned int neurGrpLength = Utilities::getUInt((std::string)row["Length"]);
			if((neurGrpWidth == patternWidth && neurGrpLength == patternLength) || (neurGrpWidth == patternLength && neurGrpLength == patternWidth)){
                                string neurGrpString((std::string)row["Name"] + " [" += (std::string)row["NeuronGrpID"] += "]");
                                stringList += neurGrpString.data();
			}
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"SimulationWidget: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when getting neuron group names: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Group Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"SimulationWidget: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown getting neuron group names: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Group Error", errorString);
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"SimulationWidget: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown getting neuron group names: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Group Error", errorString);
	}
}


/*! Loads a table with all the available patterns. 
	Enables a pattern to be assigned to a suitable neuron group before the simulation starts. */
void SimulationWidget::loadDeviceTable(){
	//Empty device table
	deviceTable->setNumRows(0);
	
	//Load up a row for each pattern in the database
	try{
		Query deviceQuery = deviceDBInterface->getQuery();
		deviceQuery.reset();
		deviceQuery<<"SELECT DeviceID, Description, IPAddress, Port, Type, TotalNumColumns, TotalNumRows FROM Devices";
                StoreQueryResult result = deviceQuery.store();
                for(StoreQueryResult::iterator iter = result.begin(); iter != result.end(); ++iter){
			Row row(*iter);
		
			//Add new row to the table
			int currentRow = deviceTable->numRows();
			deviceTable->insertRows(currentRow, 1);
		
			//Populate row with pattern information
			deviceTable->setItem(currentRow, 0,
                                new Q3TableItem(deviceTable, Q3TableItem::Never, ((std::string)row["DeviceID"]).data()));//DeviceID
	
			deviceTable->setItem(currentRow, 1,
                                new Q3TableItem(deviceTable, Q3TableItem::Never, ((std::string)row["Description"]).data()));//Description
		
			unsigned int deviceType = Utilities::getUInt((std::string)row["Type"]);
			deviceTable->setItem(currentRow, 2,
				new Q3TableItem(deviceTable, Q3TableItem::Never, DeviceTypes::getDescription(deviceType)));//Type
		
			deviceTable->setItem(currentRow, 3,
                                new Q3TableItem(deviceTable, Q3TableItem::Never, ((std::string)row["IPAddress"]).data()));//IPAddress
	
			deviceTable->setItem(currentRow, 4,
                                new Q3TableItem(deviceTable, Q3TableItem::Never, ((std::string)row["Port"]).data()));//Port
	
			deviceTable->setItem(currentRow, 5,
                                new Q3TableItem(deviceTable, Q3TableItem::Never, ((std::string)row["TotalNumColumns"]).data()));//Width
	
			deviceTable->setItem(currentRow, 6,
                                new Q3TableItem(deviceTable, Q3TableItem::Never, ((std::string)row["TotalNumRows"]).data()));//Length
		
			/*Get a list of neuron group names that have the correct width and length
				for this device and add them to a combo box in the table */
			QStringList neuronNamesList;
			unsigned int deviceWidth = Utilities::getUInt((std::string)row["TotalNumColumns"]);
			unsigned int deviceLength = Utilities::getUInt((std::string)row["TotalNumRows"]);
			getNeuronGrpNames(neuronNamesList, deviceWidth, deviceLength);
			deviceTable->setItem(currentRow, 7, new Q3ComboTableItem(deviceTable, neuronNamesList));

			//If the device is supplying input to the network, add combo with choices of type of input
			if(DeviceTypes::isInputDevice(deviceType)){
				QStringList firingModeStrList;
				firingModeStrList += "Direct";
				for(double i=-1.0; i<=1.0; i += 0.2){
					if(rint(i * 10) == 0.0)//QString::number gives a strange very small value for zero
						firingModeStrList += "Synaptic weight 0";
					else
						firingModeStrList += "Synaptic weight " + QString::number(i);
				}
				Q3ComboTableItem* tempFiringModeCombo = new Q3ComboTableItem(deviceTable, firingModeStrList);
				deviceTable->setItem(currentRow, deviceFiringModeCol, tempFiringModeCombo);
			}
			//Otherwise just add a combo with n/a
			else{
				QStringList firingModeStrList;
				firingModeStrList += "N/A";
				Q3ComboTableItem* tempFiringModeCombo = new Q3ComboTableItem(deviceTable, firingModeStrList);
				deviceTable->setItem(currentRow, deviceFiringModeCol, tempFiringModeCombo);
			}
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"SimulationWidget: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when filling device table: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Device Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"SimulationWidget: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown filling device table: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Device Error", errorString);
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"SimulationWidget: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown filling device table: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Device Error", errorString);
	}
}


/*! Loads a list of layer names and IDs into a combo box 
	Adapted from method in connection properties dialog. */
void SimulationWidget::loadNeuronGrpNames(QComboBox *comboBox){
	comboBox->clear();
	try{
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"SELECT Name, NeuronGrpID FROM NeuronGroups";
                StoreQueryResult neurGrpResult= query.store();
                for(StoreQueryResult::iterator resIter = neurGrpResult.begin(); resIter < neurGrpResult.end(); resIter++){
			Row row(*resIter);
                        string neurGrpString((std::string)row["Name"] + " [" += (std::string)row["NeuronGrpID"] += "]");
                        comboBox->insertItem(neurGrpString.data());
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"SimulationWidget: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when loading neuron group names: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Group Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"SimulationWidget: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown loading neuron group names: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Group Error", errorString);
	}
}


/*! Loads a table with all the available patterns. 
	Enables a pattern to be assigned to a suitable neuron group before the simulation starts. */
void SimulationWidget::loadPatternTable(){
	//Empty pattern table
	patternTable->setNumRows(0);
	
	//Load up a row for each pattern in the database
	try{
		Query patternQuery = patternDBInterface->getQuery();
		patternQuery.reset();
		patternQuery<<"SELECT PatternGrpID, Description, PatternType, Width, Length, NumberOfPatterns FROM PatternDescriptions";
                StoreQueryResult result = patternQuery.store();
                for(StoreQueryResult::iterator iter = result.begin(); iter != result.end(); ++iter){
			Row row(*iter);
			//Add new row to the table
			int currentRow = patternTable->numRows();
			patternTable->insertRows(currentRow, 1);
		
			//Populate row with pattern information
			patternTable->setItem(currentRow, 0,
                                new Q3TableItem(patternTable, Q3TableItem::Never, ((std::string)row["PatternGrpID"]).data()));//PatternGrpID
	
			patternTable->setItem(currentRow, 1,
                                new Q3TableItem(patternTable, Q3TableItem::Never, ((std::string)row["Description"]).data()));//Description
		
			unsigned int patternType = Utilities::getUInt((std::string)row["PatternType"]);
			patternTable->setItem(currentRow, 2,
				new Q3TableItem(patternTable, Q3TableItem::Never, PatternTypes::getDescription(patternType)));//Pattern Type
		
			patternTable->setItem(currentRow, 3,
                                new Q3TableItem(patternTable, Q3TableItem::Never, ((std::string)row["Width"]).data()));//Width
		
			patternTable->setItem(currentRow, 4,
                                new Q3TableItem(patternTable, Q3TableItem::Never, ((std::string)row["Length"]).data()));//Length
		
			patternTable->setItem(currentRow, 5,
                                new Q3TableItem(patternTable, Q3TableItem::Never, ((std::string)row["NumberOfPatterns"]).data()));//Width
		
			//Get a list of neuron group names
			QStringList neuronNamesList;
			unsigned int patternWidth = Utilities::getUInt((std::string)row["Width"]);
			unsigned int patternLength = Utilities::getUInt((std::string)row["Length"]);
			getNeuronGrpNames(neuronNamesList, patternWidth, patternLength);
			patternTable->setItem(currentRow, 6, new Q3ComboTableItem(patternTable, neuronNamesList));
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"SimulationWidget: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when filling pattern table: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Pattern Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"SimulationWidget: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown filling pattern table: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Pattern Error", errorString);
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"SimulationWidget: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown filling pattern table: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Pattern Error", errorString);
	}
}


/*! Checks to see if any of the neuron groups are lacking connections with another
	neuron group. These will never receive spike messages and so will not be updated.
	Returns true if there are isolated neuron groups or if there has been an error. */
bool SimulationWidget::neuronGroupsIsolated(){
	try{
		Query query = networkDBInterface->getQuery();
		query.reset();
	
		//Get a complete list of neuron groups
		query<<"SELECT NeuronGrpID FROM NeuronGroups";
                StoreQueryResult neurGrpRes = query.store();
		
		//Work through the neuron groups and check that they are all in the ConnectionGroup database
                for(StoreQueryResult::iterator iter = neurGrpRes.begin(); iter != neurGrpRes.end(); ++iter){
			Row neurGrpRow (*iter);
			query.reset();
			query<<"SELECT ConnGrpID FROM ConnectionGroups WHERE FromNeuronGrpID = "<<neurGrpRow["NeuronGrpID"]<<" OR ToNeuronGrpID = "<<neurGrpRow["NeuronGrpID"];
                        StoreQueryResult connGrpRes = query.store();
			if(connGrpRes.size() == 0)
				return true;
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"SimulationWidget: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when checking for isolated neuron groups: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Group Error", errorString);
		return true;
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"SimulationWidget: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown checking for isolated neuron groups: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Group Error", errorString);
		return true;
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"SimulationWidget: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown checking for isolated neuron groups: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Group Error", errorString);
		return true;
	}
	return false;
}




