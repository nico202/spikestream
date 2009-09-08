//SpikeStream includes
#include "SpikeStreamException.h"
#include "TestNetworkDao.h"
#include "NetworkDao.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;

/*----------------------------------------------------------*/
/*-----                  TEST METHODS                  -----*/
/*----------------------------------------------------------*/

/*! Called after each test */
void TestNetworkDao::cleanup(){
}


/*! Called after all the tests */
void TestNetworkDao::cleanupTestCase() {
    //Close database connection
    database.close();
}


/*! Called before each test */
void TestNetworkDao::init(){
    cleanTestDatabases();
}


/*! Called before all the tests */
void TestNetworkDao::initTestCase(){
    connectToDatabase("SpikeStreamNetworkTest");
}


/*! Tests the addition of a connection group to the network */
void TestNetworkDao::testAddConnectionGroup(){
//    NetworkInfo netInfo(0, "test3", 0);
//    NetworkDao networkDao (dbInfo);
//
//    //Build the connection group that is to be added
//    QHash<QString, double> paramMap;
//    paramMap["param3"] = 0.7;
//    paramMap["param4"] = 0.8;
//    ConnectionGroupInfo connGrpInfo(0, "testConnGroup1Name", "testConnGroup1Desc", paramMap, 1, 0);
//
//    try{
//	/* Add network - slightly sloppy to use the network dao method, but it has been tested elsewhere
//	    and we need a network because of foreign key constraints */
//	networkDao.addNetwork(netInfo);
//
//	//Test the neuron group addition
//	networkDao.addNeuronGroup(netInfo.getID(), neurGrpInfo);
//	QSqlQuery query = getQuery("SELECT NeuralNetworkID, Name, Description, Parameters, NeuronTypeID FROM NeuronGroups");
//	executeQuery(query);
//	QCOMPARE(1, query.size());
//	query.next();
//	QCOMPARE(query.value(0).toString(), QString::number(netInfo.getID()));//Check network id
//	QCOMPARE(query.value(1).toString(), neurGrpInfo.getName());//Check name
//	QCOMPARE(query.value(2).toString(), neurGrpInfo.getDescription());//Check description
//	QCOMPARE(query.value(3).toString(), neurGrpInfo.getParameterXML());//Check parameters
//	QCOMPARE(query.value(4).toString(), QString::number(neurGrpInfo.getNeuronType()));//Check neuron type
//	QVERIFY( neurGrpInfo.getID() != 0);
//    }
//    catch(SpikeStreamException& ex){
//	QFAIL(ex.getMessage().toAscii());
//    }
}


/*! Tests the addition of a neuron group to the network */
void TestNetworkDao::testAddNeuronGroup(){
    NetworkInfo netInfo(0, "test2", 0);
    NetworkDao networkDao (dbInfo);

    //Build the neuron group that is to be added
    QHash<QString, double> paramMap;
    paramMap["param1"] = 0.5;
    paramMap["param2"] = 0.6;
    NeuronGroupInfo neurGrpInfo(0, "testNeuronGroup1Name", "testNeuronGroup1Desc", paramMap, 1, 0);

    try{
	/* Add network - slightly sloppy to use the network dao method, but it has been tested elsewhere
	    and we need a network because of foreign key constraints */
	networkDao.addNetwork(netInfo);

	//Test the neuron group addition
	networkDao.addNeuronGroup(netInfo.getID(), neurGrpInfo);
	QSqlQuery query = getQuery("SELECT NeuralNetworkID, Name, Description, Parameters, NeuronTypeID FROM NeuronGroups");
	executeQuery(query);
	QCOMPARE(1, query.size());
	query.next();
	QCOMPARE(query.value(0).toString(), QString::number(netInfo.getID()));//Check network id
	QCOMPARE(query.value(1).toString(), neurGrpInfo.getName());//Check name
	QCOMPARE(query.value(2).toString(), neurGrpInfo.getDescription());//Check description
	QCOMPARE(query.value(3).toString(), neurGrpInfo.getParameterXML());//Check parameters
	QCOMPARE(query.value(4).toString(), QString::number(neurGrpInfo.getNeuronType()));//Check neuron type
	QVERIFY( neurGrpInfo.getID() != 0);
    }
    catch(SpikeStreamException& ex){
	QFAIL(ex.getMessage().toAscii());
    }
}


/*! Tests the addition of a network to the database */
void TestNetworkDao::testAddNetwork(){
    //Create the network dao
    NetworkDao networkDao (dbInfo);

    //Information about the network to be added
    NetworkInfo netInfo(0, "test1", 0);
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
    //Add a test network with known properties
    addTestNetwork1();

    //Create the network dao
    NetworkDao networkDao (dbInfo);

    //Get list of the available networks
    QList<NetworkInfo> netInfoList = networkDao.getNetworksInfo();

    //Should be two entries
    //QCOMPARE(netInfoList.size(), 2);

    //Check that list is correct
    for(int i=0; i<2; ++i){
    }
}


/*! Adds a test network to the database */
void TestNetworkDao::addTestNetwork1(){
}



