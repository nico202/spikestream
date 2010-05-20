//SpikeStream includes
#include "GlobalVariables.h"
#include "SpikeStreamException.h"
#include "TestNetworkDao.h"
#include "NetworkDao.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QHash>

//Other includes
#include <iostream>
using namespace std;


/*----------------------------------------------------------*/
/*-----                     TESTS                      -----*/
/*----------------------------------------------------------*/

/*! Tests the addition of a network to the database */
void TestNetworkDao::testAddNetwork(){
	//Create the network dao
	NetworkDao networkDao (dbInfo);

	//Information about the network to be added
	NetworkInfo netInfo(0, "test1Name","test3Description");
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


void TestNetworkDao::testDeleteAllNetworks(){
	//Add test network
	addTestNetwork1();
	addTestNetwork2();
	addTestAnalysis1();
	addTestArchive1();
	addTestArchive2();

	//Check networks are there
	QSqlQuery query = getQuery("SELECT * FROM Networks");
	executeQuery(query);
	QVERIFY(query.size() > 0);

	//Check neurons are there
	query = getQuery("SELECT * FROM Neurons");
	executeQuery(query);
	QVERIFY(query.size() > 0);

	//Check archives are there
	query = getArchiveQuery("SELECT * FROM Archives");
	executeQuery(query);
	QVERIFY(query.size() > 0);

	//Check analyses are there
	query = getAnalysisQuery("SELECT * FROM Analyses");
	executeQuery(query);
	QVERIFY(query.size() > 0);

	//Invoke test method
	NetworkDao networkDao(dbInfo);
	networkDao.deleteAllNetworks();

	//Check networks have gone
	query = getQuery("SELECT * FROM Networks");
	executeQuery(query);
	QCOMPARE(query.size(), (int)0);

	//Check neurons have gone
	query = getQuery("SELECT * FROM Neurons");
	executeQuery(query);
	QCOMPARE(query.size(), (int)0);

	//Check archive has gone
	query = getArchiveQuery("SELECT * FROM Archives");
	executeQuery(query);
	QCOMPARE(query.size(), (int)0);

	//Check analysis has gone
	query = getAnalysisQuery("SELECT * FROM Analyses");
	executeQuery(query);
	QCOMPARE(query.size(), (int)0);
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



void TestNetworkDao::testGetConnectionCount1(){
	//Add a couple of networks
	addTestNetwork1();
	addTestNetwork2();

	//Check counts are correct for the two networks
	NetworkDao netDao(dbInfo);
	QCOMPARE(netDao.getConnectionCount(testNetID), (unsigned int)6);
	QCOMPARE(netDao.getConnectionCount(testNet2ID), (unsigned int)4);
}


void TestNetworkDao::testGetConnectionCount2(){
	//Add a couple of networks
	addTestNetwork1();
	addTestNetwork2();

	//Build list of connection groups for each network
	QList<ConnectionGroup*> conGrpList1;
	ConnectionGroup conGrp1(ConnectionGroupInfo(connGrp1ID, "DESC", 0, 0, QHash<QString,double>(), 1));
	conGrpList1.append(&conGrp1);

	QList<ConnectionGroup*> conGrpList2;
	ConnectionGroup conGrp2(ConnectionGroupInfo(connGrp21ID, "DESC", 0, 0, QHash<QString,double>(), 1));
	conGrpList2.append(&conGrp1);
	conGrpList2.append(&conGrp2);

	//Check counts are correct for the two lists
	NetworkDao netDao(dbInfo);
	QCOMPARE(netDao.getConnectionCount(conGrpList1), (unsigned int)6);
	QCOMPARE(netDao.getConnectionCount(conGrpList2), (unsigned int)10);
}


void TestNetworkDao::testGetConnections1(){
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


void TestNetworkDao::testGetConnections2(){
	//Add the test network
	addTestNetwork1();

	//The network dao being tested
	NetworkDao networkDao(dbInfo);

	//Test all connections mode
	unsigned int connectionMode = 0;
	QList<Connection*> conList = networkDao.getConnections(connectionMode, testNeurIDList[0], 0);
	QCOMPARE(conList.size(), (int)0);

	//Test single neuron all connections
	connectionMode = 0;
	connectionMode |= CONNECTION_MODE_ENABLED;
	conList = networkDao.getConnections(connectionMode, testNeurIDList[0], 0);
	QCOMPARE(conList.size(), (int)3);
	QCOMPARE(conList[0]->fromNeuronID, testNeurIDList[0]);
	QCOMPARE(conList[0]->toNeuronID, testNeurIDList[1]);
	QCOMPARE(conList[1]->toNeuronID, testNeurIDList[2]);
	QCOMPARE(conList[2]->toNeuronID, testNeurIDList[3]);

	//Test single neuron from connections
	connectionMode = 0;
	connectionMode |= CONNECTION_MODE_ENABLED;
	connectionMode |= SHOW_FROM_CONNECTIONS;
	conList = networkDao.getConnections(connectionMode, testNeurIDList[4], 0);
	QCOMPARE(conList.size(), (int)2);
	QCOMPARE(conList[0]->fromNeuronID, testNeurIDList[4]);
	QCOMPARE(conList[1]->fromNeuronID, testNeurIDList[4]);
	QCOMPARE(conList[1]->toNeuronID, testNeurIDList[3]);
	QCOMPARE(conList[0]->toNeuronID, testNeurIDList[1]);

	//Test single neuron to connections
	connectionMode = 0;
	connectionMode |= CONNECTION_MODE_ENABLED;
	connectionMode |= SHOW_TO_CONNECTIONS;
	conList = networkDao.getConnections(connectionMode, testNeurIDList[3], 0);
	QCOMPARE(conList.size(), (int)2);
	QCOMPARE(conList[0]->fromNeuronID, testNeurIDList[0]);
	QCOMPARE(conList[1]->fromNeuronID, testNeurIDList[4]);
	QCOMPARE(conList[1]->toNeuronID, testNeurIDList[3]);
	QCOMPARE(conList[0]->toNeuronID, testNeurIDList[3]);

	//Test between connections
	connectionMode = 0;
	connectionMode |= CONNECTION_MODE_ENABLED;
	connectionMode |= SHOW_BETWEEN_CONNECTIONS;
	conList = networkDao.getConnections(connectionMode, testNeurIDList[3], testNeurIDList[2]);
	QCOMPARE(conList.size(), (int)1);
	QCOMPARE(conList[0]->fromNeuronID, testNeurIDList[3]);
	QCOMPARE(conList[0]->toNeuronID, testNeurIDList[2]);

	//Test weight filtering
	testConnIDList.append(addTestConnection(connGrp1ID, testNeurIDList[3], testNeurIDList[1], -1.95, 1.6));
	connectionMode = 0;
	connectionMode |= CONNECTION_MODE_ENABLED;
	connectionMode |= SHOW_FROM_CONNECTIONS;
	connectionMode |= SHOW_POSITIVE_CONNECTIONS;
	conList = networkDao.getConnections(connectionMode, testNeurIDList[3], 0);
	QCOMPARE(conList.size(), (int)1);
	QCOMPARE(conList[0]->fromNeuronID, testNeurIDList[3]);
	QCOMPARE(conList[0]->toNeuronID, testNeurIDList[2]);
	QCOMPARE(conList[0]->weight, 0.6f);

	connectionMode = 0;
	connectionMode |= CONNECTION_MODE_ENABLED;
	connectionMode |= SHOW_FROM_CONNECTIONS;
	connectionMode |= SHOW_NEGATIVE_CONNECTIONS;
	conList = networkDao.getConnections(connectionMode, testNeurIDList[3], 0);
	QCOMPARE(conList.size(), (int)1);
	QCOMPARE(conList[0]->fromNeuronID, testNeurIDList[3]);
	QCOMPARE(conList[0]->toNeuronID, testNeurIDList[1]);
	QCOMPARE(conList[0]->weight, -1.95f);
}


/*! Connections in test network 1 are:
	0->1    4->3
	0->2    4->1
	0->3    3->2
	Numbers are the index of the neuron id in testNeurIDList. */
void TestNetworkDao::testGetAllFromConnections(){
	//Add the test network
	addTestNetwork1();

	//Create the network dao and run test method
	NetworkDao networkDao (dbInfo);
	QHash<unsigned int, QHash<unsigned int, bool> > fromConMap;
	networkDao.getAllFromConnections(testNetID, fromConMap);
	QCOMPARE(fromConMap[testNeurIDList[0]].size(), (int)3);
	QCOMPARE(fromConMap[testNeurIDList[1]].size(), (int)0);
	QCOMPARE(fromConMap[testNeurIDList[2]].size(), (int)0);
	QCOMPARE(fromConMap[testNeurIDList[3]].size(), (int)1);
	QCOMPARE(fromConMap[testNeurIDList[4]].size(), (int)2);
}


/*! Connections in test network 1 are:
	0->1    4->3
	0->2    4->1
	0->3    3->2
	Numbers are the index of the neuron id in testNeurIDList. */
void TestNetworkDao::testGetAllToConnections(){
	//Add the test network
	addTestNetwork1();

	//Create the network dao and run test method
	NetworkDao networkDao (dbInfo);
	QHash<unsigned int, QHash<unsigned int, bool> > toConMap;
	networkDao.getAllToConnections(testNetID, toConMap);
	QCOMPARE(toConMap[testNeurIDList[0]].size(), (int)0);
	QCOMPARE(toConMap[testNeurIDList[1]].size(), (int)2);
	QCOMPARE(toConMap[testNeurIDList[2]].size(), (int)2);
	QCOMPARE(toConMap[testNeurIDList[3]].size(), (int)2);
	QCOMPARE(toConMap[testNeurIDList[4]].size(), (int)0);
}


void TestNetworkDao::testGetDefaultNeuronParameters(){
	try{
		//Get the parameter map - no need for data
		NetworkDao networkDao(dbInfo);
		QHash<QString, double> paramMap = networkDao.getDefaultNeuronParameters(1);

		//Check the default values of the parameters are correct
		QCOMPARE(paramMap.size(), (int)1);
		QVERIFY(paramMap.contains("a"));
		QCOMPARE(paramMap["a"], 1.0);
	}
	catch(SpikeStreamException ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("Unrecognized exception thrown.");
	}
}


void TestNetworkDao::testGetDefaultSynapseParameters(){

}


/*! Connections in test network 1 are:
	0->1    4->3
	0->2    4->1
	0->3    3->2
	Numbers are the index of the neuron id in testNeurIDList. */
void TestNetworkDao::testGetFromConnections(){
	//Add the test network
	addTestNetwork1();

	//Create the network dao and run test method
	NetworkDao networkDao (dbInfo);
	QList<unsigned int> fromConList = networkDao.getFromConnections(testNeurIDList.at(0));
	QCOMPARE(fromConList.size(), (int)3);
	QCOMPARE(fromConList.at(0), (unsigned int)testNeurIDList[1]);
	QCOMPARE(fromConList.at(1), (unsigned int)testNeurIDList[2]);
	QCOMPARE(fromConList.at(2), (unsigned int)testNeurIDList[3]);

	fromConList = networkDao.getFromConnections(testNeurIDList.at(2));
	QCOMPARE(fromConList.size(), (int)0);
}


/*! Connections in test network 1 are:
	0->1    4->3
	0->2    4->1
	0->3    3->2
	Numbers are the index of the neuron id in testNeurIDList. */
void TestNetworkDao::testGetToConnections(){
	//Add the test network
	addTestNetwork1();

	//Create the network dao and run test method
	NetworkDao networkDao (dbInfo);
	QList<unsigned int> toConList = networkDao.getToConnections(testNeurIDList.at(1));
	QCOMPARE(toConList.size(), (int)2);
	QCOMPARE(toConList.at(0), (unsigned int)testNeurIDList[0]);
	QCOMPARE(toConList.at(1), (unsigned int)testNeurIDList[4]);

	toConList = networkDao.getToConnections(testNeurIDList.at(4));
	QCOMPARE(toConList.size(), (int)0);
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


void TestNetworkDao::testGetNeuronCount1(){
	//Adds test network with known properties
	addTestNetwork1();

	//Create test class
	NetworkDao networkDao(dbInfo);

	//Box that should intersect with test network 1
	Box intersectBox1(-1,-5,-6,0,0,0);
	Box intersectBox2(0,-1,0,2,2,2);
	Box intersectBox3(1,1,1,2,2,2);

	QVERIFY(networkDao.getNeuronCount(testNetID, intersectBox1) > 0);
	QVERIFY(networkDao.getNeuronCount(testNetID, intersectBox2) > 0);
	QVERIFY(networkDao.getNeuronCount(testNetID, intersectBox3) == 0);
}


void TestNetworkDao::testGetNeuronCount2(){
	//Add a couple of networks
	addTestNetwork1();
	addTestNetwork2();

	//Check counts are correct for the two networks
	NetworkDao netDao(dbInfo);
	QCOMPARE(netDao.getNeuronCount(testNetID), (unsigned int)5);
	QCOMPARE(netDao.getNeuronCount(testNet2ID), (unsigned int)4);
}



void TestNetworkDao::testGetNeuronCount3(){
	//Add a couple of networks
	addTestNetwork1();
	addTestNetwork2();

	//Build list of neuron groups for each network
	QList<NeuronGroup*> neurGrpList1;
	NeuronGroup neurGrp1(NeuronGroupInfo(neurGrp1ID, "NAME", "DESC", QHash<QString,double>(), 1));
	neurGrpList1.append(&neurGrp1);

	QList<NeuronGroup*> neurGrpList2;
	NeuronGroup neurGrp2(NeuronGroupInfo(neurGrp21ID, "NAME", "DESC", QHash<QString,double>(), 1));
	NeuronGroup neurGrp3(NeuronGroupInfo(neurGrp22ID, "NAME", "DESC", QHash<QString,double>(), 1));
	neurGrpList2.append(&neurGrp2);
	neurGrpList2.append(&neurGrp3);

	//Check counts are correct for the two lists
	NetworkDao netDao(dbInfo);
	QCOMPARE(netDao.getNeuronCount(neurGrpList1), (unsigned int)3);
	QCOMPARE(netDao.getNeuronCount(neurGrpList2), (unsigned int)4);
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
	QCOMPARE(neurGrpInfoList[0].getNeuronTypeID(), (unsigned int)1);
	QCOMPARE(neurGrpInfoList[0].getParameterMap()["neurparam1"], 1.63);

	QCOMPARE(neurGrpInfoList[1].getID(), neurGrp2ID);
	QCOMPARE(neurGrpInfoList[1].getName(), QString("name2"));
	QCOMPARE(neurGrpInfoList[1].getDescription(), QString("desc2"));

	QCOMPARE(neurGrpInfoList[2].getID(), neurGrp3ID);
	QCOMPARE(neurGrpInfoList[2].getName(), QString("name3"));
	QCOMPARE(neurGrpInfoList[2].getDescription(), QString("desc3"));
}


void TestNetworkDao::testGetNeuronIDs(){
	//Add test networks - only want to select neurons from the second
	addTestNetwork1();
	addTestNetwork2();

	//Invoke method being tested
	NetworkDao networkDao(dbInfo);
	QList<unsigned int> neurIDList = networkDao.getNeuronIDs(testNet2ID);

	//Check that list is correct
	QCOMPARE(neurIDList.size(), 4);//4 test neurons in list

	//Convert neurons in test network 2 into a map
	QHash<unsigned int, bool> testNet2NeurMap;
	foreach(unsigned int neurID, testNeurIDList2)
		testNet2NeurMap[neurID] = true;

	//Check that all neurons in list are in map
	foreach(unsigned int neurID, neurIDList)
		if(!testNet2NeurMap.contains(neurID))
			QFAIL("Neuron ID list contains neuron that is not in the network.");
}


void TestNetworkDao::testGetNeuronParameters(){
	//Add test network
	addTestNetwork1();

	try{
		//Get the values of the parameters - this should be the default
		NetworkDao networkDao(dbInfo);
		NeuronGroupInfo neurGrpInfo (neurGrp1ID, "name", "description", QHash<QString, double>(), 1);
		QHash<QString, double> neurParamMap = networkDao.getNeuronParameters(neurGrpInfo);

		//Check parameters
		QCOMPARE(neurParamMap.size(), (int)1);
		QCOMPARE(neurParamMap["a"], 1.0);
	}
	catch(SpikeStreamException ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("Unrecognized exception thrown.");
	}
}


void TestNetworkDao::testGetNeuronType(){
	try{
		NetworkDao networkDao(dbInfo);

		//Test type 1 neurons
		NeuronType type = networkDao.getNeuronType(1);
		QCOMPARE(type.getDescription(), QString("Izhikevich Neuron"));
		QCOMPARE(type.getParameterTableName(), QString("IzhikevichNeuronParameters"));
		QCOMPARE(type.getParameterInfoList().size(), (int)1);
		QCOMPARE(type.getParameterInfoList().at(0).getName(), QString("a"));
		QCOMPARE(type.getParameterInfoList().at(0).getDescription(), QString("Time scale of the recovery variable."));

		//Test type 2 neurons
		type = networkDao.getNeuronType(2);
		QCOMPARE(type.getDescription(), QString("Weightless Neuron"));
		QCOMPARE(type.getParameterTableName(), QString("WeightlessNeuronParameters"));
		QCOMPARE(type.getParameterInfoList().size(), (int)1);
		QCOMPARE(type.getParameterInfoList().at(0).getName(), QString("Generalization"));
		QCOMPARE(type.getParameterInfoList().at(0).getDescription(), QString("Degree of match using hamming distance with incoming pattern"));
	}
	catch(SpikeStreamException ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("Unrecognized exception thrown.");
	}
}


/*! Note this test will need to be updated if the default types change. */
void TestNetworkDao::testGetNeuronTypes(){
	//Invoke method being tested
	NetworkDao networkDao(dbInfo);
	try{
		QList<NeuronType> neurTypesList = networkDao.getNeuronTypes();
		QCOMPARE(neurTypesList.size(), (int)2);

		//Check ids
		QCOMPARE(neurTypesList.at(0).getID(), (unsigned int)1);
		QCOMPARE(neurTypesList.at(1).getID(), (unsigned int)2);

		//Check descriptions
		QCOMPARE(neurTypesList.at(0).getDescription(), QString("Izhikevich Neuron"));
		QCOMPARE(neurTypesList.at(1).getDescription(), QString("Weightless Neuron"));

		//Check parameter table names
		QCOMPARE(neurTypesList.at(0).getParameterTableName(), QString("IzhikevichNeuronParameters"));
		QCOMPARE(neurTypesList.at(1).getParameterTableName(), QString("WeightlessNeuronParameters"));

		//Check class names
		QCOMPARE(neurTypesList.at(0).getClassLibaryName(), QString(""));
		QCOMPARE(neurTypesList.at(1).getClassLibaryName(), QString(""));
	}
	catch(SpikeStreamException ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("Unrecognized exception thrown.");
	}
}


void TestNetworkDao::testGetSynapseParameters(){

}


void TestNetworkDao::testGetSynapseType(){

}


/*! Note this test will need to be updated if the default types change. */
void TestNetworkDao::testGetSynapseTypes(){
	//Invoke method being tested
	NetworkDao networkDao(dbInfo);
	try{
		QList<SynapseType> synTypesList = networkDao.getSynapseTypes();
		QCOMPARE(synTypesList.size(), (int)2);

		//Check ids
		QCOMPARE(synTypesList.at(0).getID(), (unsigned int)1);
		QCOMPARE(synTypesList.at(1).getID(), (unsigned int)2);

		//Check descriptions
		QCOMPARE(synTypesList.at(0).getDescription(), QString("Izhikevich Synapse 1"));
		QCOMPARE(synTypesList.at(1).getDescription(), QString("Weightless Synapse"));

		//Check parameter table names
		QCOMPARE(synTypesList.at(0).getParameterTableName(), QString("IzhikevichSynapse1Parameters"));
		QCOMPARE(synTypesList.at(1).getParameterTableName(), QString("WeightlessSynapseParameters"));

		//Check class names
		QCOMPARE(synTypesList.at(0).getClassLibaryName(), QString(""));
		QCOMPARE(synTypesList.at(1).getClassLibaryName(), QString(""));
	}
	catch(SpikeStreamException ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("Unrecognized exception thrown.");
	}
}


void TestNetworkDao::testGetWeightlessNeuron(){
	//Add test network
	addWeightlessTestNetwork1();

	//Invoke method being tested
	NetworkDao networkDao(dbInfo);
	try{
		WeightlessNeuron* neuron = networkDao.getWeightlessNeuron(testNeurIDList[1]);
		QHash<unsigned int, QList<unsigned int> > conMap = neuron->getConnectionMap();
		QCOMPARE(conMap.size(), (int)2);
		QCOMPARE(conMap[testNeurIDList[0]][0], (unsigned int)0);
		QCOMPARE(conMap[testNeurIDList[4]][0], (unsigned int)1);
		QList<byte*> trainingDataList = neuron->getTrainingData();
		QCOMPARE(trainingDataList.size(), (int)2);
		QVERIFY( bitsEqual(trainingDataList[0], "10000000", 1) );
		QVERIFY( bitsEqual(trainingDataList[1], "11000000", 1) );
		delete neuron;

		//Check a second neuron
		neuron = networkDao.getWeightlessNeuron(testNeurIDList[2]);
		conMap = neuron->getConnectionMap();
		QCOMPARE(conMap.size(), (int)2);
		QCOMPARE(conMap[testNeurIDList[0]][0], (unsigned int)0);
		QCOMPARE(conMap[testNeurIDList[3]][0], (unsigned int)1);
		trainingDataList = neuron->getTrainingData();
		QCOMPARE(trainingDataList.size(), (int)2);
		QVERIFY( bitsEqual(trainingDataList[0], "00000000", 0) );
		QVERIFY( bitsEqual(trainingDataList[1], "11000000", 0) );
		delete neuron;
	}
	catch(SpikeStreamException ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("Unrecognized exception thrown.");
	}
}


void TestNetworkDao::testGetNeuronGroupID(){
	//Add test networks - only want to select neurons from the second
	addTestNetwork1();
	addTestNetwork2();

	//Get id of neuron from second network
	NetworkDao networkDao(dbInfo);
	QCOMPARE(networkDao.getNeuronGroupID(testNeurIDList2[0]), neurGrp21ID);
}


void TestNetworkDao::testIsWeightlessNetwork(){
	//Add test networks
	addWeightlessTestNetwork1();
	addTestNetwork2();

	//Create test class
	NetworkDao networkDao(dbInfo);

	//Neuron in group 1 should be weightless
	QVERIFY( networkDao.isWeightlessNetwork(testNetID) );
	QVERIFY( !networkDao.isWeightlessNetwork(testNet2ID) );
}


void TestNetworkDao::testIsWeightlessNeuron(){
	//Add test networks
	addWeightlessTestNetwork1();
	addTestNetwork2();

	//Create test class
	NetworkDao networkDao(dbInfo);

	//Neuron in group 1 should be weightless
	QVERIFY(networkDao.isWeightlessNeuron(testNeurIDList[0]));
	QVERIFY(!networkDao.isWeightlessNeuron(testNeurIDList2[0]));
}


void TestNetworkDao::testSetNeuronParameters(){
	//Add test network
	addTestNetwork1();

	try{
		//Build neuron group info and parameter map for the update
		NeuronGroupInfo info(neurGrp1ID, "name", "description", QHash<QString, double>(), 1);
		QHash<QString, double> paramMap;
		paramMap["a"] = 27.3;

		//Check values of parameters
		NetworkDao netDao(dbInfo);
		QHash<QString, double> resultsMap = netDao.getNeuronParameters(info);
		QCOMPARE(resultsMap.size(), (int)1);
		QVERIFY(resultsMap.contains("a"));
		QCOMPARE(resultsMap["a"], 1.0);

		//Set parameters to new values
		netDao.setNeuronParameters(info, paramMap);

		//Check that parameters have changed
		resultsMap = netDao.getNeuronParameters(info);
		QCOMPARE(resultsMap.size(), (int)1);
		QVERIFY(resultsMap.contains("a"));
		QCOMPARE(resultsMap["a"], 27.3);
	}
	catch(SpikeStreamException ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("Unrecognized exception thrown.");
	}
}


void TestNetworkDao::testSetSynapseParameters(){

}


void TestNetworkDao::testSetTempWeight(){
	//Add test network
	addTestNetwork1();

	//Check the temp weight of a neuron
	QSqlQuery query = getQuery("SELECT TempWeight FROM Connections WHERE FromNeuronID = " + QString::number(testNeurIDList[4]) + " AND ToNeuronID=" + QString::number(testNeurIDList[3]));
	executeQuery(query);
	query.next();
	QCOMPARE(Util::getDouble(query.value(0).toString()), 0.0);

	//Execute test method
	NetworkDao networkDao(dbInfo);
	networkDao.setTempWeight(testNeurIDList[4],testNeurIDList[3], 2.132);

	//Check weight has been updated
	query = getQuery("SELECT TempWeight FROM Connections WHERE FromNeuronID = " + QString::number(testNeurIDList[4]) + " AND ToNeuronID=" + QString::number(testNeurIDList[3]));
	executeQuery(query);
	query.next();
	QCOMPARE(Util::getDouble(query.value(0).toString()), 2.132);
}


/*----------------------------------------------------------*/
/*-----              PRIVATE METHODS                   -----*/
/*----------------------------------------------------------*/

bool TestNetworkDao::bitsEqual(const unsigned char* byteArr, const QString bitPattStr, int output){
	if(byteArr[0] != output)
		return false;

	for(int i=0; i<bitPattStr.length(); ++i){
		if(bitPattStr[i] == '1' && (byteArr[1 + i/8] & ( 1<<(i % 8) )))//1 is equal
			;//do nothing
		else if(bitPattStr[i] == '0' && !(byteArr[1 + i/8] & ( 1<<(i % 8) )))//0 is equal
			;//Do nothing
		else
			return false;//String and byte array do not match
	}
	return true;
}

