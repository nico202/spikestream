#include "TestNemo.h"

//Nemo includes
#include "Nemo.h"

//Qt includes
#include <QDebug>


void TestNemo::testNemoDLL(){
	//Set up the configuration with the default parameters
	nemo_configuration_t nemoConfig = nemo_new_configuration();
qDebug()<<"PROBE1";
	//Add neurons
	nemo_network_t nemoNet = nemo_new_network();

	nemo_status_t result = nemo_add_neuron(nemoNet, 1, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7);
	if(result != NEMO_OK)
		QFAIL(nemo_strerror());

	result = nemo_add_neuron(nemoNet, 2, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7);
	if(result != NEMO_OK)
		QFAIL(nemo_strerror());

	result = nemo_add_neuron(nemoNet, 3, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7);
	if(result != NEMO_OK)
		QFAIL(nemo_strerror());

	result = nemo_add_neuron(nemoNet, 4, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7);
	if(result != NEMO_OK)
		QFAIL(nemo_strerror());
qDebug()<<"PROBE2";
	//Load the network into the simulator
	nemo_simulation_t nemoSim ;
	try{
		nemoSim = nemo_new_simulation(nemoNet, nemoConfig);
	}
	catch(...){
		QFAIL("ERROR LOADING NEMO SIMULATION.");
	}

	qDebug()<<"PROBE2A"<<nemoSim;
	if(nemoSim == NULL) {
		QFAIL(nemo_strerror());
	}
	qDebug()<<"PROBE3";
}
