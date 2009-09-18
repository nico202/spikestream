#include "TestNetwork.h"
#include "Network.h"
using namespace spikestream;


/*! Called after each test */
void TestNetwork::cleanup(){
}


/*! Called after all the tests */
void TestNetwork::cleanupTestCase() {
    //Leave databases in clean state
    cleanTestDatabases();

    //Close database connection
    closeDatabase();
}


/*! Called before each test */
void TestNetwork::init(){
    cleanTestDatabases();
}


/*! Called before all the tests */
void TestNetwork::initTestCase(){
    connectToDatabase("SpikeStreamNetworkTest");
}


/*----------------------------------------------------------*/
/*-----                     TESTS                      -----*/
/*----------------------------------------------------------*/
void TestNetwork::testGetBoundingBox(){
    //Adds test network with known properties
    addTestNetwork1();

    NetworkDao networkDao(dbInfo);
    Network network(NetworkInfo(testNetID, "undefined net name", "undefined net description", 0), &networkDao);

    //Check bounding box around neuron group 1
    Box box = network.getBoundingBox();
    QCOMPARE(box.x1, -3.0f);
    QCOMPARE(box.x2, 9.0f);
    QCOMPARE(box.y1, -7.0f);
    QCOMPARE(box.y2, -1.0f);
    QCOMPARE(box.z1, -6.0f);
    QCOMPARE(box.z2, 10.0f);

}



