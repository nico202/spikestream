#ifndef TESTDATABASEDAO_H
#define TESTDATABASEDAO_H

//SpikeStream includes
#include "TestDao.h"
#include "WeightlessNeuron.h"
using namespace spikestream;

//Qt includes
#include <QtTest>
#include <QString>

class TestDatabaseDao : public TestDao {
	Q_OBJECT

	private slots:
		void init();
		void testExecuteSQLFile();
		void testGetDatabaseNames();

	private:
		QString workingDirectory;

};

#endif//TESTDATABASEDAO_H

