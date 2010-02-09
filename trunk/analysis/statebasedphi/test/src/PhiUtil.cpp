#include "PhiUtil.h"
#include "SpikeStreamException.h"
using namespace spikestream;


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

