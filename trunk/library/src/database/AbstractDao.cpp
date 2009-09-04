#include "AbstractDao.h"
#include "SpikeStreamDBException.h"

//Declare static variables
unsigned int AbstractDao::dbCounter = 0;


/*! Constructor. Creats connection with unique name. */
AbstractDao::AbstractDao(const DBInfo& dbInfo){
    //Store the information about the database
    this->dbInfo = dbInfo;

    //Record the thread that this database was created in - this class cannot be used across multiple threads
    dbThread = QThread::currentThread();

    //Get a unique name that can be used to access the database
    dbName = AbstractDao::getUniqueDBName();

    //Create database unique to this thread. No need to store reference because it is held statically
    QSqlDatabase database = QSqlDatabase::addDatabase("QMYSQL", dbName);
    database.setHostName(this->dbInfo.getHost());
    database.setDatabaseName(this->dbInfo.getDatabase());
    database.setUserName(this->dbInfo.getUser());
    database.setPassword(this->dbInfo.getPassword());
    bool ok = database.open();
    if(!ok)
	throw SpikeStreamDBException( QString("Cannot connect to database ") + this->dbInfo.toString() + ". Error: " + database.lastError().text() );
}


/*! Destructor. Closes connection and removes database */
AbstractDao::~AbstractDao(){
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


/*! Executes the query */
void AbstractDao::executeQuery(QSqlQuery& query){
    bool result = query.exec();
    if(!result){
	qCritical()<<"Error executing query: '"<<query.lastQuery()<<"'; Error='"<<query.lastError()<<"'.";
    }
}


/*! Returns a query object for the database */
QSqlQuery AbstractDao::getQuery(){
    checkDatabase();
    return QSqlQuery(QSqlDatabase::database(dbName));
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Returns a unique name that is used to access the database associated with this class
    and thread */
QString AbstractDao::getUniqueDBName(){
    ++dbCounter;
    return QString("Database-") + QString::number(dbCounter);
}


