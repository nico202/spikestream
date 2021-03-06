#ifndef TESTNRMTRAININGLOADER_H
#define TESTNRMTRAININGLOADER_H

#include <QtTest/QtTest>

namespace spikestream {

	class TestNRMTrainingLoader : public QObject {
		Q_OBJECT

		private slots:
			void init();
			void testLoadTraining();

		private:
			QString workingDirectory;

	};

}

#endif // TESTNRMTRAININGLOADER_H
