#ifndef TESTWEIGHTLESSLIVELINESSANALYZER_H
#define TESTWEIGHTLESSLIVELINESSANALYZER_H

//SpikeStream includes
#include "AnalysisInfo.h"
#include "Cluster.h"
#include "TestDao.h"
#include "WeightlessLivelinessAnalyzer.h"
using namespace spikestream;

//Qt includes
#include <QtTest>
#include <QString>

class TestWeightlessLivelinessAnalyzer : public TestDao {
	Q_OBJECT

	private slots:
		void testCalculateConnectionLiveliness();
		void testFillInputArray();
		void testFlipBits();
		void testIdentifyClusters();

	private:
		void checkClusters(QList<Cluster>& clusterList, QString clusterStr, QString clusterLivelinessStr);
		AnalysisInfo getAnalysisInfo();
};

#endif//TESTWEIGHTLESSLIVELINESSANALYZER_H


