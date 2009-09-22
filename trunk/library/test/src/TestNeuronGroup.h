#ifndef TESTNEURONGROUP_H
#define TESTNEURONGROUP_H

//Qt includes
#include <QtTest>

class TestNeuronGroup : public QObject {
	Q_OBJECT

	private slots:
	    void testAddLayer();
	    void testAddNeuron();


	private:

};



#endif//TESTNEURONGROUP_H

