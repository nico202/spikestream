#ifndef TESTNRMCONFIGLOADER_H
#define TESTNRMCONFIGLOADER_H

#include <QtTest/QtTest>
#include <QString>

class TestNRMConfigLoader : public QObject {
	Q_OBJECT

	private slots:
		void cleanup();
		void init();
		void testLoadConnectionParameters();
		void testLoadInputs();
		void testLoadNeuralLayers();
    private:
	QString workingDirectory;
};


#endif // TESTNRMCONFIGLOADER_H
