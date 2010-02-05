//SpikeStream includes
#include "TestRunner.h"
#include "TestLivelinessDao.h"


/*! Runs all of the tests */
void TestRunner::runTests(){
	TestLivelinessDao testLivelinessDao;
	QTest::qExec(&testLivelinessDao);

}


