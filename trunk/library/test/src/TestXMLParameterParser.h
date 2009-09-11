#ifndef TESTXMLPARAMETERPARSER_H
#define TESTXMLPARAMETERPARSER_H

//Qt includes
#include <QtTest>
#include <QString>

class TestXMLParameterParser : public QObject {
	Q_OBJECT

	private slots:
	    void cleanup();
	    void cleanupTestCase();
	    void init();
	    void initTestCase();
	    void testGetParameterMap();

	private:

};

#endif//TESTXMLPARAMETERPARSER_H

