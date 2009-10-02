#ifndef TESTNRMDATASETIMPORTER_H
#define TESTNRMDATASETIMPORTER_H

#include <QtTest/QtTest>
#include <QString>

class TestNRMDataSetImporter : public QObject {
	Q_OBJECT

	private slots:
	    void cleanup();
	    void init();
	    void testLoadDataSet();

	private:
	    QString workingDirectory;
};

#endif//TESTNRMDATASETIMPORTER_H

