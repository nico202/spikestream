#include "TestDao.h"
#include "SpikeStreamException.h"
#include "SpikeStreamDBException.h"


/*! Connects to the test database.
    FIXME: LOAD PARAMETERS FROM FILE. */
void TestDao::connectToDatabase(const QString& dbName){    //Set the database parameters to be used in the test
    dbInfo = DBInfo("localhost", "SpikeStream", "ekips", dbName);

    //Create database unique to this thread. No need to store reference because it is held statically
    database = QSqlDatabase::addDatabase("QMYSQL", dbName);
    database.setHostName(dbInfo.getHost());
    database.setDatabaseName(dbInfo.getDatabase());
    database.setUserName(dbInfo.getUser());
    database.setPassword(dbInfo.getPassword());
    bool ok = database.open();
    if(!ok)
	throw SpikeStreamDBException( QString("Cannot connect to database ") + dbInfo.toString() + ". Error: " + database.lastError().text() );
}

/*! Cleans up everything from the test networks database */
void TestDao::cleanTestDatabases(){
    executeQuery("DELETE FROM NeuralNetworks");
    executeQuery("DELETE FROM NeuronGroups");
    executeQuery("DELETE FROM ConnectionGroups");
    executeQuery("DELETE FROM Neurons");
    executeQuery("DELETE FROM Connections");
}


/*! Executes the query */
void TestDao::executeQuery(QSqlQuery& query){
    bool result = query.exec();
    if(!result){
	QString errMsg = "Error executing query: '" + query.lastQuery() + "'; Error='" + query.lastError().text() + "'.";
	QFAIL(errMsg.toAscii());
    }
}


/*! Executes the query */
void TestDao::executeQuery(const QString& queryStr){
    QSqlQuery query = getQuery(queryStr);
    executeQuery(query);
}


/*! Returns a query object for the database */
QSqlQuery TestDao::getQuery(){
    return QSqlQuery(database);
}


/*! Returns a query object for the database */
QSqlQuery TestDao::getQuery(const QString& queryStr){
    QSqlQuery tmpQuery(database);
    tmpQuery.prepare(queryStr);
    return tmpQuery;
}

