#include "TestTransferEntropyCalculator.h"
#include "TransferEntropyCalculator.h"
#include "SpikeStreamException.h"
#include "SpikeStreamAnalysisException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

//Other includes
#include <iostream>
using namespace std;


void TestTransferEntropyCalculator::testGetBitString(){
	try{
		TransferEntropyCalculator testCalculator(5, 5, 20);

		QCOMPARE(testCalculator.getBitString(1), QString("00000000 00000000 00000000 00000001"));
		QCOMPARE(testCalculator.getBitString( (1<<7) ), QString("00000000 00000000 00000000 10000000"));
		QCOMPARE(testCalculator.getBitString( (1<<8) - 1 ), QString("00000000 00000000 00000000 11111111"));

	}
	catch(...){
		QFAIL("An unknown exception occurred");
	}

}


void TestTransferEntropyCalculator::testLoad_I_k_probabilities(){
	try{
		//Create calculator with k_param=5, l_param=5 and timeWindow=10
		TransferEntropyCalculator testCalculator(5, 5, 10);

		//Create iVector with 13 data items
		vector<unsigned> testIData;
		testIData.push_back(0);
		testIData.push_back(1);
		testIData.push_back(1);
		testIData.push_back(0);
		testIData.push_back(0);

		testIData.push_back(1);
		testIData.push_back(1);
		testIData.push_back(0);
		testIData.push_back(0);
		testIData.push_back(1);

		testIData.push_back(1);
		testIData.push_back(0);
		testIData.push_back(1);

		//Invoke the method
		testCalculator.load_I_k_probabilities(2, testIData);

		//Check that the appropriate probabilities have been stored
		QHash<unsigned, double>& probMap = testCalculator.getI_k_probs();
		QCOMPARE(probMap.size(), 4);

		QVERIFY(probMap.contains(19));
		QVERIFY(probMap.contains(6));
		QVERIFY(probMap.contains(12));
		QVERIFY(probMap.contains(25));

		QCOMPARE(probMap[19], 2.0/6.0);
		QCOMPARE(probMap[6], 2.0/6.0);
		QCOMPARE(probMap[12], 1.0/6.0);
		QCOMPARE(probMap[25], 1.0/6.0);

	}
	catch(SpikeStreamAnalysisException& ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(SpikeStreamException& ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("An unknown exception occurred");
	}
}


void TestTransferEntropyCalculator::testLoad_I_k_plus_1_probabilities(){
	try{
		//Create calculator
		TransferEntropyCalculator testCalculator(3, 3, 11);

		//Create iVector
		vector<unsigned> testIData;
		testIData.push_back(0);
		testIData.push_back(1);
		testIData.push_back(1);
		testIData.push_back(0);
		testIData.push_back(0);

		testIData.push_back(1);
		testIData.push_back(1);
		testIData.push_back(0);
		testIData.push_back(0);
		testIData.push_back(1);

		testIData.push_back(1);
		testIData.push_back(0);
		testIData.push_back(1);

		//Invoke the method
		testCalculator.load_I_k_plus_1_probabilities(1, testIData);

		//Check that the appropriate probabilities have been stored
		QHash<unsigned, double>& probMap = testCalculator.getI_k_plus_1_probs();
		QCOMPARE(probMap.size(), 4);

		QVERIFY(probMap.contains(6));
		QVERIFY(probMap.contains(12));
		QVERIFY(probMap.contains(9));
		QVERIFY(probMap.contains(3));

		QCOMPARE(probMap[6], 2.0/8.0);
		QCOMPARE(probMap[12], 2.0/8.0);
		QCOMPARE(probMap[9], 2.0/8.0);
		QCOMPARE(probMap[3], 2.0/8.0);

	}
	catch(SpikeStreamAnalysisException& ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(SpikeStreamException& ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("An unknown exception occurred");
	}
}


void TestTransferEntropyCalculator::testLoad_I_k_J_k_probabilities(){
	try{
		//Create calculator
		TransferEntropyCalculator testCalculator(3, 3, 11);

		//Create iVector
		vector<unsigned> testIData;
		testIData.push_back(0);
		testIData.push_back(1);
		testIData.push_back(1);
		testIData.push_back(0);
		testIData.push_back(0);

		testIData.push_back(1);
		testIData.push_back(1);
		testIData.push_back(0);
		testIData.push_back(0);
		testIData.push_back(1);

		testIData.push_back(1);
		testIData.push_back(0);
		testIData.push_back(1);

		//Invoke the method
		testCalculator.load_I_k_plus_1_probabilities(1, testIData);

		//Check that the appropriate probabilities have been stored
		QHash<unsigned, double>& probMap = testCalculator.getI_k_plus_1_probs();
		QCOMPARE(probMap.size(), 4);

		QVERIFY(probMap.contains(6));
		QVERIFY(probMap.contains(12));
		QVERIFY(probMap.contains(9));
		QVERIFY(probMap.contains(3));

		QCOMPARE(probMap[6], 2.0/8.0);
		QCOMPARE(probMap[12], 2.0/8.0);
		QCOMPARE(probMap[9], 2.0/8.0);
		QCOMPARE(probMap[3], 2.0/8.0);

	}
	catch(SpikeStreamAnalysisException& ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(SpikeStreamException& ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("An unknown exception occurred");
	}
}







