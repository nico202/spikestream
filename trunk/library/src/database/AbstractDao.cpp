#include "AbstractDao.h"
#include "SpikeStreamDBException.h"

//Declare static variables
unsigned int AbstractDao::dbCounter = 0;


/*! Constructor. Creats connection with unique name. */
AbstractDao::AbstractDao(DBInfo& dbInfo){
    //Store the information about the database
    this->dbInfo = dbInfo;

    //Record the thread that this database was created in - this class cannot be used across multiple threads
    dbThread = QThread::currentThread();

    //Get a unique name that can be used to access the database
    dbName = AbstractDao::getDBName();

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


/*! Destructor. Closes connection and removes database */
AbstractDao::~AbstractDao(){
    database.close();
    QSqlDatabase::removeDatabase(dbName);
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Returns a copy of the information about the database */
DBInfo AbstractDao::getDBInfo(){
    return dbInfo;
}


/*----------------------------------------------------------*/
/*-----               PROTECTED METHODS                -----*/
/*----------------------------------------------------------*/

/*! Runs basic checks on the database before a query.
    NOTE: It is assumed that it exists and is open, since this should be checked in the constructor.
    Thread checks have to be done at runtime. */
void AbstractDao::checkDatabase(){
    /* Check that we are accessing datbase connection from the thread it was created on.
       The Qt database class only works within a single thread.*/
    if(QThread::currentThread() != dbThread){
	throw SpikeStreamDBException("Attempting to access database from different thread. This is not supported in Qt4");
    }
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Returns a unique name that is used to access the database associated with this class
    and thread */
QString AbstractDao::getDBName(){
    ++dbCounter;
    return QString("Database-") + QString::number(dbCounter);
}


