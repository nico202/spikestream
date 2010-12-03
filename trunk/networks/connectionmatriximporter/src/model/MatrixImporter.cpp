//SpikeStream includes
#include "ConnectionGroupBuilder.h"
#include "Globals.h"
#include "MatrixImporter.h"
#include "Network.h"
#include "NeuronGroupBuilder.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QFile>


/*! Constructor */
MatrixImporter::MatrixImporter() : SpikeStreamThread() {

}


/*! Destructor */
MatrixImporter::~MatrixImporter(){

}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

//Inherited from QThread
void MatrixImporter::run(){
	clearError();
	stopThread = false;
	try{
		//Create random number generator
		rng_t rng(randomSeed);
		urng_t ranNumGen( rng, boost::uniform_real<double>(0, 1) );//Constructor of the random number generator

		//Add neurons
		NeuronGroupBuilder neuronGroupBuilder;
		connect(&neuronGroupBuilder, SIGNAL(progress(int,int,QString)), this, SLOT(updateProgress(int,int,QString)));
		neuronGroupBuilder.addNeuronGroups(newNetwork, coordinatesFilePath, nodeNamesFilePath, parameterMap, ranNumGen);

		//Check for cancellation
		if(stopThread)
			return;

		//Add connections
		ConnectionGroupBuilder connectionGroupBuilder(neuronGroupBuilder.getExcitatoryNeuronGroups(), neuronGroupBuilder.getInhibitoryNeuronGroups());
		connect(&connectionGroupBuilder, SIGNAL(progress(int,int,QString)), this, SLOT(updateProgress(int,int,QString)));
		connectionGroupBuilder.addConnectionGroups(newNetwork, &stopThread, weightsFilePath, delaysFilePath, parameterMap, ranNumGen);
	}
	catch(SpikeStreamException& ex){
		setError(ex.getMessage());
	}
	catch(...){
		setError("An unknown error occurred");
	}

	stopThread = true;
}


/*! Checks and stores the necessary parameters and starts the thread running. */
void MatrixImporter::startImport(Network* network, const QString& coordinatesFilePath, const QString& nodeNamesFilePath, const QString& weightsFilePath, const QString& delaysFilePath, QHash<QString, double>& parameterMap){
	//Check and store file locations
	if(!QFile::exists(coordinatesFilePath))
		throw SpikeStreamException("Coordinates file does not exist: " + coordinatesFilePath);
	this->coordinatesFilePath = coordinatesFilePath;
	if(!QFile::exists(nodeNamesFilePath))
		throw SpikeStreamException("Node names file does not exist: " + nodeNamesFilePath);
	this->nodeNamesFilePath = nodeNamesFilePath;
	if(!QFile::exists(weightsFilePath))
		throw SpikeStreamException("Connectivity matrix file does not exist: " + weightsFilePath);
	this->weightsFilePath = weightsFilePath;
	if(!QFile::exists(delaysFilePath))
		throw SpikeStreamException("Delays file does not exist: " + delaysFilePath);
	this->delaysFilePath = delaysFilePath;

	//Check and store the parameters and network
	newNetwork = network;
	storeParameters(parameterMap);

	//Start thread running
	this->start();
}


/*! Called by other classes to report progress,
	emits signal with this progress information. */
void MatrixImporter::updateProgress(int stepsCompleted, int totalSteps, QString message){
	if(this->isRunning())
		emit progress(stepsCompleted, totalSteps, message);
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Checks and stores the parameters */
void MatrixImporter::storeParameters(QHash<QString, double>& paramMap){
	if(!paramMap.contains("neuron_group_size"))
		throw SpikeStreamException("MatrixLoader: neuron_group_size parameter missing");

	if(!paramMap.contains("proportion_excitatory_neurons"))
		throw SpikeStreamException("MatrixLoader: proportion_excitatory_neurons parameter missing.");

	if(paramMap["proportion_excitatory_neurons"] < 0.0 || paramMap["proportion_excitatory_neurons"] > 1.0)
		throw SpikeStreamException("MatrixLoader: proportion_excitatory_neurons parameter out of range. It should be between 0.0 and 1.0.");

	if(!paramMap.contains("rewire_probability"))
		throw SpikeStreamException("MatrixLoader: rewire_probability parameter missing.");

	if(paramMap["rewire_probability"] < 0.0 || paramMap["rewire_probability"] > 1.0)
		throw SpikeStreamException("MatrixLoader: rewire_probability parameter out of range. It should be between 0.0 and 1.0.");

	randomSeed = Util::getUIntParameter("random_seed", paramMap);

	this->parameterMap = paramMap;
}

