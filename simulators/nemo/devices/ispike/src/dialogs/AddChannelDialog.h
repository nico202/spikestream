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
#include <vector>
using namespace std;

namespace spikestream {

	/*! Dialog that enables user to configure channel linking iSpike with
		a neuron group in SpikeStream */
	class AddChannelDialog : public QDialog {
		Q_OBJECT

		public:
			AddChannelDialog(QWidget* parent = 0);
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
			/*! Holds information about the available channel types */
			QComboBox* channelTypeCombo;

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

			NeuronGroup* neuronGroup;

			bool inputChannel;

			/*! Creates output channels */
			OutputChannelFactory* outputFactory;

			/*! Creates input channels */
			InputChannelFactory* inputFactory;

			vector<InputChannelDescription> inChanDesc ;

			vector<OutputChannelDescription> outChanDesc;

			vector<WriterDescription> writerDescVector;

			vector<ReaderDescription> readerDescVector ;

			InputChannel* newInputChannel;

			OutputChannel* newOutputChannel;


			//======================  METHODS  =======================
			void reset();

	};

}

#endif//ADDCHANNELDIALOG_H
