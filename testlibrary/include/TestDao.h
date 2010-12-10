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
		/*! Information about the SpikeStreamNetwork database used in the tests */
		DBInfo networkDBInfo;

		/*! Name of SpikeStreamNetwork database connection */
		QString networkDBRef;

		/*! Information about the SpikeStreamArchive database used in the tests */
		DBInfo archiveDBInfo;

		/*! Reference used to statically access the SpikeStream archive database */
		QString archiveDBRef;

		/*! Information about the SpikeStreamAnalysis database used in the tests */
		DBInfo analysisDBInfo;

		/*! Name used to access SpikeStreamAnalysis database connection */
		QString analysisDBRef;

		//Parameters for test networks
		unsigned int testNetID;
		unsigned int neurGrp1ID;
		unsigned int neurGrp2ID;
		unsigned int connGrp1ID;
		QList<unsigned int> testConnIDList;
		QList<unsigned int> testNeurIDList;

		unsigned int testNet2ID;
		unsigned int neurGrp21ID;
		unsigned int neurGrp22ID;
		unsigned int connGrp21ID;
		QList<unsigned int> testConnIDList2;
		QList<unsigned int> testNeurIDList2;

		//Variables storing information about test archives
		unsigned int testArchive1ID;
		unsigned int testArchive2ID;

		//Variables storing information about test analyses
		unsigned int testAnalysis1ID;


		//==========================  METHODS  ==========================
		void addTestAnalysis1();
		void addTestArchive1();
		void addTestArchive2();
		void addTestNetwork1();
		void addTestNetwork2();
		unsigned int addTestConnection(unsigned int connGrpID, unsigned int fromID, unsigned int toID, float weight, float delay);
		unsigned int addTestNeuron(unsigned int neurGrpID, float x, float y, float z);
		void addWeightlessTestNetwork1();
		void addWeightlessTrainingData(unsigned int neuronID, const QString& pattern, unsigned int output);
		void addWeightlessPatternIndex(unsigned int connectionID, unsigned int index);
		QString getConnectionParameterXML();
		QString getNeuronParameterXML();

		void connectToDatabases(const QString& networkDBName, const QString& archiveDBName, const QString& analysisDBName);
		void cleanTestDatabases();
		void closeDatabases();
		void executeQuery(QSqlQuery& query);
		void executeQuery(const QString& queryStr);
		void executeArchiveQuery(const QString& queryStr);
		void executeAnalysisQuery(const QString& queryStr);
		QSqlQuery getAnalysisQuery();
		QSqlQuery getAnalysisQuery(const QString& queryStr);
		QSqlQuery getArchiveQuery();
		QSqlQuery getArchiveQuery(const QString& queryStr);
		QSqlQuery getQuery();
		QSqlQuery getQuery(const QString& queryStr);
		void resetTestNetwork1();
		void resetTestNetwork2();
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


