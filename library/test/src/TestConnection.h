#ifndef TESTCONNECTION_H
#define TESTCONNECTION_H

//Qt includes
#include <QtTest>

namespace spikestream {

	class TestConnection : public QObject {
		Q_OBJECT

		private slots:
			void testCompression();
			void testGetDelay();
			void testGetWeight();
			void testSetWeight();
			void testGetTempWeight();
			void testSetTempWeight();

		private:

	};

}


#endif//TESTCONNECTION_H
