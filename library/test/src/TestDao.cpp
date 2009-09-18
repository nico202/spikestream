#include "TestDao.h"
#include "SpikeStreamException.h"
#include "SpikeStreamDBException.h"

#include <iostream>
using namespace std;

/*! Connects to the test database.
    FIXME: LOAD PARAMETERS FROM FILE. */
void TestDao::connectToDatabase(const QString& dbName){    //Set the database parameters to be used in the test
    dbRefName = "TestDBRef";

    dbInfo = DBInfo("localhost", "SpikeStream", "ekips", dbName);

    //Create database unique to this thread. No need to store reference because it is held statically
    database = QSqlDatabase::addDatabase("QMYSQL", dbRefName);
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


void TestDao::closeDatabase(){
    QSqlDatabase::removeDatabase(dbRefName);
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


/*! Adds a test network to the database with known properties */
void TestDao::addTestNetwork1(){
    //Initialize everything to do with the test network
    resetTestNetwork();

    //Add network with known properties
    QSqlQuery query = getQuery("INSERT INTO NeuralNetworks (Name, Description) VALUES ('testNetwork1Name', 'testNetwork1Description')");
    executeQuery(query);
    testNetID = query.lastInsertId().toUInt();

    //Add two neuron groups
    QString queryStr = "INSERT INTO NeuronGroups (NeuralNetworkID, Name, Description, Parameters, NeuronTypeID ) VALUES (";
    queryStr += QString::number(testNetID) + ", " + "'name1', 'desc1', '" + getConnectionParameterXML() + "', 1)";
    query = getQuery(queryStr);
    executeQuery(query);
    neurGrp1ID = query.lastInsertId().toUInt();

    queryStr = "INSERT INTO NeuronGroups (NeuralNetworkID, Name, Description, Parameters, NeuronTypeID ) VALUES (";
    queryStr += QString::number(testNetID) + ", " + "'name2', 'desc2', '" + getNeuronParameterXML() + "', 1)";
    query = getQuery(queryStr);
    executeQuery(query);
    neurGrp2ID = query.lastInsertId().toUInt();

    //Create connection group between the two neuron groups
    queryStr = "INSERT INTO ConnectionGroups (NeuralNetworkID, Description, FromNeuronGroupID, ToNeuronGroupID, Parameters, SynapseTypeID ) VALUES (";
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


void TestDao::resetTestNetwork(){
    testNeurIDList.clear();
    testConnIDList.clear();
    testNetID = 0;
    neurGrp1ID = 0;
    neurGrp2ID = 0;
    connGrp1ID = 0;
}

