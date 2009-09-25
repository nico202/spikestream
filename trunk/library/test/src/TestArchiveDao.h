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
	    void testDeleteArchive();
	    void testGetArchivesInfo();
	    void testGetArchiveSize();

};

#endif//TESTARCHIVEDAO_H

