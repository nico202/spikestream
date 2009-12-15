#ifndef TESTANALYSISDAO_H
#define TESTANALYSISDAO_H

//SpikeStream includes
#include "TestDao.h"

//Qt includes
#include <QtTest>
#include <QString>

class TestAnalysisDao : public TestDao {
	Q_OBJECT

	private slots:
	    void testAddAnalysis();
	    void testDeleteAnalysis();
	    void testGetAnalysesInfo();
	    void testUpdateDescription();
};

#endif//TESTANALYSISDAO_H
