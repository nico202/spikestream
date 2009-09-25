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
	    void testAddConnectionGroup();
	    void testAddNeuronGroup();
	    void testLoadConnections();
	    void testLoadNeurons();

	private:
	    //======================  VARIABLES  =======================


	    //=======================  METHODS  ========================
	    void runThread(NetworkDaoThread& thread);

};


#endif//TESTNETWORKDAOTHREAD_H

