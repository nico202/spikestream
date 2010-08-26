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
		void testGetAllFromConnections();
		void testGetAllToConnections();
		void testGetConnectionCount1();
		void testGetConnectionCount2();
		void testGetConnections1();
		void testGetConnections2();
		void testGetDefaultNeuronParameters();
		void testGetDefaultSynapseParameters();
		void testGetFromConnections();
		void testGetToConnections();
		void testGetConnectionGroupsInfo();
		void testGetConnectionGroupSize();
		void testGetNeuronCount1();
		void testGetNeuronCount2();
		void testGetNeuronCount3();
		void testGetNeuronGroupBoundingBox();
		void testGetNetworksInfo();
		void testGetNeuronGroupsInfo();
		void testGetNeuronIDs();
		void testGetNeuronGroupID();
		void testGetNeuronParameters();
		void testGetNeuronType();
		void testGetNeuronTypes();
		void testGetStartNeuronID();
		void testGetSynapseParameters();
		void testGetSynapseType();
		void testGetSynapseTypes();
		void testGetWeightlessNeuron();
		void testIsWeightlessNetwork();
		void testIsWeightlessNeuron();
		void testSetNeuronParameters();
		void testSetSynapseParameters();
		void testSetWeight();

	private:
		bool bitsEqual(const unsigned char* byteArr, const QString pattern, int output);
};

#endif//TESTNETWORKDAO_H

