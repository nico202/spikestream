
//SpikeStream includes
#include "TestRunner.h"
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
}

