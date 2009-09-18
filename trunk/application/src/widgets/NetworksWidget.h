#ifndef NETWORKSWIDGET_H
#define NETWORKSWIDGET_H

//SpikeStream includes
#include "Network.h"
using namespace spikestream;

//Qt includes
#include <QWidget>
#include <QLayout>
#include <QHash>
#include <QProgressDialog>
#include <QTimer>

namespace spikestream {

    class NetworksWidget : public QWidget {
	Q_OBJECT

	public:
	    NetworksWidget(QWidget *parent);
	    ~NetworksWidget();

	signals:
	    void networkChanged();

	private slots:
	    void checkLoadingProgress();
	    void loadNetwork();
	    void reloadNetworkList();

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

	    //=======================  METHODS  ======================
	    void loadNetwork(NetworkInfo& netInfo);
	    void reset();
    };

}

#endif//NETWORKSWIDGET_H

