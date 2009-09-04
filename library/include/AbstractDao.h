#ifndef ABSTRACTDAO_H
#define ABSTRACTDAO_H

//SpikeStream includes
#include "DBInfo.h"
using namespace spikestream;

//Qt includes
#include <QThread>
#include <QtSql>


class AbstractDao {
    public:
	AbstractDao(const DBInfo& dbInfo);
	virtual ~AbstractDao();
	DBInfo getDBInfo();

    protected:
	void checkDatabase();
	void executeQuery(QSqlQuery& query);
	QSqlQuery getQuery();

    private:
	//=========================  VARIABLES  ============================
	/*! In QSql library can only use connections within the thread that created them.
	    So record id of thread and throw exception if different id calls functions. */
	QThread* dbThread;

	/*! Parameters of the database connection */
	DBInfo dbInfo;

	/*! Unique name of the database */
	QString dbName;



	/*! Static counter that is used to assign a unique id to each QSql database */
	static unsigned int dbCounter;


	//=========================  METHODS  ===============================
	static QString getUniqueDBName();

};


#endif//ABSTRACTDAO_H

