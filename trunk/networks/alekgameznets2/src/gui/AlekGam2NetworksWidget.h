#ifndef ALEKGAM2NETWORKSWIDGET_H
#define ALEKGAM2NETWORKSWIDGET_H

//SpikeStream includes
#include "NetworksBuilder.h"
using namespace spikestream;

//Qt includes
#include <QLayout>
#include <QLineEdit>
#include <QProgressDialog>
#include <QPushButton>
#include <QString>
#include <QWidget>

namespace spikestream {

	/*! Adds a selection of networks designed to exhibit different levels and areas of information integration
		to enable comparison between information integration algorithms. These networks are fully documented
		elsewhere. */
	class AlekGam2NetworksWidget : public QWidget {
		Q_OBJECT

		public:
			AlekGam2NetworksWidget();
			~AlekGam2NetworksWidget();

		private slots:
			void addNetwork();
			void threadFinished();
			void updateProgress(int stepsCompleted, int totalSteps);

		private:
			//======================  VARIABLES  =======================
			/*! Class that adds the networks */
			NetworksBuilder* networksBuilder;

			/*! Holds the network description */
			QLineEdit* networkName;

			/*! Used to exit heavy operations */
			bool stop;

			/*! Displays feedback about the progress */
			QProgressDialog* progressDialog;

			//======================  METHODS  =========================
			QPushButton* addNetworkButton(QGridLayout* gridLayout, const QString& description);
			QString getNetworkName();
	};

}

#endif//ALEKGAM2NETWORKSWIDGET_H

