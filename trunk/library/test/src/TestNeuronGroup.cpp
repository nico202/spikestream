#include "TestNeuronGroup.h"
#include "NeuronGroup.h"
#include "Neuron.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;


/*----------------------------------------------------------*/
/*-----                     TESTS                      -----*/
/*----------------------------------------------------------*/

void TestNeuronGroup::testAddLayer(){
    NeuronGroup neurGrp( NeuronGroupInfo(0, "no name", "no description", QHash<QString, double>(), 2) );

    //Add a couple of neurons to check the offset
    neurGrp.addNeuron(-1, -1, -1);
    neurGrp.addNeuron(-2, -2, -2);

    neurGrp.addLayer(13, 15, 2, 3, 4);

    NeuronMap* neuronMap = neurGrp.getNeuronMap();
    QCOMPARE(neuronMap->size(), (int)197);

	//Group should have a neuron at location (2, 3, 4);
	QVERIFY(neurGrp.getNeuronIDAtLocation(Point3D(2.0f, 3.0f, 4.0f)) != 0);

	//Group should have a neuron at location (2, 13, 4)
	QVERIFY(neurGrp.getNeuronIDAtLocation(Point3D(2.0f, 13.0f, 4.0f)) != 0);
}


void TestNeuronGroup::testAddNeuron(){
    NeuronGroup neurGrp( NeuronGroupInfo(0, "no name", "no description", QHash<QString, double>(), 2) );
    neurGrp.addNeuron(23, 27, 12);
    neurGrp.addNeuron(230, 270, 120);
    neurGrp.addNeuron(2334, 2745, 1245);

    //Check that map contains three matching neurons
    NeuronMap* neuronMap = neurGrp.getNeuronMap();
    QCOMPARE(neuronMap->size(), (int)3);

	//Check neurons exist at correct locations
	QVERIFY(neurGrp.getNeuronIDAtLocation(Point3D(23.0f, 27.0f, 12.0f)) != 0);
	QVERIFY(neurGrp.getNeuronIDAtLocation(Point3D(230.0f, 270.0f, 120.0f)) != 0);
	QVERIFY(neurGrp.getNeuronIDAtLocation(Point3D(2334.0f, 2745.0f, 1245.0f)) != 0);
}


void TestNeuronGroup::testGetPointFromPositionKey(){
	uint64_t key = 0b000000000000000000010000000000000000000111000000000000000001011;
	Point3D point = NeuronGroup::getPointFromPositionKey(key);
	Point3D expectedPoint(2,7,11);
	QCOMPARE(point, expectedPoint);
}


void TestNeuronGroup::testGetPositionKey(){
	try{
		uint64_t key = NeuronGroup::getPositionKey(2,7,11);
		uint64_t expectedResult = 0b000000000000000000010000000000000000000111000000000000000001011;
		QCOMPARE(key, expectedResult);
	}
	catch(SpikeStreamException& ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("NeuronGroup test failure. An unknown exception occurred.");
	}
}


void TestNeuronGroup::testPositionIterator(){
	NeuronGroup neurGrp( NeuronGroupInfo(0, "no name", "no description", QHash<QString, double>(), 2) );
	Neuron* neur5 = neurGrp.addNeuron(23, 27, 14);//Should be 5th
	Neuron* neur3 = neurGrp.addNeuron(23, 27, 12);//Should be 3rd
	Neuron* neur2 = neurGrp.addNeuron(23, 26, 13);//Should be 2nd
	Neuron* neur4 = neurGrp.addNeuron(23, 27, 13);//Should be 4th
	Neuron* neur1 = neurGrp.addNeuron(22, 26, 14);//Should be 1st

	int counter = 1;
	for(NeuronPositionIterator iter = neurGrp.positionBegin(); iter != neurGrp.positionEnd(); ++iter){
		switch(counter){
			case 1:
				QCOMPARE(iter.value()->getID(), neur1->getID());
			break;
			case 2:
				QCOMPARE(iter.value()->getID(), neur2->getID());
			break;
			case 3:
				QCOMPARE(iter.value()->getID(), neur3->getID());
			break;
			case 4:
				QCOMPARE(iter.value()->getID(), neur4->getID());
			break;
			case 5:
				QCOMPARE(iter.value()->getID(), neur5->getID());
			break;
			default: QFAIL("Iterator out of range");
		}
		++counter;
	}
}


