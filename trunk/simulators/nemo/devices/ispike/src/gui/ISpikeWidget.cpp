//SpikeStream includes
#include "AddChannelDialog.h"
#include "ChannelTableView.h"
#include "Globals.h"
#include "ISpikeWidget.h"
using namespace spikestream;

//Qt includes
#include <QLayout>
#include <QLabel>

//iSpike includes
#include "iSpike/Channel/InputChannel/InputChannel.hpp"
#include "iSpike/Channel/OutputChannel/OutputChannel.hpp"

//Other includes
#include <iostream>
#include <map>
using namespace std;


//Functions for dynamic library loading
extern "C" {
	/*! Creates an QWidget class when library is dynamically loaded. */
	QWidget* getClass(){
		return new ISpikeWidget();
	}

	/*! Returns a descriptive name for this widget */
	QString getName(){
		return QString("iSpike");
	}
}


/*! Constructor */
ISpikeWidget::ISpikeWidget(QWidget* parent) : AbstractDeviceWidget(parent){
	//Create layout for the entire widget
	QVBoxLayout* mainVBox = new QVBoxLayout(this);

	//Validators
	QRegExp ipRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
	QRegExpValidator* ipAddressValidator = new QRegExpValidator(ipRegExp, this);

	//Add inputs to set the DNS server
	QHBoxLayout* dnsBox = new QHBoxLayout();
	dnsBox->addWidget(new QLabel("DNS Server Address: "));
	dnsEdit = new QLineEdit();
	dnsEdit->setValidator(ipAddressValidator);
	dnsBox->addWidget(dnsEdit);
	connectButton = new QPushButton("Connect");
	connectButton->setMinimumSize(60, 20);
	connect(connectButton, SIGNAL(clicked()), this, SLOT(connectButtonClicked()));
	dnsBox->addWidget(connectButton);
	disconnectButton = new QPushButton("Disconnect");
	disconnectButton->setMinimumSize(60, 20);
	disconnectButton->setEnabled(false);
	connect(disconnectButton, SIGNAL(clicked()), this, SLOT(disconnectButtonClicked()));
	dnsBox->addWidget(disconnectButton);
	mainVBox->addLayout(dnsBox);

	//Combo boxes to connect layers
	QHBoxLayout* channelBox = new QHBoxLayout();
	addChannelButton = new QPushButton("Add channel");
	connect(addChannelButton, SIGNAL(clicked()), this, SLOT(addChannel()));
	channelBox->addWidget(addChannelButton);
	channelBox->addStretch(5);
	mainVBox->addLayout(channelBox);

	//Create iSpike manager
	iSpikeManager = new ISpikeManager();

	//Model and view to display active channels
	channelModel = new ChannelModel(iSpikeManager);
	ChannelTableView* channelView = new ChannelTableView(channelModel);
	mainVBox->addWidget(channelView);

	//Listen for changes in network
	connect(Globals::getEventRouter(), SIGNAL(networkChangedSignal()), this, SLOT(networkChanged()));

	mainVBox->addStretch(5);
}


/*! Destructor */
ISpikeWidget::~ISpikeWidget(){
}


/*----------------------------------------------------------*/
/*------                PUBLIC METHODS                ------*/
/*----------------------------------------------------------*/

/*! Returns a pointer to the device manager */
AbstractDeviceManager* ISpikeWidget::getDeviceManager(){
	return (AbstractDeviceManager*)iSpikeManager;
}


/*----------------------------------------------------------*/
/*------                 PRIVATE SLOTS                ------*/
/*----------------------------------------------------------*/

/*! Adds a new channel, which will produce or listen for neuron spikes. */
void ISpikeWidget::addChannel(){
	try{
		AddChannelDialog* tmpDlg = new AddChannelDialog(this);
		if(tmpDlg->exec() == QDialog::Accepted){
			//Add channel to model
			if(tmpDlg->isInputChannel())
				iSpikeManager->addChannel(tmpDlg->getInputChannel(), tmpDlg->getNeuronGroup());//Add channel to iSpikeManager
			else
				iSpikeManager->addChannel(tmpDlg->getOutputChannel(), tmpDlg->getNeuronGroup());//Add channel to iSpikeManager

			//Instruct model to reload data.
			channelModel->reload();
		}
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
	catch(...){
		qCritical()<<"An unknown exception occurred configuring a channel";
	}

	//Reload table displaying list of channels
	channelModel->reload();
}


/*! Called when the connect button is clicked.
	Tries to connect to DNS server and gives an error message if this fails. */
void ISpikeWidget::connectButtonClicked(){
	//CHECK INPUT

	//TRY TO CONNECT TO DNS SERVER

	//CHECK THAT CHANNELS ARE STILL VALID

	//Enable/disable appropriate graphical components
	connectButton->setEnabled(false);
	dnsEdit->setEnabled(false);
	disconnectButton->setEnabled(true);
}


/*! Called when the disconnect button is clicked.
	Tries to connect to DNS server and gives an error message if this fails. */
void ISpikeWidget::disconnectButtonClicked(){
	//CHECK INPUT

	//TRY TO DISCONNECT FROM DNS SERVER

	//ENABLE CONNECT BUTTON; DISENABLE DISCONNECT BUTTON.

	connectButton->setEnabled(true);
	dnsEdit->setEnabled(true);
	disconnectButton->setEnabled(false);
}


/*! Called when network is changed */
void ISpikeWidget::networkChanged(){
	if(channelModel->isEmpty()){
		return;
	}
	if(Globals::networkLoaded()){
		iSpikeManager->deleteAllChannels();
		channelModel->reload();
	}
}


/*----------------------------------------------------------*/
/*------                PRIVATE METHODS               ------*/
/*----------------------------------------------------------*/







