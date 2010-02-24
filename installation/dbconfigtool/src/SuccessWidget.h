#ifndef SUCCESSWIDGET_H
#define SUCCESSWIDGET_H

//SpikeStream includes
#include "DBInfo.h"

//Qt includes
#include <QLabel>
#include <QWidget>

namespace spikestream {

	/*! Class that informs uses that database configuration is complete */
	class SuccessWidget : public QWidget {
		Q_OBJECT

		public:
			SuccessWidget(QWidget* parent=0);
			~SuccessWidget();
			void setResults(bool netDBConfigured, bool archDBConfigured, bool anaDBConfigured);


		signals:
			void closed();
			void dbDetailsEntered(const DBInfo&, const DBInfo&, const DBInfo&);


		private slots:
			void okButtonClicked();


		private:
			//=======================  VARIABLES  =======================
			QLabel* networkResultsLabel;
			QLabel* archiveResultsLabel;
			QLabel* analysisResultsLabel;


	};

}

#endif//SUCCESSWIDGET_H
