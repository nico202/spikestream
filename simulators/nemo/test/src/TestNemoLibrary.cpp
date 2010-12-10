#include "TestNemoLibrary.h"

//SpikeStream includes
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

//Other includes
#include <vector>
#include <cstdlib>
#include <iostream>
#include <cassert>

#define DEBUG true

//Initialize static variables
synapse_id TestNemoLibrary::synapseIDCntr = 1;



void TestNemoLibrary::addExcitatoryNeuron(nemo_network_t c_net, unsigned nidx){
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


void TestNemoLibrary::addExcitatorySynapses(nemo_network_t c_net, unsigned source, unsigned ncount, unsigned scount){
	for(unsigned s = 0; s < scount; ++s) {
		synapse_id synID = getSynapseID();
		nemo_add_synapse(
				c_net, //Nemo network
				source, //Source neuron ID
				(rand() % ncount), //Target neuron ID
				1, //Delay
				0.5f * (float(rand()) / RAND_MAX), //Weight
				0, //Is plastic
				&synID //ID of synapse being created
		);
	}
}


void TestNemoLibrary::addInhibitoryNeuron(nemo_network_t c_net, unsigned nidx){
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


void TestNemoLibrary::addInhibitorySynapses(nemo_network_t c_net, unsigned source, unsigned ncount, unsigned scount){
	for(unsigned s = 0; s < scount; ++s) {
		synapse_id synID = getSynapseID();
		nemo_add_synapse(
				c_net, //Nemo network
				source, //Source neuron ID
				(rand() % ncount), //Target neuron ID
				1, //Delay
				-1.0f * (float(rand()) / RAND_MAX), //Weight
				0, //Is plastic
				&synID //ID of synapse being created
		);
	}
}


void TestNemoLibrary::c_runSimulation(const nemo_network_t net, const nemo_configuration_t conf, unsigned seconds,	std::vector<unsigned>* fcycles,	std::vector<unsigned>* fnidx){
	if(DEBUG) qDebug()<<"About to create simulation";
	nemo_simulation_t sim = nemo_new_simulation(net, conf);
	if(DEBUG) qDebug()<<"Simulation created successfully.";

	fcycles->clear();
	fnidx->clear();

	unsigned* firedArray;
	unsigned firedCount;

	for(unsigned ms = 0; ms < 10; ++ms) {
		if(DEBUG) qDebug()<<"Stepping simulation; milliseconds="<<ms;
			nemo_step(sim, NULL, 0, NULL, NULL, 0, &firedArray, &firedCount);
		if(DEBUG) qDebug()<<"Simulation stepped; milliseconds="<<ms;
	}

	if(DEBUG) qDebug()<<"Deleting simulation";
	nemo_delete_simulation(sim);
	if(DEBUG) qDebug()<<"Simulation successfully deleted.";
}


void TestNemoLibrary::testNemoDLL1(){
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

	nemo_delete_configuration(c_conf);
	nemo_delete_network(c_net);
}


void TestNemoLibrary::testNemoDLL2(){
	//Set up the configuration with the default parameters
	if(DEBUG) qDebug()<<"Creating nemo configuration.";
	nemo_configuration_t nemoConfig = nemo_new_configuration();
	if(DEBUG) qDebug()<<"Nemo configuration successfully created.";

	//Add neurons
	if(DEBUG) qDebug()<<"Adding neurons.";
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

	if(DEBUG) qDebug()<<"Neurons successfully added.";

	//Load the network into the simulator
	if(DEBUG) qDebug()<<"Loading neurons into simulator.";
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
	if(DEBUG) qDebug()<<"Neurons successfully loaded into simulator";
}


void TestNemoLibrary::testNemoConfiguration(){
	try{
		//Create default configuration object
		nemo_configuration_t nemoConfig = nemo_new_configuration();

		//Get the number of threads
		int numThreads;
		checkNemoOutput(nemo_cpu_thread_count(nemoConfig, &numThreads), "Failed to get CPU thread count.");
		qDebug()<<"Number of threads is "<<numThreads;

		//Test the STDP function
		float* pre = new float[20];
		float* post = new float[20];
		for(unsigned i = 0; i < 20; ++i) {
			float dt = float(i + 1);
			pre[i] = 1.0f * expf(-dt / 20.0f);
			post[i] = -0.8f * expf(-dt / 20.0f);
		}
		nemo_set_stdp_function(nemoConfig, pre, 20,	post, 20, -10, 10);

		//Test getting the cuda device
		int cudaDev;
		checkNemoOutput(nemo_cuda_device(nemoConfig, &cudaDev), "Error getting CUDA device from NeMo");
		qDebug()<<"CUDA device is: "<<cudaDev;

		//Test getting the backend
		backend_t backend;
		checkNemoOutput(nemo_backend(nemoConfig, &backend), "Failed to get NeMo backend.");
		if(backend == NEMO_BACKEND_CUDA)
			qDebug()<<"CUDA backend";
		else if(backend == NEMO_BACKEND_CPU)
			qDebug()<<"CPU backend";
		else
			QFAIL("Unrecognized backend from NeMo");

		//Test listing the CUDA devices
		unsigned numCudaDevices = 0;
		bool testCUDA = true;
		checkNemoOutput(nemo_cuda_device_count(&numCudaDevices), "Failed to get list of NeMo devices");
		if(numCudaDevices == 0){
			qDebug()<<"No CUDA devices available.";
			testCUDA = false;
		}
		else {
			for(unsigned i=0; i<numCudaDevices; ++i){
				const char* devDesc;
				checkNemoOutput(nemo_cuda_device_description(i, &devDesc), "Error getting device description.");
				qDebug()<<"CUDA Device available: "<<devDesc;
			}
		}

		//Set backend to CPU
		checkNemoOutput(nemo_set_cpu_backend(nemoConfig, 1), "Failed to set backend to CPU: ");
		checkNemoOutput(nemo_backend(nemoConfig, &backend), "Failed to get NeMo backend.");
		QVERIFY(backend == NEMO_BACKEND_CPU);
		checkNemoOutput(nemo_cpu_thread_count(nemoConfig, &numThreads), "Failed to get CPU thread count.");
		QCOMPARE(numThreads, 1);

		//Set backend to CUDA
		if(testCUDA){
			checkNemoOutput(nemo_set_cuda_backend(nemoConfig, 0), "Failed to set CUDA device: ");
			checkNemoOutput(nemo_backend(nemoConfig, &backend), "Failed to get NeMo backend.");
			checkNemoOutput(nemo_cuda_device(nemoConfig, &cudaDev), "Error getting CUDA device from NeMo");
			QVERIFY(backend == NEMO_BACKEND_CUDA);
			QCOMPARE(cudaDev, 0);
		}
	}
	catch(SpikeStreamException& ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("ERROR LOADING NEMO SIMULATION.");
	}
}


/*! Returns a unique synapse id. */
synapse_id TestNemoLibrary::getSynapseID(){
	++synapseIDCntr;
	return synapseIDCntr;
}


/*! Checks the output from a nemo function call and throws exception if there is an error */
void TestNemoLibrary::checkNemoOutput(nemo_status_t result, const QString& errorMessage){
	if(result != NEMO_OK)
		throw SpikeStreamException(errorMessage + ": " + nemo_strerror());
}
