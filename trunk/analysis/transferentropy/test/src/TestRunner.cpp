//SpikeStream includes
#include "TestRunner.h"
#include "TestTransferEntropyCalculator.h"
using namespace spikestream;

/*! Runs all of the tests */
void TestRunner::runTests(){
	//Create core application object so that drivers load in Windows
	int argsSize = 0;
	char* argsChar[0];
	QCoreApplication coreApplication(argsSize, argsChar);

	TestTransferEntropyCalculator testTransferEntropyCalculator;
	QTest::qExec(&testTransferEntropyCalculator);

}


