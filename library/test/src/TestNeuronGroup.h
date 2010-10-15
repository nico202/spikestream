#ifndef TESTNEURONGROUP_H
#define TESTNEURONGROUP_H

//Qt includes
#include <QtTest>

class TestNeuronGroup : public QObject {
	Q_OBJECT

	private slots:
	    void testAddLayer();
	    void testAddNeuron();
		void testGetPointFromPositionKey();
		void testGetPositionKey();
		void testPositionIterator();

	private:

};



#endif//TESTNEURONGROUP_H

