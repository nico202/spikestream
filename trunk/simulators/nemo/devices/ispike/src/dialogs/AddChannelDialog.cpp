//SpikeStream includes
#include "AddChannelDialog.h"
#include "Globals.h"
#include "Network.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//iSpike includes
#include "iSpike/Property.hpp"
#include "iSpike/Reader/ReaderFactory.hpp"
#include "iSpike/Writer/WriterFactory.hpp"


//Qt includes
#include <QLayout>

//Other includes
#include <map>
using namespace std;


/*! Constructor */
AddChannelDialog::AddChannelDialog(QWidget *parent) : QDialog(parent){
	//Create layout for the entire widget
	QVBoxLayout* mainVBox = new QVBoxLayout(this);

	//Channel type configuration
	QHBoxLayout* chanTypeBox = new QHBoxLayout();
	chanTypeBox->addWidget(new QLabel("Channel type: "));
	channelTypeCombo = new QComboBox();
	channelTypeCombo->addItem("Input");
	channelTypeCombo->addItem("Output");
	chanTypeBox->addWidget(channelTypeCombo);
	QPushButton* selectChannelTypeButton = new QPushButton("Select");
	selectChannelTypeButton->setMaximumSize(60, 20);
	connect(selectChannelTypeButton, SIGNAL(clicked()), this, SLOT(selectChannelType()));
	chanTypeBox->addWidget(selectChannelTypeButton);
	mainVBox->addLayout(chanTypeBox);

	//Channel configuration
	QHBoxLayout* chanBox = new QHBoxLayout();
	channelLabel = new QLabel("Channel: ");

	chanBox->addWidget(channelLabel);
	channelCombo = new QComboBox();
	chanBox->addWidget(channelCombo);
	configureChannelButton = new QPushButton("Configure");
	configureChannelButton->setMaximumSize(60, 20);
	connect(configureChannelButton, SIGNAL(clicked()), this, SLOT(configureChannel()));
	chanBox->addWidget(configureChannelButton);
	mainVBox->addLayout(chanBox);

	//Reader/writer configuration
	QHBoxLayout* readerWriterBox = new QHBoxLayout();
	readerWriterLabel = new QLabel("Reader/writer: ");
	readerWriterBox->addWidget(readerWriterLabel);
	readerWriterCombo = new QComboBox();
	readerWriterBox->addWidget(readerWriterCombo);
	configureReaderWriterButton = new QPushButton("Configure");
	configureReaderWriterButton->setMaximumSize(60, 20);
	connect(configureReaderWriterButton, SIGNAL(clicked()), this, SLOT(configureReaderWriter()));
	readerWriterBox->addWidget(configureReaderWriterButton);
	mainVBox->addLayout(readerWriterBox);

	//Ok/reset/cancel
	QHBoxLayout* okCancelBox = new QHBoxLayout();
	okCancelBox->addStretch(2);
	okButton = new QPushButton("Ok");
	connect(okButton, SIGNAL(clicked()), this, SLOT(okButtonClicked()));
	okCancelBox->addWidget(okButton);
	QPushButton* resetButton = new QPushButton("Reset");
	connect(resetButton, SIGNAL(clicked()), this, SLOT(resetButtonClicked()));
	okCancelBox->addWidget(resetButton);
	QPushButton* cancelButton = new QPushButton("Cancel");
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelButtonClicked()));
	okCancelBox->addWidget(cancelButton);
	okCancelBox->addStretch(2);
	mainVBox->addStretch(5);
	mainVBox->addLayout(okCancelBox);

	//iSpike classes
	outputFactory = new OutputChannelFactory();
	inputFactory = new InputChannelFactory();

	//Finish off
	reset();//Set default visibility
	this->setMinimumSize(500, 300);
}


/*! Destructor */
AddChannelDialog::~AddChannelDialog(){

}


/*----------------------------------------------------------*/
/*------                 PRIVATE SLOTS                ------*/
/*----------------------------------------------------------*/

/*! Cancels add channel and closes dialog */
void AddChannelDialog::cancelButtonClicked(){
	this->reject();
}


/*! Configures the channel */
void AddChannelDialog::configureChannel(){
	readerWriterCombo->clear();
	if(channelTypeCombo->currentText() == "Input"){
		if(channelCombo->currentIndex() >= inChanDesc.size())
			throw SpikeStreamException("Index out of range");
		map<string, Property*> propertyMap = inChanDesc[channelCombo->currentIndex()].getChannelProperties();

		//EDIT IF REQUIRED
		/* One property will be called 'width' and another 'height'
		   Use this to connect to neuron group appropriately */

		string readerType = inChanDesc[channelCombo->currentIndex()].getReaderType();
		ReaderFactory readerFactory;
		readerDescVector = readerFactory.getReadersOfType(readerType);
		for(int i=0; i< readerDescVector.size(); ++i){
			readerWriterCombo->addItem(QString(readerDescVector[i].getReaderName().data()));
		}

	}
	else if(channelTypeCombo->currentText() == "Output"){
		if(channelCombo->currentIndex() >= outChanDesc.size())
			throw SpikeStreamException("Index out of range");
		map<string, Property*> propertyMap = outChanDesc[channelCombo->currentIndex()].getChannelProperties();

		//EDIT IF REQUIRED
		/* One property will be called 'width' and another 'height'
		   Use this to connect to neuron group appropriately */

		string writerType = outChanDesc[channelCombo->currentIndex()].getWriterType();
		WriterFactory writerFactory;
		writerDescVector = writerFactory.getWritersOfType(writerType);
		for(int i=0; i< writerDescVector.size(); ++i){
			readerWriterCombo->addItem(QString(writerDescVector[i].getWriterName().data()));
		}

	}
	else
		throw SpikeStreamException("Channel type not recognized.");

	//FIXME SELECT NEURON GROUP
	QList<NeuronGroup*> neuronGroupList = Globals::getNetwork()->getNeuronGroups();
	if(neuronGroupList.isEmpty())
		throw SpikeStreamException("NO Neuorn tuops");
	neuronGroup = neuronGroupList.at(0);

	readerWriterLabel->setVisible(true);
	readerWriterCombo->setVisible(true);
	configureReaderWriterButton->setVisible(true);
}


/*! Configures the reader/writer associated with a channel */
void AddChannelDialog::configureReaderWriter(){
	if(channelTypeCombo->currentText() == "Input"){
		//Edit properties
		if(readerWriterCombo->currentIndex() >= readerDescVector.size())
			throw SpikeStreamException("Index out of range");
		map<string, Property*> propertyMap = readerDescVector[readerWriterCombo->currentIndex()].getReaderProperties();

		//EDIT IF REQUIRED
	}
	else if(channelTypeCombo->currentText() == "Output"){
		//Edit properties
		if(readerWriterCombo->currentIndex() >= writerDescVector.size())
			throw SpikeStreamException("Index out of range");
		map<string, Property*> propertyMap = writerDescVector[readerWriterCombo->currentIndex()].getWriterProperties();

		//EDIT IF REQUIRED

	}
	else
		throw SpikeStreamException("Channel type not recognized.");

	okButton->setEnabled(true);
}


/*! Adds channel and closes dialog */
void AddChannelDialog::okButtonClicked(){
	if(inputChannel){
		//Create the reader
		ReaderFactory readerFactory;
		ReaderDescription& tmpReaderDesc = readerDescVector[readerWriterCombo->currentIndex()];
		Reader* newReader = readerFactory.create(tmpReaderDesc.getReaderName(), tmpReaderDesc.getReaderProperties());

		//Create channel
		InputChannelDescription& tmpChanDesc = inChanDesc[channelCombo->currentIndex()];
		newInputChannel = inputFactory->create(tmpChanDesc.getChannelName(), newReader, tmpChanDesc.getChannelProperties());
	}
	else {
		//Create the writer
		WriterFactory writerFactory;
		WriterDescription& tmpWriterDesc = writerDescVector[readerWriterCombo->currentIndex()];
		Writer* newWriter = writerFactory.create(tmpWriterDesc.getWriterName(), tmpWriterDesc.getWriterProperties());

		//Create channel
		OutputChannelDescription& tmpChanDesc = outChanDesc[channelCombo->currentIndex()];
		newOutputChannel = outputFactory->create(tmpChanDesc.getChannelName(), newWriter, tmpChanDesc.getChannelProperties());
	}

	this->accept();
}


/*! Resets dialog to initial state. */
void AddChannelDialog::resetButtonClicked(){
	reset();
}


/*! Selects the type of channel */
void AddChannelDialog::selectChannelType(){
	channelCombo->clear();
	if(channelTypeCombo->currentText() == "Input"){
		inputChannel = true;
		//Get the input channels
		inChanDesc = inputFactory->getAllChannels();
		for(int i=0; i<inChanDesc.size(); ++i){
			channelCombo->addItem(QString(inChanDesc[i].getChannelName().data()));
		}
	}
	else if(channelTypeCombo->currentText() == "Output"){
		inputChannel = false;
		//Get the output channels
		outChanDesc = outputFactory->getAllChannels();
		for(int i=0; i<outChanDesc.size(); ++i){
			channelCombo->addItem(QString(outChanDesc[i].getChannelName().data()));
		}
	}
	else
		throw SpikeStreamException("Channel type not recognized.");

	channelLabel->setVisible(true);
	channelCombo->setVisible(true);
	configureChannelButton->setVisible(true);
}


void AddChannelDialog::reset(){
	channelLabel->setVisible(false);
	channelCombo->setVisible(false);
	configureChannelButton->setVisible(false);

	readerWriterLabel->setVisible(false);
	readerWriterCombo->setVisible(false);
	configureReaderWriterButton->setVisible(false);

	okButton->setEnabled(false);
}

