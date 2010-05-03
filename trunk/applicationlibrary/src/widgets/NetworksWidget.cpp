//SpikeStream includes
#include "Globals.h"
#include "NetworkDao.h"
#include "NewNetworkDialog.h"
#include "NetworksWidget.h"
#include "PluginsDialog.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>

#include <iostream>
using namespace std;

/*! Constructor */
NetworksWidget::NetworksWidget(QWidget* parent) : QWidget(parent){
	//this->setStyleSheet("* { background-color: white; }");

	QVBoxLayout* verticalBox = new QVBoxLayout(this);

	//Add button to add a new empty network to the database
	QHBoxLayout* buttonBox = new QHBoxLayout();
	QPushButton* newNetworkButton = new QPushButton("New Network");
	newNetworkButton->setMaximumSize(120, 30);
	connect (newNetworkButton, SIGNAL(clicked()), this, SLOT(addNewNetwork()));
	buttonBox->addWidget(newNetworkButton);

	//Add button to launch networks dialog
	QPushButton* addNetworksButton = new QPushButton("Add Networks");
	addNetworksButton->setMaximumSize(120, 30);
	connect (addNetworksButton, SIGNAL(clicked()), this, SLOT(addNetworks()));
	buttonBox->addWidget(addNetworksButton);
	buttonBox->addStretch(10);
	verticalBox->addLayout(buttonBox);

	//Add grid holding networks
	gridLayout = new QGridLayout();
	gridLayout->setMargin(10);
	gridLayout->setColumnMinimumWidth(0, 50);//ID
	gridLayout->setColumnMinimumWidth(1, 100);//Name
	gridLayout->setColumnMinimumWidth(2, 50);//Spacer
	gridLayout->setColumnMinimumWidth(3, 250);//Description
	gridLayout->setColumnMinimumWidth(4, 100);//Load button
	gridLayout->setColumnMinimumWidth(5, 100);//Delete button

	QHBoxLayout* gridLayoutHolder = new QHBoxLayout();
	gridLayoutHolder->addLayout(gridLayout);
	gridLayoutHolder->addStretch(5);
	verticalBox->addLayout(gridLayoutHolder);

	//Load the network into the grid layout
	loadNetworkList();

	verticalBox->addStretch(10);

	//Connect to global reload signal
	connect(Globals::getEventRouter(), SIGNAL(reloadSignal()), this, SLOT(loadNetworkList()), Qt::QueuedConnection);
	//connect(Globals::getEventRouter(), SIGNAL(networkChangedSignal()), this, SLOT(loadNetworkList()), Qt::QueuedConnection);
	connect(Globals::getEventRouter(), SIGNAL(networkListChangedSignal()), this, SLOT(loadNetworkList()), Qt::QueuedConnection);
	connect(this, SIGNAL(networkChanged()), Globals::getEventRouter(), SLOT(networkChangedSlot()), Qt::QueuedConnection);
}


/*! Destructor */
NetworksWidget::~NetworksWidget(){
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

/*! Shows a plugins dialog to enable the user to add a network */
void NetworksWidget::addNetworks(){
	try{
		PluginsDialog* pluginsDialog = new PluginsDialog(this, "/plugins/networks", "Add Network");
		pluginsDialog->exec();
		delete pluginsDialog;
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Deletes a network */
void NetworksWidget::deleteNetwork(){
	//Get the ID of the network to be deleted
	unsigned int networkID = Util::getUInt(sender()->objectName());
	if(!networkInfoMap.contains(networkID)){
		qCritical()<<"Network with ID "<<networkID<<" cannot be found.";
		return;
	}

	//Confirm that user wants to take this action.
	QMessageBox msgBox;
	msgBox.setText("Deleting Network");
	msgBox.setInformativeText("Are you sure that you want to delete network with ID " + QString::number(networkID) + "? This step cannot be undone.");
	msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Cancel);
	int ret = msgBox.exec();
	if(ret != QMessageBox::Ok)
		return;

	//Delete the network from the database
	try{
		Globals::getNetworkDao()->deleteNetwork(networkID);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<"Exception thrown when deleting network.";
	}

	/* If we have deleted the current network, use event router to inform other classes that the network has changed.
	   This will automatically reload the network list. */
	if(Globals::networkLoaded() && Globals::getNetwork()->getID() == networkID){
		Globals::setNetwork(NULL);
		emit networkChanged();
	}

	//Reload the network list
	loadNetworkList();
}


void NetworksWidget::loadNetworkList(){
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
	if(Globals::networkLoaded() && networkInfoMap.contains(Globals::getNetwork()->getID())){
		currentNetworkID = Globals::getNetwork()->getID();
	}


	//Display the list in the widget
	for(int i=0; i<networkInfoList.size(); ++i){
		//Create labels
		QLabel* idLabel = new QLabel(QString::number(networkInfoList[i].getID()));
		QLabel* nameLabel = new QLabel(networkInfoList[i].getName());
		QLabel* descriptionLabel = new QLabel(networkInfoList[i].getDescription());

		//Create the load button and name it with the object id so we can tell which button was invoked
		QPushButton* loadButton = new QPushButton("Load");
		loadButton->setObjectName(QString::number(networkInfoList[i].getID()));
		connect ( loadButton, SIGNAL(clicked()), this, SLOT( loadNetwork() ) );

		//Create the delete button and name it with the object id so we can tell which button was invoked
		QPushButton* deleteButton = new QPushButton("Delete");
		deleteButton->setObjectName(QString::number(networkInfoList[i].getID()));
		connect ( deleteButton, SIGNAL(clicked()), this, SLOT( deleteNetwork() ) );

		//Set labels and buttons depending on whether it is the current network
		if(currentNetworkID == networkInfoList[i].getID()){
			if(Globals::getArchiveDao()->networkIsLocked(currentNetworkID)){
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
		else if (Globals::getArchiveDao()->networkIsLocked(currentNetworkID)) {
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
		gridLayout->addWidget(deleteButton, i, 5);
	}

	//FIXME: HACK TO GET IT TO DISPLAY PROPERLY
	this->setMinimumHeight(networkInfoList.size() * 100);
}


/*! Shows a plugins dialog to enable the user to add a network */
void NetworksWidget::addNewNetwork(){
	try{
		NewNetworkDialog* newNetworkDialog = new NewNetworkDialog(this);
		newNetworkDialog->exec();
		delete newNetworkDialog;
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
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

	try{
		/* Create new network and store it in global scope.
			Network is set to null because if an exception is thrown during construction
			then we need to know if the object was created */
		newNetwork = NULL;
		newNetwork = new Network(netInfo, Globals::getNetworkDao(), Globals::getArchiveDao());

		//Start network loading, all the heavy work is done by separate threads
		newNetwork->load();
	}
	catch (SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
		if(newNetwork != NULL)
			delete newNetwork;
		return;
	}

	//Set up progress dialog, this checks on the network every 200ms until loading is complete
	progressDialog = new QProgressDialog("Loading network", "Abort load", 0, newNetwork->getTotalNumberOfSteps(), this);
	progressDialog->setWindowModality(Qt::WindowModal);
	loadingTimer  = new QTimer(this);
	connect(loadingTimer, SIGNAL(timeout()), this, SLOT(checkLoadingProgress()));
	loadingTimer->start(200);
}



void NetworksWidget::checkLoadingProgress(){
	//Check for errors during loading
	if(newNetwork->isError()){
		loadingTimer->stop();
		newNetwork->cancel();
		progressDialog->setValue(progressDialog->maximum());
		delete progressDialog;
		qCritical()<<"Error occurred loading network: '"<<newNetwork->getErrorMessage()<<"'.";
		return;
	}

	//Check for cancelation - stop timer and abort operation
	else if(progressDialog->wasCanceled()){
		loadingTimer->stop();
		newNetwork->cancel();
		progressDialog->setValue(progressDialog->maximum());
		delete progressDialog;
		return;
	}

	//If networks is busy, update progress bar and return with loading timer still running
	else if(newNetwork->isBusy()){
		progressDialog->setValue(newNetwork->getNumberOfCompletedSteps());
		return;
	}

	//If we have reached this point, loading is complete
	loadingTimer->stop();
	progressDialog->setValue(progressDialog->maximum());
	delete progressDialog;

	//Store network in global scope
	Globals::setNetwork(newNetwork);

	//Use event router to inform other classes that the network has changed.
	emit networkChanged();

	/* Reload network list to reflect color changes and buttons enabled
		Should not lead to an infinite loop because network in Globals
		should match one in the list returned from the database */
	loadNetworkList();
}


/*! Resets the state of the widget.
	Deleting the widget automatically removes it from the layout. */
void NetworksWidget::reset(){
	//Clean up widgets that were scheduled to be deleted during the previous event cycle
	foreach(QWidget* widget , cleanUpList)
		widget->deleteLater();
	cleanUpList.clear();

	//Remove no networks label if it exists
	if(networkInfoMap.size() == 0){
		QLayoutItem* item = gridLayout->itemAtPosition(0, 0);
		if(item != 0){
			delete item->widget();
		}
		return;
	}

	//Remove list of networks
	for(int i=0; i<networkInfoMap.size(); ++i){
		QLayoutItem* item = gridLayout->itemAtPosition(i, 0);
		if(item != 0){
			QWidget* tmpWidget = item->widget();
			gridLayout->removeWidget(tmpWidget);
			tmpWidget->setVisible(false);
			cleanUpList.append(tmpWidget);
		}
		item = gridLayout->itemAtPosition(i, 1);
		if(item != 0){
			QWidget* tmpWidget = item->widget();
			gridLayout->removeWidget(tmpWidget);
			tmpWidget->setVisible(false);
			cleanUpList.append(tmpWidget);
		}
		item = gridLayout->itemAtPosition(i, 3);
		if(item != 0){
			QWidget* tmpWidget = item->widget();
			gridLayout->removeWidget(tmpWidget);
			tmpWidget->setVisible(false);
			cleanUpList.append(tmpWidget);
		}
		item = gridLayout->itemAtPosition(i, 4);
		if(item != 0){
			QWidget* tmpWidget = item->widget();
			gridLayout->removeWidget(tmpWidget);
			tmpWidget->setVisible(false);
			cleanUpList.append(tmpWidget);
		}
		item = gridLayout->itemAtPosition(i, 5);
		if(item != 0){
			QWidget* tmpWidget = item->widget();
			gridLayout->removeWidget(tmpWidget);
			tmpWidget->setVisible(false);
			cleanUpList.append(tmpWidget);
		}
	}
	networkInfoMap.clear();
}





