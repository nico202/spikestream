//SpikeStream includes
#include "SpikeStreamException.h"
#include "TestArchiveDao.h"
#include "ArchiveDao.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;

/*----------------------------------------------------------*/
/*-----                INIT AND CLEANUP                -----*/
/*----------------------------------------------------------*/

/*! Called after each test */
void TestArchiveDao::cleanup(){
}


/*! Called after all the tests */
void TestArchiveDao::cleanupTestCase() {
    //Leave databases in clean state
    cleanTestDatabases();

    //Close database connection
    closeDatabase();
}


/*! Called before each test */
void TestArchiveDao::init(){
    cleanTestDatabases();
}


/*! Called before all the tests */
void TestArchiveDao::initTestCase(){
    connectToDatabase("SpikeStreamArchiveTest");
}

/*----------------------------------------------------------*/
/*-----                    TESTS                       -----*/
/*----------------------------------------------------------*/

void TestArchiveDao::testDeleteArchive(){
    //Add a test network to get a valid network id
    addTestNetwork1();

    //Add a test archive
    addTestArchive1();

    //Check that test archive is in database
    QSqlQuery query = getArchiveQuery("SELECT * FROM Archives WHERE ArchiveID = " + QString::number(testArchiveID));
    executeQuery(query);

    //Should be a single network
    QCOMPARE(query.size(), (int)1);

    //Check that archive data is in database
    QSqlQuery query = getArchiveQuery("SELECT * FROM ArchiveData WHERE ArchiveID = " + QString::number(testArchiveID));
    executeQuery(query);

    //Should be a three time steps
    QCOMPARE(query.size(), (int)3);

    //Invoke method being tested
    ArchiveDao archiveDao(archiveDBInfo);
    archiveDao.deleteArchive(testArchiveID);

    //Check to see if archive with this id has been removed from the database
    query = getArchiveQuery("SELECT * FROM Archives WHERE ArchiveID = " + QString::number(testArchiveID));
    executeQuery(query);

    //Should be no archive
    QCOMPARE(query.size(), (int)0);

    //Check that delete has cascaded properly
    QSqlQuery query = getArchiveQuery("SELECT * FROM ArchiveData WHERE ArchiveID = " + QString::number(testArchiveID));
    executeQuery(query);

    //Should be a three time steps
    QCOMPARE(query.size(), (int)0);
}


void TestArchiveDao::testGetArchivesInfo(){
    //Add a test network to get a valid network id
    addTestNetwork1();

    //Add a test archive
    addTestArchive1();

    //Check that the information returned is correct
    ArchiveDao archiveDao(archiveDBInfo);
    QList<ArchiveInfo> = archiveDao.getArchivesInfo();


}


void TestArchiveDao::testGetArchiveSize(){
    //Add a test network to get a valid network id
    addTestNetwork1();

    //Add a test archive
    addTestArchive1();

    //Should be three rows
    ArchiveDao archiveDao(archiveDBInfo);
    int archiveSize = archiveDao.getArchiveSize(archiveID);
    QCOMPARE(archiveSize, (int)3);
}






