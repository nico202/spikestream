#ifndef TESTARCHIVEDAO_H
#define TESTARCHIVEDAO_H

//SpikeStream includes
#include "TestDao.h"

//Qt includes
#include <QtTest>
#include <QString>

class TestArchiveDao : public TestDao {
	Q_OBJECT

	private slots:
	    void testAddArchive();
	    void testAddArchiveData();
	    void testDeleteArchive();
	    void testGetArchivesInfo();
	    void testGetArchiveSize();
	    void testGetFiringNeuronIDs();
	    void testGetMaxTimeStep();
	    void testNetworkIsLocked();

};

#endif//TESTARCHIVEDAO_H

