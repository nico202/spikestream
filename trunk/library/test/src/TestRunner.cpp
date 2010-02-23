
//SpikeStream includes
#include "TestAnalysisDao.h"
#include "TestArchiveDao.h"
#include "TestDatabaseDao.h"
#include "TestRunner.h"
#include "TestNetwork.h"
#include "TestNeuronGroup.h"
#include "TestNetworkDao.h"
#include "TestNetworkDaoThread.h"
#include "TestXMLParameterParser.h"
#include "TestUtil.h"
#include "TestWeightlessNeuron.h"

//Other includes
#include <iostream>
using namespace std;

/*! Runs all of the tests */
void TestRunner::runTests(){
	TestDatabaseDao testDatabaseDao;
	QTest::qExec(&testDatabaseDao);

 /*   TestNetworkDao testNetworkDao;
    QTest::qExec(&testNetworkDao);

    TestNetworkDaoThread testNetworkDaoThread;
    QTest::qExec(&testNetworkDaoThread);

    TestXMLParameterParser testXMLParameterParser;
    QTest::qExec(&testXMLParameterParser);

    TestNetwork testNework;
    QTest::qExec(&testNework);

    TestNeuronGroup testNeuronGroup;
    QTest::qExec(&testNeuronGroup);

    TestArchiveDao testArchiveDao;
    QTest::qExec(&testArchiveDao);

    TestAnalysisDao testAnalysisDao;
    QTest::qExec(&testAnalysisDao);

    TestUtil testUtil;
    QTest::qExec(&testUtil);

    TestWeightlessNeuron testWeightlessNeuron;
	QTest::qExec(&testWeightlessNeuron);*/
}


