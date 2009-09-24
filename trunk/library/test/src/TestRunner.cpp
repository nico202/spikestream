
//SpikeStream includes
#include "TestArchiveDao.h"
#include "TestRunner.h"
#include "TestNetwork.h"
#include "TestNeuronGroup.h"
#include "TestNetworkDao.h"
#include "TestNetworkDaoThread.h"
#include "TestXMLParameterParser.h"

//Other includes
#include <iostream>
using namespace std;

/*! Runs all of the tests */
void TestRunner::runTests(){
    TestNetworkDao testNetworkDao;
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
}


