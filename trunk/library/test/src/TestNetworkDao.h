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
    //void init();
	    void testAddNetwork();
	    void testAddWeightlessConnection();
	    void testAddWeightlessNeuronTrainingPattern();
	    void testDeleteNetwork();
	    void testGetConnections();
	    void testGetConnectionGroupsInfo();
	    void testGetConnectionGroupSize();
	    void testGetNeuronGroupBoundingBox();
	    void testGetNetworksInfo();
	    void testGetNeuronGroupsInfo();
	    void testGetNeuronIDs();
	    void testGetWeightlessNeuron();

	private:

};

#endif//TESTNETWORKDAO_H

