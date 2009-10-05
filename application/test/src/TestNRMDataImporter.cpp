#include "TestNRMDataImporter.h"
//#include "NRMDataImporter.h"
//using namespace spikestream;

void TestNRMDataImporter::cleanup(){
}


void TestNRMDataImporter::init(){
    workingDirectory = getenv("SPIKESTREAM_ROOT");
}



void TestNRMDataImporter::testAddTraining(){
//    //Add test network to the database
//    addNRMTestNetwork1();
//
//    //Create Network dao and SpikeStream network to use in the tests
//    NetworkDao networkDao(dbInfo);
//    NetworkInfo netInfo(testNRMNetID, "testNRMNetwork1Name", "testNRMNetwork1Description", false);
//    Network network(netInfo);
//    network.load();
//    while(network.isBusy())
//	usleep(10000);
//    if(network.isError())
//	QFAIL(network.getErrorMessage().toAscii());
//
//    //Create NRM network for test
//    NRMConfigLoader configLoader();
//    QString fileName = workingDirectory + "/application/test/test_files/datasets/test2.set";
//    configLoader.loadConfig(fileName.toAscii());
//    Network* nrmNetwork = configLoader.getNetwork();
//
//    //Set up class being tested
//    NRMDataImporter dataImporter(dbInfo, archiveDBInfo);
//    dataImporter.prepareAddTraining(NRMNetwork* nrmNetwork, Network* network);
//
//    //Run importer and wait for result
//    dataImporter.start();
//    dataImporter.wait();
//
//    //Check that the training has been correctly added to the database.
//    QSqlQuery query = getQuery("SELECT * FROM WeightlessNeuronTrainingPatterns");
//    executeQuery(query);
//
//    //Should be one row for each neuron in the second layer
//
//    //Check patterns are correct



}

