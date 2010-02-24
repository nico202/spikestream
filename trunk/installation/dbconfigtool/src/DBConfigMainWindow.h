#ifndef DBCONFIGMAINWINDOW_H
#define DBCONFIGMAINWINDOW_H

//SpikeStream includes
#include "DBInfo.h"
#include "SuccessWidget.h"

//Qt includes
#include <QMainWindow>
#include <QWidget>
#include <QStackedWidget>

namespace spikestream {

	class DBConfigMainWindow : public QMainWindow{
		Q_OBJECT

		public:
			DBConfigMainWindow(QWidget* parent=0);
			~DBConfigMainWindow();


		private slots:
			void closeApplication();
			void configureDatabases(const DBInfo& networkDBInfo, const DBInfo& archiveDBInfo, const DBInfo& analysisDBInfo);


		private:
			//=========================  VARIABLES  =========================
			/*! Root directory of the SpikeStream installation */
			QString rootDirectory;

			/*! Layered pane */
			QStackedWidget* stackedWidget;

			/*! Widget displaying results of the configuration */
			SuccessWidget* successWidget;

			//==========================  METHODS  ==========================
			bool addAnalysisDatabases(const DBInfo& analysisDBInfo);
			bool addArchiveDatabases(const DBInfo& archiveDBInfo);
			bool addNetworkDatabases(const DBInfo& networkDBInfo);
			void writeDBInfoToConfigFile(bool writeNetDB, const DBInfo& networkDBInfo, bool writeArchDB,  const DBInfo& archiveDBInfo, bool writeAnaDB, const DBInfo& analysisDBInfo);
			bool showConfirmMessage(const QString& msg);
	};
}

#endif//DBCONFIGMAINWINDOW_H
