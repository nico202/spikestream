#include "TestStateBasedPhiAnalysisDao.h"
#include "StateBasedPhiAnalysisDao.h"
using namespace spikestream;

void TestStateBasedPhiAnalysisDao::testAddComplex(){
    //Add test analysis
    addTestAnalysis1();

    //Create class to be tested and test it
    StateBasedPhiAnalysisDao anaDao(analysisDBInfo);
    QList<unsigned int> neuronIDList;
    neuronIDList.append(23);
    neuronIDList.append(27);
    neuronIDList.append(35);
    anaDao.addComplex(testAnalysis1ID, 7, 3.45, neuronIDList);

    //Check complex was added correctly.
    QSqlQuery query = getAnalysisQuery("SELECT ComplexID, AnalysisID, TimeStep, Phi, Neurons FROM StateBasedPhiData");
    executeQuery(query);
    query.next();
    QCOMPARE(query.size(), (int)1);
    QVERIFY(query.value(0).toUInt() > (unsigned int)0);
    QCOMPARE(query.value(1).toUInt(), (unsigned int)testAnalysis1ID);
    QCOMPARE(query.value(2).toUInt(), (unsigned int)7);
    QCOMPARE(query.value(3).toDouble(), 3.45);
    QCOMPARE(query.value(4).toString(), QString("23,27,35"));
}


void TestStateBasedPhiAnalysisDao::getStateBasedPhiDataTableModel(){
}


void TestStateBasedPhiAnalysisDao::testGetComplexCount(){
    //Add test analysis
    addTestAnalysis1();

    //Add some data
    executeAnalysisQuery("INSERT INTO StateBasedPhiData (AnalysisID, TimeStep, Phi, Neurons) VALUES (" + QString::number(testAnalysis1ID) + ", 1, 3.0, '256,311,21,4')");
    executeAnalysisQuery("INSERT INTO StateBasedPhiData (AnalysisID, TimeStep, Phi, Neurons) VALUES (" + QString::number(testAnalysis1ID) + ", 2, 4.0, '1,2,3,4')");
    executeAnalysisQuery("INSERT INTO StateBasedPhiData (AnalysisID, TimeStep, Phi, Neurons) VALUES (" + QString::number(testAnalysis1ID) + ", 3, 5.0, '5,6,7,8')");

    //Create class to be tested and test it
    StateBasedPhiAnalysisDao anaDao(analysisDBInfo);
    int complexCount = anaDao.getComplexCount(testAnalysis1ID, 1, 3);
    QCOMPARE(complexCount, (int)3);
    complexCount = anaDao.getComplexCount(testAnalysis1ID, 3, 5);
    QCOMPARE(complexCount, (int)1);
    complexCount = anaDao.getComplexCount(testAnalysis1ID+1, 1, 3);
    QCOMPARE(complexCount, (int)0);
}

