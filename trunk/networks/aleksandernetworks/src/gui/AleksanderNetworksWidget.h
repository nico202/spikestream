#ifndef ALEKSANDERNETWORKSWIDGET_H
#define ALEKSANDERNETWORKSWIDGET_H

//SpikeStream includes
#include "AleksanderNetworksBuilder.h"
using namespace spikestream;

//Qt includes
#include <QWidget>
#include <QLineEdit>

namespace spikestream {

    class AleksanderNetworksWidget : public QWidget {
	Q_OBJECT

	public:
	    AleksanderNetworksWidget();
	    ~AleksanderNetworksWidget();

	private slots:
	    void add4NeuronNetworks1();

	private:
	    //======================  VARIABLES  =======================
	    /*! Class that adds the networks */
	    AleksanderNetworksBuilder* networkBuilder;

	    /*! Holds the network description */
	    QLineEdit* networkName;

	    //======================  METHODS  =========================
    };

}

#endif//ALEKSANDERNETWORKSWIDGET_H

