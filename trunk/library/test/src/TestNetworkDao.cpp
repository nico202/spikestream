//SpikeStream includes
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

/*! Called after the last test has been executed */
void TestNetworkDao::cleanupTestCase() {
    //Close database connection
    database.close();
}


/*! Called before each test */
void TestNetworkDao::init(){

}

/*! Called before all of the tests */
void TestNetworkDao::initTestCase(){
    connectToDatabase("SpikeStreamNetworkTest");
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
cout<<"SIZE: "<<netInfoList.size()<<endl;
    //Should be two entries
    //QCOMPARE(netInfoList.size(), 2);

    //Check that list is correct
    for(int i=0; i<2; ++i){
    }
}


/*! Adds a test network to the database */
void TestNetworkDao::addTestNetwork1(){
}



/*! Cleans up everything from the test networks database */

