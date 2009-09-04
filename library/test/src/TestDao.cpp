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
