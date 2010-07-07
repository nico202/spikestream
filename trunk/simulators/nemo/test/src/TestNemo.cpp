#include "TestNemo.h"

//Nemo includes
#include "Nemo.h"

//Qt includes
#include <QDebug>


#include <vector>
#include <cstdlib>
#include <iostream>
#include <cassert>
#include <nemo.h>


void addExcitatoryNeuron(nemo_network_t c_net, unsigned nidx){
	float v = -65.0f;
	float a = 0.02f;
	float b = 0.2f;

	float r1 = (float(rand()) / RAND_MAX);
	float r2 = (float(rand()) / RAND_MAX);
	float c = v + 15.0f * r1 * r1;
	float d = 8.0f - 6.0f * r2 * r2;
	float u = b * v;
	float sigma = 5.0f;
	nemo_add_neuron(c_net, nidx, a, b, c, d, u, v, sigma);
}


void addExcitatorySynapses(nemo_network_t c_net, unsigned source, unsigned ncount, unsigned scount){
	std::vector<unsigned> targets(scount, 0U);
	std::vector<unsigned> delays(scount, 1U);
	std::vector<float> weights(scount, 0.0f);
	std::vector<unsigned char> isPlastic(scount, 0);

	for(unsigned s = 0; s < scount; ++s) {
		targets.at(s) = rand() % ncount;
		weights.at(s) = 0.5f * (float(rand()) / RAND_MAX);
	}

	nemo_add_synapses(c_net, source, &targets[0], &delays[0], &weights[0], &isPlastic[0], targets.size());
}


void addInhibitoryNeuron(nemo_network_t c_net, unsigned nidx){
	float v = -65.0f;
	float r1 = (float(rand()) / RAND_MAX);
	float a = 0.02f + 0.08f * r1;
	float r2 = (float(rand()) / RAND_MAX);
	float b = 0.25f - 0.05f * r2;
	float c = v;
	float d = 2.0f;
	float u = b * v;
	float sigma = 2.0f;

	nemo_add_neuron(c_net, nidx, a, b, c, d, u, v, sigma);
}


void addInhibitorySynapses(nemo_network_t c_net, unsigned source, unsigned ncount, unsigned scount){
	std::vector<unsigned> targets(scount, 0);
	std::vector<unsigned> delays(scount, 1U);
	std::vector<float> weights(scount, 0.0f);
	std::vector<unsigned char> isPlastic(scount, 0);

	for(unsigned s = 0; s < scount; ++s) {
		targets.at(s) = rand() % ncount;
		weights.at(s) = -(float(rand()) / RAND_MAX);
	}
	nemo_add_synapses(c_net, source, &targets[0], &delays[0], &weights[0], &isPlastic[0], targets.size());
}


void c_runSimulation(const nemo_network_t net, const nemo_configuration_t conf, unsigned seconds,	std::vector<unsigned>* fcycles,	std::vector<unsigned>* fnidx){
	qDebug()<<"About to create simulation";
	nemo_simulation_t sim = nemo_new_simulation(net, conf);
	qDebug()<<"Simulation created successfully.";

	fcycles->clear();
	fnidx->clear();

	//! todo vary the step size between reads to firing buffer

	//for(unsigned s = 0; s < 1; ++s){
		for(unsigned ms = 0; ms < 10; ++ms) {
			qDebug()<<"Stepping simulation; milliseconds="<<ms;
			nemo_step(sim, NULL, 0);
			qDebug()<<"Simulation stepped; milliseconds="<<ms;

			//! \todo could modify API here to make this nicer
			unsigned* cycles_tmp;
			unsigned* nidx_tmp;
			unsigned nfired;
			unsigned ncycles;

			//if(ms >= 6){
				qDebug()<<"Reading firing";
				nemo_status_t result = nemo_read_firing(sim, &cycles_tmp, &nidx_tmp, &nfired, &ncycles);
				if(result != NEMO_OK)
					QFAIL(nemo_strerror());
				qDebug()<<"Firing successfully read.";

				// push data back onto local buffers
				qDebug()<<"Copying firing data";
				std::copy(cycles_tmp, cycles_tmp + nfired, back_inserter(*fcycles));
				std::copy(nidx_tmp, nidx_tmp + nfired, back_inserter(*fnidx));
				qDebug()<<"Firing data successfully copied";
			//}
		}
//	}

	qDebug()<<"Deleting simulation";
	nemo_delete_simulation(sim);
	qDebug()<<"Simulation successfully deleted.";
}


void compareSimulationResults(const std::vector<unsigned>& cycles2, const std::vector<unsigned>& nidx2){
	std::cerr << "Comparing results (" << cycles2.size() << " firings)\n";
	assert(cycles2.size() == nidx2.size());

}



void TestNemo::testNemoDLL1(){
	unsigned ncount = 1000;
	unsigned scount = 1000;

	std::cerr << "Creating network (C API)\n";
	nemo_network_t c_net = nemo_new_network();

	std::cerr << "Populating networks\n";
	for(unsigned nidx=0; nidx < ncount; ++nidx) {
		if(nidx < (ncount * 4) / 5) { // excitatory
			addExcitatoryNeuron(c_net, nidx);
			addExcitatorySynapses(c_net, nidx, ncount, scount);
		}
		else { // inhibitory
			addInhibitoryNeuron(c_net, nidx);
			addInhibitorySynapses(c_net, nidx, ncount, scount);
		}
	}

	unsigned duration = 2; // seconds
	std::vector<unsigned>cycles2, nidx2;

	nemo_configuration_t c_conf = nemo_new_configuration();
	std::cerr << "Running network (C API)\n";
	c_runSimulation(c_net, c_conf, duration, &cycles2, &nidx2);
	compareSimulationResults(cycles2, nidx2);

	nemo_delete_configuration(c_conf);
	nemo_delete_network(c_net);
}


void TestNemo::testNemoDLL2(){
	//Set up the configuration with the default parameters
	qDebug()<<"Creating nemo configuration.";
	nemo_configuration_t nemoConfig = nemo_new_configuration();
	qDebug()<<"Nemo configuration successfully created.";

	//Add neurons
	qDebug()<<"Adding neurons.";
	nemo_network_t nemoNet = nemo_new_network();

	nemo_status_t result = nemo_add_neuron(nemoNet, 1, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7);
	if(result != NEMO_OK){
		qDebug()<<"Add neuron failed.";
		QFAIL(nemo_strerror());
	}

	result = nemo_add_neuron(nemoNet, 2, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7);
	if(result != NEMO_OK){
		qDebug()<<"Add neuron failed.";
		QFAIL(nemo_strerror());
	}

	result = nemo_add_neuron(nemoNet, 3, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7);
	if(result != NEMO_OK){
		qDebug()<<"Add neuron failed.";
		QFAIL(nemo_strerror());
	}

	result = nemo_add_neuron(nemoNet, 4, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7);
	if(result != NEMO_OK){
		qDebug()<<"Add neuron failed.";
		QFAIL(nemo_strerror());
	}

	qDebug()<<"Neurons successfully added.";

	//Load the network into the simulator
	qDebug()<<"Loading neurons into simulator.";
	nemo_simulation_t nemoSim ;
	try{
		nemoSim = nemo_new_simulation(nemoNet, nemoConfig);
	}
	catch(...){
		QFAIL("ERROR LOADING NEMO SIMULATION.");
	}
	if(nemoSim == NULL) {
		qDebug()<<"Nemo simulation is null";
		QFAIL(nemo_strerror());
	}
	qDebug()<<"Neurons successfully loaded into simulator";
}
