#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

//SpikeStream includes
#include "DBInfo.h"
#include "SpikeStreamThread.h"

namespace spikestream {

	/*! Handles potentially heavy database tasks, such as deleting all databases */
	class DatabaseManager : public SpikeStreamThread {
		Q_OBJECT

		public:
			DatabaseManager(const DBInfo& networkDBInfo, const DBInfo& archiveDBInfo, const DBInfo& analysisDBInfo);
			~DatabaseManager();
			void startClearDatabases();
			void run();
			int getTaskID() { return taskID; }

			/*! Task of clearing all databases */
			static const int CLEAR_DATABASES_TASK = 1;

		private:
			//=========================  VARIABLES  =========================
			/*! ID of the task that is currently running */
			int taskID;

			/*! Information about network database */
			DBInfo networkDBInfo;

			/*! Information about archive database */
			DBInfo archiveDBInfo;

			/*! Information about analysis database */
			DBInfo analysisDBInfo;


			//==========================  METHODS  ==========================
	};

}

#endif//DATABASEMANAGER_H
