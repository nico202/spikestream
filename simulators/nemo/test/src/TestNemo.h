#ifndef TESTNEMO_H
#define TESTNEMO_H

//Qt includes
#include <QtTest>
#include <QString>

class TestNemo : public QObject {
	Q_OBJECT

	private slots:
		void testNemoDLL();

};

#endif//TESTNEMO_H
