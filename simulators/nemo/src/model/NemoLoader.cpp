//SpikeStream includes
#include "Globals.h"
#include "NemoLoader.h"
#include "NeuronGroup.h"
#include "SpikeStreamSimulationException.h"
#include "SpikeStreamIOException.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

//Other includes
#include <vector>
#include <iostream>
using namespace std;

// Outputs verbose debugging behaviour about the loading of the network.
//#define DEBUG_NEURONS
//#define DEBUG_SYNAPSES

//Neuron type IDs in database. FIXME: WOULD BE BETTER TO USE THE NAME
#define IZHIKEVICH_EXCITATORY_NEURON_ID 1
#define IZHIKEVICH_INHIBITORY_NEURON_ID 2


/*! Constructor */
NemoLoader::NemoLoader(){
	//Open up log file if logging is enabled
	#if defined(DEBUG_NEURONS) || defined(DEBUG_SYNAPSES)
		logFile = new QFile(Globals::getSpikeStreamRoot() + "/log/NemoLoader.log");
		if(logFile->open(QFile::WriteOnly | QFile::Truncate))
			logTextStream = new QTextStream(logFile);
		else{
			throw SpikeStreamIOException("Cannot open log file for NemoLoader.");
		}
	#endif//DEBUG_NEURONS || DEBUG_SYNAPSES
}


/*! Destructor */
NemoLoader::~NemoLoader(){
	//Clean up log file if logging is enabled
	#if defined(DEBUG_NEURONS) || defined(DEBUG_SYNAPSES)
		logFile->close();
		delete logFile;
		delete logTextStream;
	#endif//DEBUG_NEURONS || DEBUG_SYNAPSES
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Loads the simulation */
nemo_network_t NemoLoader::buildNemoNetwork(Network* network, QHash<unsigned, synapse_id*>& volatileConGrpMap, const bool* stop){
	//Initialize the nemo network
	nemo_network_t nemoNet = nemo_new_network();

	//Check that list of volatie connection is empty
	if(!volatileConGrpMap.isEmpty())
		throw SpikeStreamSimulationException("Volatile connection group map should have been cleared when simulation was unloaded.");

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
		if(neurTypeID == IZHIKEVICH_EXCITATORY_NEURON_ID)
			addExcitatoryNeuronGroup(neurGrpList.at(i), nemoNet, ranNumGen);
		else if(neurTypeID == IZHIKEVICH_INHIBITORY_NEURON_ID)
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
			addConnectionGroup(conGrpList.at(i), nemoNet, volatileConGrpMap);

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
void NemoLoader::addConnectionGroup(ConnectionGroup* conGroup, nemo_network_t nemoNetwork, QHash<unsigned, synapse_id*>& volatileConGrpMap){
	//Extract parameters
	unsigned char learning = 0;
	synapse_id* synapseIDArray = NULL;
	if(conGroup->getParameter("Learning") != 0.0){
		learning = 1;
		synapseIDArray = new synapse_id[conGroup->size()];
		volatileConGrpMap[conGroup->getID()] = synapseIDArray;
	}
	double weightFactor = conGroup->getParameter("weight_factor");

	//Work through each connection
	unsigned conCntr = 0;
	nemo_status_t result;
	synapse_id newNemoSynapseID;
	ConnectionIterator endConGrp = conGroup->end();
	for(ConnectionIterator conIter = conGroup->begin(); conIter != endConGrp; ++conIter){
		//Add synapse
		result = nemo_add_synapse(nemoNetwork, conIter->getFromNeuronID(), conIter->getToNeuronID(), conIter->getDelay(), weightFactor * conIter->getWeight(), learning, &newNemoSynapseID);
		#ifdef DEBUG_SYNAPSES
<<<<<<< .mine
			(*logTextStream)<<"nemo_add_synapse(nemoNetwork, "<<conIter->getFromNeuronID()<<", "<<conIter->getToNeuronID()<<", "<<conIter->getDelay()<<", "<<(weightFactor * conIter->getWeight())<<", "<<learning<<", &newNemoSynapseID);"<<endl;
=======
			(*logTextStream)<<"nemo_add_synapse(nemoNetwork, "<<conIter->getFromNeuronID()<<", "<<conIter->getToNeuronID()<<", "<<conIter->getDelay()<<", "<<(weightFactor * conIter->getWeight())<<", "<<learning<<", "<<newNemoSynapseID<<");"<<endl;
>>>>>>> .r232
		#endif//DEBUG_SYNAPSES
		if(result != NEMO_OK)
			throw SpikeStreamException("Error code returned from Nemo when adding synapse." + QString(nemo_strerror()));

		//Store link between connection group ID and map linking nemo connection IDs and SpikeStream connection IDs
		if(learning){
			synapseIDArray[conCntr] = newNemoSynapseID;
			++conCntr;
		}
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
		c = v + c_1 * rand1 * rand1;
		d = d_1 - d_2 * rand2 * rand2;
		u = b * v;

		//Add the neuron to the network
		#ifdef DEBUG_NEURONS
			(*logTextStream)<<"nemo_add_neuron(nemoNetwork, "<<iter.value()->getID()<<", "<<a<<", "<<b<<", "<<c<<", "<<d<<", "<<u<<", "<<v<<", "<<sigma<<");"<<endl;
		#endif//DEBUG_NEURONS
		nemo_status_t result = nemo_add_neuron_iz(nemoNetwork, iter.value()->getID(), a, b, c, d, u, v, sigma);
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
		#ifdef DEBUG_NEURONS
			(*logTextStream)<<"nemo_add_neuron(nemoNetwork, "<<iter.value()->getID()<<", "<<a<<", "<<b<<", "<<v<<", "<<d<<", "<<u<<", "<<v<<", "<<sigma<<");"<<endl;
		#endif//DEBUG_NEURONS
		nemo_status_t result = nemo_add_neuron_iz(nemoNetwork, iter.value()->getID(), a, b, v, d, u, v, sigma);
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


