
//SpikeStream includes
#include "TestRunner.h"
#include "TestNRMRandom.h"
#include "TestNRMConfigLoader.h"
#include "TestNRMConnection.h"
#include "TestNRMTrainingLoader.h"
#include "TestNRMDataSetImporter.h"
#include "TestNRMDataImporter.h"

//Other includes
#include <iostream>
using namespace std;

/*! Runs all of the tests */
void TestRunner::runTests(){
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


