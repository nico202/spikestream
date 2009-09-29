//SpikeStream includes
#include "SpikeStreamException.h"
#include "TestArchiveDao.h"
#include "ArchiveDao.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

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
}


void TestArchiveDao::testGetArchiveSize(){
    //Add a test archive
    addTestArchive1();

    //Should be three rows
    ArchiveDao archiveDao(archiveDBInfo);
    int archiveSize = archiveDao.getArchiveSize(testArchive1ID);
    QCOMPARE(archiveSize, (int)3);
}


void TestArchiveDao::testGetFiringNeuronIDs(){
    //Add a test archive
    addTestArchive1();

    ArchiveDao archiveDao(archiveDBInfo);

    //Check that correct IDs are returned for each time step
    QStringList firingNeuronIDs = archiveDao.getFiringNeuronIDs(testArchive1ID, 1);
    QCOMPARE(firingNeuronIDs.size(), (int)4);
    QCOMPARE(Util::getUInt(firingNeuronIDs[0]), (unsigned int)256);
    QCOMPARE(Util::getUInt(firingNeuronIDs[1]), (unsigned int)311);
    QCOMPARE(Util::getUInt(firingNeuronIDs[2]), (unsigned int)21);
    QCOMPARE(Util::getUInt(firingNeuronIDs[3]), (unsigned int)4);

    firingNeuronIDs = archiveDao.getFiringNeuronIDs(testArchive1ID, 2);
    QCOMPARE(firingNeuronIDs.size(), (int)3);
    QCOMPARE(Util::getUInt(firingNeuronIDs[0]), (unsigned int)22);
    QCOMPARE(Util::getUInt(firingNeuronIDs[1]), (unsigned int)31);
    QCOMPARE(Util::getUInt(firingNeuronIDs[2]), (unsigned int)4888888);

    firingNeuronIDs = archiveDao.getFiringNeuronIDs(testArchive1ID, 3);
    QCOMPARE(firingNeuronIDs.size(), (int)0);

    firingNeuronIDs = archiveDao.getFiringNeuronIDs(testArchive1ID, 5);
    QCOMPARE(firingNeuronIDs.size(), (int)1);
    QCOMPARE(Util::getUInt(firingNeuronIDs[0]), (unsigned int)3);

}



void TestArchiveDao::testGetMaxTimeStep(){
    //Add a test archive
    addTestArchive1();

    ArchiveDao archiveDao(archiveDBInfo);

    //Check maximum time step is correct
    unsigned int maxTimeStep = archiveDao.getMaxTimeStep(testArchive1ID);
    QCOMPARE(maxTimeStep, (unsigned int)5);
}



