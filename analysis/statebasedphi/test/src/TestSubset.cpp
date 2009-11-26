#include "TestSubset.h"
#include "Subset.h"
using namespace spikestream;


void TestSubset::testAddNeuronIndex(){
    //Create list of neurons that subset will relate to
    QList<unsigned int> neuronIDs;
    neuronIDs.append(12);
    neuronIDs.append(13);
    neuronIDs.append(14);
    neuronIDs.append(15);

    //Create subset
    Subset subset(&neuronIDs);

    //Add two neuron indexes to the subset
    subset.addNeuronIndex(0);
    subset.addNeuronIndex(3);
    QCOMPARE(subset.size(), (int)2);

    //Check that the correct neuron ids are returned
    QList<unsigned int> tmpNeurIDs = subset.getNeuronIDs();
    QCOMPARE(tmpNeurIDs.size(), (int)2);
    QCOMPARE(tmpNeurIDs[0], (unsigned int)12);
    QCOMPARE(tmpNeurIDs[1], (unsigned int)15);
}


void TestSubset::testContains(){
    //Create list of neurons that all subsets will relate to
    QList<unsigned int> neuronIDs;
    neuronIDs.append(12);
    neuronIDs.append(13);
    neuronIDs.append(14);
    neuronIDs.append(15);
    neuronIDs.append(16);
    neuronIDs.append(17);

    //Create subset 1 with neurons 12,13,14,15
    Subset subset1(&neuronIDs);
    subset1.addNeuronIndex(0);
    subset1.addNeuronIndex(1);
    subset1.addNeuronIndex(2);
    subset1.addNeuronIndex(3);
    QCOMPARE(subset1.size(), (int)4);//Spot check that size is working ok

    //Create subset 2 with neurons 13,14,15,16
    Subset subset2(&neuronIDs);
    subset2.addNeuronIndex(1);
    subset2.addNeuronIndex(2);
    subset2.addNeuronIndex(3);
    subset2.addNeuronIndex(4);

    //Create subset3  with neurons 13,14,15
    Subset subset3(&neuronIDs);
    subset3.addNeuronIndex(1);
    subset3.addNeuronIndex(2);
    subset3.addNeuronIndex(3);
    QCOMPARE(subset3.size(), (int)3);//Spot check that size is working ok

    //Subset 1 should not contain subsets 2; it should contain subset 3
    QCOMPARE(subset1.contains(&subset2), false);
    QCOMPARE(subset1.contains(&subset3), true);

    //Subset 2 should not contain subset 1; it should contain subset 3
    QCOMPARE(subset2.contains(&subset1), false);
    QCOMPARE(subset2.contains(&subset3), true);

    //Subset 3 should not contain subset 1 or subset 2
    QCOMPARE(subset3.contains(&subset1), false);
    QCOMPARE(subset3.contains(&subset2), false);
}



