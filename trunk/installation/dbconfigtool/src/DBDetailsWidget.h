#ifndef DBDETAILSWIDGET_H
#define DBDETAILSWIDGET_H

//SpikeStream includes
#include "DBInfo.h"

//Qt includes
#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>


namespace spikestream {

	/*! Class that enables user to enter the host, username and password of the
		databases that she wants to use for SpikeStream */
	class DBDetailsWidget : public QWidget {
		Q_OBJECT

		public:
			DBDetailsWidget(QWidget* parent=0);
			~DBDetailsWidget();


		signals:
			void cancel();
			void dbInformationEntered(const DBInfo&, const DBInfo&, const DBInfo&);


		private slots:
			void cancelButtonPressed();
			void enableAnalysisDBConfiguration(int state);
			void enableArchiveDBConfiguration(int state);
			void enableNetworkDBConfiguration(int state);
			void getDBInformation();


		private:
			//=========================  VARIABLES  =========================
			QCheckBox* netChkBox;
			QCheckBox* archChkBox;
			QCheckBox* anaChkBox;

			QLineEdit* networkHost;
			QLineEdit* networkUsername;
			QLineEdit* networkPassword;

			QLineEdit* archiveHost;
			QLineEdit* archiveUsername;
			QLineEdit* archivePassword;

			QLineEdit* analysisHost;
			QLineEdit* analysisUsername;
			QLineEdit* analysisPassword;

			//==========================  METHODS  ===========================
			QString getDBInfoString(QLineEdit* lineEdit);
	};

}

#endif//DBDETAILSWIDGET_H
