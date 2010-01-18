//SpikeStream includes
#include "EventRouter.h"
#include "Globals.h"
#include "SpikeStreamException.h"
#include "TononiNetworksWidget.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QDialog>

//Other includes
#include <typeinfo>

//Functions for dynamic library loading
extern "C" {
    /*! Creates a StateBasedPhiWidget class when library is dynamically loaded. */
    TononiNetworksWidget* getClass(){
	return new TononiNetworksWidget();
    }

    /*! Returns a descriptive name for this widget */
    QString getName(){
	return QString("Tononi Networks");
    }
}


/*! Constructor */
TononiNetworksWidget::TononiNetworksWidget(){
    //Create class that will be used to build networks
    networkBuilder = new TononiNetworkBuilder();

    //Layout to manage whole widget
    QVBoxLayout* mainVBox = new QVBoxLayout(this);

    //Layout using grid layout
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setMargin(10);

    //Field to enable user to enter network name
    gridLayout->addWidget(new QLabel("Network name: "), 0, 0);
    networkName = new QLineEdit("Unnamed");
    networkName->setMinimumSize(250, 30);
    gridLayout->addWidget(networkName, 0, 1);

    //Add buttons for each network
    QPushButton* newButton = addNetworkButton(gridLayout, "Balduzzi and Tononi (2008), Figure 5");
    connect (newButton, SIGNAL(clicked()), this, SLOT(addNetwork()));

    newButton = addNetworkButton(gridLayout, "Balduzzi and Tononi (2008), Figure 6");
    connect (newButton, SIGNAL(clicked()), this, SLOT(addNetwork()));

    newButton = addNetworkButton(gridLayout, "Balduzzi and Tononi (2008), Figure 12");
    connect (newButton, SIGNAL(clicked()), this, SLOT(addNetwork()));

    mainVBox->addLayout(gridLayout);
    mainVBox->addStretch(5);

}


/*! Destructor */
TononiNetworksWidget::~TononiNetworksWidget(){
}


/*----------------------------------------------------------*/
/*-----                  PRIVATE SLOTS                 -----*/
/*----------------------------------------------------------*/

/*! Adds a network matching the name of the sender to the database */
void TononiNetworksWidget::addNetwork(){
    QString netDesc = sender()->objectName();
    try{
	if(netDesc == "Balduzzi and Tononi (2008), Figure 5")
	    networkBuilder->addBalduzziTononiFigure5(getNetworkName(), netDesc);

	else if(netDesc == "Balduzzi and Tononi (2008), Figure 6")
	    networkBuilder->addBalduzziTononiFigure6(getNetworkName(), netDesc);

	else if(netDesc == "Balduzzi and Tononi (2008), Figure 12")
	    networkBuilder->addBalduzziTononiFigure12(getNetworkName(), netDesc);

	else
	    throw SpikeStreamException("Network descrption not recognized: " + netDesc);

	Globals::getEventRouter()->reloadSlot();
    }
    catch(SpikeStreamException& ex){
	qCritical()<<ex.getMessage();
    }
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Returns the name of the network to be added, or 'Unnamed' if this is not specified. */
QString TononiNetworksWidget::getNetworkName(){
    if(networkName->text() == "")
	return QString("Unnamed");
    return networkName->text();
}


/*! Adds a button for adding a network to the GUI */
QPushButton* TononiNetworksWidget::addNetworkButton(QGridLayout* gridLayout, const QString& description){
    int row = gridLayout->rowCount();
    gridLayout->addWidget(new QLabel(description), row, 0);
    QPushButton* addButton = new QPushButton("Add");
    addButton->setObjectName(description);
    addButton->setMaximumSize(120, 30);
    gridLayout->addWidget(addButton, row, 1);
    return addButton;
}


