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



