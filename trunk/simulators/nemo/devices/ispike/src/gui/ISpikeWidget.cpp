//SpikeStream includes
#include "AddChannelDialog.h"
#include "ChannelTableView.h"
#include "Globals.h"
#include "ISpikeWidget.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QLayout>
#include <QLabel>

//iSpike includes
#include "iSpike/Channel/InputChannel/InputChannel.hpp"
#include "iSpike/Channel/OutputChannel/OutputChannel.hpp"
#include "iSpike/Reader/ReaderFactory.hpp"
#include "iSpike/Writer/WriterFactory.hpp"
#include "iSpike/ISpikeException.hpp"

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
	QIntValidator* unsignedIntValidator = new QIntValidator(0, 1000000, this);

	//Add inputs to set the DNS server
	QHBoxLayout* dnsBox = new QHBoxLayout();
	dnsBox->addWidget(new QLabel("YARP DNS Server Address: "));
	dnsEdit = new QLineEdit("127.0.0.1");
	dnsEdit->setValidator(ipAddressValidator);
	dnsBox->addWidget(dnsEdit);
	dnsBox->addWidget(new QLabel("Port"));
	portEdit = new QLineEdit("10000");
	portEdit->setValidator(unsignedIntValidator);
	portEdit->setMaximumSize(60,20);
	dnsBox->addWidget(portEdit);
	connectButton = new QPushButton("Set");
	connectButton->setMinimumSize(60, 20);
	connect(connectButton, SIGNAL(clicked()), this, SLOT(setIPAddressButtonClicked()));
	dnsBox->addWidget(connectButton);
	disconnectButton = new QPushButton("Unset");
	disconnectButton->setMinimumSize(60, 20);
	disconnectButton->setEnabled(false);
	connect(disconnectButton, SIGNAL(clicked()), this, SLOT(unsetIPAddressButtonClicked()));
	dnsBox->addWidget(disconnectButton);
	mainVBox->addLayout(dnsBox);

	//Combo boxes to connect layers
	QHBoxLayout* channelBox = new QHBoxLayout();
	fireOrCurrentCombo = new QComboBox();
	fireOrCurrentCombo->addItem("Fire");
	fireOrCurrentCombo->addItem("1");
	for(int i=10; i<=100; i += 10)
		fireOrCurrentCombo->addItem(QString::number(i));
	fireOrCurrentCombo->setMinimumSize(60, 20);
	connect(fireOrCurrentCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setFireOrCurrent(int)));
	channelBox->addWidget(new QLabel("Input fire or current: "));
	channelBox->addWidget(fireOrCurrentCombo);
	channelBox->addSpacing(10);
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

	//Default variable values
	ipAddressSet = false;

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
		AddChannelDialog* tmpDlg = NULL;
		if(ipAddressSet)
			tmpDlg = new AddChannelDialog(dnsEdit->text(), portEdit->text(), this);
		else
			tmpDlg = new AddChannelDialog(this);
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


/*! Switches the entire device between fire or current mode */
void ISpikeWidget::setFireOrCurrent(int index){
	if(index == 0){
		getDeviceManager()->setFireNeuronMode(true);
	}
	else{
		getDeviceManager()->setFireNeuronMode(false);
		getDeviceManager()->setCurrent(Util::getDouble(fireOrCurrentCombo->currentText()));
	}
}


/*! Called when the connect button is clicked.
	Tries to connect to DNS server and gives an error message if this fails. */
void ISpikeWidget::setIPAddressButtonClicked(){
	if(dnsEdit->text().isEmpty() || portEdit->text().isEmpty()){
		qCritical()<<"IP address and/or port is missing";
		return;
	}

	//First test IP and port to see if a reader can access it
	ipAddressSet = false;
	try {
		ReaderFactory readerFactory(dnsEdit->text().toStdString(), portEdit->text().toStdString());
		WriterFactory writerFactory(dnsEdit->text().toStdString(), portEdit->text().toStdString());
	}
	catch(ISpikeException& ex){
		qCritical()<<"Cannot connect to DNS server at IP: " + dnsEdit->text() + " and port " + portEdit->text()<<" Message: "<<ex.what();
		return;
	}
	catch(...){
		qCritical()<<"Cannot connect to DNS server at IP: " + dnsEdit->text() + " and port " + portEdit->text();
		return;
	}
	ipAddressSet = true;

	//Enable/disable appropriate graphical components
	connectButton->setEnabled(false);
	dnsEdit->setEnabled(false);
	portEdit->setEnabled(false);
	disconnectButton->setEnabled(true);
}


/*! Called when the disconnect button is clicked.
	Tries to connect to DNS server and gives an error message if this fails. */
void ISpikeWidget::unsetIPAddressButtonClicked(){
	ipAddressSet = false;

	connectButton->setEnabled(true);
	dnsEdit->setEnabled(true);
	portEdit->setEnabled(true);
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







