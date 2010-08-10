#ifndef TESTNEMOLIBRARY_H
#define TESTNEMOLIBRARY_H

//Qt includes
#include <QtTest>
#include <QString>

#include <nemo.h>

class TestNemoLibrary : public QObject {
	Q_OBJECT

	private slots:
		void testNemoConfiguration();
		void testNemoDLL1();

	private:
		void testNemoDLL2();
		void checkNemoOutput(nemo_status_t result, const QString& errorMessage);

};

#endif//TESTNEMOLIBRARY_H
