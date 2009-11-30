#ifndef TONONITESTWIDGET_H
#define TONONITESTWIDGET_H

//SpikeStream includes
#include "TononiNetworkBuilder.h"
using namespace spikestream;

//Qt includes
#include <QWidget>
#include <QLineEdit>

namespace spikestream {

    class TononiNetworksWidget : public QWidget {
	Q_OBJECT

	public:
	    TononiNetworksWidget();
	    ~TononiNetworksWidget();

	private slots:
	    void addBalduzziTononiFigure6();

	private:
	    //======================  VARIABLES  =======================
	    /*! Class that adds the networks */
	    TononiNetworkBuilder* networkBuilder;

	    /*! Holds the network description */
	    QLineEdit* networkName;

	    //======================  METHODS  =========================
    };

}

#endif//TONONITESTWIDGET_H

