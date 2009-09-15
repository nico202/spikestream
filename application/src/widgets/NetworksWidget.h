#ifndef NETWORKSWIDGET_H
#define NETWORKSWIDGET_H

//SpikeStream includes
#include "NetworkInfo.h"
using namespace spikestream;

//Qt includes
#include <QWidget>
#include <QLayout>
#include <QHash>

namespace spikestream {

    class NetworksWidget : public QWidget {
	Q_OBJECT

	public:
	    NetworksWidget(QWidget *parent);
	    ~NetworksWidget();

	signals:
	    void networkChanged();

	private slots:
	    void reloadNetworkList();
	    void loadNetwork();

	private:
	    //======================  VARIABLES  =====================
	    /*! Keep the current list of networks in memory */
	    QHash<unsigned int, NetworkInfo> networkInfoMap;

	    /*! Layout used for organising widget. Need a reference to enable
		networks to be dynamically reloaded */
	    QGridLayout* gridLayout;


	    //=======================  METHODS  ======================
	    void loadNetwork(NetworkInfo& netInfo);
	    void reset();
    };

}

#endif//NETWORKSWIDGET_H

