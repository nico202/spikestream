#include "TestProbabilityTable.h"
#include "ProbabilityTable.h"
#include "SpikeStreamException.h"
using namespace spikestream;


void TestProbabilityTable::testBuildTable(){
    //Create a table with five elements
    ProbabilityTable probTable(5);
    QCOMPARE(probTable.getNumberOfElements(), (int)5);
    QCOMPARE(probTable.getNumberOfRows(), (int)32);

    //Spot check that some combinations exist
    try{
	QString key = "00000";
	probTable.get(key);
	probTable.get(key="11111");
	probTable.get(key="10100");
    }
    catch(SpikeStreamException ex){
	QFAIL(ex.getMessage().toAscii());
    }
}


void TestProbabilityTable::testIterator(){
    ProbabilityTable probTable(2);

    probTable.set("00", 0.1);
    probTable.set("01", 0.2);
    probTable.set("10", 0.3);
    probTable.set("11", 0.4);

    for(QHash<QString, double>::iterator iter=probTable.begin(); iter != probTable.end(); ++iter){
	if(iter.key() == "00")
	    QCOMPARE(iter.value(), 0.1);
	else if(iter.key() == "01")
	    QCOMPARE(iter.value(), 0.2);
	else if(iter.key() == "10")
	    QCOMPARE(iter.value(), 0.3);
	else if(iter.key() == "11")
	    QCOMPARE(iter.value(), 0.4);
	else
	    QFAIL("Key not recognized");
    }
}


void TestProbabilityTable::testSetGet(){
    //Table with 7 elements
    ProbabilityTable probTable(7);
    probTable.set("1010111", 0.345);
    probTable.set("1010110", 0.543);
    QCOMPARE(probTable.get("1010111"), 0.345);
    QCOMPARE(probTable.get("1010110"), 0.543);
}

