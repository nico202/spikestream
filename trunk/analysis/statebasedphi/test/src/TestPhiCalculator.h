#ifndef TESTPHICALCULATOR_H
#define TESTPHICALCULATOR_H

//SpikeStream includes
#include "PhiCalculator.h"
#include "WeightlessNeuron.h"
using namespace spikestream;

//Qt includes
#include <QTest>

class TestPhiCalculator : public QObject {
    Q_OBJECT

    private slots:
	void testCalculateReverseProbability();
	void testFillPartitionLists();
	void testFillProbabilityTable();
	void testGetCausalProbability();
	void testGetPartitionPhi();
	void testGetSubsetPhi();

    private:
};


#endif//TESTPHICALCULATOR_H

