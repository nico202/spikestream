#ifndef TESTNEMOWRAPPER_H
#define TESTNEMOWRAPPER_H

//Qt includes
#include <QTest>


class TestNemoWrapper : public QObject {
	Q_OBJECT

	private slots:
		void testConstructor();

};


#endif//TESTNEMOWRAPPER_H
