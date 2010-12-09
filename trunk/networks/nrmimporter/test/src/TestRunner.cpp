//SpikeStream includes
#include "TestRunner.h"
#include "TestNRMRandom.h"
#include "TestNRMConfigLoader.h"
#include "TestNRMConnection.h"
#include "TestNRMTrainingLoader.h"
#include "TestNRMDataSetImporter.h"
#include "TestNRMDataImporter.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;

/*! Runs all of the tests */
void TestRunner::runTests(){

	//Create core application object so that drivers load in Windows
	int argsSize = 0;
	char* argsChar[0];
	QCoreApplication coreApplication(argsSize, argsChar);

	TestNRMConfigLoader testNRMConfigLoader;
	QTest::qExec(&testNRMConfigLoader);

	TestNRMConnection testNRMConnection;
	QTest::qExec(&testNRMConnection);

	TestNRMTrainingLoader testNRMTrainingLoader;
	QTest::qExec(&testNRMTrainingLoader);

	TestNRMRandom testNRMRandom;
	QTest::qExec(&testNRMRandom);

	TestNRMDataSetImporter testNRMDataSetImporter;
	QTest::qExec(&testNRMDataSetImporter);

	TestNRMDataImporter testNRMDataImporter;
	QTest::qExec(&testNRMDataImporter);
}


