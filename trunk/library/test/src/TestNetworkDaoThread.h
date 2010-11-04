#ifndef TESTNETWORKDAOTHREAD_H
#define TESTNETWORKDAOTHREAD_H

//SpikeStream includes
#include "NetworkDaoThread.h"
#include "TestDao.h"

//Qt includes
#include <QtTest>
#include <QString>

class TestNetworkDaoThread : public TestDao {
	Q_OBJECT

	private slots:
		void testAddConnectionGroup();
	    void testAddNeuronGroup();
		void testDeleteConnectionGroups();
		void testDeleteNetwork();
		void testDeleteNeuronGroups();
	    void testLoadConnections();
	    void testLoadNeurons();

	private:
	    //======================  VARIABLES  =======================


	    //=======================  METHODS  ========================
	    void runThread(NetworkDaoThread& thread);

};


#endif//TESTNETWORKDAOTHREAD_H

