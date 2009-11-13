#ifndef TESTSTATEBASEDPHIANALYSISDAO_H
#define TESTSTATEBASEDPHIANALYSISDAO_H

//SpikeStream includes
#include "TestDao.h"

//Qt includes
#include <QtTest>
#include <QString>

class TestStateBasedPhiAnalysisDao : public TestDao {
	Q_OBJECT

	private slots:
	    void testAddComplex();
	    void testDeleteTimeSteps();
	    void testGetComplexCount();
	    void testGetStateBasedPhiDataTableModel();

	private:
	    void addTestAnalysis1Data();
};

#endif//TESTSTATEBASEDPHIANALYSISDAO_H


