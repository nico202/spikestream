#ifndef ISPIKEWIDGET_H
#define ISPIKEWIDGET_H

//SpikeStream includes
#include "AbstractDeviceWidget.h"
#include "ChannelModel.h"
#include "ISpikeManager.h"

//Qt includes
#include <QComboBox>
#include <QLineEdit>
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
			void setIPAddressButtonClicked();
			void unsetIPAddressButtonClicked();
			void networkChanged();

		private:
			//====================  VARIABLES  ======================
			/*! Manager that interfaces with iSpike library */
			ISpikeManager* iSpikeManager;

			/*! Button to add channels */
			QPushButton* addChannelButton;

			/*! Model linked to ISpikeManager, which displays current connected channels */
			ChannelModel* channelModel;

			/*! Place to enter the DNS server address */
			QLineEdit* dnsEdit;

			/*! Place to enter the port address */
			QLineEdit* portEdit;

			/*! Does the system try to connect to an IP address */
			bool ipAddressSet;

			/*! Button to connect to DNS server */
			QPushButton* connectButton;

			/*! Button to disconnect from DNS server */
			QPushButton* disconnectButton;


			//====================  METHODS  ========================

	};
}

#endif//ISPIKEWIDGET_H
