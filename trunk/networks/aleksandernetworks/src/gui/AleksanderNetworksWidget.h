#ifndef ALEKSANDERNETWORKSWIDGET_H
#define ALEKSANDERNETWORKSWIDGET_H

//SpikeStream includes
#include "AleksanderNetworksBuilder.h"
using namespace spikestream;

//Qt includes
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QWidget>

namespace spikestream {

    class AleksanderNetworksWidget : public QWidget {
	Q_OBJECT

	public:
	    AleksanderNetworksWidget();
	    ~AleksanderNetworksWidget();

	private slots:
	    void addNetwork();

	private:
	    //======================  VARIABLES  =======================
	    /*! Class that adds the networks */
	    AleksanderNetworksBuilder* networkBuilder;

	    /*! Holds the network description */
	    QLineEdit* networkName;

	    //======================  METHODS  =========================
	    QPushButton* addNetworkButton(QGridLayout* gridLayout, const QString& description);
	    QString getNetworkName();
    };

}

#endif//ALEKSANDERNETWORKSWIDGET_H

