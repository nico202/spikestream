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
		void testDeleteAllNetworks();
		void testDeleteNetwork();
		void testGetConnections1();
		void testGetConnections2();
		void testGetAllFromConnections();
		void testGetAllToConnections();
		void testGetFromConnections();
		void testGetToConnections();
		void testGetConnectionGroupsInfo();
		void testGetConnectionGroupSize();
		void testGetNeuronCount();
		void testGetNeuronGroupBoundingBox();
		void testGetNetworksInfo();
		void testGetNeuronGroupsInfo();
		void testGetNeuronIDs();
		void testGetNeuronGroupID();
		void testGetNeuronTypes();
		void testGetSynapseTypes();
		void testGetWeightlessNeuron();
		void testIsWeightlessNetwork();
		void testIsWeightlessNeuron();
		void testSetTempWeight();

	private:
		bool bitsEqual(const unsigned char* byteArr, const QString pattern, int output);
};

#endif//TESTNETWORKDAO_H
