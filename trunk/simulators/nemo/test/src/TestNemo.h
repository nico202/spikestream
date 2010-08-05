#ifndef TESTNEMO_H
#define TESTNEMO_H

//Qt includes
#include <QtTest>
#include <QString>

#include <nemo.h>

class TestNemo : public QObject {
	Q_OBJECT

	private slots:
		void testNemoConfiguration();

	private:
		void testNemoDLL1();
		void testNemoDLL2();
		void checkNemoOutput(nemo_status_t result, const QString& errorMessage);

};

#endif//TESTNEMO_H
