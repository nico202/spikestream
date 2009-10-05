//SpikeStream includes
#include "SpikeStreamException.h"
#include "TestNetworkDao.h"
#include "NetworkDao.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;

//void TestNetworkDao::init(){
//    cout<<"HELLO FROM INIT"<<endl;
//}
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
	QSqlQuery query = getQuery("SELECT Name FROM Networks");
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


void TestNetworkDao::testAddWeightlessConnection(){
    //Add a test network to give us a valid connection id
    addTestNetwork1();

    //The network dao being tested
    NetworkDao networkDao(dbInfo);

    //Add a pattern index to the database
    try{
	networkDao.addWeightlessConnection(testConnIDList[0], 2);
    }
    catch(SpikeStreamException& ex){
	QFAIL(ex.getMessage().toAscii());
    }

    //Check pattern index has been added
    QSqlQuery query = getQuery("SELECT ConnectionID, PatternIndex FROM WeightlessConnections");
    executeQuery(query);

    //Should be 1 entry
    QCOMPARE(query.size(), (int)1);

    //Check entries are correct
    query.next();
    QCOMPARE(query.value(0).toUInt(), testConnIDList[0]);
    QCOMPARE(query.value(1).toUInt(), (unsigned int)2);
}


void TestNetworkDao::testAddWeightlessNeuronTrainingPattern(){
    //Add a test network to give us a valid connection id
    addTestNetwork1();

    //The network dao being tested
    NetworkDao networkDao(dbInfo);

    //Create the pattern array and add it to the database
    unsigned char* patternArray = new unsigned char[3];
    patternArray[0] = 55;
    patternArray[1] = 2;
    patternArray[2] = 231;
    unsigned int patternID = networkDao.addWeightlessNeuronTrainingPattern(testNeurIDList[1], patternArray, 1, 3);

    //Check training pattern has been added
    QSqlQuery query = getQuery("Select NeuronID, PatternID, Output, Pattern FROM WeightlessNeuronTrainingPatterns");
    executeQuery(query);

    //Should be 1 entry
    QCOMPARE(query.size(), (int)1);

    //Check entries are correct
    query.next();
    QCOMPARE(query.value(0).toUInt(), testNeurIDList[1]);
    QCOMPARE(query.value(1).toUInt(), patternID);
    QCOMPARE(query.value(2).toBool(), true);

    QByteArray byteArrayRes = query.value(3).toByteArray();
    QCOMPARE(byteArrayRes.size(), 3);
    QCOMPARE((unsigned char)byteArrayRes[0], (unsigned char)55);
    QCOMPARE((unsigned char)byteArrayRes[1], (unsigned char)2);
    QCOMPARE((unsigned char)byteArrayRes[2], (unsigned char)231);
}


void TestNetworkDao::testDeleteNetwork(){
    //Add test network
    addTestNetwork1();

    //Check that test network is in database
    QSqlQuery query = getQuery("SELECT * FROM Networks WHERE NetworkID = " + QString::number(testNetID));
    executeQuery(query);

    //Should be a single network
    QCOMPARE(query.size(), (int)1);

    //Invoke method being tested
    NetworkDao networkDao(dbInfo);
    networkDao.deleteNetwork(testNetID);

    //Check to see if network with this id has been removed from the database
    query = getQuery("SELECT * FROM Networks WHERE NetworkID = " + QString::number(testNetID));
    executeQuery(query);

    //Should be no networks
    QCOMPARE(query.size(), (int)0);
}


void TestNetworkDao::testGetConnections(){
    //Add a test network to give us a valid connection id
    addTestNetwork1();

    //The network dao being tested
    NetworkDao networkDao(dbInfo);

    //Execute method
    QList<Connection> conList = networkDao.getConnections(testNeurIDList[4], testNeurIDList[3]);

    //Should be a single connection
    QCOMPARE(conList.size(), (int)1);

    //Check values of connection
    QCOMPARE(conList[0].fromNeuronID, testNeurIDList[4]);
    QCOMPARE(conList[0].toNeuronID, testNeurIDList[3]);
    QCOMPARE(conList[0].delay, 1.4f);
    QCOMPARE(conList[0].weight, 0.4f);
}


/*! Tests the getNeworksInfo method in the NetworkDao.
    This method returns a list containing information about the networks. */
void TestNetworkDao::testGetNetworksInfo(){
    //Add two networks with known properties
    QSqlQuery query = getQuery("INSERT INTO Networks (Name, Description) VALUES ('testNetwork1Name', 'testNetwork1Description')");
    executeQuery(query);
    unsigned int network1ID = query.lastInsertId().toUInt();
    query = getQuery("INSERT INTO Networks (Name, Description) VALUES ('testNetwork2Name', 'testNetwork2Description')");
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
    QSqlQuery query = getQuery("INSERT INTO Networks (Name, Description) VALUES ('testNetwork1Name', 'testNetwork1Description')");
    executeQuery(query);
    unsigned int networkID = query.lastInsertId().toUInt();

    //Add two neuron groups
    QString queryStr = "INSERT INTO NeuronGroups (NetworkID, Name, Description, Parameters, NeuronTypeID ) VALUES (";
    queryStr += QString::number(networkID) + ", " + "'name1', 'desc1', '" + getNeuronParameterXML() + "', 1)";
    query = getQuery(queryStr);
    executeQuery(query);
    unsigned int neurGrp1ID = query.lastInsertId().toUInt();

    queryStr = "INSERT INTO NeuronGroups (NetworkID, Name, Description, Parameters, NeuronTypeID ) VALUES (";
    queryStr += QString::number(networkID) + ", " + "'name2', 'desc2', '" + getNeuronParameterXML() + "', 1)";
    query = getQuery(queryStr);
    executeQuery(query);
    unsigned int neurGrp2ID = query.lastInsertId().toUInt();

    //Create two connection group between the two neuron groups
    queryStr = "INSERT INTO ConnectionGroups (NetworkID, Description, FromNeuronGroupID, ToNeuronGroupID, Parameters, SynapseTypeID ) VALUES (";
    queryStr += QString::number(networkID) + ", 'conngrpdesc1', " + QString::number(neurGrp1ID) + ", " + QString::number(neurGrp2ID) + ", '" + getConnectionParameterXML() + "', 1)";
    query = getQuery(queryStr);
    executeQuery(query);
    unsigned int connGrp1ID = query.lastInsertId().toUInt();

    queryStr = "INSERT INTO ConnectionGroups (NetworkID, Description, FromNeuronGroupID, ToNeuronGroupID, Parameters, SynapseTypeID ) VALUES (";
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
    QSqlQuery query = getQuery("INSERT INTO Networks (Name, Description) VALUES ('testNetwork1Name', 'testNetwork1Description')");
    executeQuery(query);
    unsigned int networkID = query.lastInsertId().toUInt();

    //Add three neuron groups
    QString queryStr = "INSERT INTO NeuronGroups (NetworkID, Name, Description, Parameters, NeuronTypeID ) VALUES (";
    queryStr += QString::number(networkID) + ", " + "'name1', 'desc1', '" + getNeuronParameterXML() + "', 1)";
    query = getQuery(queryStr);
    executeQuery(query);
    unsigned int neurGrp1ID = query.lastInsertId().toUInt();

    queryStr = "INSERT INTO NeuronGroups (NetworkID, Name, Description, Parameters, NeuronTypeID ) VALUES (";
    queryStr += QString::number(networkID) + ", " + "'name2', 'desc2', '" + getNeuronParameterXML() + "', 1)";
    query = getQuery(queryStr);
    executeQuery(query);
    unsigned int neurGrp2ID = query.lastInsertId().toUInt();

    queryStr = "INSERT INTO NeuronGroups (NetworkID, Name, Description, Parameters, NeuronTypeID ) VALUES (";
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




