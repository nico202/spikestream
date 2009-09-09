#ifndef TESTNETWORKDAOTHREAD_H
#define TESTNETWORKDAOTHREAD_H

//SpikeStream includes
#include "TestDao.h"

//Qt includes
#include <QtTest>
#include <QString>

class TestNetworkDaoThread : public TestDao {
	Q_OBJECT

	private slots:
	    void cleanup();
	    void cleanupTestCase();
	    void init();
	    void initTestCase();
	    void testAddConnectionGroup();
	    void testAddNeuronGroup();

	private:


};


#endif//TESTNETWORKDAOTHREAD_H

