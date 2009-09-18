//SpikeStream includes
#include "SpikeStreamException.h"
#include "TestNetworkDao.h"
#include "NetworkDao.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;

/*----------------------------------------------------------*/
/*-----                INIT AND CLEANUP                -----*/
/*----------------------------------------------------------*/

/*! Called after each test */
void TestNetworkDao::cleanup(){
}


/*! Called after all the tests */
void TestNetworkDao::cleanupTestCase() {
    //Leave databases in clean state
    cleanTestDatabases();

    //Close database connection
    closeDatabase();
}


/*! Called before each test */
void TestNetworkDao::init(){
    cleanTestDatabases();
}


/*! Called before all the tests */
void TestNetworkDao::initTestCase(){
    connectToDatabase("SpikeStreamNetworkTest");
}


/*----------------------------------------------------------*/
/*-----                     TESTS                      -----*/
/*----------------------------------------------------------*/

/*! Tests the addition of a network to the database */
void TestNetworkDao::testAddNetwork(){
    //Create the network dao
    NetworkDao networkDao (dbInfo);

    //Information about the network to be added
    NetworkInfo netInfo(0, "test1Name","test3Description", 0);
    try{
	//Invoke method that is being tested
	networkDao.addNetwork(netInfo);

	//Check that network has been added
	QSqlQuery query = getQuery("SELECT Name FROM NeuralNetworks");
	executeQuery(query);
	QCOMPARE(query.size(), 1);
	query.next();
	QCOMPARE(query.value(0).toString(), netInfo.getName());
	QVERIFY( netInfo.getID() != 0);

    }
    catch(SpikeStreamException& ex){
	QFAIL(ex.getMessage().toAscii());
    }
}


/*! Tests the getNeworksInfo method in the NetworkDao.
    This method returns a list containing information about the networks. */
void TestNetworkDao::testGetNetworksInfo(){
    //Add two networks with known properties
    QSqlQuery query = getQuery("INSERT INTO NeuralNetworks (Name, Description) VALUES ('testNetwork1Name', 'testNetwork1Description')");
    executeQuery(query);
    unsigned int network1ID = query.lastInsertId().toUInt();
    query = getQuery("INSERT INTO NeuralNetworks (Name, Description) VALUES ('testNetwork2Name', 'testNetwork2Description')");
    executeQuery(query);
    unsigned int network2ID = query.lastInsertId().toUInt();

    //Create the network dao
    NetworkDao networkDao (dbInfo);

    //Get list of the available networks
    QList<NetworkInfo> netInfoList = networkDao.getNetworksInfo();

    //Should be two entries
    QCOMPARE(netInfoList.size(), 2);

    //Check that list is correct
    QCOMPARE(netInfoList[0].getID(), network1ID);
    QCOMPARE(netInfoList[0].getName(), QString("testNetwork1Name"));
    QCOMPARE(netInfoList[0].getDescription(), QString("testNetwork1Description"));
    QCOMPARE(netInfoList[1].getID(), network2ID);
    QCOMPARE(netInfoList[1].getName(), QString("testNetwork2Name"));
    QCOMPARE(netInfoList[1].getDescription(), QString("testNetwork2Description"));
}


void TestNetworkDao::testGetConnectionGroupsInfo(){
    //Add network with known properties
    QSqlQuery query = getQuery("INSERT INTO NeuralNetworks (Name, Description) VALUES ('testNetwork1Name', 'testNetwork1Description')");
    executeQuery(query);
    unsigned int networkID = query.lastInsertId().toUInt();

    //Add two neuron groups
    QString queryStr = "INSERT INTO NeuronGroups (NeuralNetworkID, Name, Description, Parameters, NeuronTypeID ) VALUES (";
    queryStr += QString::number(networkID) + ", " + "'name1', 'desc1', '" + getNeuronParameterXML() + "', 1)";
    query = getQuery(queryStr);
    executeQuery(query);
    unsigned int neurGrp1ID = query.lastInsertId().toUInt();

    queryStr = "INSERT INTO NeuronGroups (NeuralNetworkID, Name, Description, Parameters, NeuronTypeID ) VALUES (";
    queryStr += QString::number(networkID) + ", " + "'name2', 'desc2', '" + getNeuronParameterXML() + "', 1)";
    query = getQuery(queryStr);
    executeQuery(query);
    unsigned int neurGrp2ID = query.lastInsertId().toUInt();

    //Create two connection group between the two neuron groups
    queryStr = "INSERT INTO ConnectionGroups (NeuralNetworkID, Description, FromNeuronGroupID, ToNeuronGroupID, Parameters, SynapseTypeID ) VALUES (";
    queryStr += QString::number(networkID) + ", 'conngrpdesc1', " + QString::number(neurGrp1ID) + ", " + QString::number(neurGrp2ID) + ", '" + getConnectionParameterXML() + "', 1)";
    query = getQuery(queryStr);
    executeQuery(query);
    unsigned int connGrp1ID = query.lastInsertId().toUInt();

    queryStr = "INSERT INTO ConnectionGroups (NeuralNetworkID, Description, FromNeuronGroupID, ToNeuronGroupID, Parameters, SynapseTypeID ) VALUES (";
    queryStr += QString::number(networkID) + ", 'conngrpdesc2', " + QString::number(neurGrp2ID) + ", " + QString::number(neurGrp1ID) + ", '" + getConnectionParameterXML() + "', 1)";
    query = getQuery(queryStr);
    executeQuery(query);
    unsigned int connGrp2ID = query.lastInsertId().toUInt();

    //Create the network dao and get list of connection groups in the test network
    NetworkDao networkDao (dbInfo);
    QList<ConnectionGroupInfo> connGrpInfoList;
    try{
	connGrpInfoList = networkDao.getConnectionGroupsInfo(networkID);
    }
    catch(SpikeStreamException& ex){
	QFAIL(ex.getMessage().toAscii());
    }

    //Should be 2 entries
    QCOMPARE(connGrpInfoList.size(), 2);

    //Check that list is correct
    QCOMPARE(connGrpInfoList[0].getID(), connGrp1ID);
    QCOMPARE(connGrpInfoList[0].getDescription(), QString("conngrpdesc1"));
    QCOMPARE(connGrpInfoList[0].getFromNeuronGroupID(), neurGrp1ID);
    QCOMPARE(connGrpInfoList[0].getToNeuronGroupID(), neurGrp2ID);
    QCOMPARE(connGrpInfoList[0].getParameterMap()["connparam2"], 1.31);
    QCOMPARE(connGrpInfoList[0].getSynapseType(), (unsigned int)1);

    QCOMPARE(connGrpInfoList[1].getID(), connGrp2ID);
    QCOMPARE(connGrpInfoList[1].getDescription(), QString("conngrpdesc2"));
    QCOMPARE(connGrpInfoList[1].getFromNeuronGroupID(), neurGrp2ID);
    QCOMPARE(connGrpInfoList[1].getToNeuronGroupID(), neurGrp1ID);
    QCOMPARE(connGrpInfoList[1].getSynapseType(), (unsigned int)1);
}



void TestNetworkDao::testGetNeuronGroupBoundingBox(){
    //Adds test network with known properties
    addTestNetwork1();

    NetworkDao networkDao(dbInfo);

    //Check bounding box around neuron group 1
    Box box = networkDao.getNeuronGroupBoundingBox(neurGrp1ID);
    QCOMPARE(box.x1, -1.0f);
    QCOMPARE(box.x2, 9.0f);
    QCOMPARE(box.y1, -5.0f);
    QCOMPARE(box.y2, -1.0f);
    QCOMPARE(box.z1, -6.0f);
    QCOMPARE(box.z2, 0.0f);

    //Check bounding box around neuron group 2
    box = networkDao.getNeuronGroupBoundingBox(neurGrp2ID);
    QCOMPARE(box.x1, -3.0f);
    QCOMPARE(box.x2, 0.0f);
    QCOMPARE(box.y1, -7.0f);
    QCOMPARE(box.y2, -1.0f);
    QCOMPARE(box.z1, 5.0f);
    QCOMPARE(box.z2, 10.0f);
}


void TestNetworkDao::testGetConnectionGroupSize(){
    //Adds test network with known properties
    addTestNetwork1();

    NetworkDao networkDao(dbInfo);
    unsigned int numConns = networkDao.getConnectionGroupSize(connGrp1ID);
    QCOMPARE(numConns, (unsigned int)6);
}


void TestNetworkDao::testGetNeuronGroupsInfo(){
    //Add network with known properties
    QSqlQuery query = getQuery("INSERT INTO NeuralNetworks (Name, Description) VALUES ('testNetwork1Name', 'testNetwork1Description')");
    executeQuery(query);
    unsigned int networkID = query.lastInsertId().toUInt();

    //Add three neuron groups
    QString queryStr = "INSERT INTO NeuronGroups (NeuralNetworkID, Name, Description, Parameters, NeuronTypeID ) VALUES (";
    queryStr += QString::number(networkID) + ", " + "'name1', 'desc1', '" + getNeuronParameterXML() + "', 1)";
    query = getQuery(queryStr);
    executeQuery(query);
    unsigned int neurGrp1ID = query.lastInsertId().toUInt();

    queryStr = "INSERT INTO NeuronGroups (NeuralNetworkID, Name, Description, Parameters, NeuronTypeID ) VALUES (";
    queryStr += QString::number(networkID) + ", " + "'name2', 'desc2', '" + getNeuronParameterXML() + "', 1)";
    query = getQuery(queryStr);
    executeQuery(query);
    unsigned int neurGrp2ID = query.lastInsertId().toUInt();

    queryStr = "INSERT INTO NeuronGroups (NeuralNetworkID, Name, Description, Parameters, NeuronTypeID ) VALUES (";
    queryStr += QString::number(networkID) + ", " + "'name3', 'desc3', '" + getNeuronParameterXML() + "', 1)";
    query = getQuery(queryStr);
    executeQuery(query);
    unsigned int neurGrp3ID = query.lastInsertId().toUInt();

    //Create the network dao and get list of neuron groups in the test network
    NetworkDao networkDao (dbInfo);
    QList<NeuronGroupInfo> neurGrpInfoList;
    try{
	neurGrpInfoList = networkDao.getNeuronGroupsInfo(networkID);
    }
    catch(SpikeStreamException& ex){
	QFAIL(ex.getMessage().toAscii());
    }

    //Should be three entries
    QCOMPARE(neurGrpInfoList.size(), 3);

    //Check that list is correct
    QCOMPARE(neurGrpInfoList[0].getID(), neurGrp1ID);
    QCOMPARE(neurGrpInfoList[0].getName(), QString("name1"));
    QCOMPARE(neurGrpInfoList[0].getDescription(), QString("desc1"));
    QCOMPARE(neurGrpInfoList[0].getNeuronType(), (unsigned int)1);
    QCOMPARE(neurGrpInfoList[0].getParameterMap()["neurparam1"], 1.63);

    QCOMPARE(neurGrpInfoList[1].getID(), neurGrp2ID);
    QCOMPARE(neurGrpInfoList[1].getName(), QString("name2"));
    QCOMPARE(neurGrpInfoList[1].getDescription(), QString("desc2"));

    QCOMPARE(neurGrpInfoList[2].getID(), neurGrp3ID);
    QCOMPARE(neurGrpInfoList[2].getName(), QString("name3"));
    QCOMPARE(neurGrpInfoList[2].getDescription(), QString("desc3"));
}




