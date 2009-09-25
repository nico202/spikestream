//SpikeStream includes
#include "SpikeStreamException.h"
#include "TestArchiveDao.h"
#include "ArchiveDao.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;



/*----------------------------------------------------------*/
/*-----                    TESTS                       -----*/
/*----------------------------------------------------------*/

void TestArchiveDao::testDeleteArchive(){
    //Add a test archive
    addTestArchive1();

    //Check that test archive is in database
    QSqlQuery query = getArchiveQuery("SELECT * FROM Archives WHERE ArchiveID = " + QString::number(testArchive1ID));
    executeQuery(query);

    //Should be a single network
    QCOMPARE(query.size(), (int)1);

    //Check that archive data is in database
    query = getArchiveQuery("SELECT * FROM ArchiveData WHERE ArchiveID = " + QString::number(testArchive1ID));
    executeQuery(query);

    //Should be a three time steps
    QCOMPARE(query.size(), (int)3);

    //Invoke method being tested
    ArchiveDao archiveDao(archiveDBInfo);
    archiveDao.deleteArchive(testArchive1ID);

    //Check to see if archive with this id has been removed from the database
    query = getArchiveQuery("SELECT * FROM Archives WHERE ArchiveID = " + QString::number(testArchive1ID));
    executeQuery(query);

    //Should be no archive
    QCOMPARE(query.size(), (int)0);

    //Check that delete has cascaded properly
    query = getArchiveQuery("SELECT * FROM ArchiveData WHERE ArchiveID = " + QString::number(testArchive1ID));
    executeQuery(query);

    //Should be no time steps
    QCOMPARE(query.size(), (int)0);
}


void TestArchiveDao::testGetArchivesInfo(){
    //Add two test archives
    addTestArchive1();
    addTestArchive2();

    //Check that the information returned is correct
    ArchiveDao archiveDao(archiveDBInfo);
    QList<ArchiveInfo> archInfoList = archiveDao.getArchivesInfo(testNetID);

    //Should be two archives
    QCOMPARE(archInfoList.size(), (int)2);

    //Check details from database
    QCOMPARE(archInfoList[0].getDescription(), QString("testArchive1Description"));
    QCOMPARE(archInfoList[1].getDescription(), QString("testArchive2Description"));
    QCOMPARE(archInfoList[0].getDateTime(), QDateTime::fromTime_t(1011));
    QCOMPARE(archInfoList[1].getDateTime(), QDateTime::fromTime_t(2022211));
    QCOMPARE(archInfoList[0].getNetworkID(), testNetID);
    QCOMPARE(archInfoList[1].getNetworkID(), testNetID);
    QCOMPARE(archInfoList[0].size(), (int)3);
    QCOMPARE(archInfoList[1].size(), (int)0);
}


void TestArchiveDao::testGetArchiveSize(){
    //Add a test archive
    addTestArchive1();

    //Should be three rows
    ArchiveDao archiveDao(archiveDBInfo);
    int archiveSize = archiveDao.getArchiveSize(testArchive1ID);
    QCOMPARE(archiveSize, (int)3);
}






