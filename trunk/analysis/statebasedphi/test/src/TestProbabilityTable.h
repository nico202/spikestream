#ifndef TESTPROBABILITYTABLE_H
#define TESTPROBABILITYTABLE_H

//Qt includes
#include <QTest>

class TestProbabilityTable : public QObject {
    Q_OBJECT

    private slots:
	void testBuildTable();
	void testSet();

};


#endif//TESTPROBABILITYTABLE_H
