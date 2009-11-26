#ifndef PHIUTIL_H
#define PHIUTIL_H

//SpikeStream includes
#include "PhiCalculator.h"

//Qt includes
#include <QString>

class PhiUtil {
    public:
	static void addTraining(WeightlessNeuron& neuron, QString trainingPattern, int output);
	bool bitsEqual(byte* byteArr, QString bitPattStr, int output);
	static PhiCalculator* buildPhiTestNetwork1();
	static PhiCalculator* buildPhiTestNetwork2();
	static void fillByteArray(byte*& byteArr, int& arrLen, QString byteStr);
	static void setGeneralization(QHash<unsigned int, WeightlessNeuron*>& weiNeurMap, double gen);

};

#endif//PHIUTIL_H
