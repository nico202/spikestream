//SpikeStream includes
#include "EventRouter.h"
#include "Globals.h"
#include "SpikeStreamException.h"
#include "AleksanderNetworksWidget.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QDialog>

//Other includes
#include <typeinfo>

//Functions for dynamic library loading
extern "C" {
	/*! Creates a AleksanderNetworksWidget class when library is dynamically loaded. */
	QWidget* getClass(){
		return new AleksanderNetworksWidget();
	}

	/*! Returns a descriptive name for this widget */
	QString getName(){
		return QString("Aleksander Networks");
	}
}


/*! Constructor */
AleksanderNetworksWidget::AleksanderNetworksWidget(){
	//Create class that will be used to build networks
	networkBuilder = new AleksanderNetworksBuilder();

	//Layout using grid layout
	QGridLayout* gridLayout = new QGridLayout(this);
	gridLayout->setMargin(10);

	//Field to enable user to enter network name
	gridLayout->addWidget(new QLabel("Network name: "), 0, 0);
	networkName = new QLineEdit("Unnamed");
	networkName->setMinimumSize(250, 30);
	gridLayout->addWidget(networkName, 0, 1);

	//Add buttons for each network
	//Network 1
	QPushButton* newButton = addNetworkButton(gridLayout, "4 neurons. A<->B; C<->D. AND");
	connect (newButton, SIGNAL(clicked()), this, SLOT(addNetwork()));

	//Network 2
	newButton = addNetworkButton(gridLayout, "4 neurons. A<->B; A->C; C<->D. AND");
	connect (newButton, SIGNAL(clicked()), this, SLOT(addNetwork()));

	newButton = addNetworkButton(gridLayout, "4 neurons. A<->B; A->C; C<->D. XOR");
	connect (newButton, SIGNAL(clicked()), this, SLOT(addNetwork()));

	//Network 3
	newButton = addNetworkButton(gridLayout, "4 neurons. A<->B; A<->C; C<->D. AND");
	connect (newButton, SIGNAL(clicked()), this, SLOT(addNetwork()));

	newButton = addNetworkButton(gridLayout, "4 neurons. A<->B; A<->C; C<->D. XOR");
	connect (newButton, SIGNAL(clicked()), this, SLOT(addNetwork()));

	//Network 4
	newButton = addNetworkButton(gridLayout, "4 neurons. A<->B; A<->C; B<->D; C<->D. AND");
	connect (newButton, SIGNAL(clicked()), this, SLOT(addNetwork()));

	newButton = addNetworkButton(gridLayout, "4 neurons. A<->B; A<->C; B<->D; C<->D. XOR");
	connect (newButton, SIGNAL(clicked()), this, SLOT(addNetwork()));

	//Network 5
	newButton = addNetworkButton(gridLayout, "4 neurons. A<->B; A<->C; A<->D; B<->C; B<->D; C<->D. AND");
	connect (newButton, SIGNAL(clicked()), this, SLOT(addNetwork()));

	newButton = addNetworkButton(gridLayout, "4 neurons. A<->B; A<->C; A<->D; B<->C; B<->D; C<->D. XOR");
	connect (newButton, SIGNAL(clicked()), this, SLOT(addNetwork()));
}


/*! Destructor */
AleksanderNetworksWidget::~AleksanderNetworksWidget(){
}


void AleksanderNetworksWidget::addNetwork(){
	QString netDesc = sender()->objectName();
	try{
	if(netDesc == "4 neurons. A<->B; C<->D. AND")
		networkBuilder->add4NeuronNetwork1(getNetworkName(), netDesc);

	else if(netDesc == "4 neurons. A<->B; A->C; C<->D. AND")
		networkBuilder->add4NeuronNetwork2_AND(getNetworkName(), netDesc);

	else if(netDesc == "4 neurons. A<->B; A->C; C<->D. XOR")
		networkBuilder->add4NeuronNetwork2_XOR(getNetworkName(), netDesc);

	else if(netDesc == "4 neurons. A<->B; A<->C; C<->D. AND")
		networkBuilder->add4NeuronNetwork3_AND(getNetworkName(), netDesc);

	else if(netDesc == "4 neurons. A<->B; A<->C; C<->D. XOR")
		networkBuilder->add4NeuronNetwork3_XOR(getNetworkName(), netDesc);

	else if(netDesc == "4 neurons. A<->B; A<->C; B<->D; C<->D. AND")
		networkBuilder->add4NeuronNetwork4_AND(getNetworkName(), netDesc);

	else if(netDesc == "4 neurons. A<->B; A<->C; B<->D; C<->D. XOR")
		networkBuilder->add4NeuronNetwork4_XOR(getNetworkName(), netDesc);

	else if(netDesc == "4 neurons. A<->B; A<->C; A<->D; B<->C; B<->D; C<->D. AND")
		networkBuilder->add4NeuronNetwork5_AND(getNetworkName(), netDesc);

	else if(netDesc == "4 neurons. A<->B; A<->C; A<->D; B<->C; B<->D; C<->D. XOR")
		networkBuilder->add4NeuronNetwork5_XOR(getNetworkName(), netDesc);

	else
		throw SpikeStreamException("Network descrption not recognized: " + netDesc);

	Globals::getEventRouter()->reloadSlot();
	}
	catch(SpikeStreamException& ex){
	qCritical()<<ex.getMessage();
	}
}



QString AleksanderNetworksWidget::getNetworkName(){
	if(networkName->text() == "")
	return QString("Unnamed");
	return networkName->text();
}


QPushButton* AleksanderNetworksWidget::addNetworkButton(QGridLayout* gridLayout, const QString& description){
	int row = gridLayout->rowCount();
	gridLayout->addWidget(new QLabel(description), row, 0);
	QPushButton* addButton = new QPushButton("Add");
	addButton->setObjectName(description);
	addButton->setMaximumSize(120, 30);
	gridLayout->addWidget(addButton, row, 1);
	return addButton;
}











