//SpikeStream includes
#include "TestNetworkDaoThread.h"
#include "NetworkDao.h"
#include "NetworkDaoThread.h"
#include "SpikeStreamException.h"
#include "NetworkInfo.h"
using namespace spikestream;

#include <iostream>
using namespace std;

/*----------------------------------------------------------*/
/*-----                INIT AND CLEANUP                -----*/
/*----------------------------------------------------------*/

/*! Called after each test */
void TestNetworkDaoThread::cleanup(){
}


/*! Called after all the tests */
void TestNetworkDaoThread::cleanupTestCase() {
    //Leave databases in clean state
    //cleanTestDatabases();

    //Close database connection
    closeDatabase();
}


/*! Called before each test */
void TestNetworkDaoThread::init(){
    cleanTestDatabases();
}


/*! Called before all the tests */
void TestNetworkDaoThread::initTestCase(){
    connectToDatabase("SpikeStreamNetworkTest");
}


/*----------------------------------------------------------*/
/*-----                     TESTS                      -----*/
/*----------------------------------------------------------*/

/*! Tests the addition of a connection group to the network */
void TestNetworkDaoThread::testAddConnectionGroup(){
    try{
	/* Add network - slightly sloppy to use the network dao method, but it has been tested elsewhere
	    and we need a network because of foreign key constraints */
	NetworkInfo netInfo(0, "test3Name", "test3Description",  0);
	NetworkDao networkDao (dbInfo);
	networkDao.addNetwork(netInfo);

	/* Add neuron groups - again, slightly sloppy, but it has been tested elsewhere
	   and we need neuron groups because of foreign key constraints */
	//Add from neuron group
	NeuronGroup fromGrp( NeuronGroupInfo(0, "fromNeuronGroup1Name", "fromNeuronGroup1Desc", QHash<QString, double>(), 1) );
	NeuronMap* neurMap = fromGrp.getNeuronMap();
	(*neurMap)[1] = new Point3D(0, 0, 0);
	(*neurMap)[2] = new Point3D(0, 1, 0);
	(*neurMap)[3] = new Point3D(1, 0, 0);
	(*neurMap)[4] = new Point3D(1, 1, 0);
	NetworkDaoThread netDaoThread(dbInfo);
	netDaoThread.prepareAddNeuronGroup(netInfo.getID(), &fromGrp);
	runThread(netDaoThread);

	//Add to neuron group
	NeuronGroup toGrp( NeuronGroupInfo(0, "toNeuronGroup1Name", "toNeuronGroup1Desc", QHash<QString, double>(), 1) );
	neurMap = toGrp.getNeuronMap();
	(*neurMap)[1] = new Point3D(0, 0, 10);
	(*neurMap)[2] = new Point3D(0, 3, 10);
	(*neurMap)[3] = new Point3D(2, 0, 10);
	(*neurMap)[4] = new Point3D(1, 7, 10);
	netDaoThread.prepareAddNeuronGroup(netInfo.getID(), &toGrp);
	runThread(netDaoThread);

	//Get lists of the from and to neuron ids
	QList<unsigned int> fromNeuronIDs = fromGrp.getNeuronIDs();
	QList<unsigned int> toNeuronIDs = toGrp.getNeuronIDs();
	QCOMPARE(fromNeuronIDs.size(), 4);
	QCOMPARE(toNeuronIDs.size(), 4);

	 //Build the connection group that is to be added
	QHash<QString, double> paramMap;
	paramMap["param3"] = 0.7;
	paramMap["param4"] = 0.8;
	ConnectionGroupInfo connGrpInfo(0, "testConnGroup1Desc", fromGrp.getID(), toGrp.getID(),  paramMap, 1);
	ConnectionGroup connGrp(connGrpInfo);

	//Add connections
	for(int i=0; i<4; ++i){
	    Connection* newConn = new Connection(fromNeuronIDs[i], toNeuronIDs[i],  30,  0.1,  0.4);
	    connGrp.addConnection(newConn);
	}

	//Add the connection group to the database
	netDaoThread.prepareAddConnectionGroup(netInfo.getID(), &connGrp);
	runThread(netDaoThread);

	//Check that the connection group was added correctly
	QSqlQuery query = getQuery("SELECT NeuralNetworkID, Description, FromNeuronGroupID, ToNeuronGroupID, Parameters, SynapseTypeID FROM ConnectionGroups");
	executeQuery(query);
	QCOMPARE(1, query.size());
	query.next();
	QCOMPARE(query.value(0).toString(), QString::number(netInfo.getID()));//Check network id
	QCOMPARE(query.value(1).toString(), connGrpInfo.getDescription());//Check description
	QCOMPARE(query.value(2).toString(), QString::number(fromGrp.getID()));//Check from neuron group id
	QCOMPARE(query.value(3).toString(), QString::number(toGrp.getID()));//Check to neuron group id
	QCOMPARE(query.value(4).toString(), connGrpInfo.getParameterXML());//Check parameters
	QCOMPARE(query.value(5).toString(), QString::number(connGrpInfo.getSynapseType()));//Check synapse type
	QVERIFY(connGrp.getID() != 0);

	//Check that connections were added correctly
	QCOMPARE(connGrp.isLoaded(), true);
	for(ConnectionList::iterator iter = connGrp.getConnections()->begin(); iter != connGrp.getConnections()->end(); ++iter){
	    query = getQuery("SELECT ConnectionGroupID, FromNeuronID, ToNeuronID, Delay, Weight FROM Connections WHERE ConnectionID = " + QString::number((*iter)->id));
	    executeQuery(query);
	    query.next();
	    QCOMPARE( query.value(0).toUInt(), connGrp.getID() );//Check connection group id
	    QCOMPARE( query.value(1).toUInt(), (*iter)->fromNeuronID );
	    QCOMPARE( query.value(2).toUInt(), (*iter)->toNeuronID );
	    QCOMPARE( query.value(3).toString().toFloat(), (*iter)->delay );
	    QCOMPARE( query.value(4).toString().toFloat(), (*iter)->weight );
	}
    }
    catch(SpikeStreamException& ex){
	QFAIL(ex.getMessage().toAscii());
    }
}


/*! Tests the addition of a neuron group to the network */
void TestNetworkDaoThread::testAddNeuronGroup(){
    try{
	/* Add network to get a valid id.
	    Slightly sloppy to use the network dao method, but it has been tested elsewhere
		and we need a network because of foreign key constraints */
	NetworkInfo netInfo(0, "test2Name","test3Description", 0);
	NetworkDao networkDao(dbInfo);
	networkDao.addNetwork(netInfo);

	//Build the neuron group that is to be added
	QHash<QString, double> paramMap;
	paramMap["param1"] = 0.5;
	paramMap["param2"] = 0.6;
	NeuronGroup neurGrp(NeuronGroupInfo(0, "testNeuronGroup1Name", "testNeuronGroup1Desc", paramMap, 1));
	NeuronMap* neurMap = neurGrp.getNeuronMap();
	(*neurMap)[1] = new Point3D(0, 0, 0);
	(*neurMap)[2] = new Point3D(0, 1, 0);
	(*neurMap)[3] = new Point3D(1, 0, 0);
	(*neurMap)[4] = new Point3D(1, 1, 1);

	//Add the neuron group
	NetworkDaoThread netDaoThread(dbInfo);
	netDaoThread.prepareAddNeuronGroup(netInfo.getID(), &neurGrp);
	runThread(netDaoThread);

	//Check that the neuron group was added correctly
	QSqlQuery query = getQuery("SELECT NeuralNetworkID, Name, Description, Parameters, NeuronTypeID FROM NeuronGroups");
	executeQuery(query);
	QCOMPARE(1, query.size());
	query.next();
	NeuronGroupInfo neurGrpInfo = neurGrp.getInfo();
	QCOMPARE(query.value(0).toString(), QString::number(netInfo.getID()));//Check network id
	QCOMPARE(query.value(1).toString(), neurGrpInfo.getName());//Check name
	QCOMPARE(query.value(2).toString(), neurGrpInfo.getDescription());//Check description
	QCOMPARE(query.value(3).toString(), neurGrpInfo.getParameterXML());//Check parameters
	QCOMPARE(query.value(4).toString(), QString::number(neurGrpInfo.getNeuronType()));//Check neuron type
	QVERIFY( neurGrpInfo.getID() != 0);

	//Check that the neurons were added correctly
	QCOMPARE(neurGrp.isLoaded(), true);
	query = getQuery("SELECT NeuronID, X, Y, Z FROM Neurons WHERE NeuronGroupID = " + QString::number(neurGrp.getID()));
	executeQuery(query);
	QCOMPARE(4, query.size());//4 neurons should have been added
	query.next();
	QVERIFY( neurGrp.contains(
			query.value(0).toUInt(),
			query.value(1).toString().toFloat(),
			query.value(2).toString().toFloat(),
			query.value(3).toString().toFloat()
	) );
	query.next();
	QVERIFY( neurGrp.contains(
			query.value(0).toUInt(),
			query.value(1).toString().toFloat(),
			query.value(2).toString().toFloat(),
			query.value(3).toString().toFloat()
	) );
	query.next();
	QVERIFY( neurGrp.contains(
			query.value(0).toUInt(),
			query.value(1).toString().toFloat(),
			query.value(2).toString().toFloat(),
			query.value(3).toString().toFloat()
	) );
	query.next();
	//Check the last one in a slightly different way
	QCOMPARE(query.value(1).toString(), QString::number(1));
	QCOMPARE(query.value(2).toString(), QString::number(1));
	QCOMPARE(query.value(3).toString(), QString::number(1));

    }
    catch(SpikeStreamException& ex){
	QFAIL(ex.getMessage().toAscii());
    }
}


void TestNetworkDaoThread::testLoadConnections(){
    //Add test network
    addTestNetwork1();

    //Create a connection group with the appropriate id
    ConnectionGroup connGrp(ConnectionGroupInfo(connGrp1ID, "undefined", 0, ,  QHash<QString, double>(), 1));

    //Load connections associated with this neuron group
    networkDaoThread->prepareLoadConnections(connGrp);
    runNetworkDaoThread();

    ADD ASSERTIONS ABOUT CONNECTIONS

}


void TestNetworkDaoThread::testLoadNeurons(){
}

/*----------------------------------------------------------*/
/*-----               PRIVATE METHODS                  -----*/
/*----------------------------------------------------------*/

/*! Adds a test network to the database with known properties */
void TestNetworkDaoThread::addTestNetwork1(){
    //Add network with known properties
    QSqlQuery query = getQuery("INSERT INTO NeuralNetworks (Name, Description) VALUES ('testNetwork1Name', 'testNetwork1Description')");
    executeQuery(query);
    testNetID = query.lastInsertId().toUInt();

    //Add two neuron groups
    QString queryStr = "INSERT INTO NeuronGroups (NeuralNetworkID, Name, Description, Parameters, NeuronTypeID ) VALUES (";
    queryStr += QString::number(networkID) + ", " + "'name1', 'desc1', '" + getConnectionParameterXML() + "', 1)";
    query = getQuery(queryStr);
    executeQuery(query);
    neurGrp1ID = query.lastInsertId().toUInt();

    queryStr = "INSERT INTO NeuronGroups (NeuralNetworkID, Name, Description, Parameters, NeuronTypeID ) VALUES (";
    queryStr += QString::number(networkID) + ", " + "'name2', 'desc2', '" + getConnectionParameterXML() + "', 1)";
    query = getQuery(queryStr);
    executeQuery(query);
    neurGrp2ID = query.lastInsertId().toUInt();

    //Create connection group between the two neuron groups
    queryStr = "INSERT INTO ConnectionGroups (NeuralNetworkID, Description, FromNeuronGroupID, ToNeuronGroupID, Parameters, SynapseTypeID ) VALUES (";
    queryStr += QString::number(networkID) + ", 'conngrpdesc1', " + QString::number(neurGrp1ID) + ", " + QString::number(neurGrp2ID) + ", '" + getConnectionParameterXML() + "', 1)";
    query = getQuery(queryStr);
    executeQuery(query);
    connGrp1ID = query.lastInsertId().toUInt();

    //Add neurons to the groups, storing database id in testNeurIDList
    testNeurIDList.clear();
    testNeurIDList.append(addTestNeuron(neurGrp1ID, 0, 0, 0));
    testNeurIDList.append(addTestNeuron(neurGrp1ID, 0, 1, 0));
    testNeurIDList.append(addTestNeuron(neurGrp1ID, 1, 1, 0));
    testNeurIDList.append(addTestNeuron(neurGrp2ID, 0, 0, 10));
    testNeurIDList.append(addTestNeuron(neurGrp2ID, 1, 1, 10));

    //Add test connections
    testConnIDList(addTestConnection(connGrp1ID, testNeurIDList[0], testNeurIDList[1], 0.1));
    testConnIDList(addTestConnection(connGrp1ID, testNeurIDList[0], testNeurIDList[2], 0.2));
    testConnIDList(addTestConnection(connGrp1ID, testNeurIDList[0], testNeurIDList[3], 0.3));
    testConnIDList(addTestConnection(connGrp1ID, testNeurIDList[4], testNeurIDList[3], 0.4));
    testConnIDList(addTestConnection(connGrp1ID, testNeurIDList[4], testNeurIDList[1], 0.5));
    testConnIDList(addTestConnection(connGrp1ID, testNeurIDList[3], testNeurIDList[2], 0.6));
}


/*! Adds a test connection to the database */
unsigned int TestNetworkDaoThread::addTestConnection(unsigned int connGrpID, unsigned int fromID, unsigned int toID, float weight, float delay){
    QString queryStr = "INSERT INTO Connections ( ConnectionGroupID, FromNeuronID, ToNeuronID, Delay, Weight) VALUES (";
    queryStr += QString::number(connGrpID) + ", ";
    queryStr += QString::number(fromID) + ", ";
    queryStr += QString::number(toID) + ", ";
    queryStr += QString::number(delay) + ", ";
    queryStr += QString::number(weight) + ")";
    query = getQuery(queryStr);
    executeQuery(query);
    return query.lastInsertId().toUInt();
}


/*! Adds a test neuron to the database */
void TestNetworkDaoThread::addTestNeuron(unsigned int neurGrpID, float x, float y, float z){
    QString queryStr("INSERT INTO Neurons (NeuronGroupID, X, Y, Z) VALUES (");
    queryStr += QString::number(neurGrpID) + ", ";
    queryStr += QString::number(x) + ", ";
    queryStr += QString::number(y) + ", ";
    queryStr += QString::number(z) + ")";
    query = getQuery(queryStr);
    executeQuery(query);
    return query.lastInsertId().toUInt();
}


/*! Runs the supplied thread and checks for errors */
void TestNetworkDaoThread::runThread(NetworkDaoThread& thread){
    thread.start();
    thread.wait();
    if(thread.isError()){
	throw SpikeStreamException(thread.getErrorMessage());
    }
}










