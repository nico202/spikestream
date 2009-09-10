#ifndef TESTDAO_H
#define TESTDAO_H

//SpikeStream includes
#include "DBInfo.h"
#include "NetworkDaoThread.h"
using namespace spikestream;

//Qt includes
#include <QTest>
#include <QtSql>

/*! Contains general methods that are used for testing all data access objects (DAOs). */
class TestDao : public QObject {
    Q_OBJECT


    protected:
	//=========================  VARIABLES  =========================
	/*! Information about the database used in the tests */
	DBInfo dbInfo;

	/*! Database connection */
	QSqlDatabase database;

	/*! Name to statically access database via QSqlDatabase methods */
	QString dbRefName;

	//==========================  METHODS  ==========================
	void connectToDatabase(const QString& dbName);
	void cleanTestDatabases();
	void closeDatabase();
	void executeQuery(QSqlQuery& query);
	void executeQuery(const QString& queryStr);
	QSqlQuery getQuery();
	QSqlQuery getQuery(const QString& queryStr);

};

#endif//TESTDAO_H


