#ifndef TESTSUBSET_H
#define TESTSUBSET_H

//Qt includes
#include <QTest>

class TestSubset : public QObject {
    Q_OBJECT

    private slots:
	void testAddNeuronIndex();
	void testContains();
};


#endif//TESTSUBSET_H

