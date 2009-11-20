#include "TestWeightlessNeuron.h"
#include "WeightlessNeuron.h"
using namespace spikestream;

/*! Lookup array pasted in from weightless neuron for testing here */
byte lookup[] = {
  0,1,1,2, 1,2,2,3, 1,2,2,3, 2,3,3,4,
  1,2,2,3, 2,3,3,4, 2,3,3,4, 3,4,4,5,
  1,2,2,3, 2,3,3,4, 2,3,3,4, 3,4,4,5,
  2,3,3,4, 3,4,4,5, 3,4,4,5, 4,5,5,6,
  1,2,2,3, 2,3,3,4, 2,3,3,4, 3,4,4,5,
  2,3,3,4, 3,4,4,5, 3,4,4,5, 4,5,5,6,
  2,3,3,4, 3,4,4,5, 3,4,4,5, 4,5,5,6,
  3,4,4,5, 4,5,5,6, 4,5,5,6, 5,6,6,7,
  1,2,2,3, 2,3,3,4, 2,3,3,4, 3,4,4,5,
  2,3,3,4, 3,4,4,5, 3,4,4,5, 4,5,5,6,
  2,3,3,4, 3,4,4,5, 3,4,4,5, 4,5,5,6,
  3,4,4,5, 4,5,5,6, 4,5,5,6, 5,6,6,7,
  2,3,3,4, 3,4,4,5, 3,4,4,5, 4,5,5,6,
  3,4,4,5, 4,5,5,6, 4,5,5,6, 5,6,6,7,
  3,4,4,5, 4,5,5,6, 4,5,5,6, 5,6,6,7,
  4,5,5,6, 5,6,6,7, 5,6,6,7, 6,7,7,8
};


void TestWeightlessNeuron::testLookup(){
    for(byte i=0; i<255; ++i){//Don't bother testing last entry, which is correct anyway
	 //Count the number of 1's in i
	byte oneCount = 0;
	for(int j=0; j<8; ++j){
	    if( i & 1<<j){
		++oneCount;
	    }
	}
	if(oneCount != lookup[i]){
	    QString failStr = "Value in lookup (" + QString::number(lookup[i]) + " does not match number of 1s in number (" + QString::number(oneCount);
	    QFAIL(failStr.toAscii());
	}
    }
}


void TestWeightlessNeuron::testAddTraining(){
    /* Create connection map and use it to create neuron.
	neurons with ids 10 ...19 are in their respective positions in the connection map */
    QHash<unsigned int, unsigned int> connMap;
    for(int i=0; i<10; ++i){
	connMap[i+10] = i;
    }
    WeightlessNeuron tstNeuron(connMap);

    //Check length of training data
    QCOMPARE(tstNeuron.getTrainingDataLength(), 3);

    //Add two pieces of training data
    QByteArray byteArr1, byteArr2;
    fillByteArray(byteArr1, "1010101010");
    fillByteArray(byteArr1, "1111111111");
    try{
	tstNeuron.addTraining(byteArr1, 1);
	tstNeuron.addTraining(byteArr2, 0);
    }
    catch (SpikeStreamException ex){
	QFAIL(ex.getMessage().toAscii());
    }

    //Check that training data is present and correct
    QList<byte> trainingData = tstNeuron.getTrainingData();
    QCOMPARE(trainingData.size(), 2);
    checkBits(trainingData[0], "1010101010", 1);
    checkBits(trainingData[0], "1111111111", 0);
}


void TestWeightlessNeuron::testGetFiringStateProbability(){

}


void TestWeightlessNeuron::testGetTrainingData(){
}


void TestWeightlessNeuron::testGetTransitionProbability(){
}


void TestWeightlessNeuron::testSetGeneralization(){
}
