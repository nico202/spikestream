//SpikeStream includes
#include "ChannelTableView.h"
#include "Globals.h"
#include "ISpikeWidget.h"
using namespace spikestream;

//Qt includes
#include <QLayout>
#include <QLabel>

//iSpike includes
#include "iSpike/ChannelController.hpp"

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
	channelCombo = new QComboBox();
	connect(channelCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(channelComboChanged(QString)));
	channelBox->addWidget(new QLabel("Channel: "));
	channelBox->addWidget(channelCombo);
	neuronGroupCombo = new QComboBox();
	channelBox->addWidget(new QLabel("Neuron Group: "));
	channelBox->addWidget(neuronGroupCombo);
	addChannelButton = new QPushButton("Add");
	connect(addChannelButton, SIGNAL(clicked()), this, SLOT(addChannel()));
	channelBox->addWidget(addChannelButton);
	channelBox->addStretch(5);
	mainVBox->addLayout(channelBox);

	//Load up combo boxes with available channels and matching neuron groups
	fillChannelCombo();

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
	//Run some checks
	if(channelCombo->currentText().isEmpty() || neuronGroupCombo->currentText().isEmpty()){
		qCritical()<<"Channel and/or neuron group text missing. Cannot add channel.";
		return;
	}

	//Get channel class from iSpike library
	QString channelName = channelCombo->currentText();

	//Add channel to iSpikeManager
	iSpikeManager->addChannel();

	//Reload table displaying list of channels
	channelModel->reload();
}


/*! Called when the channel combo is changed. Loads a selection of
	neuron groups that are compatible with the channel. */
void ISpikeWidget::channelComboChanged(QString channelName){
	fillNeuronGroupCombo(channelName);
}


/*! Called when the connect button is clicked.
	Tries to connect to DNS server and gives an error message if this fails. */
void ISpikeWidget::connectButtonClicked(){
	//CHECK INPUT

	//TRY TO CONNECT TO DNS SERVER

	//Load list of available channels
	fillChannelCombo();

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
	neuronGroupCombo->clear();

	if(Globals::networkLoaded())
		fillNeuronGroupCombo(channelCombo->currentText());
}


/*----------------------------------------------------------*/
/*------                PRIVATE METHODS               ------*/
/*----------------------------------------------------------*/

/*! Fills the channel combo with a list of available channels from the iSpike library */
void ISpikeWidget::fillChannelCombo(){
	channelCombo->clear();

	//Get channel infromation from iSpike library
	try{
		ChannelController* controller = new ChannelController();
		std::map<int, std::string>::iterator i;
		std::map<int,std::string>* inputChannels = controller->getInputChannels();
		std::cout << "Input Channels:" << std::endl;
		for (i = inputChannels->begin(); i != inputChannels->end(); i++)
			std::cout << i->first << "," << i->second << std::endl;
		std::map<int,std::string>* outputChannels = controller->getOutputChannels();
		std::cout << "Output Channels:" << std::endl;
		for (i = outputChannels->begin(); i != outputChannels->end(); i++)
			std::cout << i->first << "," << i->second << std::endl;
	}
	catch(...){
		qCritical()<<"iSpike has thrown an unknown exception.";
	}


	//Load matching neuron groups
	neuronGroupCombo->clear();
	if(Globals::networkLoaded())
		fillNeuronGroupCombo(channelCombo->currentText());
}


/*! Fills the neuron group combo with a list of neuron groups that match
	the currently selected channel type. */
void ISpikeWidget::fillNeuronGroupCombo(QString channelName){
	if(!Globals::networkLoaded()){
		qCritical()<<"Cannot load neuron groups into ISpikeWidget without a loaded network";
		return;
	}

	//Clear combo
	neuronGroupCombo->clear();
	addChannelButton->setEnabled(false);

	//Don't load any neuron groups if there are no channels
	if(channelName.isEmpty()){
		return;
	}

	//Load neuron groups that are compatible with the current channel
	QList<NeuronGroup*> neurGrpList = Globals::getNetwork()->getNeuronGroups();
	foreach(NeuronGroup* tmpNeurGrp, neurGrpList){
		//CHECK SIZE MATCHES CURRENT CHANNEL
		neuronGroupCombo->addItem(tmpNeurGrp->getInfo().getName());
	}
	if(neuronGroupCombo->count() > 0)
		addChannelButton->setEnabled(true);
}






