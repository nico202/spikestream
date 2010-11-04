#include "Connection.h"
#include "SpikeStreamException.h"
#include "TestConnection.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;


/*----------------------------------------------------------*/
/*-----                     TESTS                      -----*/
/*----------------------------------------------------------*/

/* http://marknelson.us/1989/10/01/lzw-data-compression/
	http://www.dspguide.com/ch27/5.htm
*/
void TestConnection::testCompression(){
	unsigned num1 = 123456;
	unsigned num2 = 4;
	unsigned num3 = 2929290000l;
	unsigned ander = 0x000000ff;

	for(int i=0; i<12; ++i){
		num1 & ander;
	}
	//int cats[7];
	//cout<<"SIZE OF CATS: "<<sizeof(cats)<<endl;
}

void TestConnection::testGetDelay(){
	//Check delay is accurately returned for different values
	Connection con1(1, 1, 1234, 0.5);
	QCOMPARE (con1.getDelay(), 1234.0f);
	Connection con2(1, 1, 6000.2, 0.5);
	QCOMPARE (con2.getDelay(), 6000.2f);
	Connection con3(1, 1, 3.7, 0.5);
	QCOMPARE (con3.getDelay(), 3.7f);

	//Check that an out of range delay value is picked up
	try{
		Connection con4(1, 1, 6560, 0.5);
		QFAIL("Exception should have been thrown by out of range delay.");
	}
	catch(SpikeStreamException& ex){
	}
}



void TestConnection::testGetWeight(){
	//Check weight is accurately returned for different values
	Connection con1(1, 1, 1, 0.5);
	QCOMPARE (con1.getWeight(), 0.5f);
	Connection con2(1, 1, 1, 1.0);
	QCOMPARE (con2.getWeight(), 1.0f);
	Connection con3(1, 1, 1, -0.0007);
	QCOMPARE (con3.getWeight(), -0.0007f);

	//Check that an out of range delay value is picked up
	try{
		Connection con4(1, 1, 1, 1.0001);
		QFAIL("Exception should have been thrown by out of range weight.");
	}
	catch(SpikeStreamException& ex){
	}
}


void TestConnection::testSetWeight(){
	Connection con1(1, 1, 1, 0.545);
	QCOMPARE (con1.getWeight(), 0.545f);
	con1.setWeight(0.546f);
	QCOMPARE (con1.getWeight(), 0.546f);
	con1.setWeight(-0.999f);
	QCOMPARE (con1.getWeight(), -0.999f);

	//Check that an out of range weight is picked up
	try{
		con1.setWeight(1.0001f);
		QFAIL("Exception should have been thrown by out of range weight.");
	}
	catch(SpikeStreamException& ex){
	}
}


void TestConnection::testGetTempWeight(){
	//Check weight is accurately returned for different values
	Connection con1(1, 1, 1, 0.5);
	QCOMPARE (con1.getTempWeight(), 0.5f);
	Connection con2(1, 1, 1, 1.0);
	QCOMPARE (con2.getTempWeight(), 1.0f);
	Connection con3(1, 1, 1, -0.0007);
	QCOMPARE (con3.getTempWeight(), -0.0007f);
}


void TestConnection::testSetTempWeight(){
	Connection con1(1, 1, 1, 0.545);
	QCOMPARE (con1.getTempWeight(), 0.545f);
	con1.setTempWeight(0.746f);
	QCOMPARE (con1.getWeight(), 0.545f);
	QCOMPARE (con1.getTempWeight(), 0.746f);
	con1.setTempWeight(-0.555f);
	QCOMPARE (con1.getTempWeight(), -0.555f);

	//Check that an out of range weight is picked up
	try{
		con1.setTempWeight(-1.0001f);
		QFAIL("Exception should have been thrown by out of range weight.");
	}
	catch(SpikeStreamException& ex){
	}
}

