//SpikeStream includes
#include "TestUtil.h"
#include "Util.h"
#include "NumberConversionException.h"
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

    Util::fillSelectionArray(selArr, 7, 7);
    QCOMPARE(selArr[0], true);
    QCOMPARE(selArr[1], true);
    QCOMPARE(selArr[2], true);
    QCOMPARE(selArr[3], true);
    QCOMPARE(selArr[4], true);
    QCOMPARE(selArr[5], true);
    QCOMPARE(selArr[6], true);

    Util::fillSelectionArray(selArr, 7, 0);
    QCOMPARE(selArr[0], false);
    QCOMPARE(selArr[1], false);
    QCOMPARE(selArr[2], false);
    QCOMPARE(selArr[3], false);
    QCOMPARE(selArr[4], false);
    QCOMPARE(selArr[5], false);
    QCOMPARE(selArr[6], false);
}


void TestUtil::testGetUIntList(){
    //Check an empty list
    QList<unsigned int> tstList = Util::getUIntList("");
    QCOMPARE(tstList.size(), 0);

    //Check a good list
    tstList = Util::getUIntList("33, 56");
    QCOMPARE(tstList.size(), (int)2);
    QCOMPARE(tstList.at(0), (unsigned int)33);
    QCOMPARE(tstList.at(1), (unsigned int)56);

    //Check it fails on a dodgy list
    bool exceptionThrown = false;
    try{
	tstList = Util::getUIntList("33, -56");
    }
    catch(NumberConversionException& ex){
	exceptionThrown = true;
    }
    QVERIFY(exceptionThrown);

    //Check it fails on another dodgy list
    exceptionThrown = false;
    try{
	tstList = Util::getUIntList("678, 533.2, 56");
    }
    catch(NumberConversionException& ex){
	exceptionThrown = true;
    }
    QVERIFY(exceptionThrown);
}



void TestUtil::testRDouble(){
    QCOMPARE(Util::rDouble(0.318318, 3), 0.318);
    QCOMPARE(Util::rDouble(0.318518, 3), 0.319);
    QCOMPARE(Util::rDouble(4.2020, 2), 4.20);
    QCOMPARE(Util::rDouble(23.3, 1), 23.3);
    QCOMPARE(Util::rDouble(23.3, 2), 23.3);
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
