
//SpikeStream includes
#include "TestRunner.h"
#include "TestStateBasedPhiAnalysisDao.h"


//Other includes
#include <iostream>
using namespace std;

/*! Runs all of the tests */
void TestRunner::runTests(){
    TestStateBasedPhiAnalysisDao testStateBasedPhiAnalysisDao;
    QTest::qExec(&testStateBasedPhiAnalysisDao);
}


