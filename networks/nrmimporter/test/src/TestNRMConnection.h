#ifndef TESTNRMCONNECTION_H
#define TESTNRMCONNECTION_H

#include <QtTest/QtTest>

namespace spikestream {

	class TestNRMConnection : public QObject {
		Q_OBJECT

		private slots:
			void init();
			void testCreateConnections();

		private:
			QString workingDirectory;

	};

}

#endif // TESTNRMCONNECTION_H
