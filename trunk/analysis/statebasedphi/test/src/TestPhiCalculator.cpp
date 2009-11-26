#include "TestPhiCalculator.h"
#include "SpikeStreamException.h"
#include "PhiUtil.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

void TestPhiCalculator::testCalculateReverseProbability(){
    //Build table to be reversed
    ProbabilityTable causalTable(2);
    causalTable.set("00", 0.7);
    causalTable.set("01", 0.6);
    causalTable.set("10", 0.5);
    causalTable.set("11", 0.4);

    //Table to hold results
    ProbabilityTable resultTable(2);

    //Reverse the table
    PhiCalculator phiCalc;
    phiCalc.calculateReverseProbability(causalTable, resultTable);

    //Check the probabilities
    QCOMPARE(Util::rDouble(resultTable.get("00"), 3), 0.318);
    QCOMPARE(Util::rDouble(resultTable.get("01"), 3), 0.273);
    QCOMPARE(Util::rDouble(resultTable.get("10"), 3), 0.227);
    QCOMPARE(Util::rDouble(resultTable.get("11"), 3), 0.182);
}


void TestPhiCalculator::testFillPartitionLists(){
    //Lists
    QList<unsigned int> aPartition, bPartition, subsetNeurIDs;

    //Fill list of neuron ids
    subsetNeurIDs.append(23);
    subsetNeurIDs.append(33);
    subsetNeurIDs.append(43);
    subsetNeurIDs.append(53);
    subsetNeurIDs.append(63);

    //fill partition array
    bool partitionArray[5];
    partitionArray[0] = false;
    partitionArray[1] = true;
    partitionArray[2] = true;
    partitionArray[3] = false;
    partitionArray[4] = true;

    //Invoke the method being tested
    PhiCalculator phiCalc;
    phiCalc.fillPartitionLists(aPartition, bPartition, &partitionArray[0], 5, subsetNeurIDs);

    //Check the A and B partition are correct
    QCOMPARE(aPartition.size(), (int)3);
    QCOMPARE(bPartition.size(), (int)2);

    QCOMPARE(aPartition[0], (unsigned int)33);
    QCOMPARE(aPartition[1], (unsigned int)43);
    QCOMPARE(aPartition[2], (unsigned int)63);

    QCOMPARE(bPartition[0], (unsigned int)23);
    QCOMPARE(bPartition[1], (unsigned int)53);
}


void TestPhiCalculator::testFillProbabilityTable(){
}


void TestPhiCalculator::testGetCausalProbability(){
    /* Create three weightless neurons
	n1 and n2 are connected to n3
	n3 is trained with 00->1; 11->0 */
    QHash<unsigned int, unsigned int> conMap;
    QHash<unsigned int, WeightlessNeuron*> weightlessNeuronMap;
    weightlessNeuronMap[1] = new WeightlessNeuron(conMap);
    weightlessNeuronMap[2] = new WeightlessNeuron(conMap);
    conMap[1] = 0;
    conMap[2] = 1;
    weightlessNeuronMap[3] = new WeightlessNeuron(conMap);

    //Add two pieces of training data to n3
    PhiUtil::addTraining(*weightlessNeuronMap[3], "00", 1);
    PhiUtil::addTraining(*weightlessNeuronMap[3], "11", 0);

    //Create firing neuron map
    QHash<unsigned int, bool> firingNeuronMap;
    firingNeuronMap[2] = true;

    //Build phi calculator
    PhiCalculator phiCalc;
    phiCalc.setWeightlessNeuronMap(weightlessNeuronMap);
    phiCalc.setFiringNeuronMap(firingNeuronMap);

    /* Create neuron id list
       list can have neuron ids in any order and their firng state is the correspondign position in the pattern */
    QList<unsigned int> neurIDList;
    neurIDList.append(3);
    neurIDList.append(1);
    neurIDList.append(2);

    try{
	//Test with n1=0, n2=0, n3=1 and generalization = 1.0
	QString x0Pattern = "100";
	PhiUtil::setGeneralization(weightlessNeuronMap, 1.0);
	QCOMPARE(phiCalc.getCausalProbability(neurIDList, x0Pattern), 0.0);

	//Test with n1=1, n2=1, n3=1 and generlization = 1.0
	x0Pattern = "111";
	QCOMPARE(phiCalc.getCausalProbability(neurIDList, x0Pattern), 0.25);

	//Test with n1=1, n2=0, n3=0 and generalization = 0.5
	x0Pattern = "010";
	PhiUtil::setGeneralization(weightlessNeuronMap, 0.5);
	QCOMPARE(phiCalc.getCausalProbability(neurIDList, x0Pattern), 0.125);

    }
    catch(SpikeStreamException ex){
	QFAIL(ex.getMessage().toAscii());
    }
}


void TestPhiCalculator::testGetPartitionPhi(){
    //Create an instance of PhiCalculator set up for phi test network 1
    PhiCalculator* phiCalc = PhiUtil::buildPhiTestNetwork1();

    //Lists used to hold partitions
    QList<unsigned int> aList, bList;

    try{
	//Get the phi for the entire partition
	bList.append(1);
	bList.append(2);
	bList.append(3);
	bList.append(4);
	QCOMPARE(phiCalc->getPartitionPhi(aList, bList), 4.0);

	//Get phi for partition into left and right halves
	aList.clear();
	bList.clear();
	aList.append(1);
	aList.append(2);
	bList.append(3);
	bList.append(4);
	QCOMPARE(phiCalc->getPartitionPhi(aList, bList), 0.0);

	//Get phi for partition into top and bottom halves
	aList.clear();
	bList.clear();
	aList.append(1);
	aList.append(3);
	bList.append(2);
	bList.append(4);
	QCOMPARE(phiCalc->getPartitionPhi(aList, bList), 4.0);

	//Get phi for partition with 1 in A and 2,3,4 in B
	aList.clear();
	bList.clear();
	aList.append(1);
	bList.append(2);
	bList.append(3);
	bList.append(4);
	QCOMPARE(phiCalc->getPartitionPhi(aList, bList), 2.0);
    }
    catch(SpikeStreamException& ex){
	QFAIL(ex.getMessage().toAscii());
    }


    //Clean up
    delete phiCalc;
}


void TestPhiCalculator::testGetSubsetPhi(){
    //Create an instance of PhiCalculator set up for phi test network 1
    PhiCalculator* phiCalc1 = PhiUtil::buildPhiTestNetwork1();
    QList<unsigned int> subsetList;

    try{
	//Calculate phi for whole subset
	subsetList.append(1);
	subsetList.append(2);
	subsetList.append(3);
	subsetList.append(4);
	QCOMPARE(phiCalc1->getSubsetPhi(subsetList), 0.0);

	//Calculate phi for two neurons
	subsetList.clear();
	subsetList.append(1);
	subsetList.append(2);
	QCOMPARE(phiCalc1->getSubsetPhi(subsetList), 2.0);
    }
    catch(SpikeStreamException& ex){
	QFAIL(ex.getMessage().toAscii());
    }

    //Clean up
    delete phiCalc1;

    //Create an instance of PhiCalculator set up for phi test network 2
    PhiCalculator* phiCalc2 = PhiUtil::buildPhiTestNetwork2();

    try{
	//Calculate phi for whole network
	subsetList.clear();
	subsetList.append(1);
	subsetList.append(2);
	subsetList.append(3);
	subsetList.append(4);
	subsetList.append(5);
	subsetList.append(6);
	QCOMPARE(phiCalc2->getSubsetPhi(subsetList), 0.0);

	//Calculate phi for subset of neurons 1.2.3
	subsetList.clear();
	subsetList.append(1);
	subsetList.append(2);
	subsetList.append(3);
	QCOMPARE(phiCalc2->getSubsetPhi(subsetList), 3.0);

	//Calculate phi for subset with neurons 3,6
	subsetList.clear();
	subsetList.append(3);
	subsetList.append(6);
	QCOMPARE(phiCalc2->getSubsetPhi(subsetList), 1.0);

    }
    catch(SpikeStreamException& ex){
	QFAIL(ex.getMessage().toAscii());
    }

    //Clean up
    delete phiCalc2;
}



