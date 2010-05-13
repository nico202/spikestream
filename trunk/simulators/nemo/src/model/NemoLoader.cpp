//SpikeStream includes
#include "Globals.h"
#include "NemoLoader.h"
using namespace spikestream;

//Qt includes
#include <QDebug>


/*! Constructor */
NemoLoader::NemoLoader(){
}

/*! Destructor */
NemoLoader::~NemoLoader(){
}


/*! Loads the simulation */
nemo::Simulation* NemoLoader::loadSimulation(Network* network, const bool* stop){
	//Get the parameters for the network

	//Load the network into the simulator
	int cntr = 0;
	while(cntr < 20 && !*stop){
		sleep(1);
		qDebug()<<"LOADING SIMULATION. COUNTER="<<cntr;
		emit(progress(cntr, 10);
		++cntr;
	}

	//Return the new simulation object
	return NULL;
}

