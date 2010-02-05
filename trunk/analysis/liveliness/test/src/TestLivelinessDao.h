#ifndef TESTLIVELINESSDAO_H
#define TESTLIVELINESSDAO_H

//SpikeStream includes
#include "TestDao.h"

//Qt includes
#include <QtTest>
#include <QString>

class TestLivelinessDao : public TestDao {
	Q_OBJECT

	private slots:
		void testAddCluster();
		void testContainsAnalysisData();
		void testDeleteTimeSteps();
		void testGetClusters();
		void testGetNeuronLiveliness();
		void testSetNeuronLiveliness();

	private:
		void addTestAnalysis1Data();
};

#endif//TESTLIVELINESSDAO_H


