//SpikeStream includes
#include "AddChannelDialog.h"
#include "EditPropertiesDialog.h"
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
	selectChannelTypeButton = new QPushButton("Select");
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

	if(inputChannel){
		if(channelCombo->currentIndex() >= inChanDesc.size())
			throw SpikeStreamException("Index out of range");

		//Edit properties of channel - abort channel configuration if operation is cancelled
		bool propsConfigured = configureProperties(inChanDesc[channelCombo->currentIndex()].getChannelProperties(), true);
		if(!propsConfigured)
			return;

		//Load list of available readers
		string readerType = inChanDesc[channelCombo->currentIndex()].getReaderType();
		ReaderFactory readerFactory;
		readerDescVector = readerFactory.getReadersOfType(readerType);
		for(int i=0; i< readerDescVector.size(); ++i){
			readerWriterCombo->addItem(QString(readerDescVector[i].getReaderName().data()));
		}

	}
	else{
		if(channelCombo->currentIndex() >= outChanDesc.size())
			throw SpikeStreamException("Index out of range");

		//Edit properties of channel - abort channel configuration if operation is cancelled
		bool propsConfigured = configureProperties(outChanDesc[channelCombo->currentIndex()].getChannelProperties(), true);
		if(!propsConfigured)
			return;

		//Load list of available writers
		string writerType = outChanDesc[channelCombo->currentIndex()].getWriterType();
		WriterFactory writerFactory;
		writerDescVector = writerFactory.getWritersOfType(writerType);
		for(int i=0; i< writerDescVector.size(); ++i){
			readerWriterCombo->addItem(QString(writerDescVector[i].getWriterName().data()));
		}
	}

	//Enable/disable appropriate graphical components
	channelCombo->setEnabled(false);
	configureChannelButton->setEnabled(false);
	readerWriterLabel->setVisible(true);
	readerWriterCombo->setVisible(true);
	configureReaderWriterButton->setVisible(true);
}


/*! Configures the reader/writer associated with a channel */
void AddChannelDialog::configureReaderWriter(){
	if(inputChannel){
		//Edit properties
		if(readerWriterCombo->currentIndex() >= readerDescVector.size())
			throw SpikeStreamException("Index out of range");

		//Edit properties of channel - abort reader configuration if operation is cancelled
		bool propsConfigured = configureProperties(readerDescVector[readerWriterCombo->currentIndex()].getReaderProperties());
		if(!propsConfigured)
			return;
	}
	else{
		//Edit properties
		if(readerWriterCombo->currentIndex() >= writerDescVector.size())
			throw SpikeStreamException("Index out of range");

		//Edit properties of channel - abort reader configuration if operation is cancelled
		bool propsConfigured = configureProperties(writerDescVector[readerWriterCombo->currentIndex()].getWriterProperties());
		if(!propsConfigured)
			return;
	}

	readerWriterCombo->setEnabled(false);
	configureReaderWriterButton->setEnabled(false);
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


/*! Called when reset button is clicked. Resets dialog to initial state. */
void AddChannelDialog::resetButtonClicked(){
	reset();
}


/*! Selects the type of channel */
void AddChannelDialog::selectChannelType(){
	channelCombo->clear();

	//Input or output are the two options
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

	channelTypeCombo->setEnabled(false);
	selectChannelTypeButton->setEnabled(false);
	channelLabel->setVisible(true);
	channelCombo->setVisible(true);
	configureChannelButton->setVisible(true);
}


/*----------------------------------------------------------*/
/*------                PRIVATE METHODS               ------*/
/*----------------------------------------------------------*/

/*! Launches dialog to configure the supplied properties */
bool AddChannelDialog::configureProperties(map<string, Property*> propertyMap, bool selectNeuronGroup){
	try{
		if(selectNeuronGroup){
			EditPropertiesDialog* tmpDlg = new EditPropertiesDialog(propertyMap, Globals::getNetwork()->getNeuronGroups());
			if(tmpDlg->exec() == QDialog::Accepted){
				neuronGroup = tmpDlg->getNeuronGroup();
				if(neuronGroup == NULL){
					qCritical()<<"Neuron group has not been set.";
					return false;
				}
				return true;
			}
		}
		else {
			EditPropertiesDialog* tmpDlg = new EditPropertiesDialog(propertyMap);
			if(tmpDlg->exec() == QDialog::Accepted){
				return true;
			}
		}
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
	catch(...){
		qCritical()<<"An unknown exception occurred configuring a channel";
	}

	//Dialog not accepted or an error occurred
	return false;
}


/*! Resets dialog to original state */
void AddChannelDialog::reset(){
	channelTypeCombo->setEnabled(true);
	selectChannelTypeButton->setEnabled(true);

	channelLabel->setVisible(false);
	channelCombo->setVisible(false);
	channelCombo->setEnabled(true);
	configureChannelButton->setVisible(false);
	configureChannelButton->setEnabled(true);

	readerWriterLabel->setVisible(false);
	readerWriterCombo->setVisible(false);
	readerWriterCombo->setEnabled(true);
	configureReaderWriterButton->setVisible(false);
	configureReaderWriterButton->setEnabled(true);

	okButton->setEnabled(false);
}

