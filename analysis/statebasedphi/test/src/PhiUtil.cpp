#include "PhiUtil.h"
#include "SpikeStreamException.h"
using namespace spikestream;


/*! Adds the trainign pattern to the weightless neuron */
void PhiUtil::addTraining(WeightlessNeuron& neuron, QString trainingPattern, int output){
    unsigned char* byteArr;
    int arrLen;
    fillByteArray(byteArr, arrLen, trainingPattern);
    QByteArray qByteArr = QByteArray::fromRawData((const char*)byteArr, arrLen);
    neuron.addTraining(qByteArr, output);
    delete [] byteArr;
}


/*! Returns true if the string of 1s and 0s matches the 1s and 0s in the byte array and the output */
bool PhiUtil::bitsEqual(unsigned char* byteArr, QString bitPattStr, int output){
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

/*! Builds and returns a phi calculator instance set up with
   the network specified in Balduzzi and Tononi (2008), Figure 3. */
PhiCalculator* PhiUtil::buildPhiTestNetwork1(){
    //Same con map can be used for all neurons since the neurons copy its contents
    QHash<unsigned int, QList<unsigned int> > conMap;
    QHash<unsigned int, WeightlessNeuron*> weightlessNeuronMap;

    //Build neuron 1 - it copies the output from neuron 2
    conMap[2].append(0);
    weightlessNeuronMap[1] = new WeightlessNeuron(conMap, 0);
    addTraining(*weightlessNeuronMap[1], "0", 0);
    addTraining(*weightlessNeuronMap[1], "1", 1);

    //Build neuron 2 - it copies the output from neuron 1
    conMap.clear();
    conMap[1].append(0);
    weightlessNeuronMap[2] = new WeightlessNeuron(conMap, 0);
    addTraining(*weightlessNeuronMap[2], "0", 0);
    addTraining(*weightlessNeuronMap[2], "1", 1);

    //Build neuron 3 - it copies the output from neuron 4
    conMap.clear();
    conMap[4].append(0);
    weightlessNeuronMap[3] = new WeightlessNeuron(conMap, 0);
    addTraining(*weightlessNeuronMap[3], "0", 0);
    addTraining(*weightlessNeuronMap[3], "1", 1);

    //Build neuron 4 - it copies the output from neuron 3
    conMap.clear();
    conMap[3].append(0);
    weightlessNeuronMap[4] = new WeightlessNeuron(conMap, 0);
    addTraining(*weightlessNeuronMap[4], "0", 0);
    addTraining(*weightlessNeuronMap[4], "1", 1);

    //Create firing neuron map with current state of the network
    QHash<unsigned int, bool> firingNeuronMap;
    firingNeuronMap[2] = true;
    firingNeuronMap[3] = true;

    //Build and return phi calculator
    PhiCalculator* phiCalc = new PhiCalculator();
    phiCalc->setWeightlessNeuronMap(weightlessNeuronMap);
    phiCalc->setFiringNeuronMap(firingNeuronMap);
    return phiCalc;
}


/*! Builds and returns a phi calculator instance set up with
   the network specified in Balduzzi and Tononi (2008), Figure 6. */
PhiCalculator* PhiUtil::buildPhiTestNetwork2(){
    //Same con map can be used for all neurons since the neurons copy its contents
    QHash<unsigned int, QList<unsigned int> > conMap;
    QHash<unsigned int, WeightlessNeuron*> weightlessNeuronMap;

    //Build neuron 1 - it is an AND gate receiving input from 2 and 3
    conMap[2].append(0);
    conMap[3].append(1);
    weightlessNeuronMap[1] = new WeightlessNeuron(conMap, 0);
    addTraining(*weightlessNeuronMap[1], "00", 0);
    addTraining(*weightlessNeuronMap[1], "01", 0);
    addTraining(*weightlessNeuronMap[1], "10", 0);
    addTraining(*weightlessNeuronMap[1], "11", 1);

    //Build neuron 2 - it is an AND gate receiving input from 1 and 3
    conMap.clear();
    conMap[1].append(0);
    conMap[3].append(1);
    weightlessNeuronMap[2] = new WeightlessNeuron(conMap, 0);
    addTraining(*weightlessNeuronMap[2], "00", 0);
    addTraining(*weightlessNeuronMap[2], "01", 0);
    addTraining(*weightlessNeuronMap[2], "10", 0);
    addTraining(*weightlessNeuronMap[2], "11", 1);

    //Build neuron 3 - it is an AND gate receiving input from 1 and 2
    conMap.clear();
    conMap[1].append(0);
    conMap[2].append(1);
    weightlessNeuronMap[3] = new WeightlessNeuron(conMap, 0);
    addTraining(*weightlessNeuronMap[3], "00", 0);
    addTraining(*weightlessNeuronMap[3], "01", 0);
    addTraining(*weightlessNeuronMap[3], "10", 0);
    addTraining(*weightlessNeuronMap[3], "11", 1);

    //Build neuron 4 - it copies the output from neuron 1
    conMap.clear();
    conMap[1].append(0);
    weightlessNeuronMap[4] = new WeightlessNeuron(conMap, 0);
    addTraining(*weightlessNeuronMap[4], "0", 0);
    addTraining(*weightlessNeuronMap[4], "1", 1);

    //Build neuron 5 - it copies the output from neuron 2
    conMap.clear();
    conMap[2].append(0);
    weightlessNeuronMap[5] = new WeightlessNeuron(conMap, 0);
    addTraining(*weightlessNeuronMap[5], "0", 0);
    addTraining(*weightlessNeuronMap[5], "1", 1);

    //Build neuron 6 - it copies the output from neuron 3
    conMap.clear();
    conMap[3].append(0);
    weightlessNeuronMap[6] = new WeightlessNeuron(conMap, 0);
    addTraining(*weightlessNeuronMap[6], "0", 0);
    addTraining(*weightlessNeuronMap[6], "1", 1);

    //Create firing neuron map with current state of the network
    QHash<unsigned int, bool> firingNeuronMap;
    firingNeuronMap[3] = true;
    firingNeuronMap[4] = true;
    firingNeuronMap[5] = true;

    //Build and return phi calculator
    PhiCalculator* phiCalc = new PhiCalculator();
    phiCalc->setWeightlessNeuronMap(weightlessNeuronMap);
    phiCalc->setFiringNeuronMap(firingNeuronMap);
    return phiCalc;
}


void PhiUtil::fillByteArray(unsigned char*& byteArr, int& arrLen, QString byteStr){
    if(byteStr.length() % 8 == 0)
	arrLen = byteStr.length() / 8;
    else
	arrLen = byteStr.length() / 8 + 1;
    byteArr = new unsigned char[arrLen];

    //Initialize array
    for(int i=0; i<arrLen; ++i)
	byteArr[i] = 0;

    //Set bits corresponding to 1's in byte string
    for(int i=0; i<byteStr.length(); ++i){
	if(byteStr[i] == '1')
	    byteArr[i/8] |= 1<<(i % 8);
    }
}


void PhiUtil::setGeneralization(QHash<unsigned int, WeightlessNeuron*>& weiNeurMap, double gen){
    for(QHash<unsigned int, WeightlessNeuron*>::iterator iter = weiNeurMap.begin(); iter != weiNeurMap.end(); ++iter)
	iter.value()->setGeneralization(gen);
}


