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
		//Create calculator with equal k and l
		TransferEntropyCalculator testCalculator(3, 3, 10);

		//Create iVector
		vector<unsigned> testIData;
		testIData.push_back(1);
		testIData.push_back(0);
		testIData.push_back(1);
		testIData.push_back(1);
		testIData.push_back(0);

		testIData.push_back(1);
		testIData.push_back(1);
		testIData.push_back(1);
		testIData.push_back(0);
		testIData.push_back(1);

		//Create jVector
		vector<unsigned> testJData;
		testJData.push_back(0);
		testJData.push_back(0);
		testJData.push_back(1);
		testJData.push_back(0);
		testJData.push_back(0);

		testJData.push_back(1);
		testJData.push_back(0);
		testJData.push_back(0);
		testJData.push_back(0);
		testJData.push_back(1);

		//Invoke the method
		testCalculator.load_I_k_J_l_probabilities(0, testIData, testJData);

		//Check that the appropriate probabilities have been stored
		QHash<unsigned, double>& probMap = testCalculator.getI_k_J_l_probs();
		QCOMPARE(probMap.size(), 5);

		QVERIFY(probMap.contains(41));
		QVERIFY(probMap.contains(26));
		QVERIFY(probMap.contains(52));
		QVERIFY(probMap.contains(60));
		QVERIFY(probMap.contains(48));

		QCOMPARE(probMap[41], 3.0/8.0);
		QCOMPARE(probMap[26], 2.0/8.0);
		QCOMPARE(probMap[52], 1.0/8.0);
		QCOMPARE(probMap[60], 1.0/8.0);
		QCOMPARE(probMap[48], 1.0/8.0);


		//Create calculator with unequal k and l
		TransferEntropyCalculator testCalculator2(4, 2, 10);

		//Invoke the method
		testCalculator2.load_I_k_J_l_probabilities(0, testIData, testJData);

		//Check that the appropriate probabilities have been stored
		probMap = testCalculator2.getI_k_J_l_probs();
		QCOMPARE(probMap.size(), 5);

		QVERIFY(probMap.contains(46));
		QVERIFY(probMap.contains(24));
		QVERIFY(probMap.contains(53));
		QVERIFY(probMap.contains(28));
		QVERIFY(probMap.contains(56));

		QCOMPARE(probMap[46], 2.0/7.0);
		QCOMPARE(probMap[24], 1.0/7.0);
		QCOMPARE(probMap[53], 2.0/7.0);
		QCOMPARE(probMap[28], 1.0/7.0);
		QCOMPARE(probMap[56], 1.0/7.0);

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


void TestTransferEntropyCalculator::testLoad_I_k_plus_1_J_l_probabilities(){
	try{
		//Create calculator with equal k and l
		TransferEntropyCalculator testCalculator(3, 3, 10);

		//Create iVector
		vector<unsigned> testIData;
		testIData.push_back(1);
		testIData.push_back(1);
		testIData.push_back(0);
		testIData.push_back(0);
		testIData.push_back(1);

		testIData.push_back(1);
		testIData.push_back(1);
		testIData.push_back(0);
		testIData.push_back(0);
		testIData.push_back(0);

		testIData.push_back(1);
		testIData.push_back(1);

		//Create jVector
		vector<unsigned> testJData;
		testJData.push_back(0);
		testJData.push_back(1);
		testJData.push_back(0);
		testJData.push_back(1);
		testJData.push_back(1);

		testJData.push_back(0);
		testJData.push_back(1);
		testJData.push_back(0);
		testJData.push_back(0);
		testJData.push_back(1);

		testJData.push_back(1);
		testJData.push_back(0);

		//Invoke the method
		testCalculator.load_I_k_plus_1_J_l_probabilities(2, testIData, testJData);

		//Check that the appropriate probabilities have been stored
		QHash<unsigned, double>& probMap = testCalculator.getI_k_plus_1_J_l_probs();
		QCOMPARE(probMap.size(), 6);

		QVERIFY(probMap.contains(27));
		QVERIFY(probMap.contains(62));
		QVERIFY(probMap.contains(117));
		QVERIFY(probMap.contains(98));
		QVERIFY(probMap.contains(68));
		QVERIFY(probMap.contains(9));

		QCOMPARE(probMap[27], 2.0/7.0);
		QCOMPARE(probMap[62], 1.0/7.0);
		QCOMPARE(probMap[117], 1.0/7.0);
		QCOMPARE(probMap[98], 1.0/7.0);
		QCOMPARE(probMap[68], 1.0/7.0);
		QCOMPARE(probMap[9], 1.0/7.0);


		//Create calculator with unequal k and l
		TransferEntropyCalculator testCalculator2(3, 2, 10);

		//Invoke the method
		testCalculator2.load_I_k_plus_1_J_l_probabilities(2, testIData, testJData);

		//Check that the appropriate probabilities have been stored
		probMap = testCalculator2.getI_k_plus_1_J_l_probs();
		QCOMPARE(probMap.size(), 6);

		QVERIFY(probMap.contains(15));
		QVERIFY(probMap.contains(30));
		QVERIFY(probMap.contains(57));
		QVERIFY(probMap.contains(50));
		QVERIFY(probMap.contains(32));
		QVERIFY(probMap.contains(5));

		QCOMPARE(probMap[15], 2.0/7.0);
		QCOMPARE(probMap[30], 1.0/7.0);
		QCOMPARE(probMap[57], 1.0/7.0);
		QCOMPARE(probMap[50], 1.0/7.0);
		QCOMPARE(probMap[32], 1.0/7.0);
		QCOMPARE(probMap[5], 1.0/7.0);

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


void TestTransferEntropyCalculator::testGetTransferEntropy(){
	try{
		//Create calculator with equal k and l
		TransferEntropyCalculator testCalculator(3, 3, 10);

		//Create iVector
		vector<unsigned> testIData;
		testIData.push_back(0);
		testIData.push_back(1);
		testIData.push_back(1);
		testIData.push_back(0);
		testIData.push_back(0);

		testIData.push_back(1);
		testIData.push_back(1);
		testIData.push_back(1);
		testIData.push_back(0);
		testIData.push_back(0);

		testIData.push_back(0);
		testIData.push_back(1);

		//Create jVector
		vector<unsigned> testJData;
		testJData.push_back(1);
		testJData.push_back(1);
		testJData.push_back(0);
		testJData.push_back(0);
		testJData.push_back(1);

		testJData.push_back(1);
		testJData.push_back(1);
		testJData.push_back(0);
		testJData.push_back(0);
		testJData.push_back(0);

		testJData.push_back(1);
		testJData.push_back(1);

		//Invoke the method and check result
		QCOMPARE(testCalculator.getTransferEntropy(0, testIData, testJData), 3.0/7.0);

		//Should produce zero if run in other direction
		QCOMPARE(testCalculator.getTransferEntropy(0, testJData, testIData), 0.0);
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





