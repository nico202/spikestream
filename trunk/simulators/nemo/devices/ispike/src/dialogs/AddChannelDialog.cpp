//SpikeStream includes
#include "AddChannelDialog.h"
#include "EditPropertiesDialog.h"
#include "Globals.h"
#include "Network.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;

//iSpike includes
#include "iSpike/Property.hpp"
#include "iSpike/Reader/ReaderFactory.hpp"
#include "iSpike/Writer/WriterFactory.hpp"
#include "iSpike/ISpikeException.hpp"
using namespace ispike;

//Qt includes
#include <QLayout>

//Other includes
#include <exception>
using namespace std;


/*! Constructor */
AddChannelDialog::AddChannelDialog(QWidget *parent) : QDialog(parent){
	//Default variables
	ipAddressSet = false;
	inputFactory = NULL;
	outputFactory = NULL;
	readerFactory = NULL;
	writerFactory = NULL;

	//Construct user interface
	buildGUI();
}


/*! Constructor using DNS server and port */
AddChannelDialog::AddChannelDialog(QString ipString, QString portString, QWidget *parent) : QDialog(parent){
	//Store variables
	ipAddressSet = true;
	dnsIPAddress = ipString;
	dnsPort = portString;

	//Check variables
	if(dnsIPAddress.isEmpty() || dnsPort.isEmpty())
		throw SpikeStreamException("IP Address or Port are not specified. These are essential for adding Ethernet enabled channels.");

	//Construct user interface
	buildGUI();
}


/*! Destructor */
AddChannelDialog::~AddChannelDialog(){
	if(inputFactory != NULL)
		delete inputFactory;
	if(outputFactory != NULL)
		delete outputFactory;
	if(readerFactory != NULL)
		delete readerFactory;
	if(writerFactory != NULL)
		delete writerFactory;
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

	try{
		//Create reader and writer factories
		if(ipAddressSet){
			readerFactory = new ReaderFactory(dnsIPAddress.toStdString(), Util::getUInt(dnsPort));
			writerFactory = new WriterFactory(dnsIPAddress.toStdString(), Util::getUInt(dnsPort));
		}
		else{
			readerFactory = new ReaderFactory();
			writerFactory = new WriterFactory();
		}

		//Load lists of available readers or writers
		if(inputChannel){
			if(channelCombo->currentIndex() >= (int)inChanDesc.size())
				throw SpikeStreamException("Index out of range");

			//Edit properties of channel - abort channel configuration if operation is cancelled
			bool propsConfigured = configureProperties(inputFactory->getDefaultProperties(inChanDesc[channelCombo->currentIndex()]), channelProperties, true);
			if(!propsConfigured)
				return;

			//Load list of available readers
			string readerType = inChanDesc[channelCombo->currentIndex()].getType();
			readerDescVector = readerFactory->getReadersOfType(readerType);
			for(size_t i=0; i< readerDescVector.size(); ++i){
				readerWriterCombo->addItem(QString(readerDescVector[i].getName().data()));
			}
		}
		else{
			if(channelCombo->currentIndex() >= (int)outChanDesc.size())
				throw SpikeStreamException("Index out of range");

			//Edit properties of channel - abort channel configuration if operation is cancelled
			bool propsConfigured = configureProperties(outputFactory->getDefaultProperties(outChanDesc[channelCombo->currentIndex()]), channelProperties, true);
			if(!propsConfigured)
				return;

			//Load list of available writers
			string writerType = outChanDesc[channelCombo->currentIndex()].getType();
			writerDescVector = writerFactory->getWritersOfType(writerType);
			for(size_t i=0; i< writerDescVector.size(); ++i){
				readerWriterCombo->addItem(QString(writerDescVector[i].getName().data()));
			}
		}
	}
	catch(SpikeStreamException& ex){
		qCritical()<<"SpikeStreamException thrown configuring channel: "<<ex.getMessage();
	}
	catch(ISpikeException& ex){
		qCritical()<<"ISpikeException thrown configuring channel: "<<ex.what();
	}
	catch(...){
		qCritical()<<"AddChannelDialog: An unknown exception occurred configuring channel.";
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
	try{
		if(inputChannel){
			if(readerWriterCombo->currentIndex() >= (int)readerDescVector.size())
				throw SpikeStreamException("Index out of range");

			//Edit properties of reader - abort configuration if operation is cancelled
			bool propsConfigured = configureProperties(readerFactory->getDefaultProperties(readerDescVector[readerWriterCombo->currentIndex()]), readerWriterProperties);
			if(!propsConfigured)
				return;
		}
		else{
			if(readerWriterCombo->currentIndex() >= (int)writerDescVector.size())
				throw SpikeStreamException("Index out of range");

			//Edit properties of writer - abort configuration if operation is cancelled
			bool propsConfigured = configureProperties(writerFactory->getDefaultProperties(writerDescVector[readerWriterCombo->currentIndex()]), readerWriterProperties);
			if(!propsConfigured)
				return;
		}
	}
	catch(SpikeStreamException& ex){
		qCritical()<<"SpikeStreamException thrown configuring reader or writer: "<<ex.getMessage();
	}
	catch(ISpikeException& ex){
		qCritical()<<"ISpikeException thrown configuring reader or writer: "<<ex.what();
	}
	catch(...){
		qCritical()<<"AddChannelDialog: An unknown exception occurred configuring reader or writer.";
	}

	readerWriterCombo->setEnabled(false);
	configureReaderWriterButton->setEnabled(false);
	okButton->setEnabled(true);
}


/*! Adds channel and closes dialog */
void AddChannelDialog::okButtonClicked(){
	try{
		if(inputChannel){
			//Create the reader and input channel
			Reader* newReader = readerFactory->create(readerDescVector[readerWriterCombo->currentIndex()], readerWriterProperties);
			newInputChannel = inputFactory->create(inChanDesc[channelCombo->currentIndex()], newReader, channelProperties);
		}
		else {
			//Create the writer and output channel
			Writer* newWriter = writerFactory->create(writerDescVector[readerWriterCombo->currentIndex()], readerWriterProperties);
			newOutputChannel = outputFactory->create(outChanDesc[channelCombo->currentIndex()], newWriter, channelProperties);
		}
		this->accept();
	}
	catch(SpikeStreamException& ex){
		qCritical()<<"SpikeStreamException thrown creating channel and reader/writer: "<<ex.getMessage();
	}
	catch(ISpikeException& ex){
		qCritical()<<"ISpikeException thrown creating channel and reader/writer: "<<ex.what();
	}
	catch (exception& e) {
		qCritical()<<"STD exception occurred creating channel and reader/writer: "<<e.what();
	}
	catch(...){
		qCritical()<<"AddChannelDialog: An unknown exception occurred creating the channel and reader/writer.";
	}
}


/*! Called when reset button is clicked. Resets dialog to initial state. */
void AddChannelDialog::resetButtonClicked(){
	reset();
}


/*! Selects the type of channel */
void AddChannelDialog::selectChannelType(){
	channelCombo->clear();
	try{
		//Input or output are the two options
		if(channelTypeCombo->currentText() == "Input"){
			inputChannel = true;

			//Get the input channels
			inChanDesc = inputFactory->getAllChannels();
			for(size_t i=0; i<inChanDesc.size(); ++i){
				channelCombo->addItem(QString(inChanDesc[i].getName().data()));
			}
		}
		else if(channelTypeCombo->currentText() == "Output"){
			inputChannel = false;

			//Get the output channels
			outChanDesc = outputFactory->getAllChannels();
			for(size_t i=0; i<outChanDesc.size(); ++i){
				channelCombo->addItem(QString(outChanDesc[i].getName().data()));
			}
		}
		else
			throw SpikeStreamException("Channel type not recognized.");
	}
	catch(SpikeStreamException& ex){
		qCritical()<<"SpikeStreamException thrown selecting channel type: "<<ex.getMessage();
	}
	catch(ISpikeException& ex){
		qCritical()<<"ISpikeException thrown selecting channel type: "<<ex.what();
	}
	catch(...){
		qCritical()<<"AddChannelDialog: An unknown exception occurred selecting the channel type.";
	}

	channelTypeCombo->setEnabled(false);
	selectChannelTypeButton->setEnabled(false);
	channelLabel->setVisible(true);
	channelCombo->setVisible(true);
	configureChannelButton->setVisible(true);
}


/*----------------------------------------------------------*/
/*------                PRIVATE METHODS               ------*/
/*----------------------------------------------------------*/

/*! Constructs the GUI */
void AddChannelDialog::buildGUI(){
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


/*! Launches dialog to configure the source properties, which are copied into the destination property map. */
bool AddChannelDialog::configureProperties(map<string, Property> srcPropertyMap, map<string, Property>& destPropertyMap, bool selectNeuronGroup){
	try{
		if(selectNeuronGroup){
			EditPropertiesDialog* tmpDlg = new EditPropertiesDialog(srcPropertyMap, false, Globals::getNetwork()->getNeuronGroups());
			if(tmpDlg->exec() == QDialog::Accepted){
				neuronGroup = tmpDlg->getNeuronGroup();
				if(neuronGroup == NULL){
					qCritical()<<"Neuron group has not been set.";
					return false;
				}
				destPropertyMap = tmpDlg->getPropertyMap();
				return true;
			}
		}
		else {
			EditPropertiesDialog* tmpDlg = new EditPropertiesDialog(srcPropertyMap, false);
			if(tmpDlg->exec() == QDialog::Accepted){
				destPropertyMap = tmpDlg->getPropertyMap();
				return true;
			}
		}
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
	catch(ISpikeException& ex){
		qCritical()<<"ISpikeException thrown configuring properties: "<<ex.what();
	}
	catch (exception& e) {
		qCritical()<<"Exception occurred configuring properties: "<<e.what();
	}
	catch(...){
		qCritical()<<"An unknown exception occurred configuring properties.";
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

