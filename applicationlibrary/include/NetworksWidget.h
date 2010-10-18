#ifndef NETWORKSWIDGET_H
#define NETWORKSWIDGET_H

//SpikeStream includes
#include "NetworkDaoThread.h"
#include "Network.h"
using namespace spikestream;

//Qt includes
#include <QWidget>
#include <QLayout>
#include <QHash>
#include <QProgressDialog>
#include <QTimer>

namespace spikestream {

	/*! Displays a list of widgets with buttons to add and delete networks. */
    class NetworksWidget : public QWidget {
		Q_OBJECT

		public:
			NetworksWidget(QWidget *parent=0);
			~NetworksWidget();

		signals:
			void networkChanged();

		private slots:
			void addNetworks();
			void addNewNetwork();
			void checkLoadingProgress();
			void deleteNetwork();
			void loadNetwork();
			void loadNetworkList();
			void networkDaoThreadFinished();
			void networkSaveFinished();
			void prototypeNetwork();
			void saveNetwork();
			void setNetworkProperties();

		private:
			//======================  VARIABLES  =====================
			/*! Keep the current list of networks in memory */
			QHash<unsigned int, NetworkInfo> networkInfoMap;

			/*! Layout used for organising widget. Need a reference to enable
			networks to be dynamically reloaded */
			QGridLayout* gridLayout;

			/*! Timer to keep track of the loading of the network. */
			QTimer* loadingTimer;

			/*! Dialog providing feedback about the loading of the network */
			QProgressDialog* progressDialog;

			/*! The network that is being loaded */
			Network* newNetwork;

			/** Holds widgets that were set to be deleted during earlier event cycle */
			QList<QWidget*> cleanUpList;

			/*! Database Dao thread for heavy operations */
			NetworkDaoThread* networkDaoThread;

			/*! ID of network being deleted */
			unsigned deleteNetworkID;

			/*! Columns for each type of data. */
			static const int propCol = 0;
			static const int idCol = 1;
			static const int nameCol = 2;
			static const int descCol = 3;
			static const int spacer1Col = 4;
			static const int loadCol = 5;
			static const int protoCol = 6;
			static const int delCol = 7;
			static const int saveCol = 8;

			static const int numCols = 9;

			//=======================  METHODS  ======================
			void loadNetwork(NetworkInfo& netInfo, bool prototypeMode = false);
			void reset();
    };

}

#endif//NETWORKSWIDGET_H
