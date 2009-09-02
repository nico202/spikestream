
//SpikeStream includes
#include "TestRunner.h"
#include "TestNRMRandom.h"
#include "TestNRMConfigLoader.h"
#include "TestNRMConnection.h"
#include "TestNRMTrainingLoader.h"

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
}


