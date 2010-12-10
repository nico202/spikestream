//SpikeStream includes
#include "DatabaseDao.h"
#include "TestDatabaseDao.h"
#include "SpikeStreamException.h"
using namespace spikestream;

void TestDatabaseDao::init(){
	workingDirectory = QCoreApplication::applicationDirPath();
	workingDirectory.truncate(workingDirectory.size() - 4);//Trim the "/bin" off the end
}


/*----------------------------------------------------------*/
/*-----                     TESTS                      -----*/
/*----------------------------------------------------------*/
void TestDatabaseDao::testExecuteSQLFile(){
	//Check database does not exist
	QSqlQuery query = getQuery("SHOW DATABASES");
	executeQuery(query);
	while (query.next()){
		if(query.value(0).toString() == "SpikeStreamTest1")
			QFAIL("SpikeStreamTest1 database already exists");
	}

	try{
		//Execute first test method
		DBInfo tmpDBInfo (networkDBInfo);
		tmpDBInfo.setDatabase("");
		DatabaseDao dbDao;
		dbDao.connectToDatabase(tmpDBInfo);
		dbDao.executeSQLFile(workingDirectory + "/library/test/test_files/TestAddDatabase1.sql");


		//Check database has been added
		query = getQuery("SHOW DATABASES");
		executeQuery(query);
		bool dbFound = false;
		while (query.next()){
			if(query.value(0).toString().toLower() == "spikestreamtest1")
				dbFound = true;
		}
		QVERIFY(dbFound);

		query = getQuery("SHOW TABLES FROM SpikeStreamTest1");
		executeQuery(query);
		QCOMPARE(query.size(), (int)2);
		query.next();
		QCOMPARE(query.value(0).toString().toLower(), QString("test1table"));

		//Execute second test method
		dbDao.executeSQLFile("SpikeStreamTest1", workingDirectory + "/library/test/test_files/TestAddTable1.sql");

		//Check table has been added
		query = getQuery("SHOW TABLES FROM SpikeStreamTest1");
		executeQuery(query);
		QCOMPARE(query.size(), (int)3);
		bool tableFound = false;
		while (query.next()){
			if(query.value(0).toString().toLower()=="test3table")
				tableFound = true;
		}
		QVERIFY(tableFound);

		//Clean up
		executeQuery("DROP DATABASE SpikeStreamTest1");
	}
	catch(SpikeStreamException& ex){
		QFAIL(ex.getMessage().toAscii());
	}
}

void TestDatabaseDao::testGetDatabaseNames(){
		DatabaseDao dbDao(networkDBInfo);
		QList<QString> dbNames = dbDao.getDatabaseNames();

		//Convert to lower case to handle Windows and Linux
		QList<QString> dbNamesLower;
		foreach(QString dbName, dbNames)
			dbNamesLower.append(dbName.toLower());
		QVERIFY(dbNamesLower.contains("spikestreamnetwork"));
		QVERIFY(dbNamesLower.contains("spikestreamanalysistest"));
}


