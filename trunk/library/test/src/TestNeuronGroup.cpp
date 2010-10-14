#include "TestNeuronGroup.h"
#include "NeuronGroup.h"
#include "Neuron.h"
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

    //Neurons in layer should have been added with temporary ids starting from 2 because the map started with a neuron already in it

    //First neuron should be at the layer location (2, 3, 4);
    QVERIFY(neuronMap->contains(2));
    Neuron* tmpNeuron = (*neuronMap)[2];
    QCOMPARE(tmpNeuron->getXPos(), 2.0f);
    QCOMPARE(tmpNeuron->getYPos(), 3.0f);
    QCOMPARE(tmpNeuron->getZPos(), 4.0f);

    //Twelfth neuron should be at (2, 13, 4)
    QVERIFY(neuronMap->contains(12));
    tmpNeuron = (*neuronMap)[12];
    QCOMPARE(tmpNeuron->getXPos(), 2.0f);
    QCOMPARE(tmpNeuron->getYPos(), 13.0f);
    QCOMPARE(tmpNeuron->getZPos(), 4.0f);

}


void TestNeuronGroup::testAddNeuron(){
    NeuronGroup neurGrp( NeuronGroupInfo(0, "no name", "no description", QHash<QString, double>(), 2) );
    neurGrp.addNeuron(23, 27, 12);
    neurGrp.addNeuron(230, 270, 120);
    neurGrp.addNeuron(2334, 2745, 1245);

    //Check that map contains three matching neurons
    NeuronMap* neuronMap = neurGrp.getNeuronMap();
    QCOMPARE(neuronMap->size(), (int)3);

    //Neurons should have been added with temporary ids starting from zero
    QVERIFY(neuronMap->contains(0));
    Neuron* tmpNeuron = (*neuronMap)[0];
    QCOMPARE(tmpNeuron->getXPos(), 23.0f);
    QCOMPARE(tmpNeuron->getYPos(), 27.0f);
    QCOMPARE(tmpNeuron->getZPos(), 12.0f);

    QVERIFY(neuronMap->contains(1));
    tmpNeuron = (*neuronMap)[1];
    QCOMPARE(tmpNeuron->getXPos(), 230.0f);
    QCOMPARE(tmpNeuron->getYPos(), 270.0f);
    QCOMPARE(tmpNeuron->getZPos(), 120.0f);

    QVERIFY(neuronMap->contains(2));
    tmpNeuron = (*neuronMap)[2];
    QCOMPARE(tmpNeuron->getXPos(), 2334.0f);
    QCOMPARE(tmpNeuron->getYPos(), 2745.0f);
    QCOMPARE(tmpNeuron->getZPos(), 1245.0f);
}


void testGetPointFromPositionKey(){

}


void testGetPositionKey(){
	uint64_t key = getPositionKey(2,7,11);
	uint64t expectedResult = 0b0000000000000000000010000000000000000000111000000000000000001011;
	QCOMPARE(key, expectedResult);
}



