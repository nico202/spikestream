#ifndef TESTNRMRANDOM_H
#define TESTNRMRANDOM_H

#include <QtTest/QtTest>

namespace spikestream {

	class TestNRMRandom : public QObject {
		Q_OBJECT

		private slots:
			void cleanup();
			void init();
			void testRandom();

	};

}

#endif // TESTNRMRANDOM_H
