#ifndef TESTNETWORK_H
#define TESTNETWORK_H

//SpikeStream includes
#include "TestDao.h"

//Qt includes
#include <QtTest>

class TestNetwork : public TestDao {
	Q_OBJECT

	private slots:
	    void testGetBoundingBox();
		//void testGetConnections();

	private:

};



#endif//TESTNETWORK_H
