//SpikeStream includes
#include "ChannelTableView.h"
#include "Globals.h"
#include "ISpikeWidget.h"
using namespace spikestream;

//Qt includes
#include <QLayout>
#include <QLabel>


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
	fillNeuronGroupCombo(channelCombo->currentText());

	//Create iSpike manager
	iSpikeManager = new ISpikeManager();

	//Model and view to display active channels
	channelModel = new ChannelModel(iSpikeManager);
	ChannelTableView* channelView = new ChannelTableView(channelModel);
	mainVBox->addWidget(channelView);

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


/*----------------------------------------------------------*/
/*------                PRIVATE METHODS               ------*/
/*----------------------------------------------------------*/

/*! Fills the channel combo with a list of available channels from the iSpike library */
void ISpikeWidget::fillChannelCombo(){

}


/*! Fills the neuron group combo with a list of neuron groups that match
	the currently selected channel type. */
void ISpikeWidget::fillNeuronGroupCombo(QString channelName){
	if(!Globals::networkLoaded()){
		qCritical()<<"Cannot initialize ISpikeWidget without a loaded network";
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







