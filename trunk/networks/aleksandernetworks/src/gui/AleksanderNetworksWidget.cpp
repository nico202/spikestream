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
    /*! Creates a StateBasedPhiWidget class when library is dynamically loaded. */
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
    gridLayout->addWidget(new QLabel("Aleksander 4 neuron network 1"), 1, 0);
    QPushButton* addButton = new QPushButton("Add");
    addButton->setMaximumSize(120, 30);
    connect (addButton, SIGNAL(clicked()), this, SLOT(add4NeuronNetworks1()));
    gridLayout->addWidget(addButton, 1, 1);

}


/*! Destructor */
AleksanderNetworksWidget::~AleksanderNetworksWidget(){
}


void AleksanderNetworksWidget::add4NeuronNetworks1(){
    try{
	QString netName = networkName->text();
	if(netName == "")
	    netName = "Unnamed";
	networkBuilder->add4NeuronNetworks1(netName);
	Globals::getEventRouter()->reloadSlot();
    }
    catch(SpikeStreamException& ex){
	qCritical()<<ex.getMessage();
    }
}















