//SpikeStream includes
#include "TestRunner.h"
#include "TestLivelinessDao.h"
#include "TestWeightlessLivelinessAnalyzer.h"


/*! Runs all of the tests */
void TestRunner::runTests(){
	TestLivelinessDao testLivelinessDao;
	QTest::qExec(&testLivelinessDao);

	TestWeightlessLivelinessAnalyzer testWeightlessLivelinessAnalyzer;
	QTest::qExec(&testWeightlessLivelinessAnalyzer);

}


