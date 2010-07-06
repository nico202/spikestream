//SpikeStream includes
#include "TestRunner.h"
#include "TestLivelinessDao.h"
#include "TestWeightlessLivelinessAnalyzer.h"


/*! Runs all of the tests */
void TestRunner::runTests(){
	//Create core application object so that drivers load in Windows
	int argsSize = 0;
	char* argsChar[0];
	QCoreApplication coreApplication(argsSize, argsChar);

	TestLivelinessDao testLivelinessDao;
	QTest::qExec(&testLivelinessDao);

	TestWeightlessLivelinessAnalyzer testWeightlessLivelinessAnalyzer;
	QTest::qExec(&testWeightlessLivelinessAnalyzer);

}


