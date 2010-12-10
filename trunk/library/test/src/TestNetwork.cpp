#include "GlobalVariables.h"
#include "TestNetwork.h"
#include "Network.h"
using namespace spikestream;


/*----------------------------------------------------------*/
/*-----                     TESTS                      -----*/
/*----------------------------------------------------------*/
void TestNetwork::testGetBoundingBox(){
    //Adds test network with known properties
    addTestNetwork1();

	//Create network
	Network network(NetworkInfo(testNetID, "undefined net name", "undefined net description"), networkDBInfo, archiveDBInfo);
	network.load();

	//Wait for network to load in separate thread
	while(network.isBusy())
		;

    //Check bounding box around neuron group 1
    Box box = network.getBoundingBox();
    QCOMPARE(box.x1, -3.0f);
	QCOMPARE(box.x2, 9.1f);
    QCOMPARE(box.y1, -7.0f);
	QCOMPARE(box.y2, -0.9f);
    QCOMPARE(box.z1, -6.0f);
	QCOMPARE(box.z2, 10.1f);
}

//void TestNetwork::testGetConnections(){
//	//Adds test network with known properties
//	addTestNetwork1();

//	//Create network and load it
//	Network network(NetworkInfo(testNetID, "undefined net name", "undefined net description"), dbInfo, archiveDBInfo);
//	network.loadWait();
//	if(network.isError())
//		QFAIL(network.getErrorMessage().toAscii());

//	//Test BETWEEN connections
//	unsigned conMode = 0;
//	conMode |= CONNECTION_MODE_ENABLED;
//	conMode |= SHOW_BETWEEN_CONNECTIONS;
//	QList<Connection*> conList = network.getConnections(conMode, testNeurIDList[4], testNeurIDList[3]);
//	QCOMPARE(conList.size(), (int)1);
//	QCOMPARE(conList.at(0)->getID(), testConnIDList[3]);
//	QCOMPARE(conList.at(0)->getFromNeuronID(), testNeurIDList[4]);
//	QCOMPARE(conList.at(0)->getToNeuronID(), testNeurIDList[3]);

//	//Test FROM connections
//	conMode = 0;
//	conMode |= CONNECTION_MODE_ENABLED;
//	conMode |= SHOW_FROM_CONNECTIONS;
//	conList = network.getConnections(conMode, testNeurIDList[0], 0);
//	QCOMPARE(conList.size(), (int)3);
//	QCOMPARE(conList.at(0)->getFromNeuronID(), testNeurIDList[0]);
//	QCOMPARE(conList.at(1)->getFromNeuronID(), testNeurIDList[0]);
//	QCOMPARE(conList.at(2)->getFromNeuronID(), testNeurIDList[0]);

//	//Test TO connections
//	conMode = 0;
//	conMode |= CONNECTION_MODE_ENABLED;
//	conMode |= SHOW_TO_CONNECTIONS;
//	conList = network.getConnections(conMode, testNeurIDList[1], 0);
//	QCOMPARE(conList.size(), (int)2);
//	QCOMPARE(conList.at(0)->getToNeuronID(), testNeurIDList[1]);
//	QCOMPARE(conList.at(1)->getToNeuronID(), testNeurIDList[1]);
//	QCOMPARE(conList.at(0)->getFromNeuronID(), testNeurIDList[0]);
//	QCOMPARE(conList.at(1)->getFromNeuronID(), testNeurIDList[4]);

//	//Test ALL connection mode connections
//	conMode = 0;
//	conMode |= CONNECTION_MODE_ENABLED;
//	conList = network.getConnections(conMode, testNeurIDList[3], 0);
//	QCOMPARE(conList.size(), (int)3);
//	QVERIFY( (conList.at(0)->getToNeuronID() == testNeurIDList[3]) || (conList.at(0)->getFromNeuronID() == testNeurIDList[3]) );
//	QVERIFY( (conList.at(1)->getToNeuronID() == testNeurIDList[3]) || (conList.at(1)->getFromNeuronID() == testNeurIDList[3]) );
//	QVERIFY( (conList.at(2)->getToNeuronID() == testNeurIDList[3]) || (conList.at(2)->getFromNeuronID() == testNeurIDList[3]) );
//}



