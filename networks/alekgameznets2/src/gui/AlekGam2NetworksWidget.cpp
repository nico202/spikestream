//SpikeStream includes
#include "AlekGam2NetworksWidget.h"
#include "FullyConnectedNetworksBuilder.h"
#include "PartitionedNetworksBuilder.h"
#include "ModularNetworksBuilder.h"
#include "SensoryNetworksBuilder.h"
#include "Globals.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QLabel>


//Functions for dynamic library loading
extern "C" {
	/*! Creates a AlekGam2NetworksWidget class when library is dynamically loaded. */
	QWidget* getClass(){
		return new AlekGam2NetworksWidget();
	}

	/*! Returns a descriptive name for this widget */
	QString getName(){
		return QString("Aleksander/Gamez Test Networks 2");
	}
}


/*! Constructor */
AlekGam2NetworksWidget::AlekGam2NetworksWidget(){

	//Layout using grid layout
	QGridLayout* gridLayout = new QGridLayout(this);
	gridLayout->setMargin(10);

	//Field to enable user to enter network name
	gridLayout->addWidget(new QLabel("Network name: "), 0, 0);
	networkName = new QLineEdit("Unnamed");
	networkName->setMinimumSize(250, 30);
	gridLayout->addWidget(networkName, 0, 1);

	//Add buttons for each network
	QPushButton* newButton = addNetworkButton(gridLayout, "Set of 4 fully connected 12 neuron networks with different training");
	connect (newButton, SIGNAL(clicked()), this, SLOT(addNetwork()));

	newButton = addNetworkButton(gridLayout, "Set of 4 partitioned 12 neuron networks with different training");
	connect (newButton, SIGNAL(clicked()), this, SLOT(addNetwork()));

	newButton = addNetworkButton(gridLayout, "Set of 4 modular 12 neuron networks with different training");
	connect (newButton, SIGNAL(clicked()), this, SLOT(addNetwork()));

	newButton = addNetworkButton(gridLayout, "Set of 4 sensory 12 neuron networks with different training");
	connect (newButton, SIGNAL(clicked()), this, SLOT(addNetwork()));
}


/*! Destructor */
AlekGam2NetworksWidget::~AlekGam2NetworksWidget(){
}


/*! Adds a network to the database. Sender's object name is used to determine type of
	network to add. */
void AlekGam2NetworksWidget::addNetwork(){
	QString netDesc = sender()->objectName();
	try{
		if(netDesc == "Set of 4 fully connected 12 neuron networks with different training"){
			FullyConnectedNetworksBuilder* fullConBuilder = new FullyConnectedNetworksBuilder();
			networksBuilder = fullConBuilder;
			progressDialog = new QProgressDialog("Building fully connected network", "Cancel", 0, 100, this);
			progressDialog->setWindowModality(Qt::WindowModal);
			progressDialog->setMinimumDuration(2000);
			connect(fullConBuilder, SIGNAL( progress(int, int) ), this, SLOT( updateProgress(int, int) ) );
			connect(fullConBuilder, SIGNAL( finished() ), this, SLOT( threadFinished() ) );
			stop = false;
			fullConBuilder->prepareAddNetworks(networkName->text(), netDesc, &stop);
			fullConBuilder->start();
		}
		else if(netDesc == "Set of 4 partitioned 12 neuron networks with different training"){
			PartitionedNetworksBuilder* partitionedNetBuilder = new PartitionedNetworksBuilder();
			networksBuilder = partitionedNetBuilder;
			progressDialog = new QProgressDialog("Building partitioned network", "Cancel", 0, 100, this);
			progressDialog->setWindowModality(Qt::WindowModal);
			progressDialog->setMinimumDuration(2000);
			connect(partitionedNetBuilder, SIGNAL( progress(int, int) ), this, SLOT( updateProgress(int, int) ) );
			connect(partitionedNetBuilder, SIGNAL( finished() ), this, SLOT( threadFinished() ) );
			stop = false;
			partitionedNetBuilder->prepareAddNetworks(networkName->text(), netDesc, &stop);
			partitionedNetBuilder->start();
		}
		else if(netDesc == "Set of 4 modular 12 neuron networks with different training"){
			ModularNetworksBuilder* modularNetBuilder = new ModularNetworksBuilder();
			networksBuilder = modularNetBuilder;
			progressDialog = new QProgressDialog("Building modular network", "Cancel", 0, 100, this);
			progressDialog->setWindowModality(Qt::WindowModal);
			progressDialog->setMinimumDuration(2000);
			connect(modularNetBuilder, SIGNAL( progress(int, int) ), this, SLOT( updateProgress(int, int) ) );
			connect(modularNetBuilder, SIGNAL( finished() ), this, SLOT( threadFinished() ) );
			stop = false;
			modularNetBuilder->prepareAddNetworks(networkName->text(), netDesc, &stop);
			modularNetBuilder->start();
		}
		else if(netDesc == "Set of 4 sensory 12 neuron networks with different training"){
			SensoryNetworksBuilder* sensoryNetBuilder = new SensoryNetworksBuilder();
			networksBuilder = sensoryNetBuilder;
			progressDialog = new QProgressDialog("Building sensory network", "Cancel", 0, 100, this);
			progressDialog->setWindowModality(Qt::WindowModal);
			progressDialog->setMinimumDuration(2000);
			connect(sensoryNetBuilder, SIGNAL( progress(int, int) ), this, SLOT( updateProgress(int, int) ) );
			connect(sensoryNetBuilder, SIGNAL( finished() ), this, SLOT( threadFinished() ) );
			stop = false;
			sensoryNetBuilder->prepareAddNetworks(networkName->text(), netDesc, &stop);
			sensoryNetBuilder->start();
		}
		else {
			throw SpikeStreamException("Network descrption not recognized: " + netDesc);
		}
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


void AlekGam2NetworksWidget::threadFinished(){
	//Check for errors
	if(networksBuilder->isError()){
		qCritical()<<networksBuilder->getErrorMessage();
	}

	Globals::getEventRouter()->reloadSlot();
	delete networksBuilder;
}


QString AlekGam2NetworksWidget::getNetworkName(){
	if(networkName->text() == "")
		return QString("Unnamed");
	return networkName->text();
}


QPushButton* AlekGam2NetworksWidget::addNetworkButton(QGridLayout* gridLayout, const QString& description){
	int row = gridLayout->rowCount();
	gridLayout->addWidget(new QLabel(description), row, 0);
	QPushButton* addButton = new QPushButton("Add");
	addButton->setObjectName(description);
	addButton->setMaximumSize(120, 30);
	gridLayout->addWidget(addButton, row, 1);
	return addButton;
}


void AlekGam2NetworksWidget::updateProgress(int stepsCompleted, int totalSteps){
	if(stepsCompleted < totalSteps){
		progressDialog->setValue(stepsCompleted);
		progressDialog->setMaximum(totalSteps);
	}
	else{
		progressDialog->close();
	}

	if(progressDialog->wasCanceled())
		stop = true;
}


