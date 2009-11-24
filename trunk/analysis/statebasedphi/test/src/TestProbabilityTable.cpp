#include "TestProbabilityTable.h"


void TestProbabilityTable::testBuildTable(){
    //Create a table with five elements
    ProbabilityTable probTable(5);
    QCOMPARE(probTable.getNumberOfElements(), (int)5);
    QCOMPARE(probTable.getNumberOfRows(), (int)32);

    //Spot check that some combinations exist
    try{
	QString key = "00000";
	probTable.get(key);
	probTable.get(key="111111");
    }
    catch(SpikeStreamException ex){
	QFAIL(ex.getMessage().toAscii());
    }
}


void TestProbabilityTable::testSet(){
}

