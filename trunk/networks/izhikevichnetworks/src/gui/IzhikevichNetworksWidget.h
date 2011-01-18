#ifndef IZHIKEVICHNETWORKSWIDGET_H
#define IZHIKEVICHNETWORKSWIDGET_H

//SpikeStream includes
#include "IzhikevichNetworkBuilder.h"
#include "Network.h"
using namespace spikestream;

//Qt includes
#include <QLayout>
#include <QLineEdit>
#include <QProgressDialog>
#include <QPushButton>
#include <QWidget>

namespace spikestream {

	/*! Adds prototype Izhikevich network to SpikeStream */
	class IzhikevichNetworksWidget : public QWidget {
		Q_OBJECT

	public:
		IzhikevichNetworksWidget();
		~IzhikevichNetworksWidget();

	private slots:
	    void addNetwork();
		void networkBuilderFinished();
		void updateProgress(int stepsCompleted, int totalSteps, QString message);

	private:
	    //======================  VARIABLES  =======================
	    /*! Class that adds the networks */
		IzhikevichNetworkBuilder* networkBuilder;

	    /*! Holds the network description */
		QLineEdit* networkNameEdit;

		/*! Network that is being built. */
		Network* newNetwork;

		/*! Dialog for showing progress */
		QProgressDialog* progressDialog;

		/*! Avoids multiple calls to progress dialog whilst it is redrawing */
		bool progressUpdating;

	    //======================  METHODS  =========================
	    QPushButton* addNetworkButton(QGridLayout* gridLayout, const QString& description);
	    QString getNetworkName();
    };

}

#endif//IZHIKEVICHNETWORKSWIDGET_H

