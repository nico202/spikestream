#ifndef ADDCHANNELDIALOG_H
#define ADDCHANNELDIALOG_H

//SpikeStream includes
#include "NeuronGroup.h"

//iSpike includes
#include "iSpike/Channel/InputChannel/InputChannelFactory.hpp"
#include "iSpike/Channel/OutputChannel/OutputChannelFactory.hpp"
#include "iSpike/Channel/InputChannel/InputChannelDescription.hpp"
#include "iSpike/Channel/OutputChannel/OutputChannelDescription.hpp"
#include "iSpike/Reader/ReaderDescription.hpp"
#include "iSpike/Writer/WriterDescription.hpp"
#include "iSpike/Channel/InputChannel/InputChannel.hpp"
#include "iSpike/Channel/OutputChannel/OutputChannel.hpp"


//Qt includes
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QPushButton>

//Other includes
#include <map>
#include <vector>
using namespace std;

namespace spikestream {

	/*! Dialog that enables user to configure channel linking iSpike with
		a neuron group in SpikeStream */
	class AddChannelDialog : public QDialog {
		Q_OBJECT

		public:
			AddChannelDialog(QWidget* parent = 0);
			AddChannelDialog(QString ipString, QString portString, QWidget* parent = 0);
			~AddChannelDialog();
			bool isInputChannel(){ return inputChannel; }
			InputChannel* getInputChannel() { return newInputChannel; }
			NeuronGroup* getNeuronGroup(){ return neuronGroup; }
			OutputChannel* getOutputChannel() { return newOutputChannel; }

		private slots:
			void cancelButtonClicked();
			void configureChannel();
			void configureReaderWriter();
			void okButtonClicked();
			void resetButtonClicked();
			void selectChannelType();

		private:
			//=====================  VARIABLES  =======================
			/*! String holding IP address of DNS server */
			QString dnsIPAddress;

			/*! String holding port of DNS server */
			QString dnsPort;

			/*! Records whether ip address and port have been set */
			bool ipAddressSet;

			/*! Holds information about the available channel types */
			QComboBox* channelTypeCombo;

			/*! Button to select channel type */
			QPushButton* selectChannelTypeButton;

			/*! Holds information about available channels */
			QComboBox* channelCombo;

			/*! Label for channel combo */
			QLabel* channelLabel;

			/*! Launches dialog to configure the channel */
			QPushButton* configureChannelButton;

			/*! Holds information about the available readers and writers for the channel */
			QComboBox* readerWriterCombo;

			/*! Label for reader / writer combo */
			QLabel* readerWriterLabel;

			/*! Launches dialog to configure reader/writer */
			QPushButton* configureReaderWriterButton;

			/*! Confirms creation of channel and closes dialog */
			QPushButton* okButton;

			/*! Neuron group that will be linked to the channel. */
			NeuronGroup* neuronGroup;

			/*! Records whether channel is input or output */
			bool inputChannel;

			/*! Creates output channels */
			OutputChannelFactory* outputFactory;

			/*! Creates input channels */
			InputChannelFactory* inputFactory;

			/*! Descriptions of the available input channels. */
			vector<InputChannelDescription> inChanDesc ;

			/*! Descriptions of the available output channels. */
			vector<OutputChannelDescription> outChanDesc;

			/*! Descriptions of the available writers. */
			vector<WriterDescription> writerDescVector;

			/*! Descriptions of the available readers */
			vector<ReaderDescription> readerDescVector ;

			/*! Input channel that we are creating. */
			InputChannel* newInputChannel;

			/*! Output channel that we are creating */
			OutputChannel* newOutputChannel;


			//======================  METHODS  =======================
			void buildGUI();
			bool configureProperties(map<string, Property*> propertyMap, bool selectNeuronGroup = false);
			void reset();

	};

}

#endif//ADDCHANNELDIALOG_H
