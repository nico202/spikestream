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
    cleanTestDatabases();

    //Close database connection
    database.close();
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
//    NetworkInfo netInfo(0, "test3Name", "test3Description",  0);
//    NetworkDao networkDao (dbInfo);
//
//    try{
//	/* Add network - slightly sloppy to use the network dao method, but it has been tested elsewhere
//	    and we need a network because of foreign key constraints */
//	networkDao.addNetwork(netInfo);
//
//	/* Add neuron groups - again, slightly sloppy, but it has been tested elsewhere
//	   and we need neuron groups because of foreign key constraints */
//	NeuronGroupInfo fromGrpInfo(0, "fromNeuronGroup1Name", "fromNeuronGroup1Desc", QHash<QString, double>(), 1);
//	NeuronGroupInfo toGrpInfo(0, "toNeuronGroup1Name", "toNeuronGroup1Desc", QHash<QString, double>(), 1);
//	networkDao.addNeuronGroup(netInfo.getID(), fromGrpInfo);
//	networkDao.addNeuronGroup(netInfo.getID(), toGrpInfo);
//
//	 //Build the connection group that is to be added
//	QHash<QString, double> paramMap;
//	paramMap["param3"] = 0.7;
//	paramMap["param4"] = 0.8;
//	ConnectionGroupInfo connGrpInfo(0, "testConnGroup1Desc", fromGrpInfo.getID(), toGrpInfo.getID(),  paramMap, 1);
//
//	//Test the connection group addition
//	networkDao.addConnectionGroup(netInfo.getID(), connGrpInfo);
//	QSqlQuery query = getQuery("SELECT NeuralNetworkID, Description, FromNeuronGroupID, ToNeuronGroupID, Parameters, SynapseTypeID FROM ConnectionGroups");
//	executeQuery(query);
//	QCOMPARE(1, query.size());
//	query.next();
//	QCOMPARE(query.value(0).toString(), QString::number(netInfo.getID()));//Check network id
//	QCOMPARE(query.value(1).toString(), connGrpInfo.getDescription());//Check description
//	QCOMPARE(query.value(2).toString(), QString::number(fromGrpInfo.getID()));//Check from neuron group id
//	QCOMPARE(query.value(3).toString(), QString::number(toGrpInfo.getID()));//Check to neuron group id
//	QCOMPARE(query.value(4).toString(), connGrpInfo.getParameterXML());//Check parameters
//	QCOMPARE(query.value(5).toString(), QString::number(connGrpInfo.getSynapseType()));//Check synapse type
//	QVERIFY(connGrpInfo.getID() != 0);
//    }
//    catch(SpikeStreamException& ex){
//	QFAIL(ex.getMessage().toAscii());
//    }
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
	NetworkDaoThread netDaoThread(dbInfo, netInfo.getID(), &neurGrp);
	netDaoThread.start();
	netDaoThread.wait();

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
	query = getQuery("SELECT X, Y, Z FROM Neurons WHERE NeuronGroupID = " + QString::number(neurGrp.getID()));
	executeQuery(query);
	QCOMPARE(4, query.size());//4 neurons should have been added
	query.next();
	QCOMPARE(query.value(0).toString(), QString::number(0));
	QCOMPARE(query.value(1).toString(), QString::number(0));
	QCOMPARE(query.value(2).toString(), QString::number(0));
	query.next();
	QCOMPARE(query.value(0).toString(), QString::number(0));
	QCOMPARE(query.value(1).toString(), QString::number(1));
	QCOMPARE(query.value(2).toString(), QString::number(0));
	query.next();
	QCOMPARE(query.value(0).toString(), QString::number(1));
	QCOMPARE(query.value(1).toString(), QString::number(0));
	QCOMPARE(query.value(2).toString(), QString::number(0));
	query.next();
	QCOMPARE(query.value(0).toString(), QString::number(1));
	QCOMPARE(query.value(1).toString(), QString::number(1));
	QCOMPARE(query.value(2).toString(), QString::number(1));

    }
    catch(SpikeStreamException& ex){
	QFAIL(ex.getMessage().toAscii());
    }
}
