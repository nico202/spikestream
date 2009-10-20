#include "TestDao.h"
#include "SpikeStreamException.h"
#include "SpikeStreamDBException.h"

#include <iostream>
using namespace std;


/*----------------------------------------------------------*/
/*-----                INIT AND CLEANUP                -----*/
/*----------------------------------------------------------*/

/*! Called after each test */
void TestDao::cleanup(){
}


/*! Called after all the tests */
void TestDao::cleanupTestCase() {
    //Leave databases in clean state
    cleanTestDatabases();

    //Close database connection
    closeDatabases();
}


/*! Called before each test */
void TestDao::init(){
    cleanTestDatabases();
}


/*! Called before all the tests */
void TestDao::initTestCase(){
    connectToDatabases("SpikeStreamNetworkTest", "SpikeStreamArchiveTest", "SpikeStreamAnalysisTest");
}


/*----------------------------------------------------------*/
/*-----                PROTECTED METHODS               -----*/
/*----------------------------------------------------------*/

/*! Connects to the test database.
    FIXME: LOAD PARAMETERS FROM FILE. */
void TestDao::connectToDatabases(const QString& networkDBName, const QString& archiveDBName, const QString& analysisDBName){    //Set the database parameters to be used in the test
    //Connect to the network database. This is the default database
    dbRefName = "NetworkTestDBRef";
    dbInfo = DBInfo("localhost", "SpikeStream", "ekips", networkDBName);
    database = QSqlDatabase::addDatabase("QMYSQL", dbRefName);
    database.setHostName(dbInfo.getHost());
    database.setDatabaseName(dbInfo.getDatabase());
    database.setUserName(dbInfo.getUser());
    database.setPassword(dbInfo.getPassword());
    bool ok = database.open();
    if(!ok)
	throw SpikeStreamDBException( QString("Cannot connect to network database ") + dbInfo.toString() + ". Error: " + database.lastError().text() );

    //Connect to the archive database
    archiveDBRefName = "ArchiveTestDBRef";
    archiveDBInfo = DBInfo("localhost", "SpikeStream", "ekips", archiveDBName);
    archiveDatabase = QSqlDatabase::addDatabase("QMYSQL", archiveDBRefName);
    archiveDatabase.setHostName(archiveDBInfo.getHost());
    archiveDatabase.setDatabaseName(archiveDBInfo.getDatabase());
    archiveDatabase.setUserName(archiveDBInfo.getUser());
    archiveDatabase.setPassword(archiveDBInfo.getPassword());
    ok = archiveDatabase.open();
    if(!ok)
	throw SpikeStreamDBException( QString("Cannot connect to archive database ") + archiveDBInfo.toString() + ". Error: " + archiveDatabase.lastError().text() );

    //Connect to the analysis database
    analysisDBRefName = "AnalysisTestDBRef";
    analysisDBInfo = DBInfo("localhost", "SpikeStream", "ekips", analysisDBName);
    analysisDatabase = QSqlDatabase::addDatabase("QMYSQL", analysisDBRefName);
    analysisDatabase.setHostName(analysisDBInfo.getHost());
    analysisDatabase.setDatabaseName(analysisDBInfo.getDatabase());
    analysisDatabase.setUserName(analysisDBInfo.getUser());
    analysisDatabase.setPassword(analysisDBInfo.getPassword());
    ok = analysisDatabase.open();
    if(!ok)
	throw SpikeStreamDBException( QString("Cannot connect to analysis database ") + analysisDBInfo.toString() + ". Error: " + analysisDatabase.lastError().text() );
}

/*! Cleans up everything from the test networks database */
void TestDao::cleanTestDatabases(){
    //Clean up network database
    executeQuery("DELETE FROM Networks");
    executeQuery("DELETE FROM NeuronGroups");
    executeQuery("DELETE FROM ConnectionGroups");
    executeQuery("DELETE FROM Neurons");
    executeQuery("DELETE FROM Connections");
    executeQuery("DELETE FROM WeightlessConnections");
    executeQuery("DELETE FROM WeightlessNeuronTrainingPatterns");

    //Clean up archive database
    executeArchiveQuery("DELETE FROM Archives");
    executeArchiveQuery("DELETE FROM ArchiveData");

    //Clean up analysis database
    executeAnalysisQuery("DELETE FROM Analyses");
    executeAnalysisQuery("DELETE FROM StateBasedPhiData");
}


/*! Closes all the open databases.
    FIXME: THIS CURRENTLY GENERATES A WARNING ABOUT OPEN QUERIES */
void TestDao::closeDatabases(){
    QSqlDatabase::removeDatabase(dbRefName);
    QSqlDatabase::removeDatabase(archiveDBRefName);
    QSqlDatabase::removeDatabase(analysisDBRefName);
}


/*! Executes query on network database*/
void TestDao::executeQuery(QSqlQuery& query){
    bool result = query.exec();
    if(!result){
	QString errMsg = "Error executing query: '" + query.lastQuery() + "'; Error='" + query.lastError().text() + "'.";
	QFAIL(errMsg.toAscii());
    }
}


/*! Executes query on network database*/
void TestDao::executeQuery(const QString& queryStr){
    QSqlQuery query = getQuery(queryStr);
    executeQuery(query);
}


/*! Executes an archive query */
void TestDao::executeArchiveQuery(const QString& queryStr){
    QSqlQuery query = getArchiveQuery(queryStr);
    executeQuery(query);
}


/*! Executes an analysis query */
void TestDao::executeAnalysisQuery(const QString& queryStr){
    QSqlQuery query = getAnalysisQuery(queryStr);
    executeQuery(query);
}


/*! Returns a query object for the network database */
QSqlQuery TestDao::getQuery(){
    return QSqlQuery(database);
}


/*! Returns a query object for the network database */
QSqlQuery TestDao::getQuery(const QString& queryStr){
    QSqlQuery tmpQuery(database);
    tmpQuery.prepare(queryStr);
    return tmpQuery;
}

/*! Returns a query object for the analysis database */
QSqlQuery TestDao::getAnalysisQuery(){
    return QSqlQuery(analysisDatabase);
}


/*! Returns a query object for the analysis database */
QSqlQuery TestDao::getAnalysisQuery(const QString& queryStr){
    QSqlQuery tmpQuery(analysisDatabase);
    tmpQuery.prepare(queryStr);
    return tmpQuery;
}


/*! Returns a query object for the archive database */
QSqlQuery TestDao::getArchiveQuery(){
    return QSqlQuery(archiveDatabase);
}


/*! Returns a query object for the archive database */
QSqlQuery TestDao::getArchiveQuery(const QString& queryStr){
    QSqlQuery tmpQuery(archiveDatabase);
    tmpQuery.prepare(queryStr);
    return tmpQuery;
}


void TestDao::addTestArchive1(){
    //Add test network - this is needed because we need a valid network ID
    addTestNetwork1();

    //Reset variables associated with the test archive
    resetTestArchive1();

    //Add archive with known properties
    QSqlQuery query = getArchiveQuery("INSERT INTO Archives (StartTime, NetworkID, Description) VALUES (1011, " + QString::number(testNetID) + ", 'testArchive1Description')");
    executeQuery(query);
    testArchive1ID = query.lastInsertId().toUInt();

    //Add archive data
    executeArchiveQuery("INSERT INTO ArchiveData (ArchiveID, TimeStep, FiringNeurons) VALUES (" + QString::number(testArchive1ID) + ", 1, '256,311,21,4')");
    executeArchiveQuery("INSERT INTO ArchiveData (ArchiveID, TimeStep, FiringNeurons) VALUES (" + QString::number(testArchive1ID) + ", 2, '22,31,4888888')");
    executeArchiveQuery("INSERT INTO ArchiveData (ArchiveID, TimeStep, FiringNeurons) VALUES (" + QString::number(testArchive1ID) + ", 5, '3')");
}


/*! Adds a second test archive
    NOTE: This assumes that test network 1 has been added */
void TestDao::addTestArchive2(){
    //Reset variables associated with the test archive
    resetTestArchive2();

    //Add archive with known properties
    QSqlQuery query = getArchiveQuery("INSERT INTO Archives (StartTime, NetworkID, Description) VALUES (2022211, " + QString::number(testNetID) + ", 'testArchive2Description')");
    executeQuery(query);
    testArchive2ID = query.lastInsertId().toUInt();
}


/*! Adds a test network to the database with known properties */
void TestDao::addTestNetwork1(){
    //Initialize everything to do with the test network
    resetTestNetwork1();

    //Add network with known properties
    QSqlQuery query = getQuery("INSERT INTO Networks (Name, Description) VALUES ('testNetwork1Name', 'testNetwork1Description')");
    executeQuery(query);
    testNetID = query.lastInsertId().toUInt();

    //Add two neuron groups
    QString queryStr = "INSERT INTO NeuronGroups (NetworkID, Name, Description, Parameters, NeuronTypeID ) VALUES (";
    queryStr += QString::number(testNetID) + ", " + "'name1', 'desc1', '" + getConnectionParameterXML() + "', 1)";
    query = getQuery(queryStr);
    executeQuery(query);
    neurGrp1ID = query.lastInsertId().toUInt();

    queryStr = "INSERT INTO NeuronGroups (NetworkID, Name, Description, Parameters, NeuronTypeID ) VALUES (";
    queryStr += QString::number(testNetID) + ", " + "'name2', 'desc2', '" + getNeuronParameterXML() + "', 1)";
    query = getQuery(queryStr);
    executeQuery(query);
    neurGrp2ID = query.lastInsertId().toUInt();

    //Create connection group between the two neuron groups
    queryStr = "INSERT INTO ConnectionGroups (NetworkID, Description, FromNeuronGroupID, ToNeuronGroupID, Parameters, SynapseTypeID ) VALUES (";
    queryStr += QString::number(testNetID) + ", 'conngrpdesc1', " + QString::number(neurGrp1ID) + ", " + QString::number(neurGrp2ID) + ", '" + getConnectionParameterXML() + "', 1)";
    query = getQuery(queryStr);
    executeQuery(query);
    connGrp1ID = query.lastInsertId().toUInt();

    //Add neurons to the groups, storing database id in testNeurIDList
    testNeurIDList.clear();
    testNeurIDList.append(addTestNeuron(neurGrp1ID, -1, -5, -6));
    testNeurIDList.append(addTestNeuron(neurGrp1ID, 0, -1, 0));
    testNeurIDList.append(addTestNeuron(neurGrp1ID, 9, -4, 0));
    testNeurIDList.append(addTestNeuron(neurGrp2ID, 0, -1, 10));
    testNeurIDList.append(addTestNeuron(neurGrp2ID, -3, -7, 5));

    //Add test connections
    testConnIDList.append(addTestConnection(connGrp1ID, testNeurIDList[0], testNeurIDList[1], 0.1, 1.1));
    testConnIDList.append(addTestConnection(connGrp1ID, testNeurIDList[0], testNeurIDList[2], 0.2, 1.2));
    testConnIDList.append(addTestConnection(connGrp1ID, testNeurIDList[0], testNeurIDList[3], 0.3, 1.3));
    testConnIDList.append(addTestConnection(connGrp1ID, testNeurIDList[4], testNeurIDList[3], 0.4, 1.4));
    testConnIDList.append(addTestConnection(connGrp1ID, testNeurIDList[4], testNeurIDList[1], 0.5, 1.5));
    testConnIDList.append(addTestConnection(connGrp1ID, testNeurIDList[3], testNeurIDList[2], 0.6, 1.6));
}


/*! Adds a test connection to the database */
unsigned int TestDao::addTestConnection(unsigned int connGrpID, unsigned int fromID, unsigned int toID, float weight, float delay){
    QString queryStr = "INSERT INTO Connections ( ConnectionGroupID, FromNeuronID, ToNeuronID, Delay, Weight) VALUES (";
    queryStr += QString::number(connGrpID) + ", ";
    queryStr += QString::number(fromID) + ", ";
    queryStr += QString::number(toID) + ", ";
    queryStr += QString::number(delay) + ", ";
    queryStr += QString::number(weight) + ")";
    QSqlQuery query = getQuery(queryStr);
    executeQuery(query);
    return query.lastInsertId().toUInt();
}


/*! Adds a test neuron to the database */
unsigned int TestDao::addTestNeuron(unsigned int neurGrpID, float x, float y, float z){
    QString queryStr("INSERT INTO Neurons (NeuronGroupID, X, Y, Z) VALUES (");
    queryStr += QString::number(neurGrpID) + ", ";
    queryStr += QString::number(x) + ", ";
    queryStr += QString::number(y) + ", ";
    queryStr += QString::number(z) + ")";
    QSqlQuery query = getQuery(queryStr);
    executeQuery(query);
    return query.lastInsertId().toUInt();
}


/*! Returns XML containing connection parameters */
QString TestDao::getConnectionParameterXML(){
    QString tmpStr = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
    tmpStr += "<connection_group_parameters>";
    tmpStr +="<parameter><name>connparam1</name><value>0.63</value></parameter>";
    tmpStr +="<parameter><name>connparam2</name><value>1.31</value></parameter>";
    tmpStr += "</connection_group_parameters>";
    return tmpStr;
}


/*! Returns XML containing connection parameters */
QString TestDao::getNeuronParameterXML(){
    QString tmpStr = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
    tmpStr += "<neuron_group_parameters>";
    tmpStr +="<parameter><name>neurparam1</name><value>1.63</value></parameter>";
    tmpStr +="<parameter><name>neurparam2</name><value>2.31</value></parameter>";
    tmpStr += "</neuron_group_parameters>";
    return tmpStr;
}


/*! Resets variables holding information about the first test archive */
void TestDao::resetTestArchive1(){
    testArchive1ID = 0;
}

/*! Resets variables holding information about the second test archive */
void TestDao::resetTestArchive2(){
    testArchive2ID = 0;
}


/*! Resets stored variables relating to the first test network. */
void TestDao::resetTestNetwork1(){
    testNeurIDList.clear();
    testConnIDList.clear();
    testNetID = 0;
    neurGrp1ID = 0;
    neurGrp2ID = 0;
    connGrp1ID = 0;
}

