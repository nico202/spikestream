//SpikeStream includes
#include "ConnectionsModel.h"
#include "ConnectionsTableView.h"
#include "Globals.h"
#include "GlobalVariables.h"
#include "NetworkDisplay.h"
#include "EventRouter.h"
#include "NetworkViewerProperties.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QButtonGroup>
#include <QLayout>


/*! Constructor */
NetworkViewerProperties::NetworkViewerProperties(QWidget* parent) : QWidget(parent){
	//Main vertical layout
	QVBoxLayout* mainVerticalBox = new QVBoxLayout(this);

	//Button group to set connection mode
	QButtonGroup* conButGroup = new QButtonGroup();

	//All connection widgets
	allConsButt = new QRadioButton("All selected connections");
	conButGroup->addButton(allConsButt);
	mainVerticalBox->addWidget(allConsButt);

	//Single neuron widgets
	conSingleNeurButt = new QRadioButton("Connections to neuron");
	conButGroup->addButton(conSingleNeurButt);
	singleNeuronIDLabel = new QLabel("");
	QHBoxLayout* singleNeuronBox = new QHBoxLayout();
	singleNeuronBox->addWidget(conSingleNeurButt);
	singleNeuronBox->addWidget(singleNeuronIDLabel);
	singleNeuronBox->addStretch(5);
	mainVerticalBox->addLayout(singleNeuronBox);

	fromToCombo = new QComboBox();
	fromToCombo->addItem("From and To");
	fromToCombo->addItem("From");
	fromToCombo->addItem("To");
	connect(fromToCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(fromToSelectionChanged(int)));
	QHBoxLayout* fromToSingleBox = new QHBoxLayout();
	fromToSingleBox->addSpacing(20);
	fromToSingleBox->addWidget(fromToCombo);
	truthTableButton = new QPushButton("Truth Table");
	truthTableButton->setVisible(false);
	connect(truthTableButton, SIGNAL(clicked()), this, SLOT(showTruthTable()));
	fromToSingleBox->addWidget(truthTableButton);
	fromToSingleBox->addStretch(5);
	mainVerticalBox->addLayout(fromToSingleBox);

	//Between neuron widgets
	conBetweenNeurButt = new QRadioButton("Connection ");
	conButGroup->addButton(conBetweenNeurButt);
	QHBoxLayout* betweenBox = new QHBoxLayout();
	betweenBox->addWidget(conBetweenNeurButt);
	fromLabel = new QLabel("From: ");
	betweenBox->addWidget(fromLabel);
	fromNeuronIDLabel = new QLabel("");
	betweenBox->addWidget(fromNeuronIDLabel);
	toLabel = new QLabel("to: ");
	betweenBox->addWidget(toLabel);
	toNeuronIDLabel = new QLabel("");
	betweenBox->addWidget(toNeuronIDLabel);
	betweenBox->addStretch(5);
	mainVerticalBox->addLayout(betweenBox);

	//Positive and negative filtering
	QHBoxLayout* posNegBox = new QHBoxLayout();
	posNegBox->addWidget(new QLabel("Filter by connection weight: "));
	posNegCombo = new QComboBox();
	posNegCombo->addItem("All connections");
	posNegCombo->addItem("Positive connections");
	posNegCombo->addItem("Negative connections");
	connect(posNegCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(posNegSelectionChanged(int)));
	posNegBox->addWidget(posNegCombo);
	posNegBox->addStretch(5);
	mainVerticalBox->addLayout(posNegBox);

	//Add table view and model
	QAbstractTableModel* connectionsModel = new ConnectionsModel();
	QTableView* connectionsView = new ConnectionsTableView(this, connectionsModel);
	connectionsView->setMinimumWidth(500);
	connectionsView->setMinimumHeight(500);
	mainVerticalBox->addWidget(connectionsView);
	mainVerticalBox->addStretch(5);

	//Listen for changes in the network display
	connect(Globals::getEventRouter(), SIGNAL(networkDisplayChangedSignal()), this, SLOT(networkDisplayChanged()));

	/* Initialize truth table dialog to NULL
		Otherwise it appears as an annoying flash up during boot up of SpikeStream */
	truthTableDialog = NULL;

	//Initial state is to show all connections
	showAllConnections();
}


/*! Destructor */
NetworkViewerProperties::~NetworkViewerProperties(){
}


/*----------------------------------------------------------*/
/*-----                PRIVATE SLOTS                   -----*/
/*----------------------------------------------------------*/

/*! User has selected or deselected a from neuron. Settings are adjusted to take account of this. */
void NetworkViewerProperties::fromToSelectionChanged(int index){
	if(index == 0){
		Globals::getNetworkDisplay()->clearDirectionFiltering();
	}
	else if (index == 1){
		Globals::getNetworkDisplay()->showFromConnections();
	}
	else if (index == 2){
		Globals::getNetworkDisplay()->showToConnections();
	}
}


/*! The network display settings have changed. */
void NetworkViewerProperties::networkDisplayChanged(){
	unsigned int connectionMode = Globals::getNetworkDisplay()->getConnectionMode();
	if(connectionMode & CONNECTION_MODE_ENABLED){
		if(connectionMode & SHOW_BETWEEN_CONNECTIONS)
			showBetweenConnections();
		else
			showSingleConnections();
	}
	else{
		showAllConnections();
	}
}


/*! Filtering for showing/hiding positive/negative connections has been changed. */
void NetworkViewerProperties::posNegSelectionChanged(int index){
	if(index == 0){
		Globals::getNetworkDisplay()->clearWeightFiltering();
	}
	else if (index == 1){
		Globals::getNetworkDisplay()->showPositiveConnections();
	}
	else if (index == 2){
		Globals::getNetworkDisplay()->showNegativeConnections();
	}
}


/*! Shows a dialog with the truth table for the selected neuron */
void NetworkViewerProperties::showTruthTable(){
	//Get neuron id.
	unsigned int tmpNeurID = Globals::getNetworkDisplay()->getSingleNeuronID();
	if(tmpNeurID == 0)
		throw SpikeStreamException("Truth table cannot be displayed for an invalid neuron ID");

	//Show non modal dialog
	showTruthTableDialog(tmpNeurID);
}


/*----------------------------------------------------------*/
/*-----               PRIVATE METHODS                  -----*/
/*----------------------------------------------------------*/

/*! Sets widget state to match situation in which all connections are visible. */
void NetworkViewerProperties::showAllConnections(){
	allConsButt->setChecked(true);
	allConsButt->setEnabled(true);
	conBetweenNeurButt->setEnabled(false);
	conSingleNeurButt->setEnabled(false);
	singleNeuronIDLabel->setEnabled(false);
	singleNeuronIDLabel->setText("");
	fromToCombo->setEnabled(false);
	fromNeuronIDLabel->setEnabled(false);
	fromNeuronIDLabel->setText("");
	toNeuronIDLabel->setEnabled(false);
	toNeuronIDLabel->setText("");
	fromLabel->setEnabled(false);
	toLabel->setEnabled(false);
	truthTableButton->setVisible(false);
	hideTruthTableDialog();
}


/*! Sets widget state to match situation in which the connections between two neurons are visible. */
void NetworkViewerProperties::showBetweenConnections(){
	conBetweenNeurButt->setChecked(true);
	conBetweenNeurButt->setEnabled(true);
	fromNeuronIDLabel->setEnabled(true);
	fromNeuronIDLabel->setText(QString::number(Globals::getNetworkDisplay()->getSingleNeuronID()));
	toNeuronIDLabel->setEnabled(true);
	toNeuronIDLabel->setText(QString::number(Globals::getNetworkDisplay()->getToNeuronID()));
	fromToCombo->setEnabled(false);
	allConsButt->setEnabled(false);
	conSingleNeurButt->setEnabled(false);
	singleNeuronIDLabel->setEnabled(false);
	singleNeuronIDLabel->setText("");
	fromLabel->setEnabled(true);
	toLabel->setEnabled(true);
	truthTableButton->setVisible(false);
	hideTruthTableDialog();
}


/*! Sets widget state to match situation in which the connections to/from a single neuron are shown. */
void NetworkViewerProperties::showSingleConnections(){
	//Id of the single neuron
	unsigned int singleNeuronID = Globals::getNetworkDisplay()->getSingleNeuronID();

	//Set up graphical components appropriately
	conSingleNeurButt->setChecked(true);
	conSingleNeurButt->setEnabled(true);
	singleNeuronIDLabel->setEnabled(true);
	singleNeuronIDLabel->setText(QString::number(singleNeuronID));
	fromToCombo->setEnabled(true);
	fromNeuronIDLabel->setEnabled(false);
	fromNeuronIDLabel->setText("");
	toNeuronIDLabel->setEnabled(false);
	toNeuronIDLabel->setText("");
	allConsButt->setEnabled(false);
	conBetweenNeurButt->setEnabled(false);
	fromLabel->setEnabled(false);
	toLabel->setEnabled(false);

	//Show button to launch truth table dialog if to neuron connections are shown and it is a weightless neuron
	if(Globals::getNetworkDao()->isWeightlessNeuron(singleNeuronID)){
		//Update neuron in truth table dialog if it is visible
		if(truthTableDialog != NULL && truthTableDialog->isVisible())
			showTruthTableDialog(singleNeuronID);

		//Show button to display dialog if it is not already visible
		if(fromToCombo->currentText() == "To"){
			truthTableButton->setVisible(true);
		}
		else{
			truthTableButton->setVisible(false);
			hideTruthTableDialog();
		}
	}
	else{
		truthTableButton->setVisible(false);
		hideTruthTableDialog();
	}
}


/*! Shows the dialog with the specified neuron's truth table */
void NetworkViewerProperties::showTruthTableDialog(unsigned int neuronID){
	if(truthTableDialog == NULL)
		truthTableDialog = new TruthTableDialog();
	truthTableDialog->show(neuronID);
}


/*! Hides the dialog with the neuron's truth table */
void NetworkViewerProperties::hideTruthTableDialog(){
	if(truthTableDialog != NULL)
		truthTableDialog->hide();
}

