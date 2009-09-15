//SpikeStream includes
#include "Globals.h"
#include "NetworkDao.h"
#include "NetworksWidget.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QLabel>
#include <QPushButton>

#include <iostream>
using namespace std;

/*! Constructor */
NetworksWidget::NetworksWidget(QWidget* parent) : QWidget(parent){
    //this->setStyleSheet("* { background-color: white; }");

    QVBoxLayout* verticalBox = new QVBoxLayout(this, 2, 2);

    gridLayout = new QGridLayout();
    gridLayout->setMargin(10);
    gridLayout->setColumnMinimumWidth(0, 50);//ID
    gridLayout->setColumnMinimumWidth(1, 100);//Name
    gridLayout->setColumnMinimumWidth(2, 50);//Spacer
    gridLayout->setColumnMinimumWidth(3, 250);//Description
    gridLayout->setColumnMinimumWidth(4, 100);//Load button

    QHBoxLayout* gridLayoutHolder = new QHBoxLayout();
    gridLayoutHolder->addLayout(gridLayout);
    gridLayoutHolder->addStretch(5);
    verticalBox->addLayout(gridLayoutHolder);

    //Load the network into the grid layout
    reloadNetworkList();

    verticalBox->addStretch(10);

    //Connect to global reload signal
    connect(Globals::getEventRouter(), SIGNAL(reloadSignal()), this, SLOT(reloadNetworkList()), Qt::QueuedConnection);
    connect(this, SIGNAL(networkChanged()), Globals::getEventRouter(), SLOT(networkChangedSlot()), Qt::QueuedConnection);
}


/*! Destructor */
NetworksWidget::~NetworksWidget(){
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

void NetworksWidget::reloadNetworkList(){
    //Reset widget
    reset();

    //Get a list of the networks in the database
    NetworkDao* networkDao = Globals::getNetworkDao();
    QList<NetworkInfo> networkInfoList;
    try{
	networkInfoList = networkDao->getNetworksInfo();
    }
    catch(SpikeStreamException& ex){
	qCritical()<<ex.getMessage();
	return;
    }

    //Show "no network" message if list is empty
    if(networkInfoList.size() == 0){
	gridLayout->addWidget(new QLabel("No networks in database"), 0, 0);
	Globals::setNetwork(0);
	emit networkChanged();
	return;
    }

    //Copy network infos into map
    for(int i=0; i<networkInfoList.size(); ++i){
	networkInfoMap[networkInfoList[i].getID()] = networkInfoList[i];
    }

    /* If the current network is in the network list, then set this as the one loaded
       Otherwise currentNetworkID is set to zero and the user has to choose the loaded network */
    unsigned int currentNetworkID = 0;
    if(Globals::getNetwork() != NULL && networkInfoMap.contains(Globals::getNetwork()->getID())){
	currentNetworkID = Globals::getNetwork()->getID();
    }


    //Display the list in the widget
    for(int i=0; i<networkInfoList.size(); ++i){
	//Create labels
	QLabel* idLabel = new QLabel(QString::number(networkInfoList[i].getID()));
	QLabel* nameLabel = new QLabel(networkInfoList[i].getName());
	QLabel* descriptionLabel = new QLabel(networkInfoList[i].getDescription());

	//Create the button and name it with the object id so we can tell which button was invoked
	QPushButton* loadButton = new QPushButton("Load");
	loadButton->setObjectName(QString::number(networkInfoList[i].getID()));
	connect ( loadButton, SIGNAL(clicked()), this, SLOT( loadNetwork() ) );//networkInfoList[i].getID()

	//Set labels and buttons depending on whether it is the current network
	if(currentNetworkID == networkInfoList[i].getID()){
	    if(networkInfoList[i].isLocked()){
		idLabel->setStyleSheet( "QLabel { color: #F08080; font-weight: bold; font-style: italic; }");
		nameLabel->setStyleSheet( "QLabel { color: #F08080; font-weight: bold; font-style: italic; }");
		descriptionLabel->setStyleSheet( "QLabel { color: #F08080; font-weight: bold; font-style: italic; }");
	    }
	    else{
		idLabel->setStyleSheet( "QLabel { color: #008000; font-weight: bold; }");
		nameLabel->setStyleSheet( "QLabel { color: #008000; font-weight: bold; }");
		descriptionLabel->setStyleSheet( "QLabel { color: #008000; font-weight: bold; }");
	    }
	    loadButton->setEnabled(false);
	}
	else if (networkInfoList[i].isLocked()) {
	    idLabel->setStyleSheet( "QLabel { color: #777777; font-style: italic; }");
	    nameLabel->setStyleSheet( "QLabel { color: #777777; font-style: italic; }");
	    descriptionLabel->setStyleSheet( "QLabel { color: #777777; font-style: italic; }");
	}
	else{
	    idLabel->setStyleSheet( "QLabel { color: #777777; }");
	    nameLabel->setStyleSheet( "QLabel { color: #777777; }");
	    descriptionLabel->setStyleSheet( "QLabel { color: #777777; }");
	}

	//Add the widgets to the layout
	gridLayout->addWidget(idLabel, i, 0);
	gridLayout->addWidget(nameLabel, i, 1);
	gridLayout->addWidget(descriptionLabel, i, 3);
	gridLayout->addWidget(loadButton, i, 4);
    }


    /* Load up the appropriate network

	- if it is not already loaded
    if(Globals::getNetwork() != NULL && Globals::getNetwork()->getID() != currentNetworkID){
	loadNetwork(networkInfoMap[currentNetworkID]);
    }*/

}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Loads a particular network into memory */
void NetworksWidget::loadNetwork(){
    unsigned int networkID = sender()->objectName().toUInt();
    if(!networkInfoMap.contains(networkID)){
	qCritical()<<"Network with ID "<<networkID<<" cannot be found.";
	return;
    }

    //load the network
    loadNetwork(networkInfoMap[networkID]);
}


void NetworksWidget::loadNetwork(NetworkInfo& netInfo){
    if(!networkInfoMap.contains(netInfo.getID())){
	qCritical()<<"Network with ID "<<netInfo.getID()<<" cannot be found.";
	return;
    }

    //Create new network and store it in global scope
    Network* newNetwork = new Network(netInfo, Globals::getNetworkDao());
    Globals::setNetwork(newNetwork);

    //Use event router to inform other classes that the network has changed.
    emit networkChanged();

    /* Reload network list to reflect color changes and buttons enabled
	Should not lead to an infinite loop because network in Globals
	should match one in the list returned from the database */
    reloadNetworkList();
}


/*! Resets the state of the widget.
    Deleting the widget automatically removes it from the layout. */
void NetworksWidget::reset(){
    //Remove no networks label if it exists
    if(networkInfoMap.size() == 0){
    	QLayoutItem* item = gridLayout->itemAtPosition(0, 0);
	if(item != 0){
	    delete item->widget();
	}
	return;
    }

    //Remove list of networks
    //FIXME: GENERATES WARNING DO NOT DELETE OBJECT '8' DURING ITS EVENT HANDLER
    for(int i=0; i<networkInfoMap.size(); ++i){
	QLayoutItem* item = gridLayout->itemAtPosition(i, 0);
	if(item != 0){
	    delete item->widget();
	}
	item = gridLayout->itemAtPosition(i, 1);
	if(item != 0){
	    delete item->widget();
	}
	item = gridLayout->itemAtPosition(i, 3);
	if(item != 0){
	    delete item->widget();
	}
	item = gridLayout->itemAtPosition(i, 4);
	if(item != 0){
	    delete item->widget();
	}
    }
    networkInfoMap.clear();
}





