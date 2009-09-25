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
	/*! Information about the SpikeStreamNetwork database used in the tests */
	DBInfo dbInfo;

	/*! SpikeStreamNetwork database connection */
	QSqlDatabase database;

	/*! Name to statically access SpikeStreamNetwork database via QSqlDatabase methods */
	QString dbRefName;

	/*! Information about the SpikeStreamArchive database used in the tests */
	DBInfo archiveDBInfo;

	/*! SpikeStreamArchive database connection */
	QSqlDatabase archiveDatabase;

	/*! Name to statically access SpikeStreamArchive database via QSqlDatabase methods */
	QString archiveDBRefName;


	//Parameters for test networks
	unsigned int testNetID;
	unsigned int neurGrp1ID;
	unsigned int neurGrp2ID;
	unsigned int connGrp1ID;
	QList<unsigned int> testConnIDList;
	QList<unsigned int> testNeurIDList;

	//Variables storing information about test archives
	unsigned int testArchive1ID;
	unsigned int testArchive2ID;


	//==========================  METHODS  ==========================
	void addTestArchive1();
	void addTestArchive2();
	void addTestNetwork1();
	unsigned int addTestConnection(unsigned int connGrpID, unsigned int fromID, unsigned int toID, float weight, float delay);
	unsigned int addTestNeuron(unsigned int neurGrpID, float x, float y, float z);
	QString getConnectionParameterXML();
	QString getNeuronParameterXML();

	void connectToDatabases(const QString& networkDBName, const QString& archiveDBName);
	void cleanTestDatabases();
	void closeDatabases();
	void executeQuery(QSqlQuery& query);
	void executeQuery(const QString& queryStr);
	void executeArchiveQuery(const QString& queryStr);
	QSqlQuery getArchiveQuery();
	QSqlQuery getArchiveQuery(const QString& queryStr);
	QSqlQuery getQuery();
	QSqlQuery getQuery(const QString& queryStr);
	void resetTestNetwork1();
	void resetTestArchive1();
	void resetTestArchive2();

	protected slots:
	    //QTest methods
	    virtual void cleanup();
	    virtual void cleanupTestCase();
	    virtual void init();
	    virtual void initTestCase();

};

#endif//TESTDAO_H


