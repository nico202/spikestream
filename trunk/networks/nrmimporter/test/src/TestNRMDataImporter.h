#ifndef TESTNRMDATAIMPORTER_H
#define TESTNRMDATAIMPORTER_H

#include <QtTest/QtTest>
#include <QString>


namespace spikestream {

	class TestNRMDataImporter : public QObject {
		Q_OBJECT

		private slots:
			void cleanup();
			void init();
			void testAddTraining();

		private:
			QString workingDirectory;
	};
}

#endif//TESTNRMDATAIMPORTER_H

