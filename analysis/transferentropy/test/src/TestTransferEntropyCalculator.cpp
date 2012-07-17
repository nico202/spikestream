#include "TestTransferEntropyCalculator.h"
#include "TransferEntropyCalculator.h"
#include "SpikeStreamException.h"
#include "SpikeStreamAnalysisException.h"
using namespace spikestream;

#include "transferentropycalc.h"

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

/*! Runs test with data:
		1: 11101 10100 11001 11011 01
		2: 10101 01011 01101 10110 01
*/
void TestTransferEntropyCalculator::testPresentationData(){
	try{
		//Create calculator with k=l=1, spanning the whole range of the data
		TransferEntropyCalculator testCalculator(1, 1, 22);

		//Create iVector
		vector<unsigned> testIData;
		testIData.push_back(1);
		testIData.push_back(1);
		testIData.push_back(1);
		testIData.push_back(0);
		testIData.push_back(1);

		testIData.push_back(1);
		testIData.push_back(0);
		testIData.push_back(1);
		testIData.push_back(0);
		testIData.push_back(0);

		testIData.push_back(1);
		testIData.push_back(1);
		testIData.push_back(0);
		testIData.push_back(0);
		testIData.push_back(1);

		testIData.push_back(1);
		testIData.push_back(1);
		testIData.push_back(0);
		testIData.push_back(1);
		testIData.push_back(1);

		testIData.push_back(0);
		testIData.push_back(1);

		//Create jVector
		vector<unsigned> testJData;
		testJData.push_back(1);
		testJData.push_back(0);
		testJData.push_back(1);
		testJData.push_back(0);
		testJData.push_back(1);

		testJData.push_back(0);
		testJData.push_back(1);
		testJData.push_back(0);
		testJData.push_back(1);
		testJData.push_back(1);

		testJData.push_back(0);
		testJData.push_back(1);
		testJData.push_back(1);
		testJData.push_back(0);
		testJData.push_back(1);

		testJData.push_back(1);
		testJData.push_back(0);
		testJData.push_back(1);
		testJData.push_back(1);
		testJData.push_back(0);

		testJData.push_back(0);
		testJData.push_back(1);

		//Invoke the method
		testCalculator.load_I_k_probabilities(0, testIData);
		testCalculator.load_I_k_plus_1_probabilities(0, testIData);
		testCalculator.load_I_k_J_l_probabilities(0, testIData, testJData);
		testCalculator.load_I_k_plus_1_J_l_probabilities(0, testIData, testJData);

		//Check that the appropriate probabilities have been stored
		QHash<unsigned, double>& I_k_probs = testCalculator.getI_k_probs();
		QHash<unsigned, double>& I_k_plus_1_probs = testCalculator.getI_k_plus_1_probs();
		QHash<unsigned, double>& I_k_J_l_probs = testCalculator.getI_k_J_l_probs();
		QHash<unsigned, double>& I_k_plus_1_J_l_probs = testCalculator.getI_k_plus_1_J_l_probs();

		//Check the entries in the probability map are correct
		QVERIFY(I_k_probs.contains(0));
		QVERIFY(I_k_probs.contains(1));
		QCOMPARE(round(I_k_probs[0], 5), round(0.363636, 5));
		QCOMPARE(round(I_k_probs[1], 5), round(0.636364, 5));

		QVERIFY(I_k_plus_1_probs.contains(0));
		QVERIFY(I_k_plus_1_probs.contains(1));
		QVERIFY(I_k_plus_1_probs.contains(2));
		QVERIFY(I_k_plus_1_probs.contains(3));
		QCOMPARE(round(I_k_plus_1_probs[0], 5), round(0.0952381, 5));
		QCOMPARE(round(I_k_plus_1_probs[1], 5), round(0.285714, 5));
		QCOMPARE(round(I_k_plus_1_probs[2], 5), round(0.285714, 5));
		QCOMPARE(round(I_k_plus_1_probs[3], 5), round(0.333333, 5));

		QVERIFY(I_k_J_l_probs.contains(0));
		QVERIFY(I_k_J_l_probs.contains(1));
		QVERIFY(I_k_J_l_probs.contains(2));
		QVERIFY(I_k_J_l_probs.contains(3));
		QCOMPARE(round(I_k_J_l_probs[0], 5), round(0.136364, 5));
		QCOMPARE(round(I_k_J_l_probs[1], 5), round(0.227273, 5));
		QCOMPARE(round(I_k_J_l_probs[2], 5), round(0.272727, 5));
		QCOMPARE(round(I_k_J_l_probs[3], 5), round(0.363636, 5));

		//Note these do not match the presentation because the encoding is different
		QVERIFY(I_k_plus_1_J_l_probs.contains(1));
		QVERIFY(I_k_plus_1_J_l_probs.contains(2));
		QVERIFY(I_k_plus_1_J_l_probs.contains(3));
		QVERIFY(I_k_plus_1_J_l_probs.contains(4));
		QVERIFY(I_k_plus_1_J_l_probs.contains(5));
		QVERIFY(I_k_plus_1_J_l_probs.contains(6));
		QVERIFY(I_k_plus_1_J_l_probs.contains(7));
		QCOMPARE(round(I_k_plus_1_J_l_probs[1], 5), round(0.0952381, 5));
		QCOMPARE(round(I_k_plus_1_J_l_probs[2], 5), round(0.142857, 5));
		QCOMPARE(round(I_k_plus_1_J_l_probs[3], 5), round(0.142857, 5));
		QCOMPARE(round(I_k_plus_1_J_l_probs[4], 5), round(0.190476, 5));
		QCOMPARE(round(I_k_plus_1_J_l_probs[5], 5), round(0.0952381, 5));
		QCOMPARE(round(I_k_plus_1_J_l_probs[6], 5), round(0.0952381, 5));
		QCOMPARE(round(I_k_plus_1_J_l_probs[7], 5), round(0.238095, 5));

		//Check what their final result should be
		double sum = 0.0, total;

		/*  n=0; x_n+1=1, x_n=1, y_n=1
			p(x_n=1) = 0.636364
			p(x_n+1=1, x_n=1) = 0.333333
			p(x_n=1, y_n=1) = 0.363636
			p(x_n+1=1, x_n=1, y_n=1) = 0.238095 */
		total = 0.238095 * log10 ( (0.238095 * 0.636364) / (0.363636 * 0.333333));
		cout<<"total for n=0: "<<total<<endl;
		sum += total;

		/*  n=1; x_n+1=1, x_n=1, y_n=0
			p(x_n=1) = 0.636364
			p(x_n+1=1, x_n=1) = 0.333333
			p(x_n=1, y_n=0) = 0.272727
			p(x_n+1=1, x_n=1, y_n=0) = 0.0952381 */
		total = 0.0952381 * log10 ( (0.0952381 * 0.636364) / (0.272727 * 0.333333));
		cout<<"total for n=1: "<<total<<endl;
		sum += total;

		/*  n=2; x_n+1=0, x_n=1, y_n=1
			   p(x_n=1) = 0.636364
			   p(x_n+1=0, x_n=1) = 0.285714
			   p(x_n=1, y_n=1) = 0.363636
			   p(x_n+1=0, x_n=1, y_n=1) = 0.0952381 */
		total = 0.0952381 * log10 ( (0.0952381 * 0.636364) / (0.363636 * 0.285714));
		cout<<"total for n=2: "<<total<<endl;
		sum += total;

		/*  n=3; x_n+1=1, x_n=0, y_n=0
			p(x_n=0) = 0.363636
			p(x_n+1=1, x_n=0) = 0.285714
			p(x_n=0, y_n=0) = 0.136364
			p(x_n+1=1, x_n=0, y_n=0) = 0.142857 */
		total = 0.142857 * log10 ( (0.142857 * 0.363636) / (0.136364 * 0.285714));
		cout<<"total for n=3: "<<total<<endl;
		sum += total;

		/*  n=4; x_n+1=1, x_n=1, y_n=1
			   p(x_n=1) = 0.636364
			   p(x_n+1=1, x_n=1) = 0.333333
			   p(x_n=1, y_n=1) = 0.363636
			   p(x_n+1=1, x_n=1, y_n=1) = 0.238095 */
		total = 0.238095 * log10 ( (0.238095 * 0.636364) / (0.363636 * 0.333333));
		cout<<"total for n=4: "<<total<<endl;
		sum += total;

		/*  n=5; x_n+1=0, x_n=1, y_n=0
			  p(x_n=1) = 0.636364
			  p(x_n+1=0, x_n=1) = 0.285714
			  p(x_n=1, y_n=0) = 0.272727
			  p(x_n+1=0, x_n=1, y_n=0) = 0.190476 */
		total = 0.190476 * log10 ( (0.190476 * 0.636364) / (0.272727 * 0.285714));
		cout<<"total for n=5: "<<total<<endl;
		sum += total;

		/*  n=6; x_n+1=1, x_n=0, y_n=1
			 p(x_n=0) = 0.363636
			 p(x_n+1=1, x_n=0) = 0.285714
			 p(x_n=0, y_n=1) = 0.227273
			 p(x_n+1=1, x_n=0, y_n=1) = 0.142857 */
		total = 0.142857 * log10 ( (0.142857 * 0.363636) / (0.227273 * 0.285714));
		cout<<"total for n=6: "<<total<<endl;
		sum += total;

		/*  n=7; x_n+1=0, x_n=1, y_n=0
			  p(x_n=1) = 0.636364
			  p(x_n+1=0, x_n=1) = 0.285714
			  p(x_n=1, y_n=0) = 0.272727
			  p(x_n+1=0, x_n=1, y_n=0) = 0.190476 */
		total = 0.190476 * log10 ( (0.190476 * 0.636364) / (0.272727 * 0.285714));
		cout<<"total for n=7: "<<total<<endl;
		sum += total;

		/*  n=8; x_n+1=0, x_n=0, y_n=1
			 p(x_n=0) = 0.363636
			 p(x_n+1=0, x_n=0) = 0.0952381
			 p(x_n=0, y_n=1) = 0.227273
			 p(x_n+1=0, x_n=0, y_n=1) = 0.0952381 */
		total = 0.0952381 * log10 ( (0.0952381 * 0.363636) / (0.227273 * 0.0952381));
		cout<<"total for n=8: "<<total<<endl;
		sum += total;

		/*  n=9; x_n+1=1, x_n=0, y_n=1
			 p(x_n=0) = 0.363636
			 p(x_n+1=1, x_n=0) = 0.285714
			 p(x_n=0, y_n=1) = 0.227273
			 p(x_n+1=1, x_n=0, y_n=1) = 0.142857 */
		total = 0.142857 * log10 ( (0.142857 * 0.363636) / (0.227273 * 0.285714));
		cout<<"total for n=9: "<<total<<endl;
		sum += total;

		/*  n=10; x_n+1=1, x_n=1, y_n=0
			 p(x_n=1) = 0.636364
			 p(x_n+1=1, x_n=1) = 0.333333
			 p(x_n=1, y_n=0) = 0.272727
			 p(x_n+1=1, x_n=1, y_n=0) = 0.0952381 */
		total = 0.0952381 * log10 ( (0.0952381 * 0.636364) / (0.272727 * 0.333333));
		cout<<"total for n=10: "<<total<<endl;
		sum += total;

		/*  n=11; x_n+1=0, x_n=1, y_n=1
			   p(x_n=1) = 0.636364
			   p(x_n+1=0, x_n=1) = 0.285714
			   p(x_n=1, y_n=1) = 0.363636
			   p(x_n+1=0, x_n=1, y_n=1) = 0.0952381 */
		total = 0.0952381 * log10 ( (0.0952381 * 0.636364) / (0.363636 * 0.285714));
		cout<<"total for n=11: "<<total<<endl;
		sum += total;

		/*  n=12; x_n+1=0, x_n=0, y_n=1
			 p(x_n=0) = 0.363636
			 p(x_n+1=0, x_n=0) = 0.0952381
			 p(x_n=0, y_n=1) = 0.227273
			 p(x_n+1=0, x_n=0, y_n=1) = 0.0952381 */
		total = 0.0952381 * log10 ( (0.0952381 * 0.363636) / (0.227273 * 0.0952381));
		cout<<"total for n=12: "<<total<<endl;
		sum += total;

		/*  n=13; x_n+1=1, x_n=0, y_n=0
			p(x_n=0) = 0.363636
			p(x_n+1=1, x_n=0) = 0.285714
			p(x_n=0, y_n=0) = 0.136364
			p(x_n+1=1, x_n=0, y_n=0) = 0.142857 */
		total = 0.142857 * log10 ( (0.142857 * 0.363636) / (0.136364 * 0.285714));
		cout<<"total for n=13: "<<total<<endl;
		sum += total;

		/*  n=14; x_n+1=1, x_n=1, y_n=1
			p(x_n=1) = 0.636364
			p(x_n+1=1, x_n=1) = 0.333333
			p(x_n=1, y_n=1) = 0.363636
			p(x_n+1=1, x_n=1, y_n=1) = 0.238095 */
		total = 0.238095 * log10 ( (0.238095 * 0.636364) / (0.363636 * 0.333333));
		cout<<"total for n=14: "<<total<<endl;
		sum += total;

		/*  n=15; x_n+1=1, x_n=1, y_n=1
			   p(x_n=1) = 0.636364
			   p(x_n+1=1, x_n=1) = 0.333333
			   p(x_n=1, y_n=1) = 0.363636
			   p(x_n+1=1, x_n=1, y_n=1) = 0.238095 */
		total = 0.238095 * log10 ( (0.238095 * 0.636364) / (0.363636 * 0.333333));
		cout<<"total for n=15: "<<total<<endl;
		sum += total;

		/*  n=16; x_n+1=0, x_n=1, y_n=0
			  p(x_n=1) = 0.636364
			  p(x_n+1=0, x_n=1) = 0.285714
			  p(x_n=1, y_n=0) = 0.272727
			  p(x_n+1=0, x_n=1, y_n=0) = 0.190476 */
		total = 0.190476 * log10 ( (0.190476 * 0.636364) / (0.272727 * 0.285714));
		cout<<"total for n=16: "<<total<<endl;
		sum += total;

		/*  n=17; x_n+1=1, x_n=0, y_n=1
			 p(x_n=0) = 0.363636
			 p(x_n+1=1, x_n=0) = 0.285714
			 p(x_n=0, y_n=1) = 0.227273
			 p(x_n+1=1, x_n=0, y_n=1) = 0.142857 */
		total = 0.142857 * log10 ( (0.142857 * 0.363636) / (0.227273 * 0.285714));
		cout<<"total for n=17: "<<total<<endl;
		sum += total;

		/*  n=18; x_n+1=1, x_n=1, y_n=1
			   p(x_n=1) = 0.636364
			   p(x_n+1=1, x_n=1) = 0.333333
			   p(x_n=1, y_n=1) = 0.363636
			   p(x_n+1=1, x_n=1, y_n=1) = 0.238095 */
		total = 0.238095 * log10 ( (0.238095 * 0.636364) / (0.363636 * 0.333333));
		cout<<"total for n=18: "<<total<<endl;
		sum += total;

		/*  n=19; x_n+1=0, x_n=1, y_n=0
			  p(x_n=1) = 0.636364
			  p(x_n+1=0, x_n=1) = 0.285714
			  p(x_n=1, y_n=0) = 0.272727
			  p(x_n+1=0, x_n=1, y_n=0) = 0.190476 */
		total = 0.190476 * log10 ( (0.190476 * 0.636364) / (0.272727 * 0.285714));
		cout<<"total for n=19: "<<total<<endl;
		sum += total;

		/*  n=20; x_n+1=1, x_n=0, y_n=0
			p(x_n=0) = 0.363636
			p(x_n+1=1, x_n=0) = 0.285714
			p(x_n=0, y_n=0) = 0.136364
			p(x_n+1=1, x_n=0, y_n=0) = 0.142857 */
		total = 0.142857 * log10 ( (0.142857 * 0.363636) / (0.136364 * 0.285714));
		cout<<"total for n=20: "<<total<<endl;
		sum += total;
		cout<<"SUM 1: "<<sum<<endl<<endl;




		/* Calculate with their encoding of probabilities and their decoding to see if this is the error. */
		sum = 0.0;

		/*  n=0; x_n+1=1, x_n=1, y_n=1
			p(x_n=1) = 0.636364
			p(x_n+1=1, x_n=1) = 0.333333
			p(x_n=1, y_n=1) = 0.363636
			p(x_n+1=1, x_n=1, y_n=1) = 0.238095 */
		total = 0.238095 * log10 ( (0.238095 * 0.636364) / (0.363636 * 0.333333));
		cout<<"total2 for n=0: "<<total<<endl;
		sum += total;

		/*  n=1; x_n+1=1, x_n=1, y_n=0
			p(x_n=1) = 0.636364
			p(x_n+1=1, x_n=1) = 0.333333
			p(x_n=1, y_n=0) = 0.272727
			p(x_n+1=1, x_n=1, y_n=0) = 0.0952381 */
		total = 0.0952381 * log10 ( (0.0952381 * 0.636364) / (0.272727 * 0.333333));
		cout<<"total2 for n=1: "<<total<<endl;
		sum += total;

		/*  n=2; x_n+1=0, x_n=1, y_n=1
			   p(x_n=1) = 0.636364
			   p( x_n=1, x_n+1=0) = 0.285714
			   p(x_n=1, y_n=1) = 0.363636
			   p(x_n=1, x_n+1=0, y_n=1) = 0.142857 */
		total = 0.142857 * log10 ( (0.142857 * 0.636364) / (0.363636 * 0.285714));
		cout<<"total2 for n=2: "<<total<<endl;
		sum += total;

		/*  n=3; x_n+1=1, x_n=0, y_n=0
			p(x_n=0) = 0.363636
			p(x_n+1=1, x_n=0) = 0.285714
			p(x_n=0, y_n=0) = 0.136364
			p(x_n=0, x_n+1=1, y_n=0) = 0.190476 */
		total = 0.190476 * log10 ( (0.190476 * 0.363636) / (0.136364 * 0.285714));
		cout<<"total2 for n=3: "<<total<<endl;
		sum += total;

		/*  n=4; x_n+1=1, x_n=1, y_n=1
			   p(x_n=1) = 0.636364
			   p(x_n+1=1, x_n=1) = 0.333333
			   p(x_n=1, y_n=1) = 0.363636
			   p(x_n+1=1, x_n=1, y_n=1) = 0.238095 */
		total = 0.238095 * log10 ( (0.238095 * 0.636364) / (0.363636 * 0.333333));
		cout<<"total2 for n=4: "<<total<<endl;
		sum += total;

		/*  n=5; x_n+1=0, x_n=1, y_n=0
			  p(x_n=1) = 0.636364
			  p(x_n+1=0, x_n=1) = 0.285714
			  p(x_n=1, y_n=0) = 0.272727
			  p(x_n+1=0, x_n=1, y_n=0) = 0.142857 */
		total = 0.142857 * log10 ( (0.142857 * 0.636364) / (0.272727 * 0.285714));
		cout<<"total2 for n=5: "<<total<<endl;
		sum += total;

		/*  n=6; x_n+1=1, x_n=0, y_n=1
			 p(x_n=0) = 0.363636
			 p(x_n+1=1, x_n=0) = 0.285714
			 p(x_n=0, y_n=1) = 0.227273
			 p(x_n+1=1, x_n=0, y_n=1) = 0.0952381 */
		total = 0.0952381 * log10 ( (0.0952381 * 0.363636) / (0.227273 * 0.285714));
		cout<<"total2 for n=6: "<<total<<endl;
		sum += total;

		/*  n=7; x_n+1=0, x_n=1, y_n=0
			  p(x_n=1) = 0.636364
			  p(x_n+1=0, x_n=1) = 0.285714
			  p(x_n=1, y_n=0) = 0.272727
			  p(x_n+1=0, x_n=1, y_n=0) = 0.142857 */
		total = 0.142857 * log10 ( (0.142857 * 0.636364) / (0.272727 * 0.285714));
		cout<<"total2 for n=7: "<<total<<endl;
		sum += total;

		/*  n=8; x_n+1=0, x_n=0, y_n=1
			 p(x_n=0) = 0.363636
			 p(x_n+1=0, x_n=0) = 0.0952381
			 p(x_n=0, y_n=1) = 0.227273
			 p(x_n+1=0, x_n=0, y_n=1) = 0.0952381 */
		total = 0.0952381 * log10 ( (0.0952381 * 0.363636) / (0.227273 * 0.0952381));
		cout<<"total2 for n=8: "<<total<<endl;
		sum += total;

		/*  n=9; x_n+1=1, x_n=0, y_n=1
			 p(x_n=0) = 0.363636
			 p(x_n+1=1, x_n=0) = 0.285714
			 p(x_n=0, y_n=1) = 0.227273
			 p(x_n+1=1, x_n=0, y_n=1) = 0.0952381 */
		total = 0.0952381 * log10 ( (0.0952381 * 0.363636) / (0.227273 * 0.285714));
		cout<<"total2 for n=9: "<<total<<endl;
		sum += total;

		/*  n=10; x_n+1=1, x_n=1, y_n=0
			 p(x_n=1) = 0.636364
			 p(x_n+1=1, x_n=1) = 0.333333
			 p(x_n=1, y_n=0) = 0.272727
			 p(x_n+1=1, x_n=1, y_n=0) = 0.0952381 */
		total = 0.0952381 * log10 ( (0.0952381 * 0.636364) / (0.272727 * 0.333333));
		cout<<"total2 for n=10: "<<total<<endl;
		sum += total;

		/*  n=11; x_n+1=0, x_n=1, y_n=1
			   p(x_n=1) = 0.636364
			   p(x_n+1=0, x_n=1) = 0.285714
			   p(x_n=1, y_n=1) = 0.363636
			   p(x_n+1=0, x_n=1, y_n=1) = 0.142857 */
		total = 0.142857 * log10 ( (0.142857 * 0.636364) / (0.363636 * 0.285714));
		cout<<"total2 for n=11: "<<total<<endl;
		sum += total;

		/*  n=12; x_n+1=0, x_n=0, y_n=1
			 p(x_n=0) = 0.363636
			 p(x_n+1=0, x_n=0) = 0.0952381
			 p(x_n=0, y_n=1) = 0.227273
			 p(x_n+1=0, x_n=0, y_n=1) = 0.0952381 */
		total = 0.0952381 * log10 ( (0.0952381 * 0.363636) / (0.227273 * 0.0952381));
		cout<<"total for n=12: "<<total<<endl;
		sum += total;

		/*  n=13; x_n+1=1, x_n=0, y_n=0
			p(x_n=0) = 0.363636
			p(x_n+1=1, x_n=0) = 0.285714
			p(x_n=0, y_n=0) = 0.136364
			p(x_n+1=1, x_n=0, y_n=0) = 0.190476 */
		total = 0.190476  * log10 ( (0.190476 * 0.363636) / (0.136364 * 0.285714));
		cout<<"total2 for n=13: "<<total<<endl;
		sum += total;

		/*  n=14; x_n+1=1, x_n=1, y_n=1
			p(x_n=1) = 0.636364
			p(x_n+1=1, x_n=1) = 0.333333
			p(x_n=1, y_n=1) = 0.363636
			p(x_n+1=1, x_n=1, y_n=1) = 0.238095 */
		total = 0.238095 * log10 ( (0.238095 * 0.636364) / (0.363636 * 0.333333));
		cout<<"total2 for n=14: "<<total<<endl;
		sum += total;

		/*  n=15; x_n+1=1, x_n=1, y_n=1
			   p(x_n=1) = 0.636364
			   p(x_n+1=1, x_n=1) = 0.333333
			   p(x_n=1, y_n=1) = 0.363636
			   p(x_n+1=1, x_n=1, y_n=1) = 0.238095 */
		total = 0.238095 * log10 ( (0.238095 * 0.636364) / (0.363636 * 0.333333));
		cout<<"total2 for n=15: "<<total<<endl;
		sum += total;

		/*  n=16; x_n+1=0, x_n=1, y_n=0
			  p(x_n=1) = 0.636364
			  p(x_n+1=0, x_n=1) = 0.285714
			  p(x_n=1, y_n=0) = 0.272727
			  p(x_n+1=0, x_n=1, y_n=0) = 0.142857 */
		total = 0.142857 * log10 ( (0.142857 * 0.636364) / (0.272727 * 0.285714));
		cout<<"total2 for n=16: "<<total<<endl;
		sum += total;

		/*  n=17; x_n+1=1, x_n=0, y_n=1
			 p(x_n=0) = 0.363636
			 p(x_n+1=1, x_n=0) = 0.285714
			 p(x_n=0, y_n=1) = 0.227273
			 p(x_n+1=1, x_n=0, y_n=1) = 0.0952381 */
		total = 0.0952381 * log10 ( (0.0952381 * 0.363636) / (0.227273 * 0.285714));
		cout<<"total2 for n=17: "<<total<<endl;
		sum += total;

		/*  n=18; x_n+1=1, x_n=1, y_n=1
			   p(x_n=1) = 0.636364
			   p(x_n+1=1, x_n=1) = 0.333333
			   p(x_n=1, y_n=1) = 0.363636
			   p(x_n+1=1, x_n=1, y_n=1) = 0.238095 */
		total = 0.238095 * log10 ( (0.238095 * 0.636364) / (0.363636 * 0.333333));
		cout<<"total2 for n=18: "<<total<<endl;
		sum += total;

		/*  n=19; x_n+1=0, x_n=1, y_n=0
			  p(x_n=1) = 0.636364
			  p(x_n+1=0, x_n=1) = 0.285714
			  p(x_n=1, y_n=0) = 0.272727
			  p(x_n+1=0, x_n=1, y_n=0) = 0.142857 */
		total = 0.142857 * log10 ( (0.142857 * 0.636364) / (0.272727 * 0.285714));
		cout<<"total2 for n=19: "<<total<<endl;
		sum += total;

		/*  n=20; x_n+1=1, x_n=0, y_n=0
			p(x_n=0) = 0.363636
			p(x_n+1=1, x_n=0) = 0.285714
			p(x_n=0, y_n=0) = 0.136364
			p(x_n+1=1, x_n=0, y_n=0) = 0.190476 */
		total = 0.190476 * log10 ( (0.190476 * 0.363636) / (0.136364 * 0.285714));
		cout<<"total2 for n=20: "<<total<<endl;
		sum += total;
		cout<<"SUM 2: "<<sum<<endl<<endl;



		double xyRes = testCalculator.getTransferEntropy(0, testIData, testJData);
		//double yxRes = testCalculator.getTransferEntropy(0, testJData, testIData);
		cout<<"RESULT X->Y: "<<xyRes<<endl;
		//cout<<"RESULT Y->X: "<<yxRes<<endl;


		//Invoke the method and check result
		//QCOMPARE(testCalculator.getTransferEntropy(0, testIData, testJData), 3.0/7.0);

		//Should produce zero if run in other direction
		//QCOMPARE(testCalculator.getTransferEntropy(0, testJData, testIData), 0.0);
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


void TestTransferEntropyCalculator::testZafeiriosCode(){
	try{

		//Create iVector
		QVector<unsigned> testIData;
		testIData.push_back(1);
		testIData.push_back(1);
		testIData.push_back(1);
		testIData.push_back(0);
		testIData.push_back(1);

		testIData.push_back(1);
		testIData.push_back(0);
		testIData.push_back(1);
		testIData.push_back(0);
		testIData.push_back(0);

		testIData.push_back(1);
		testIData.push_back(1);
		testIData.push_back(0);
		testIData.push_back(0);
		testIData.push_back(1);

		testIData.push_back(1);
		testIData.push_back(1);
		testIData.push_back(0);
		testIData.push_back(1);
		testIData.push_back(1);

		testIData.push_back(0);
		testIData.push_back(1);

		//Create jVector
		QVector<unsigned> testJData;
		testJData.push_back(1);
		testJData.push_back(0);
		testJData.push_back(1);
		testJData.push_back(0);
		testJData.push_back(1);

		testJData.push_back(0);
		testJData.push_back(1);
		testJData.push_back(0);
		testJData.push_back(1);
		testJData.push_back(1);

		testJData.push_back(0);
		testJData.push_back(1);
		testJData.push_back(1);
		testJData.push_back(0);
		testJData.push_back(1);

		testJData.push_back(1);
		testJData.push_back(0);
		testJData.push_back(1);
		testJData.push_back(1);
		testJData.push_back(0);

		testJData.push_back(0);
		testJData.push_back(1);

		//Test method
		TransferEntropyCalc calc(1,1);
		double tranEnt = calc.getTrEn(testIData, testJData);
		cout<<"Zafeirios result: "<<tranEnt<<endl;

	}
	catch(...){
		QFAIL("An unknown exception occurred");
	}
}




