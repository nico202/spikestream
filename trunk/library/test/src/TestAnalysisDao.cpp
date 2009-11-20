#include "TestAnalysisDao.h"
#include "AnalysisDao.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

void TestAnalysisDao::testAddAnalysis(){
    //Add network and archive
    addTestArchive1();

    //Create analysis info with analysis information
    AnalysisInfo analysisInfo(0, testNetID, testArchive1ID, QDateTime::fromTime_t(1234321), "Test analysis description 1", QHash<QString, double>(), 1);

    try{
	//Invoke method that is being tested
	AnalysisDao analysisDao(analysisDBInfo);
	analysisDao.addAnalysis(analysisInfo);

	//Check that analysis has been added
	QSqlQuery query = getAnalysisQuery("SELECT NetworkID, ArchiveID, StartTime, Description, AnalysisTypeID FROM Analyses");
	executeQuery(query);

	//Should only be one row
	QCOMPARE(query.size(), 1);
	query.next();

	//Check details of archive
	QVERIFY( analysisInfo.getID() != 0);
	QCOMPARE(query.value(0).toUInt(), testNetID);
	QCOMPARE(query.value(1).toUInt(), testArchive1ID);
	QCOMPARE(query.value(2).toUInt(), (unsigned int)1234321);
	QCOMPARE(query.value(3).toString(), QString("Test analysis description 1"));
	QCOMPARE(query.value(4).toUInt(), (unsigned int)1);
    }
    catch(SpikeStreamException& ex){
	QFAIL(ex.getMessage().toAscii());
    }

}

void TestAnalysisDao::testUpdateDescription(){
    try{
	//Add test analysis. This has description "test analysis description"
	addTestAnalysis1();

	//Invoke analysis dao to change description
	AnalysisDao analysisDao(analysisDBInfo);
	analysisDao.updateDescription(testAnalysis1ID, "new analysis description");

	//Check new description is present in the database
	QSqlQuery query = getAnalysisQuery("SELECT Description FROM Analyses WHERE AnalysisID=" + QString::number(testAnalysis1ID));
	executeQuery(query);
	query.next();
	QCOMPARE(query.value(0).toString(), QString("new analysis description"));
    }
    catch(SpikeStreamException& ex){
	QFAIL(ex.getMessage().toAscii());
    }
}



void TestAnalysisDao::testGetAnalysesTableModel(){

}

