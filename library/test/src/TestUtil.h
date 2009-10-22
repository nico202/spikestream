#ifndef TESTUTIL_H
#define TESTUTIL_H

//Qt includes
#include <QTest>

class TestUtil : public QObject {
    Q_OBJECT

    private slots:
	void testRandom();

};


#endif//TESTUTIL_H
