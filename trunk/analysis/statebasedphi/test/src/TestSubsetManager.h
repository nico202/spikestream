#ifndef TESTSUBSETMANAGER_H
#define TESTSUBSETMANAGER_H

//SpikeStream includes
#include "StateBasedPhiAnalysisDaoDuck.h"
#include "Subset.h"
using namespace spikestream;

//Qt includes
#include <QTest>

class TestSubsetManager : public QObject {
    Q_OBJECT

    private slots:
	void testBuildSubsetList();
	void testIdentifyComplexes();

    private:
	bool complexExists(QList<Complex>& complexList, const QString neurIDStr, double phi);
	bool subsetContains(QList<Subset*>& subsetList, const QString neuronIDs);
};


#endif//TESTSUBSETMANAGER_H

