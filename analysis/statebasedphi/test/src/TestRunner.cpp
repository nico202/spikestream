
//SpikeStream includes
#include "TestRunner.h"
#include "TestStateBasedPhiAnalysisDao.h"
#include "TestSubset.h"
#include "TestProbabilityTable.h"
#include "TestPhiCalculator.h"
#include "TestSubsetManager.h"


//Other includes
#include <iostream>
using namespace std;

/*! Runs all of the tests */
void TestRunner::runTests(){
	//Create core application object so that drivers load in Windows
	int argsSize = 0;
	char* argsChar[0];
	QCoreApplication coreApplication(argsSize, argsChar);

    TestStateBasedPhiAnalysisDao testStateBasedPhiAnalysisDao;
    QTest::qExec(&testStateBasedPhiAnalysisDao);

    TestProbabilityTable testProbabilityTable;
    QTest::qExec(&testProbabilityTable);

    TestSubset testSubset;
    QTest::qExec(&testSubset);

    TestPhiCalculator testPhiCalculator;
    QTest::qExec(&testPhiCalculator);

    TestSubsetManager testSubsetManager;
    QTest::qExec(&testSubsetManager);
}


