#include "Connection.h"
#include "TestMemory.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QList>

#include <vector>
using namespace std;


void TestMemory::run(){
	unsigned sleepPeriod = 10;
	unsigned numCons = 1000000;

	qDebug()<<"Starting test";
	this->sleep(2*sleepPeriod);

	qDebug()<<"Building array with a million connection pointers";
	Connection** conArray = new Connection*[numCons];
	this->sleep(sleepPeriod);

	qDebug()<<"Allocating memory for connections";
	for(unsigned i=0; i<numCons; ++i)
		conArray[i] = new Connection(1, 1, 0.2f, 0.1f);
	this->sleep(sleepPeriod);

	qDebug()<<"Creating hash map with million connections";
	QHash<unsigned, Connection*> hashMap;
	for(unsigned i=0; i<numCons; ++i)
		hashMap[i] = conArray[i];
	this->sleep(sleepPeriod);

	qDebug()<<"Shrinking hash map with million connections";
	hashMap.squeeze();
	this->sleep(sleepPeriod);

	qDebug()<<"Creating vector with pointers to connections";
	vector<Connection*> ptrVect;
	for(unsigned i=0; i<numCons; ++i)
		ptrVect.push_back(conArray[i]);
	this->sleep(sleepPeriod);

	qDebug()<<"Creating vector on heap with connections";
	vector<Connection>* conVect = new vector<Connection>();
	for(unsigned i=0; i<numCons; ++i){
		conVect->push_back(Connection(1, 1, 0.2f, 0.1f));
	}
	this->sleep(sleepPeriod);

	qDebug()<<"Creating list with connections";
	QList<Connection> conList;
	for(unsigned i=0; i<numCons; ++i){
		conList.push_back(Connection(1, 1, 0.2f, 0.1f));
	}
	this->sleep(sleepPeriod);

	qDebug()<<"Creating array with connection pointers";
	Connection* conArray2 = new Connection[numCons];
	this->sleep(sleepPeriod);
	delete conArray2;

	for(unsigned i=0; i<numCons; ++i)
		delete conArray[i];
	delete [] conArray;
	qDebug()<<"Memory tests complete";
}


void TestMemory::runMemoryTests(){
	this->start();
	this->wait();
}

