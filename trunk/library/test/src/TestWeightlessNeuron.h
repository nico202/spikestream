#ifndef TESTWEIGHTLESSNEURON_H
#define TESTWEIGHTLESSNEURON_H

#include "WeightlessNeuron.h"
using namespace spikestream;

//Qt includes
#include <QTest>

class TestWeightlessNeuron : public QObject {
    Q_OBJECT

    private slots:
	void testLookup();
	void testAddTraining();
	void testGetFiringStateProbability();
	void testGetTransitionProbability();
	void testSetGeneralization();

    private:
	void addTraining(WeightlessNeuron& neuron, QString trainingPattern, int output);
	bool bitsEqual(unsigned char* byteArr, QString bitPattStr, int output);
	void fillByteArray(unsigned char*& byteArr, int& arrLen, QString byteStr);
};

#endif//TESTWEIGHTLESSNEURON_H

