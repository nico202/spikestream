//SpikeStream includes
#include "DatabaseDao.h"
#include "SpikeStreamDBException.h"
#include "SpikeStreamIOException.h"
using namespace spikestream;

//Qt includes
#include <QTextStream>

//Other includes
#include <iostream>
using namespace std;

/*! Constructor. DBInfo is stored by AbstractDao */
DatabaseDao::DatabaseDao(const DBInfo& dbInfo) : AbstractDao(dbInfo){
}


/*! Empty constructor, used to test connection to database */
DatabaseDao::DatabaseDao() : AbstractDao(){
}


/*! Destructor */
DatabaseDao::~DatabaseDao(){
	closeDatabaseConnection();
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Allows manual connection to database */
void DatabaseDao::connectToDatabase(const DBInfo& dbInfo){
	//Store the information about the database
	setDBInfo(dbInfo);

	//Connect to the database
	connectToDatabase();
}


/*! Returns a list of names of the current databases */
QList<QString> DatabaseDao::getDatabaseNames(){
	QSqlQuery query = getQuery("SHOW DATABASES");
	executeQuery(query);
	QList<QString> dbList;
	while ( query.next() ) {
		dbList.append(query.value(0).toString());
	}
	return dbList;
}


/*! Executes the contents of the supplied file */
void DatabaseDao::executeSQLFile(const QString& fileName){
	QFile dbFile(fileName);
	if(!dbFile.exists()){
		throw SpikeStreamDBException("Attempting to load file " + fileName + " that does not exist");
	}
	QString fileStr;
	loadFileIntoString(dbFile, fileStr);

	/* There is a limit to the length of string that can be executed, so have to
		break string down into individual commands */
	QStringList cmdList = fileStr.split(";", QString::SkipEmptyParts);
	foreach(QString sqlCmd, cmdList){
		executeQuery(sqlCmd);
	}
}


/*! Executes the contents of the supplied file on the specified database */
void DatabaseDao::executeSQLFile(const QString& dbName, const QString& fileName){
	QFile dbFile(fileName);
	if(!dbFile.exists()){
		throw SpikeStreamDBException("Attempting to load file " + fileName + " that does not exist");
	}
	QString fileStr = "USE " + dbName + ";\n";
	loadFileIntoString(dbFile, fileStr);

	/* There is a limit to the length of string that can be executed, so have to
		break string down into individual commands */
	QStringList cmdList = fileStr.split(";", QString::SkipEmptyParts);
	foreach(QString sqlCmd, cmdList){
		executeQuery(sqlCmd);
	}
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Loads file contents into the supplied string. String is passed in rather than
	returned to avoid copying long strings or returning references to locals */
void DatabaseDao::loadFileIntoString(QFile& file, QString& fileString){
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		throw SpikeStreamIOException("Cannot open file: " + file.fileName());

	QTextStream in(&file);
	while (!in.atEnd()) {
		fileString += in.readLine();
	}
}



