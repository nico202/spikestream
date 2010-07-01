
//SpikeStream includes
#include "TestRunner.h"
#include "TestNemo.h"

//Other includes
#include <iostream>
using namespace std;

/*! Runs all of the tests */
void TestRunner::runTests(){

	//Create core application object so that drivers load in Windows
	int argsSize = 0;
	char* argsChar[0];
	QCoreApplication coreApplication(argsSize, argsChar);

	TestNemo testNemo;
	QTest::qExec(&testNemo);

}


