#include "TestWeightlessNeuron.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;

#include <iostream>
using namespace std;

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
    QHash<unsigned int, QList<unsigned int> > connMap;
    for(int i=0; i<10; ++i){
	connMap[i+10].append(i);
    }
    WeightlessNeuron tstNeuron(connMap, 0);

    //Check length of training data
    QCOMPARE(tstNeuron.getTrainingDataLength(), 3);

    //Add two pieces of training data
    addTraining(tstNeuron, "1010101010", 1);
    addTraining(tstNeuron, "1111111111", 0);

    //Check that training data is present and correct
    QList<byte*> trainingData = tstNeuron.getTrainingData();
    QCOMPARE(trainingData.size(), 2);
    QVERIFY(bitsEqual(trainingData[0], "1010101010", 1));
    QVERIFY(bitsEqual(trainingData[1], "1111111111", 0));
}


void TestWeightlessNeuron::testGetFiringStateProbability(){
    //Build a weightless neuron with four neurons
    QHash<unsigned int, QList<unsigned int> > connMap;
    for(int i=0; i<4; ++i){
	connMap[i].append(i);
    }
    WeightlessNeuron tstNeuron(connMap, 0);

    //Set generalization to 3/4. Patterns should match on at least three places
    tstNeuron.setGeneralization(0.75);

    //Add two pieces of training data
    addTraining(tstNeuron, "0011", 1);
    addTraining(tstNeuron, "1001", 0);

    //Test with a pattern that should match both within the hamming distance
    byte* inPattern;
    int arrLen;
    fillByteArray(inPattern, arrLen, "1011");
    try{
	//Contradictory match
	QCOMPARE(tstNeuron.getFiringStateProbability(inPattern, arrLen, 1), 0.5);//Should match both patterns and emit random 1's and 0's
	QCOMPARE(tstNeuron.getFiringStateProbability(inPattern, arrLen, 0), 0.5);//Should match both patterns and emit random 1's and 0's

	//Build a different input pattern
	delete [] inPattern;
	fillByteArray(inPattern, arrLen, "0011");

	//Should be an exact match
	QCOMPARE(tstNeuron.getFiringStateProbability(inPattern, arrLen, 1), 1.0);
	QCOMPARE(tstNeuron.getFiringStateProbability(inPattern, arrLen, 0), 0.0);

	//Build a different input pattern
	delete [] inPattern;
	fillByteArray(inPattern, arrLen, "1101");

	//Should be an exact match with generalization at 0.75
	QCOMPARE(tstNeuron.getFiringStateProbability(inPattern, arrLen, 0), 1.0);

	//Should fail to match when generalization is set to 1.0
	tstNeuron.setGeneralization(1.0);
	QCOMPARE(tstNeuron.getFiringStateProbability(inPattern, arrLen, 0), 0.5);
	delete [] inPattern;

    }
    catch(SpikeStreamException& ex){
	QFAIL(ex.getMessage().toAscii());
    }
}



void TestWeightlessNeuron::testGetTransitionProbability(){
    //Build a weightless neuron with four neurons, ids 1,2,3,4
    QHash<unsigned int, QList<unsigned int> > connMap;
    for(int i=1; i<5; ++i){
	connMap[i].append(i-1);
    }
    WeightlessNeuron tstNeuron(connMap, 0);

    //Add two pieces of training data
    addTraining(tstNeuron, "0011", 1);
    addTraining(tstNeuron, "1001", 0);

    //Build neuron id list and corresponding firing pattern with two of the neurons in it
    QList<unsigned int> neurIDList;
    neurIDList.append(10);
    neurIDList.append(11);
    neurIDList.append(4);
    neurIDList.append(12);
    neurIDList.append(1);
    QString s0Patt = "10100";//Neuron 4 is firing; neuron 1 is not

    try{
	//Check transition probability for 0 and 1 with generalization at 0.75
	tstNeuron.setGeneralization(0.75);
	QCOMPARE(tstNeuron.getTransitionProbability(neurIDList,s0Patt, 0), 0.25);
	QCOMPARE(tstNeuron.getTransitionProbability(neurIDList,s0Patt, 1), 0.75);

	//Change generalization to 1 and check transition probabilites again
	tstNeuron.setGeneralization(1.0);
	QCOMPARE(tstNeuron.getTransitionProbability(neurIDList,s0Patt, 0), 0.375);
	QCOMPARE(tstNeuron.getTransitionProbability(neurIDList,s0Patt, 1), 0.625);
    }
    catch(SpikeStreamException& ex){
	QFAIL(ex.getMessage().toAscii());
    }


    //Build a weightless neuron with five neurons, ids 23,24,25,26,27
    QHash<unsigned int, QList<unsigned int> > connMap2;
    for(int i=0; i<5; ++i){
	connMap2[i+23].append(i);
    }
    WeightlessNeuron tstNeuron2(connMap2, 0);

    //Add one piece of training data
    addTraining(tstNeuron2, "11000", 1);

    //Build neuron id list and corresponding firing pattern with two of the neurons in it
    QList<unsigned int> neurIDList2;
    neurIDList2.append(23);
    neurIDList2.append(24);
    neurIDList2.append(4);
    neurIDList2.append(12);
    neurIDList2.append(1);
    QString s0Patt2 = "11111";//Both neurons are firing

    try{
	//Check transition probability for 0 and 1 with 0.8 generalization
	tstNeuron2.setGeneralization(0.8);
	QCOMPARE(tstNeuron2.getTransitionProbability(neurIDList2,s0Patt2, 0), 0.25);
	QCOMPARE(tstNeuron2.getTransitionProbability(neurIDList2,s0Patt2, 1), 0.75);

	//Check transition probabilities with 0.6 generalization
	tstNeuron2.setGeneralization(0.6);
	QCOMPARE(tstNeuron2.getTransitionProbability(neurIDList2,s0Patt2, 0), 0.0625);
	QCOMPARE(tstNeuron2.getTransitionProbability(neurIDList2,s0Patt2, 1), 0.9375);
    }
    catch(SpikeStreamException& ex){
	QFAIL(ex.getMessage().toAscii());
    }


    //Build a weightless neuron with three input neurons, ids 1,2,3 There are three connections from neuron 2
    QHash<unsigned int, QList<unsigned int> > connMap3;
    connMap3[1].append(0);
    connMap3[2].append(1);
    connMap3[3].append(2);
    connMap3[2].append(3);
    connMap3[2].append(4);

    WeightlessNeuron tstNeuron3(connMap3, 0);
    QCOMPARE(tstNeuron3.getNumberOfConnections(), (int)5);

    //Add  training data
    addTraining(tstNeuron3, "01011", 1);//Output 1 when 2 is firing and other neurons are quiescent
    addTraining(tstNeuron3, "10100", 1);//Output 0 when 1 and 3 are firing and 2 is are quiescent

    //Build neuron id list and corresponding firing pattern
    QList<unsigned int> neurIDList3;
    neurIDList3.append(1);
    neurIDList3.append(2);
    neurIDList3.append(3);
    QString s0Patt3a = "010";//Neuron 2 is firing
    QString s0Patt3b = "101";//Neurons 1 and 3 are firing
    QString s0Patt3c = "000";//No neurons firing

    try{
	//Check transition probability
	tstNeuron3.setGeneralization(1.0);
	QCOMPARE(tstNeuron3.getTransitionProbability(neurIDList3,s0Patt3a, 1), 1.0);
	QCOMPARE(tstNeuron3.getTransitionProbability(neurIDList3,s0Patt3b, 1), 1.0);
	QCOMPARE(tstNeuron3.getTransitionProbability(neurIDList3,s0Patt3c, 1), 0.5);
    }
    catch(SpikeStreamException& ex){
	QFAIL(ex.getMessage().toAscii());
    }
}


void TestWeightlessNeuron::testSetGeneralization(){
    /* Create connection map and use it to create neuron.
	neurons with ids 10 ...19 are in their respective positions in the connection map */
    QHash<unsigned int, QList<unsigned int> > connMap;
    for(int i=0; i<12; ++i){
	connMap[i+10].append(i);
    }
    WeightlessNeuron tstNeuron(connMap, 0);

    //Set generalization to 0.25
    tstNeuron.setGeneralization(0.25);
    QCOMPARE(tstNeuron.getHammingThreshold(), (unsigned int)9);
}




/*-------------------------------------------------------*/
/*-------             PRIVATE METHODS             -------*/
/*-------------------------------------------------------*/

void TestWeightlessNeuron::addTraining(WeightlessNeuron& neuron, QString trainingPattern, int output){
    byte* byteArr;
    int arrLen;
    fillByteArray(byteArr, arrLen, trainingPattern);
    try{
	QByteArray qByteArr = QByteArray::fromRawData((const char*)byteArr, arrLen);
	neuron.addTraining(qByteArr, output);
    }
    catch (SpikeStreamException ex){
	QFAIL(ex.getMessage().toAscii());
    }
    delete [] byteArr;
}


bool TestWeightlessNeuron::bitsEqual(unsigned char* byteArr, QString bitPattStr, int output){
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


void TestWeightlessNeuron::fillByteArray(unsigned char*& byteArr, int& arrLen, QString byteStr){
    if(byteStr.length() % 8 == 0)
	arrLen = byteStr.length() / 8;
    else
	arrLen = byteStr.length() / 8 + 1;
    byteArr = new byte[arrLen];

    //Initialize array
    for(int i=0; i<arrLen; ++i)
	byteArr[i] = 0;

    //Set bits corresponding to 1's in byte string
    for(int i=0; i<byteStr.length(); ++i){
	if(byteStr[i] == '1')
	    byteArr[i/8] |= 1<<(i % 8);
    }
}


