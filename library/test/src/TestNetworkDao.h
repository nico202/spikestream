#ifndef TESTNETWORKDAO_H
#define TESTNETWORKDAO_H

//SpikeStream includes
#include "TestDao.h"

//Qt includes
#include <QtTest>
#include <QString>

class TestNetworkDao : public TestDao {
	Q_OBJECT

	private slots:
	    void cleanup();
	    void cleanupTestCase();
	    void init();
	    void initTestCase();
	    void testAddConnectionGroup();
	    void testAddNetwork();
	    void testGetNetworksInfo();
	    void testAddNeuronGroup();

	private:
	    void addTestNetwork1();

};

#endif//TESTNETWORKDAO_H

