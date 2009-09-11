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
	    void testLoadConnections();
	    void testLoadNeurons();

	private:
	    //======================  VARIABLES  =======================
	    unsigned int testNetID;
	    unsigned int neurGrp1ID;
	    unsigned int neurGrp2ID;
	    unsigned int connGrp1ID;
	    QList<unsigned int> testConnIDList;
	    QList<unsigned int> testNeurIDList;


	    //=======================  METHODS  ========================
	    void addTestNetwork1();
	    unsigned int addTestConnection(unsigned int connGrpID, unsigned int fromID, unsigned int toID, float weight, float delay);
	    unsigned int addTestNeuron(unsigned int neurGrpID, float x, float y, float z);
	    void runThread(NetworkDaoThread& netDaoThread);

};


#endif//TESTNETWORKDAOTHREAD_H

