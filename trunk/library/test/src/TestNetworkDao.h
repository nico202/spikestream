#ifndef TESTNETWORKDAO_H
#define TESTNETWORKDAO_H

//SpikeStream includes
#include "TestDao.h"
#include "WeightlessNeuron.h"
using namespace spikestream;

//Qt includes
#include <QtTest>
#include <QString>

class TestNetworkDao : public TestDao {
	Q_OBJECT

	private slots:
		void testAddNetwork();
		void testAddWeightlessConnection();
		void testAddWeightlessNeuronTrainingPattern();
		void testDeleteNetwork();
		void testGetConnections1();
		void testGetConnections2();
		void testGetAllFromConnections();
		void testGetAllToConnections();
		void testGetFromConnections();
		void testGetToConnections();
		void testGetConnectionGroupsInfo();
		void testGetConnectionGroupSize();
		void testGetNeuronGroupBoundingBox();
		void testGetNetworksInfo();
		void testGetNeuronGroupsInfo();
		void testGetNeuronIDs();
		void testGetNeuronGroupID();
		void testGetWeightlessNeuron();
		void testIsWeightlessNetwork();
		void testIsWeightlessNeuron();
		void testSetTempWeight();

	private:
		bool bitsEqual(byte* byteArr, const QString pattern, int output);
};

#endif//TESTNETWORKDAO_H

