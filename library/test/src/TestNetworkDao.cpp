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
	NetworkDao networkDao (networkDBInfo);

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
	NetworkDao networkDao(networkDBInfo);

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
	NetworkDao networkDao(networkDBInfo);

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

//FIXME APPLY TO DATABASE MANAGER
//void TestNetworkDao::testDeleteAllNetworks(){
//	//Add test network
//	addTestNetwork1();
//	addTestNetwork2();
//	addTestAnalysis1();
//	addTestArchive1();
//	addTestArchive2();

//	//Check networks are there
//	QSqlQuery query = getQuery("SELECT * FROM Networks");
//	executeQuery(query);
//	QVERIFY(query.size() > 0);

//	//Check neurons are there
//	query = getQuery("SELECT * FROM Neurons");
//	executeQuery(query);
//	QVERIFY(query.size() > 0);

//	//Check archives are there
//	query = getArchiveQuery("SELECT * FROM Archives");
//	executeQuery(query);
//	QVERIFY(query.size() > 0);

//	//Check analyses are there
//	query = getAnalysisQuery("SELECT * FROM Analyses");
//	executeQuery(query);
//	QVERIFY(query.size() > 0);

//	//Invoke test method
//	NetworkDao networkDao(networkDBInfo);
//	networkDao.deleteAllNetworks();

//	//Check networks have gone
//	query = getQuery("SELECT * FROM Networks");
//	executeQuery(query);
//	QCOMPARE(query.size(), (int)0);

//	//Check neurons have gone
//	query = getQuery("SELECT * FROM Neurons");
//	executeQuery(query);
//	QCOMPARE(query.size(), (int)0);

//	//Check archive has gone
//	query = getArchiveQuery("SELECT * FROM Archives");
//	executeQuery(query);
//	QCOMPARE(query.size(), (int)0);

//	//Check analysis has gone
//	query = getAnalysisQuery("SELECT * FROM Analyses");
//	executeQuery(query);
//	QCOMPARE(query.size(), (int)0);
//}


void TestNetworkDao::testGetConnectionCount1(){
	//Add a couple of networks
	addTestNetwork1();
	addTestNetwork2();

	//Check counts are correct for the two networks
	NetworkDao netDao(networkDBInfo);
	QCOMPARE(netDao.getConnectionCount(testNetID), (unsigned int)6);
	QCOMPARE(netDao.getConnectionCount(testNet2ID), (unsigned int)4);
}


void TestNetworkDao::testGetConnectionCount2(){
	NetworkDao netDao(networkDBInfo);

	//Add a couple of networks
	addTestNetwork1();
	addTestNetwork2();

	//Build list of connection groups for each network
	QList<ConnectionGroup*> conGrpList1;
	ConnectionGroup conGrp1(ConnectionGroupInfo(connGrp1ID, "DESC", 0, 0, QHash<QString,double>(), netDao.getSynapseType(1)));
	conGrpList1.append(&conGrp1);

	QList<ConnectionGroup*> conGrpList2;
	ConnectionGroup conGrp2(ConnectionGroupInfo(connGrp21ID, "DESC", 0, 0, QHash<QString,double>(), netDao.getSynapseType(1)));
	conGrpList2.append(&conGrp1);
	conGrpList2.append(&conGrp2);

	//Check counts are correct for the two lists
	QCOMPARE(netDao.getConnectionCount(conGrpList1), (unsigned int)6);
	QCOMPARE(netDao.getConnectionCount(conGrpList2), (unsigned int)10);
}


void TestNetworkDao::testGetConnections1(){
	//Add a test network to give us a valid connection id
	addTestNetwork1();

	//The network dao being tested
	NetworkDao networkDao(networkDBInfo);

	//Execute method
	QList< QPair<unsigned, Connection> > conPairList = networkDao.getConnections(testNeurIDList[4], testNeurIDList[3]);

	//Should be a single connection
	QCOMPARE(conPairList.size(), (int)1);

	//Check values of connection
	QCOMPARE(conPairList[0].second.getFromNeuronID(), testNeurIDList[4]);
	QCOMPARE(conPairList[0].second.getToNeuronID(), testNeurIDList[3]);
	QCOMPARE(conPairList[0].second.getDelay(), 1.4f);
	QCOMPARE(conPairList[0].second.getWeight(), 0.4f);
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
	NetworkDao networkDao (networkDBInfo);
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
	NetworkDao networkDao (networkDBInfo);
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
		NetworkDao networkDao(networkDBInfo);
		QHash<QString, double> resultsMap = networkDao.getDefaultNeuronParameters(1);

		//Check the default values of the parameters are correct
		QCOMPARE(resultsMap.size(), (int)8);
		QVERIFY(resultsMap.contains("a"));
		QCOMPARE(resultsMap["a"], 0.02);
		QVERIFY(resultsMap.contains("b"));
		QCOMPARE(resultsMap["b"], 0.2);
		QVERIFY(resultsMap.contains("c_1"));
		QCOMPARE(resultsMap["c_1"], 15.0);
		QVERIFY(resultsMap.contains("d_1"));
		QCOMPARE(resultsMap["d_1"], 8.0);
		QVERIFY(resultsMap.contains("d_2"));
		QCOMPARE(resultsMap["d_2"], 6.0);
		QVERIFY(resultsMap.contains("v"));
		QCOMPARE(resultsMap["v"], -65.0);
		QVERIFY(resultsMap.contains("sigma"));
		QCOMPARE(resultsMap["sigma"], 5.0);
		QVERIFY(resultsMap.contains("seed"));
		QCOMPARE(resultsMap["seed"], 123456789.0);
	}
	catch(SpikeStreamException ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("Unrecognized exception thrown.");
	}
}


void TestNetworkDao::testGetDefaultSynapseParameters(){
	try{
		//Get the parameter map - no need for data
		NetworkDao networkDao(networkDBInfo);
		QHash<QString, double> resultsMap = networkDao.getDefaultSynapseParameters(1);

		//Check the default values of the parameters are correct
		QCOMPARE(resultsMap.size(), (int)2);
		QVERIFY(resultsMap.contains("Learning"));
		QCOMPARE(resultsMap["Learning"], 0.0);
		QVERIFY(resultsMap.contains("Disable"));
		QCOMPARE(resultsMap["Disable"], 0.0);
	}
	catch(SpikeStreamException ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("Unrecognized exception thrown.");
	}
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
	NetworkDao networkDao (networkDBInfo);
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
	NetworkDao networkDao (networkDBInfo);
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
	NetworkDao networkDao (networkDBInfo);

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
	NetworkDao networkDao (networkDBInfo);
	QList<ConnectionGroupInfo> connGrpInfoList;
	try{
		networkDao.getConnectionGroupsInfo(networkID, connGrpInfoList);
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
	QCOMPARE(connGrpInfoList[0].getSynapseTypeID(), (unsigned int)1);

	QCOMPARE(connGrpInfoList[1].getID(), connGrp2ID);
	QCOMPARE(connGrpInfoList[1].getDescription(), QString("conngrpdesc2"));
	QCOMPARE(connGrpInfoList[1].getFromNeuronGroupID(), neurGrp2ID);
	QCOMPARE(connGrpInfoList[1].getToNeuronGroupID(), neurGrp1ID);
	QCOMPARE(connGrpInfoList[1].getSynapseTypeID(), (unsigned int)1);
}


void TestNetworkDao::testGetNeuronCount1(){
	//Adds test network with known properties
	addTestNetwork1();

	//Create test class
	NetworkDao networkDao(networkDBInfo);

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
	NetworkDao netDao(networkDBInfo);
	QCOMPARE(netDao.getNeuronCount(testNetID), (unsigned int)5);
	QCOMPARE(netDao.getNeuronCount(testNet2ID), (unsigned int)4);
}



void TestNetworkDao::testGetNeuronCount3(){
	//Add a couple of networks
	addTestNetwork1();
	addTestNetwork2();

	//Build list of neuron groups for each network
	QList<NeuronGroup*> neurGrpList1;
	NeuronType neurType(1, "neur type description", "neur type paramTableName", "");
	NeuronGroup neurGrp1(NeuronGroupInfo(neurGrp1ID, "NAME", "DESC", QHash<QString,double>(), neurType));
	neurGrpList1.append(&neurGrp1);

	QList<NeuronGroup*> neurGrpList2;
	NeuronGroup neurGrp2(NeuronGroupInfo(neurGrp21ID, "NAME", "DESC", QHash<QString,double>(), neurType));
	NeuronGroup neurGrp3(NeuronGroupInfo(neurGrp22ID, "NAME", "DESC", QHash<QString,double>(), neurType));
	neurGrpList2.append(&neurGrp2);
	neurGrpList2.append(&neurGrp3);

	//Check counts are correct for the two lists
	NetworkDao netDao(networkDBInfo);
	QCOMPARE(netDao.getNeuronCount(neurGrpList1), (unsigned int)3);
	QCOMPARE(netDao.getNeuronCount(neurGrpList2), (unsigned int)4);
}


void TestNetworkDao::testGetNeuronGroupBoundingBox(){
	//Adds test network with known properties
	addTestNetwork1();

	NetworkDao networkDao(networkDBInfo);

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

	NetworkDao networkDao(networkDBInfo);
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
	NetworkDao networkDao (networkDBInfo);
	QList<NeuronGroupInfo> neurGrpInfoList;
	try{
		networkDao.getNeuronGroupsInfo(networkID, neurGrpInfoList);
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
	NetworkDao networkDao(networkDBInfo);
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
		NetworkDao networkDao(networkDBInfo);
		NeuronGroupInfo neurGrpInfo (neurGrp1ID, "name", "description", QHash<QString, double>(), networkDao.getNeuronType(1));
		QHash<QString, double> neurParamMap = networkDao.getNeuronParameters(neurGrpInfo);

		//Check parameters
		QCOMPARE(neurParamMap.size(), (int)8);
		QVERIFY(neurParamMap.contains("a"));
		QCOMPARE(neurParamMap["a"], 0.02);
		QVERIFY(neurParamMap.contains("b"));
		QCOMPARE(neurParamMap["b"], 0.2);
		QVERIFY(neurParamMap.contains("c_1"));
		QCOMPARE(neurParamMap["c_1"], 15.0);
		QVERIFY(neurParamMap.contains("d_1"));
		QCOMPARE(neurParamMap["d_1"], 8.0);
		QVERIFY(neurParamMap.contains("d_2"));
		QCOMPARE(neurParamMap["d_2"], 6.0);
		QVERIFY(neurParamMap.contains("v"));
		QCOMPARE(neurParamMap["v"], -65.0);
		QVERIFY(neurParamMap.contains("sigma"));
		QCOMPARE(neurParamMap["sigma"], 5.0);
		QVERIFY(neurParamMap.contains("seed"));
		QCOMPARE(neurParamMap["seed"], 123456789.0);
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
		NetworkDao networkDao(networkDBInfo);

		//Test type 1 neurons
		NeuronType type = networkDao.getNeuronType(1);
		QCOMPARE(type.getDescription(), QString("Izhikevich Excitatory Neuron"));
		QCOMPARE(type.getParameterTableName(), QString("IzhikevichExcitatoryNeuronParameters"));
		QCOMPARE(type.getParameterInfoList().size(), (int)8);
		QCOMPARE(type.getParameterInfoList().at(0).getName(), QString("a"));
		QCOMPARE(type.getParameterInfoList().at(0).getDescription(), QString("Time scale of the recovery variable, u."));
		QCOMPARE(type.getParameterInfoList().at(5).getName(), QString("v"));
		QCOMPARE(type.getParameterInfoList().at(5).getDescription(), QString("Initial value for the membrane potential."));

		//Test type 2 neurons
		type = networkDao.getNeuronType(2);
		QCOMPARE(type.getDescription(), QString("Izhikevich Inhibitory Neuron"));
		QCOMPARE(type.getParameterTableName(), QString("IzhikevichInhibitoryNeuronParameters"));
		QCOMPARE(type.getParameterInfoList().size(), (int)8);
		QCOMPARE(type.getParameterInfoList().at(0).getName(), QString("a_1"));
		QCOMPARE(type.getParameterInfoList().at(4).getDescription(), QString("After-spike reset of the recovery variable, u."));
		QCOMPARE(type.getParameterInfoList().at(1).getName(), QString("a_2"));
		QCOMPARE(type.getParameterInfoList().at(5).getDescription(), QString("Initial value for the membrane potential."));

		//Test type 3 neurons
		type = networkDao.getNeuronType(3);
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
	NetworkDao networkDao(networkDBInfo);
	try{
		QList<NeuronType> neurTypesList = networkDao.getNeuronTypes();
		QCOMPARE(neurTypesList.size(), (int)3);

		//Check ids
		QCOMPARE(neurTypesList.at(0).getID(), (unsigned int)1);
		QCOMPARE(neurTypesList.at(1).getID(), (unsigned int)2);
		QCOMPARE(neurTypesList.at(2).getID(), (unsigned int)3);

		//Check descriptions
		QCOMPARE(neurTypesList.at(0).getDescription(), QString("Izhikevich Excitatory Neuron"));
		QCOMPARE(neurTypesList.at(1).getDescription(), QString("Izhikevich Inhibitory Neuron"));
		QCOMPARE(neurTypesList.at(2).getDescription(), QString("Weightless Neuron"));

		//Check parameter table names
		QCOMPARE(neurTypesList.at(0).getParameterTableName(), QString("IzhikevichExcitatoryNeuronParameters"));
		QCOMPARE(neurTypesList.at(1).getParameterTableName(), QString("IzhikevichInhibitoryNeuronParameters"));
		QCOMPARE(neurTypesList.at(2).getParameterTableName(), QString("WeightlessNeuronParameters"));

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
	//Add test network
	addTestNetwork1();

	try{
		//Get the values of the parameters - this should be the default
		NetworkDao networkDao(networkDBInfo);
		ConnectionGroupInfo info(connGrp1ID, "description", 0, 0, QHash<QString, double>(), networkDao.getSynapseType(1));
		QHash<QString, double> conParamMap = networkDao.getSynapseParameters(info);

		//Check parameters
		QCOMPARE(conParamMap.size(), (int)2);
		QVERIFY(conParamMap.contains("Learning"));
		QCOMPARE(conParamMap["Learning"], 0.0);
		QVERIFY(conParamMap.contains("Disable"));
		QCOMPARE(conParamMap["Disable"], 0.0);
	}
	catch(SpikeStreamException ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("Unrecognized exception thrown.");
	}
}


void TestNetworkDao::testGetSynapseType(){
	try{
		NetworkDao networkDao(networkDBInfo);

		//Test type 1 synapses
		SynapseType type = networkDao.getSynapseType(1);
		QCOMPARE(type.getDescription(), QString("Izhikevich Synapse"));
		QCOMPARE(type.getParameterTableName(), QString("IzhikevichSynapseParameters"));
		QCOMPARE(type.getParameterInfoList().size(), (int)2);
		QCOMPARE(type.getParameterInfoList().at(0).getName(), QString("Learning"));
		QCOMPARE(type.getParameterInfoList().at(0).getDescription(), QString("Switches STDP learning on and off."));
		QCOMPARE(type.getParameterInfoList().at(1).getName(), QString("Disable"));
		QCOMPARE(type.getParameterInfoList().at(1).getDescription(), QString("Disables the connection group, which will not be included in the simulation."));

		//Test type 2 synapses
		type = networkDao.getSynapseType(2);
		QCOMPARE(type.getDescription(), QString("Weightless Synapse"));
		QCOMPARE(type.getParameterTableName(), QString("WeightlessSynapseParameters"));
	}
	catch(SpikeStreamException ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("Unrecognized exception thrown.");
	}
}


/*! Note this test will need to be updated if the default types change. */
void TestNetworkDao::testGetSynapseTypes(){
	//Invoke method being tested
	NetworkDao networkDao(networkDBInfo);
	try{
		QList<SynapseType> synTypesList = networkDao.getSynapseTypes();
		QCOMPARE(synTypesList.size(), (int)2);

		//Check ids
		QCOMPARE(synTypesList.at(0).getID(), (unsigned int)1);
		QCOMPARE(synTypesList.at(1).getID(), (unsigned int)2);

		//Check descriptions
		QCOMPARE(synTypesList.at(0).getDescription(), QString("Izhikevich Synapse"));
		QCOMPARE(synTypesList.at(1).getDescription(), QString("Weightless Synapse"));

		//Check parameter table names
		QCOMPARE(synTypesList.at(0).getParameterTableName(), QString("IzhikevichSynapseParameters"));
		QCOMPARE(synTypesList.at(1).getParameterTableName(), QString("WeightlessSynapseParameters"));

		//Check class names
		QCOMPARE(synTypesList.at(0).getClassLibaryName(), QString(""));
		QCOMPARE(synTypesList.at(1).getClassLibaryName(), QString(""));

		//Check parameters
		QCOMPARE(synTypesList.at(0).getParameterInfoList().size(), (int)2);
		QCOMPARE(synTypesList.at(1).getParameterInfoList().size(), (int)0);
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
	NetworkDao networkDao(networkDBInfo);
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
	NetworkDao networkDao(networkDBInfo);
	QCOMPARE(networkDao.getNeuronGroupID(testNeurIDList2[0]), neurGrp21ID);
}


void TestNetworkDao::testGetStartNeuronID(){
	//Add test network
	addTestNetwork1();

	//Check that method executes without crashing or returning null
	try {
		NetworkDao networkDao(networkDBInfo);
		QVERIFY(networkDao.getStartNeuronID(neurGrp1ID) != 0);
	}
	catch(SpikeStreamException ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("Unrecognized exception thrown.");
	}
}


void TestNetworkDao::testIsWeightlessNetwork(){
	//Add test networks
	addWeightlessTestNetwork1();
	addTestNetwork2();

	//Create test class
	NetworkDao networkDao(networkDBInfo);

	//Neuron in group 1 should be weightless
	QVERIFY( networkDao.isWeightlessNetwork(testNetID) );
	QVERIFY( !networkDao.isWeightlessNetwork(testNet2ID) );
}


void TestNetworkDao::testIsWeightlessNeuron(){
	//Add test networks
	addWeightlessTestNetwork1();
	addTestNetwork2();

	//Create test class
	NetworkDao networkDao(networkDBInfo);

	//Neuron in group 1 should be weightless
	QVERIFY(networkDao.isWeightlessNeuron(testNeurIDList[0]));
	QVERIFY(!networkDao.isWeightlessNeuron(testNeurIDList2[0]));
}


void TestNetworkDao::testSetConnectionGroupProperties(){
	addTestNetwork1();

	try{
		//Check description before update
		QSqlQuery query = getQuery("SELECT Description FROM ConnectionGroups WHERE ConnectionGroupID=" + QString::number(connGrp1ID));
		executeQuery(query);
		query.next();
		QCOMPARE(query.value(0).toString(), QString("conngrpdesc1"));

		NetworkDao netDao(networkDBInfo);
		netDao.setConnectionGroupProperties(connGrp1ID, "newConnectionGroup1Description");

		//Check description after update
		query = getQuery("SELECT Description FROM ConnectionGroups WHERE ConnectionGroupID=" + QString::number(connGrp1ID));
		executeQuery(query);
		query.next();
		QCOMPARE(query.value(0).toString(), QString("newConnectionGroup1Description"));
	}
	catch(SpikeStreamException ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("Unrecognized exception thrown.");
	}
}


void TestNetworkDao::testSetNetworkProperties(){
	addTestNetwork1();

	try{
		//Check name and description before update
		QSqlQuery query = getQuery("SELECT Name, Description FROM Networks WHERE NetworkID=" + QString::number(testNetID));
		executeQuery(query);
		query.next();
		QCOMPARE(query.value(0).toString(), QString("testNetwork1Name"));
		QCOMPARE(query.value(1).toString(), QString("testNetwork1Description"));

		NetworkDao netDao(networkDBInfo);
		netDao.setNetworkProperties(testNetID, "newNetwork1Name", "newNetwork1Description");

		//Check name and description after update
		query = getQuery("SELECT Name, Description FROM Networks WHERE NetworkID=" + QString::number(testNetID));
		executeQuery(query);
		query.next();
		QCOMPARE(query.value(0).toString(), QString("newNetwork1Name"));
		QCOMPARE(query.value(1).toString(), QString("newNetwork1Description"));
	}
	catch(SpikeStreamException ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("Unrecognized exception thrown.");
	}
}


void TestNetworkDao::testSetNeuronGroupProperties(){
	addTestNetwork1();

	try{
		//Check name and description before update
		QSqlQuery query = getQuery("SELECT Name, Description FROM NeuronGroups WHERE NeuronGroupID=" + QString::number(neurGrp1ID));
		executeQuery(query);
		query.next();
		QCOMPARE(query.value(0).toString(), QString("name1"));
		QCOMPARE(query.value(1).toString(), QString("desc1"));

		NetworkDao netDao(networkDBInfo);
		netDao.setNeuronGroupProperties(neurGrp1ID, "newNeuronGrp1Name", "newNeuronGrp1Description");

		//Check name and description after update
		query = getQuery("SELECT Name, Description FROM NeuronGroups WHERE NeuronGroupID=" + QString::number(neurGrp1ID));
		executeQuery(query);
		query.next();
		QCOMPARE(query.value(0).toString(), QString("newNeuronGrp1Name"));
		QCOMPARE(query.value(1).toString(), QString("newNeuronGrp1Description"));
	}
	catch(SpikeStreamException ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("Unrecognized exception thrown.");
	}
}


void TestNetworkDao::testSetNeuronParameters(){
	//Add test network
	addTestNetwork1();

	try{
		//Build neuron group info and parameter map for the update
		NeuronType neurType(1, "neur type description", "neur type paramTableName", "");
		NeuronGroupInfo info(neurGrp1ID, "name", "description", QHash<QString, double>(), neurType);
		QHash<QString, double> paramMap;
		paramMap["a"] = 20.3;
		paramMap["b"] = 21.3;
		paramMap["c_1"] = 22.3;
		paramMap["d_1"] = 23.3;
		paramMap["d_2"] = 24.3;
		paramMap["v"] = 25.3;
		paramMap["sigma"] = 26.3;
		paramMap["seed"] = 27.3;

		//Check values of parameters
		NetworkDao netDao(networkDBInfo);
		QHash<QString, double> resultsMap = netDao.getNeuronParameters(info);
		QCOMPARE(resultsMap.size(), (int)8);
		QVERIFY(resultsMap.contains("a"));
		QCOMPARE(resultsMap["a"], 0.02);
		QVERIFY(resultsMap.contains("b"));
		QCOMPARE(resultsMap["b"], 0.2);
		QVERIFY(resultsMap.contains("c_1"));
		QCOMPARE(resultsMap["c_1"], 15.0);
		QVERIFY(resultsMap.contains("d_1"));
		QCOMPARE(resultsMap["d_1"], 8.0);
		QVERIFY(resultsMap.contains("d_2"));
		QCOMPARE(resultsMap["d_2"], 6.0);
		QVERIFY(resultsMap.contains("v"));
		QCOMPARE(resultsMap["v"], -65.0);
		QVERIFY(resultsMap.contains("sigma"));
		QCOMPARE(resultsMap["sigma"], 5.0);
		QVERIFY(resultsMap.contains("seed"));
		QCOMPARE(resultsMap["seed"], 123456789.0);

		//Set parameters to new values
		netDao.setNeuronParameters(info, paramMap);

		//Check that parameters have changed
		resultsMap = netDao.getNeuronParameters(info);
		QCOMPARE(resultsMap.size(), (int)8);
		QVERIFY(resultsMap.contains("a"));
		QCOMPARE(resultsMap["a"], 20.3);
		QVERIFY(resultsMap.contains("b"));
		QCOMPARE(resultsMap["b"], 21.3);
		QVERIFY(resultsMap.contains("c_1"));
		QCOMPARE(resultsMap["c_1"], 22.3);
		QVERIFY(resultsMap.contains("d_1"));
		QCOMPARE(resultsMap["d_1"], 23.3);
		QVERIFY(resultsMap.contains("d_2"));
		QCOMPARE(resultsMap["d_2"], 24.3);
		QVERIFY(resultsMap.contains("v"));
		QCOMPARE(resultsMap["v"], 25.3);
		QVERIFY(resultsMap.contains("sigma"));
		QCOMPARE(resultsMap["sigma"], 26.3);
		QVERIFY(resultsMap.contains("seed"));
		QCOMPARE(resultsMap["seed"], 27.3);
	}
	catch(SpikeStreamException ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("Unrecognized exception thrown.");
	}
}


void TestNetworkDao::testSetSynapseParameters(){
	//Add test network
	addTestNetwork1();

	try{
		//Build connection group info and parameter map for the update
		SynapseType synType(1, "syn type description", "syn type paramTableName", "");
		ConnectionGroupInfo info(connGrp1ID, "description", 0, 0, QHash<QString, double>(), synType);
		QHash<QString, double> paramMap;
		paramMap["Learning"] = 1;
		paramMap["Disable"] = 1;

		//Check values of parameters
		NetworkDao netDao(networkDBInfo);
		QHash<QString, double> resultsMap = netDao.getSynapseParameters(info);
		QCOMPARE(resultsMap.size(), (int)2);
		QVERIFY(resultsMap.contains("Learning"));
		QVERIFY(resultsMap.contains("Disable"));
		QCOMPARE(resultsMap["Learning"], 0.0);
		QCOMPARE(resultsMap["Disable"], 0.0);

		//Set parameters to new values
		netDao.setSynapseParameters(info, paramMap);

		//Check that parameters have changed
		resultsMap = netDao.getSynapseParameters(info);
		QCOMPARE(resultsMap.size(), (int)2);
		QVERIFY(resultsMap.contains("Learning"));
		QVERIFY(resultsMap.contains("Disable"));
		QCOMPARE(resultsMap["Learning"], 1.0);
		QCOMPARE(resultsMap["Disable"], 1.0);
	}
	catch(SpikeStreamException ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("Unrecognized exception thrown.");
	}
}


void TestNetworkDao::testSetWeight(){
	//Add test network
	addTestNetwork1();

	//Check the temp weight of a neuron
	QSqlQuery query = getQuery("SELECT Weight FROM Connections WHERE ConnectionID = " + QString::number(testConnIDList[3]));
	executeQuery(query);
	query.next();
	QCOMPARE(Util::getDouble(query.value(0).toString()), 0.4);

	//Execute test method
	NetworkDao networkDao(networkDBInfo);
	networkDao.setWeight(testConnIDList[3], 2.132);

	//Check weight has been updated
	query = getQuery("SELECT Weight FROM Connections WHERE ConnectionID = " + QString::number(testConnIDList[3]));
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

