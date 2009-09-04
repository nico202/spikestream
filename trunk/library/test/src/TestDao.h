#ifndef TESTDAO_H
#define TESTDAO_H

//SpikeStream includes
#include "DBInfo.h"
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

	//==========================  METHODS  ==========================
	void connectToDatabase(const QString& dbName);




};

#endif//TESTDAO_H


