#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

//SpikeStream includes
#include "DBInfo.h"
#include "NetworkDao.h"
#include "ArchiveDao.h"
#include "AnalysisDao.h"

//Qt includes
#include <QThread>

namespace spikestream {

	/*! Handles potentially heavy database tasks, such as deleting all databases */
	class DatabaseManager : public QThread {
		Q_OBJECT
		public:
			DatabaseManager(const DBInfo& networkDBInfo, const DBInfo& archiveDBInfo, const DBInfo& analysisDBInfo);
			~DatabaseManager();
			void prepareClearDatabases();
			void run();
			bool isError() { return error; }
			QString getErrorMessage() { return errorMessage; }
			int getTaskID() { return taskID; }

			/*! Task of clearing all databases */
			static const int CLEAR_DATABASES_TASK = 1;

		private:
			//=========================  VARIABLES  =========================
			/*! ID of the task that is currently running */
			int taskID;

			/*! Records if there has been an error when running as a thread */
			bool error;

			/*! Message associated with an error */
			QString errorMessage;

			NetworkDao* networkDao;
			ArchiveDao* archiveDao;
			AnalysisDao* analysisDao;

			DBInfo networkDBInfo;
			DBInfo archiveDBInfo;
			DBInfo analysisDBInfo;


			//==========================  METHODS  ==========================
			void clearError();
			void cleanUp();
			void setError(const QString& errMsg);
	};

}

#endif//DATABASEMANAGER_H
