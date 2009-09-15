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

	//Parameters for test networks
	unsigned int testNetID;
	unsigned int neurGrp1ID;
	unsigned int neurGrp2ID;
	unsigned int connGrp1ID;
	QList<unsigned int> testConnIDList;
	QList<unsigned int> testNeurIDList;


	//==========================  METHODS  ==========================
	void addTestNetwork1();
	unsigned int addTestConnection(unsigned int connGrpID, unsigned int fromID, unsigned int toID, float weight, float delay);
	unsigned int addTestNeuron(unsigned int neurGrpID, float x, float y, float z);
	QString getConnectionParameterXML();
	QString getNeuronParameterXML();

	void connectToDatabase(const QString& dbName);
	void cleanTestDatabases();
	void closeDatabase();
	void executeQuery(QSqlQuery& query);
	void executeQuery(const QString& queryStr);
	QSqlQuery getQuery();
	QSqlQuery getQuery(const QString& queryStr);
	void resetTestNetwork();

};

#endif//TESTDAO_H


