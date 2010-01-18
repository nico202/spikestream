#ifndef TONONITESTWIDGET_H
#define TONONITESTWIDGET_H

//SpikeStream includes
#include "TononiNetworkBuilder.h"
using namespace spikestream;

//Qt includes
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

namespace spikestream {

    class TononiNetworksWidget : public QWidget {
	Q_OBJECT

	public:
	    TononiNetworksWidget();
	    ~TononiNetworksWidget();

	private slots:
	    void addNetwork();

	private:
	    //======================  VARIABLES  =======================
	    /*! Class that adds the networks */
	    TononiNetworkBuilder* networkBuilder;

	    /*! Holds the network description */
	    QLineEdit* networkName;

	    //======================  METHODS  =========================
	    QPushButton* addNetworkButton(QGridLayout* gridLayout, const QString& description);
	    QString getNetworkName();
    };

}

#endif//TONONITESTWIDGET_H

