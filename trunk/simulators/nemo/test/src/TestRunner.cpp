
//SpikeStream includes
#include "TestRunner.h"
#include "TestNemoLibrary.h"
#include "TestNemoWrapper.h"

/*! Runs all of the tests */
void TestRunner::runTests(){

	//Create core application object so that drivers load in Windows
	int argsSize = 0;
	char* argsChar[0];
	QCoreApplication coreApplication(argsSize, argsChar);

	TestNemoLibrary testNemoLibrary;
	QTest::qExec(&testNemoLibrary);

	TestNemoWrapper testNemoWrapper;
	QTest::qExec(&testNemoWrapper);

}


