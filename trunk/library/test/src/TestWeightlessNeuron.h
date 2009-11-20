#ifndef TESTWEIGHTLESSNEURON_H
#define TESTWEIGHTLESSNEURON_H

//Qt includes
#include <QTest>

class TestWeightlessNeuron : public QObject {
    Q_OBJECT

    private slots:
	void testLookup();
	void testAddTraining();
	void testGetFiringStateProbability();
	void testGetTrainingData();
	void testGetTransitionProbability();
	void testSetGeneralization();
};

#endif//TESTWEIGHTLESSNEURON_H

