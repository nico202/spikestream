//SpikeStream includes
#include "ConnectionGroupBuilder.h"
#include "Globals.h"
#include "MatrixImporter.h"
#include "Network.h"
#include "NeuronGroupBuilder.h"
#include "SpikeStreamException.h"
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
	Network* newNetwork = NULL;
	try{
		NeuronGroupBuilder neuronGroupBuilder;
		connect(&neuronGroupBuilder, SIGNAL(progress(int,int,QString)), this, SLOT(updateProgress(int,int,QString)));

		//Create network
		Network* newNetwork = new Network(networkName, networkDescription, Globals::getNetworkDao()->getDBInfo(), Globals::getArchiveDao()->getDBInfo());
		newNetwork->setPrototypeMode(true);

		//Add neurons
		neuronGroupBuilder.addNeuronGroups(newNetwork, coordinatesFilePath, nodeNamesFilePath, parameterMap);

		//Add connections
		ConnectionGroupBuilder connectionGroupBuilder(neuronGroupBuilder.getExcitatoryNeuronGroups(), neuronGroupBuilder.getInhibitoryNeuronGroups());
		connect(&connectionGroupBuilder, SIGNAL(progress(int,int,QString)), this, SLOT(updateProgress(int,int,QString)));
		connectionGroupBuilder.addConnectionGroups(newNetwork, &stopThread, weightsFilePath, delaysFilePath, parameterMap);

		//Make the network the current network
		Globals::setNetwork(newNetwork);
	}
	catch(SpikeStreamException& ex){
		setError(ex.getMessage());
		if(newNetwork != NULL)
			delete newNetwork;
	}
	catch(...){
		setError("An unknown error occurred");
		if(newNetwork != NULL)
			delete newNetwork;
	}

	stopThread = true;
}


/*! Checks and stores the necessary parameters and starts the thread running. */
void MatrixImporter::startImport(const QString& netName, const QString& netDesc, const QString& coordinatesFilePath, const QString& nodeNamesFilePath, const QString& weightsFilePath, const QString& delaysFilePath, QHash<QString, double>& parameterMap){
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

	//Handle name and description
	networkName = netName;
	networkDescription = netDesc;
	if(networkName.isEmpty())
		networkName = "Unnamed";
	if(networkDescription.isEmpty())
		networkDescription = "Undescribed";

	//Check and store the parameters
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

	this->parameterMap = paramMap;
}

