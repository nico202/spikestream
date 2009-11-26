#ifndef TESTUTIL_H
#define TESTUTIL_H

//Qt includes
#include <QTest>

class TestUtil : public QObject {
    Q_OBJECT

    private slots:
	void testFillSelectionArray();
	void testRDouble();
	void testRandom();

};


#endif//TESTUTIL_H
