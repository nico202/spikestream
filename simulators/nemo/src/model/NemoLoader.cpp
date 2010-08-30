//SpikeStream includes
#include "Globals.h"
#include "NemoLoader.h"
#include "NeuronGroup.h"
#include "SpikeStreamSimulationException.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

//Other includes
#include <vector>
#include <iostream>
using namespace std;

// Outputs verbose debugging behaviour about the loading of the network.
//#define DEBUG


/*! Constructor */
NemoLoader::NemoLoader(){
}


/*! Destructor */
NemoLoader::~NemoLoader(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Loads the simulation */
nemo_network_t NemoLoader::buildNemoNetwork(Network* network, QList<unsigned>* volatileConGrpList, const bool* stop){
	//Initialize the nemo network
	nemo_network_t nemoNet = nemo_new_network();

	//Clear list of volatile connections
	volatileConGrpList->clear();

	//Create the random number generator (from: nemo/examples/random1k.cpp)
	rng_t rng;
	urng_t ranNumGen( rng, boost::uniform_real<double>(0, 1) );//Constructor of the random number generator

	//Calculate progress
	QList<NeuronGroup*> neurGrpList = network->getNeuronGroups();
	QList<ConnectionGroup*> conGrpList = network->getConnectionGroups();
	int totalSteps = neurGrpList.size() + conGrpList.size();
	int stepsCompleted = 0;

	//Add the neuron groups
	for(int i=0; i<neurGrpList.size() && !*stop; ++i){
		unsigned int neurTypeID = neurGrpList.at(i)->getInfo().getNeuronTypeID();
		if(neurTypeID == 1)//FIXME: DO THIS PROPERLY USING THE NAME
			addExcitatoryNeuronGroup(neurGrpList.at(i), nemoNet, ranNumGen);
		else if(neurTypeID == 2)//FIXME: DO THIS PROPERLY USING THE NAME
			addInhibitoryNeuronGroup(neurGrpList.at(i), nemoNet, ranNumGen);
		else
			throw SpikeStreamSimulationException("Neuron group type " + QString::number(neurTypeID) + " is not supported by Nemo");

		//Update progress
		++stepsCompleted;
		emit progress(stepsCompleted, totalSteps);
	}

	//Add the connection groups that are not disabled */
	for(int i=0; i<conGrpList.size() && !*stop; ++i){
		if(conGrpList.at(i)->getParameter("Disable") == 0.0)
			addConnectionGroup(conGrpList.at(i), nemoNet, volatileConGrpList);

		//Update progress
		++stepsCompleted;
		emit progress(stepsCompleted, totalSteps);
	}

	//Return the new network
	return nemoNet;
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Adds a connection group to the network. */
void NemoLoader::addConnectionGroup(ConnectionGroup* conGroup, nemo_network_t nemoNetwork, QList<unsigned>* volatileConGrpList){
	//Extract parameters
	unsigned char learning = 0;
	if(conGroup->getParameter("Learning") != 0.0)
		learning = 1;

	//Store connection group ID if weight is going to change
	if(learning)
		volatileConGrpList->append(conGroup->getID());

	//Work through each connection
	nemo_status_t result;
	QHash<unsigned, Connection*>::const_iterator endConGrp = conGroup->end();
	for(QHash<unsigned, Connection*>::const_iterator conIter = conGroup->begin(); conIter != endConGrp; ++conIter){
		//Add synapse
		result = nemo_add_synapse(nemoNetwork, conIter.value()->getFromNeuronID(), conIter.value()->getToNeuronID(), conIter.value()->getDelay(), conIter.value()->getWeight(), learning);
		#ifdef DEBUG
			//printConnection(fromMapIter.key(), targets, delays, weights, isPlastic, numTargets);
			//cout<<"nemo_add_synapses(nemoNetwork, "<<fromMapIter.key()<<", "<<targets<<", "<<delays<<", "<<weights<<", "<<isPlastic<<", "<<numTargets<<");"<<endl;
		#endif//DEBUG
		if(result != NEMO_OK)
			throw SpikeStreamException("Error code returned from Nemo when adding synapse." + QString(nemo_strerror()));
	}
}


/*! Adds an excitatory neuron group to the simulation network */
void NemoLoader::addExcitatoryNeuronGroup(NeuronGroup* neuronGroup, nemo_network_t nemoNetwork, urng_t& ranNumGen){
	//Extract parameters
	float a = neuronGroup->getParameter("a");
	float b = neuronGroup->getParameter("b");
	float c_1 = neuronGroup->getParameter("c_1");
	float d_1 = neuronGroup->getParameter("d_1");
	float d_2 = neuronGroup->getParameter("d_2");
	float v = neuronGroup->getParameter("v");
	float sigma = neuronGroup->getParameter("sigma");

	//Add neurons to the network
	float c, d, u, rand1, rand2;
	for(NeuronMap::iterator iter = neuronGroup->begin(); iter != neuronGroup->end(); ++iter){
		//Calculate random parameters
		rand1 = ranNumGen();
		rand2 = ranNumGen();
		c = v + c_1 * rand1 * rand2;
		d = d_1 - d_2 * rand1 * rand2;
		u = b * v;

		//Add the neuron to the network
		#ifdef DEBUG
			cout<<"nemo_add_neuron(nemoNetwork, "<<iter.value()->getID()<<", "<<a<<", "<<b<<", "<<v<<", "<<d<<", "<<u<<", "<<v<<", "<<sigma<<");"<<endl;
		#endif//DEBUG
		nemo_status_t result = nemo_add_neuron(nemoNetwork, iter.value()->getID(), a, b, v, d, u, v, sigma);
		if(result != NEMO_OK)
			throw SpikeStreamException("Error code returned from Nemo when adding neuron." + QString(nemo_strerror()));
	}
}


/*! Adds an inhibitory neuron group to the simulation network */
void NemoLoader::addInhibitoryNeuronGroup(NeuronGroup* neuronGroup, nemo_network_t nemoNetwork, urng_t& ranNumGen){
	//Extract parameters
	float a_1 = neuronGroup->getParameter("a_1");
	float a_2 = neuronGroup->getParameter("a_2");
	float b_1 = neuronGroup->getParameter("b_1");
	float b_2 = neuronGroup->getParameter("b_2");
	float d = neuronGroup->getParameter("d");
	float v = neuronGroup->getParameter("v");
	float sigma = neuronGroup->getParameter("sigma");

	//Add neurons to the network
	float a, b, u, rand1, rand2;
	for(NeuronMap::iterator iter = neuronGroup->begin(); iter != neuronGroup->end(); ++iter){
		//Calculate random parameters
		rand1 = ranNumGen();
		rand2 = ranNumGen();
		a = a_1 + a_2 * rand1;
		b = b_1 - b_2 * rand2;
		u = b * v;

		//Add neuron to the network
		#ifdef DEBUG
			cout<<"nemo_add_neuron(nemoNetwork, "<<iter.value()->getID()<<", "<<a<<", "<<b<<", "<<v<<", "<<d<<", "<<u<<", "<<v<<", "<<sigma<<");"<<endl;
		#endif//DEBUG
		nemo_status_t result = nemo_add_neuron(nemoNetwork, iter.value()->getID(), a, b, v, d, u, v, sigma);
		if(result != NEMO_OK)
			throw SpikeStreamException("Error code returned from Nemo when adding neuron." + QString(nemo_strerror()));
	}
}

/*! Prints out information about a particular connection */
void NemoLoader::printConnection(unsigned source, unsigned targets[], unsigned delays[], float weights[], unsigned char is_plastic[], size_t length){
	for(size_t i=0; i<length; ++i){
		if(is_plastic[i])
			cout<<"Connection from: "<<source<<"; to: "<<targets[i]<<"; delay: "<<delays[i]<<"; weight: "<<weights[i]<<" is plastic: true"<<endl;
		else
			cout<<"Connection from: "<<source<<"; to: "<<targets[i]<<"; delay: "<<delays[i]<<"; weight: "<<weights[i]<<" is plastic: false"<<endl;
	}
}


