//SpikeStream includes
#include "TestUtil.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

void TestUtil::testFillSelectionArray(){
    bool selArr [7];
    Util::fillSelectionArray(selArr, 7, 4);
    QCOMPARE(selArr[0], false);
    QCOMPARE(selArr[1], false);
    QCOMPARE(selArr[2], false);
    QCOMPARE(selArr[3], true);
    QCOMPARE(selArr[4], true);
    QCOMPARE(selArr[5], true);
    QCOMPARE(selArr[6], true);
}

void TestUtil::testRandom(){
    Util::seedRandom();

    for(int i=0; i< 10; ++i){
	int ranNum = Util::getRandom(-50, 50);
	QVERIFY(ranNum <= 50 && ranNum >= -50);
    }

    for(int i=0; i<10; ++i){
	int ranNum = Util::getRandom(0, 10000);
	QVERIFY(ranNum <= 10000 && ranNum >= 0);
    }

    Util::seedRandom(100);
    QCOMPARE(Util::getRandom(25, 250), (int)168);
    QCOMPARE(Util::getRandom(25, 250), (int)92);
    QCOMPARE(Util::getRandom(25, 250), (int)142);
    QCOMPARE(Util::getRandom(25, 250), (int)115);
    QCOMPARE(Util::getRandom(25, 250), (int)226);
}
