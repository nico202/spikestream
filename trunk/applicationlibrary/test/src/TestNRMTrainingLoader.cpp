
//SpikeStream includes
#include "TestNRMTrainingLoader.h"
#include "NRMTrainingLoader.h"
#include "NRMConfigLoader.h"
#include "NRMException.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;


void TestNRMTrainingLoader::init(){
	workingDirectory = QCoreApplication::applicationDirPath();
	workingDirectory.truncate(workingDirectory.size() - 4);//Trim the "/bin" off the end
}


void TestNRMTrainingLoader::testLoadTraining(){

	//Start by loading up the configuration file that matches the training data
	QString configFileName = workingDirectory + "/applicationlibrary/test/test_files/training/test_training_8.cfg";
	NRMConfigLoader configLoader;
	try{
		configLoader.loadConfig(configFileName.toStdString().data());
	}
	catch(NRMException& ex){
		QFAIL(ex.getMessage());
	}
	NRMNetwork* network = configLoader.getNetwork();

	//Load up the test configuration
	QString trainingFileName = workingDirectory + "/applicationlibrary/test/test_files/training/test_training_8.ntr";
	NRMTrainingLoader trainingLoader(configLoader.getNetwork());
	try{
		trainingLoader.loadTraining(trainingFileName.toStdString().data());
	}
	catch(NRMException& ex){
		QFAIL(ex.getMessage());
	}

	/* This test network is 2x5 and the neuron at position 9 is active.
		The sixth neuron should contain training string should be 00001000 (a value of 8 in decimal)
		Position 0 in the training string is the output, position 1 is the first training byte. */
	QList<unsigned char*> trainingList = network->getNeuralLayerById(0)->getNeuron(5)->getTraining();
	QCOMPARE( (unsigned int)trainingList[0][0], (unsigned int)7 );//Check the output
	QCOMPARE( (unsigned int)trainingList[0][1], (unsigned int)8 );//Check the first byte

	/* This should correspond to a connection to the 9th neuron (neuron counting starts from zero).
	   Connection counting in the training strings is a bit funny:
		[ 7 6 5 4 3 2 1 0 ]  [ 15 14 13 12 11 10 9 8 ] ...
		When there is not full connectivity the list of random connections fills up place 1 first and so on */
	QList<unsigned int> conList = network->getNeuralLayerById(0)->getConnectionById(0)->getNeuronConnections(5);
	QCOMPARE( conList[3], (unsigned int)9 );
}


