#ifndef DATABASEDAO_H
#define NETWORKDAO_H

//SpikeStream includes
#include "AbstractDao.h"
using namespace spikestream;

//Qt includes
#include <QList>
#include <QString>

namespace spikestream{

	/*! Class for carrying out tasks that are not necessarily specific to a single database. */
	class DatabaseDao : public AbstractDao {
		public:
			DatabaseDao(const DBInfo& dbInfo);
			DatabaseDao();
			virtual ~DatabaseDao();
			void connectToDatabase(const DBInfo& dbInfo);
			using AbstractDao::connectToDatabase;
			void executeSQLFile(const QString& fileName);
			void executeSQLFile(const QString& dbName, const QString& fileName);
			QList<QString> getDatabaseNames();


		private:
			//==========================  VARIABLES  ==========================

			//===========================  METHODS  ===========================
			void loadFileIntoString(QFile& file, QString& fileString);

	};

}

#endif//NETWORKDAO_H


