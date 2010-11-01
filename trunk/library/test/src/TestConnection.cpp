#include "Connection.h"
#include "SpikeStreamException.h"
#include "TestConnection.h"
using namespace spikestream;


/*----------------------------------------------------------*/
/*-----                     TESTS                      -----*/
/*----------------------------------------------------------*/

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


void TestConnection::testSetDelay(){

}


void TestConnection::testGetWeight(){
	//Check delay is accurately returned for different values
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
	FIXME: SORTING THIS OUT

}


void TestConnection::testGetTempWeight(){


}


void TestConnection::testSetTempWeight(){

}

