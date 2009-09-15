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
	    void testAddNetwork();
	    void testGetConnectionGroupsInfo();
	    void testGetConnectionGroupSize(unsigned int connGrpID);
	    void testGetNetworksInfo();
	    void testGetNeuronGroupsInfo();

	private:

};

#endif//TESTNETWORKDAO_H
