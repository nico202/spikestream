//NRM Loader includes
#include "NRMConfigLoader.h"
#include "NRMException.h"
#include "NRMConstants.h"
#include "NRMNetwork.h"
#include "NRMInputLayer.h"
#include "NRMNeuralLayer.h"
using namespace spikestream;

//Test includes
#include "TestNRMConfigLoader.h"

#include <iostream>
using namespace std;


void TestNRMConfigLoader::cleanup(){
}


void TestNRMConfigLoader::init(){
	workingDirectory = QCoreApplication::applicationDirPath();
	workingDirectory.truncate(workingDirectory.size() - 4);//Trim the "/bin" off the end
}


void TestNRMConfigLoader::testLoadInputs(){
	QString fileName = workingDirectory + "/applicationlibrary/test/test_files/multiple_input_test.cfg";

	//Load up the test configuration
	NRMConfigLoader configLoader;
	try{
		configLoader.loadConfig(fileName.toStdString().data());
	}
	catch(NRMException& ex){
		QFAIL(ex.getMessage());
	}

	//Get the loaded network
	NRMNetwork* network = configLoader.getNetwork();

	//Check config version is supported
	QCOMPARE(network->getConfigVersion(), 26);

	//Check that the details about the layers have been correctly loaded
	QList<NRMInputLayer*> allInputList = network->getAllInputs();
	QCOMPARE(allInputList.size(), 5);
	NRMInputLayer* tmpInputLayer = network->getInputById(2);
	QCOMPARE(tmpInputLayer->height, (unsigned int)65);
	QCOMPARE(tmpInputLayer->frameName, string("Inputs A:"));
	tmpInputLayer = network->getInputById(4);
	QCOMPARE(tmpInputLayer->width, (unsigned int)98);
	QCOMPARE(tmpInputLayer->frameName, string("Input 5"));

	//Check that the right number of inputs and framed images have been loaded
	QList<NRMInputLayer*> panelInputList = network->getPanelInputs();
	QCOMPARE(panelInputList.size(), 3);
	QList<NRMInputLayer*> framedInputList = network->getFramedImageInputs();
	QCOMPARE(framedInputList.size(), 2);
}


void TestNRMConfigLoader::testLoadNeuralLayers(){
	QString fileName = workingDirectory + "/applicationlibrary/test/test_files/multiple_neural_test.cfg";

	//Load up the test configuration
	NRMConfigLoader configLoader;
	try{
		configLoader.loadConfig(fileName.toStdString().data());
	}
	catch(NRMException& ex){
		QFAIL(ex.getMessage());
	}

	//Get the loaded network
	NRMNetwork* network = configLoader.getNetwork();

	//Check config version is supported
	QCOMPARE(network->getConfigVersion(), 26);

	//Check that the details about the neural layers have been correctly loaded.
	QList<NRMNeuralLayer*> neuralLayerList = network->getAllNeuralLayers();
	QCOMPARE(neuralLayerList.size(), 5);

	NRMNeuralLayer* tmpNeurLayer = network->getNeuralLayerById(0);
	QCOMPARE(tmpNeurLayer->width, (unsigned int)50);
	QCOMPARE(tmpNeurLayer->frameName, string("Test Neural Panel"));
	QCOMPARE(tmpNeurLayer->neuralType, NEURAL_LAYER);
	QCOMPARE(tmpNeurLayer->colPlanes, -2);

	tmpNeurLayer = network->getNeuralLayerById(3);
	QCOMPARE(tmpNeurLayer->height, (unsigned int)98);
	QCOMPARE(tmpNeurLayer->neuralType, MAGNUS_WIN_PREV);
	QCOMPARE(tmpNeurLayer->colPlanes, -1);
}


/*! Checks that connection parameters are loaded correctly */
void TestNRMConfigLoader::testLoadConnectionParameters(){
	QString fileName;
	NRMNeuralLayer* neuralLayer;
	NRMConfigLoader configLoader;
	NRMConnection* conn;

	//Load up the test configuration
	fileName = workingDirectory + "/applicationlibrary/test/test_files/connection_parameters/randomdist_lincols_12conns.cfg";
	try{
		configLoader.loadConfig(fileName.toStdString().data());

	    //Get the neural layer with the connections
	    neuralLayer = configLoader.getNetwork()->getNeuralLayerById(0);

	    //Check connection parameters
	    conn = neuralLayer->getConnectionById(0);
	    QCOMPARE(conn->conParams.colScheme, (unsigned int)1);
	    QCOMPARE(conn->conParams.numCons, (unsigned int)12);
	    QCOMPARE(conn->conParams.randomCon, true);

	    //Reset config loader
	    configLoader.reset();
	}
	catch(NRMException& ex){
	    QFAIL(ex.getMessage());
	}

	//Load up the test configuration
	fileName = workingDirectory + "/applicationlibrary/test/test_files/connection_parameters/iconic_4w5h_flipvertical_6conns.cfg";
	try{
		configLoader.loadConfig(fileName.toStdString().data());

	    //Get the loaded network
	    neuralLayer = configLoader.getNetwork()->getNeuralLayerById(0);

	    //Check connection parameters
	    conn = neuralLayer->getConnectionById(0);
	    QCOMPARE(conn->conParams.randomCon, true);
	    QCOMPARE(conn->conParams.iconicMap, true);
	    QCOMPARE(conn->conParams.colScheme, (unsigned int)1);
	    QCOMPARE(conn->conParams.numCons, (unsigned int)6);
	    QCOMPARE(conn->conParams.rWidth, (unsigned int)4);
	    QCOMPARE(conn->conParams.rHeight, (unsigned int)5);

	    //Reset config loader
	    configLoader.reset();
	}
	catch(NRMException& ex){
		QFAIL(ex.getMessage());
	}


	//Load up the test configuration
	fileName = workingDirectory + "/applicationlibrary/test/test_files/connection_parameters/global_spatial_9-10-11-12_7conns.cfg";
	try{
		configLoader.loadConfig(fileName.toStdString().data());

	    //Get the loaded network
	    neuralLayer = configLoader.getNetwork()->getNeuralLayerById(0);

	    //Check connection parameters
	    conn = neuralLayer->getConnectionById(0);
	    QCOMPARE(conn->conParams.randomCon, true);
	    QCOMPARE(conn->conParams.iconicMap, false);
	    QCOMPARE(conn->conParams.globalMap, true);
	    QCOMPARE(conn->conParams.spatialAlignment, true);
	    QCOMPARE(conn->conParams.colScheme, (unsigned int)1);
	    QCOMPARE(conn->conParams.numCons, (unsigned int)7);
	    QCOMPARE(conn->conParams.sWidth, (unsigned int)9);
	    QCOMPARE(conn->conParams.sHeight, (unsigned int)10);
	    QCOMPARE(conn->conParams.sLeft, (unsigned int)11);
	    QCOMPARE(conn->conParams.sTop, (unsigned int)12);

	    //Reset config loader
	    configLoader.reset();
	}
	catch(NRMException& ex){
		QFAIL(ex.getMessage());
	}


	//Load up the test configuration
	fileName = workingDirectory + "/applicationlibrary/test/test_files/connection_parameters/seg_segrev_rancols_11conns.cfg";
	try{
		configLoader.loadConfig(fileName.toStdString().data());

	    //Get the loaded network
	    neuralLayer = configLoader.getNetwork()->getNeuralLayerById(0);

	    //Check connection parameters
	    conn = neuralLayer->getConnectionById(0);
	    QCOMPARE(conn->conParams.randomCon, true);
	    QCOMPARE(conn->conParams.iconicMap, false);
	    QCOMPARE(conn->conParams.globalMap, false);
	    QCOMPARE(conn->conParams.segmentedMap, true);
	    QCOMPARE(conn->conParams.segRev, true);
	    QCOMPARE(conn->conParams.colScheme, (unsigned int)0);
	    QCOMPARE(conn->conParams.numCons, (unsigned int)11);
	    QCOMPARE(conn->conParams.rWidth, (unsigned int)1);
	    QCOMPARE(conn->conParams.rHeight, (unsigned int)1);

	    //Reset config loader
	    configLoader.reset();
	}
	catch(NRMException& ex){
		QFAIL(ex.getMessage());
	}


	//Load up the test configuration
	fileName = workingDirectory + "/applicationlibrary/test/test_files/connection_parameters/2connections.cfg";
	try{
		configLoader.loadConfig(fileName.toStdString().data());

	    //Get the loaded network
	    neuralLayer = configLoader.getNetwork()->getNeuralLayerById(0);

	    //Check the first set of connection parameters
	    conn = neuralLayer->getConnectionById(0);
	    QCOMPARE(conn->conParams.randomCon, false);
	    QCOMPARE(conn->conParams.fullCon, true);
	    QCOMPARE(conn->conParams.iconicMap, false);
	    QCOMPARE(conn->conParams.globalMap, true);
	    QCOMPARE(conn->conParams.segmentedMap, false);
	    QCOMPARE(conn->conParams.segRev, true);
	    QCOMPARE(conn->conParams.colScheme, (unsigned int)0);
	    QCOMPARE(conn->conParams.numCons, (unsigned int)76832);

	    //Check the second set of connection parameters
	    conn = neuralLayer->getConnectionById(1);
	    QCOMPARE(conn->conParams.randomCon, true);
	    QCOMPARE(conn->conParams.iconicMap, false);
	    QCOMPARE(conn->conParams.globalMap, true);
	    QCOMPARE(conn->conParams.segmentedMap, false);
	    QCOMPARE(conn->conParams.segRev, true);
	    QCOMPARE(conn->conParams.colScheme, (unsigned int)1);
	    QCOMPARE(conn->conParams.numCons, (unsigned int)13);
	    //QCOMPARE(conn->conParams.rWidth, (unsigned int)1);
	    //QCOMPARE(conn->conParams.rHeight, (unsigned int)1);

	    //Reset config loader
	    configLoader.reset();
	}
	catch(NRMException& ex){
		QFAIL(ex.getMessage());
	}

}
