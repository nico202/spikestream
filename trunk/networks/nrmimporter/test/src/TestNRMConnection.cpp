//SpikeStream includes
#include "TestNRMConnection.h"
#include "NRMConfigLoader.h"
#include "NRMException.h"
#include "NRMConnection.h"
using namespace spikestream;

//Qt includes
#include <QList>

//Other includes
#include <iostream>
using namespace std;


void TestNRMConnection::init(){
	workingDirectory = QCoreApplication::applicationDirPath();
	workingDirectory.truncate(workingDirectory.size() - 4);//Trim the "/bin" off the end
	workingDirectory += "/networks/nrmimporter/test/test_files/";
}


/*! Tests connection building by loading up config files and examining the resulting networks
	NOTE: Not a 'unit' way of doing it because there are many dependencies on other components! */
void TestNRMConnection::testCreateConnections(){
	NRMConfigLoader configLoader;

	//Load up the test configuration
	QString fileName = workingDirectory + "connection_building/test_connectionbuilding_1.cfg";
	try{
		configLoader.loadConfig(fileName.toStdString().data());
	}
	catch(NRMException& ex){
		QFAIL(ex.getMessage().toAscii());
	}
	NRMNetwork* network = configLoader.getNetwork();

	//Check that neurons make appropriate connections to each other
	//Get the list of connections for neural layer 0
	QList<NRMConnection*> connList = network->getNeuralLayerById(0)->getConnections();

	//Should be a single connection for this test file
	QCOMPARE(1, connList.size());

	//Check that the 11th neuron in the layer has correct connections
	unsigned int expectedConns[] = {71, 72, 61, 41};
	QList<unsigned int> neurConns = connList[0]->getNeuronConnections(11);
	for(int i=0; i<neurConns.size(); ++i){
		QCOMPARE(expectedConns[i], neurConns[i]);
	}
}

