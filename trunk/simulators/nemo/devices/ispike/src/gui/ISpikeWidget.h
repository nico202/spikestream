#ifndef ISPIKEWIDGET_H
#define ISPIKEWIDGET_H

//SpikeStream includes
#include "AbstractDeviceWidget.h"
#include "ChannelModel.h"
#include "ISpikeManager.h"

//Qt includes
#include <QComboBox>
#include <QPushButton>
#include <QWidget>

namespace spikestream {

	/*! Interface used to set up communication between iSpike library and
		NeMo. */
	class ISpikeWidget : public AbstractDeviceWidget {
		Q_OBJECT

		public:
			ISpikeWidget(QWidget* parent = 0);
			~ISpikeWidget();
			AbstractDeviceManager* getDeviceManager();

		private slots:
			void addChannel();
			void channelComboChanged(QString channelName);

		private:
			//====================  VARIABLES  ======================
			/*! Manager that interfaces with iSpike library */
			ISpikeManager* iSpikeManager;

			/*! Combo box holding list of available channels from iSpike */
			QComboBox* channelCombo;

			/*! Combo box holding neuron groups that are suitable for this channel */
			QComboBox* neuronGroupCombo;

			/*! Button to add channels */
			QPushButton* addChannelButton;

			/*! Model linked to ISpikeManager, which displays current connected channels */
			ChannelModel* channelModel;


			//====================  METHODS  ========================
			void fillChannelCombo();
			void fillNeuronGroupCombo(QString channelName);

	};
}

#endif//ISPIKEWIDGET_H
