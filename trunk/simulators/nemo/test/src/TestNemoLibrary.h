#ifndef TESTNEMOLIBRARY_H
#define TESTNEMOLIBRARY_H

//Qt includes
#include <QtTest>
#include <QString>

#include <nemo.h>

class TestNemoLibrary : public QObject {
	Q_OBJECT

	private slots:
		void testNemoDLL1();
		void testNemoDLL2();
		void testNemoConfiguration();

	private:
		void checkNemoOutput(nemo_status_t result, const QString& errorMessage);

};

#endif//TESTNEMOLIBRARY_H
