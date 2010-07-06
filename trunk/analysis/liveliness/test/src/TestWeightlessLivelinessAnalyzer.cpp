#include "LivelinessDaoDuck.h"
#include "TestWeightlessLivelinessAnalyzer.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;


void TestWeightlessLivelinessAnalyzer::testCalculateConnectionLiveliness(){
	/* Create four weightless neurons n1, n2 and n3 are connected to n4*/
	QHash<unsigned int, QList<unsigned int> > conMap;
	QHash<unsigned int, WeightlessNeuron*> weightlessNeuronMap;
	weightlessNeuronMap[1] = new WeightlessNeuron(conMap, 1);
	weightlessNeuronMap[2]  = new WeightlessNeuron(conMap, 2);
	weightlessNeuronMap[3] = new WeightlessNeuron(conMap, 3);
	conMap[1].append(0);
	conMap[2].append(1);
	conMap[3].append(2);
	weightlessNeuronMap[4]  = new WeightlessNeuron(conMap, 4);
	Util::setGeneralization(weightlessNeuronMap, 1.0);

	//Train weightless neuron 4 with AND function
	Util::addTraining(*weightlessNeuronMap[4], "000", 0);
	Util::addTraining(*weightlessNeuronMap[4], "001", 0);
	Util::addTraining(*weightlessNeuronMap[4], "010", 0);
	Util::addTraining(*weightlessNeuronMap[4], "100", 0);
	Util::addTraining(*weightlessNeuronMap[4], "011", 0);
	Util::addTraining(*weightlessNeuronMap[4], "101", 0);
	Util::addTraining(*weightlessNeuronMap[4], "110", 0);
	Util::addTraining(*weightlessNeuronMap[4], "111", 1);

	//Test liveliness with no neurons firing
	try	{
		WeightlessLivelinessAnalyzer weiLivAna;
		weiLivAna.setAnalysisInfo(getAnalysisInfo());
		weiLivAna.setWeightlessNeuronMap(weightlessNeuronMap);
		weiLivAna.calculateConnectionLiveliness();
		QHash<unsigned int, QHash<unsigned int, double> > conLiveMap = weiLivAna.getFromConnectionLivelinessMap();
		QHash<unsigned int, double> neurLiveMap = weiLivAna.getNeuronLivelinessMap();
		QVERIFY(conLiveMap.contains(1) && conLiveMap[1].contains(4));
		QVERIFY(conLiveMap.contains(2) && conLiveMap[2].contains(4));
		QVERIFY(conLiveMap.contains(3) && conLiveMap[3].contains(4));
		QCOMPARE(conLiveMap[1][4], 0.0);
		QCOMPARE(conLiveMap[2][4], 0.0);
		QCOMPARE(conLiveMap[3][4], 0.0);
		QCOMPARE(neurLiveMap.size(), (int)4);
		QVERIFY(neurLiveMap.contains(4));
		QCOMPARE(neurLiveMap[4], 0.0);

		//Test liveliness with all neurons firing - all connections should be lively
		QHash<unsigned int, bool> firingNeuronMap;
		firingNeuronMap[1] = true;
		firingNeuronMap[2] = true;
		firingNeuronMap[3] = true;
		weiLivAna.setFiringNeuronMap(firingNeuronMap);
		weiLivAna.calculateConnectionLiveliness();
		conLiveMap = weiLivAna.getFromConnectionLivelinessMap();
		neurLiveMap = weiLivAna.getNeuronLivelinessMap();
		QVERIFY(conLiveMap.contains(1) && conLiveMap[1].contains(4));
		QVERIFY(conLiveMap.contains(2) && conLiveMap[2].contains(4));
		QVERIFY(conLiveMap.contains(3) && conLiveMap[3].contains(4));
		QCOMPARE(conLiveMap[1][4], 1.0);
		QCOMPARE(conLiveMap[2][4], 1.0);
		QCOMPARE(conLiveMap[3][4], 1.0);
		QCOMPARE(neurLiveMap.size(), (int)4);
		QVERIFY(neurLiveMap.contains(4));
		QCOMPARE(neurLiveMap[4], 3.0);

		//Test liveliness with 1 and 2 neurons firing - only 3 should be lively
		firingNeuronMap.clear();;
		firingNeuronMap[1] = true;
		firingNeuronMap[2] = true;
		weiLivAna.setFiringNeuronMap(firingNeuronMap);
		weiLivAna.calculateConnectionLiveliness();
		conLiveMap = weiLivAna.getFromConnectionLivelinessMap();
		neurLiveMap = weiLivAna.getNeuronLivelinessMap();
		QVERIFY(conLiveMap.contains(1) && conLiveMap[1].contains(4));
		QVERIFY(conLiveMap.contains(2) && conLiveMap[2].contains(4));
		QVERIFY(conLiveMap.contains(3) && conLiveMap[3].contains(4));
		QCOMPARE(conLiveMap[1][4], 0.0);
		QCOMPARE(conLiveMap[2][4], 0.0);
		QCOMPARE(conLiveMap[3][4], 1.0);
		QCOMPARE(neurLiveMap.size(), (int)4);
		QVERIFY(neurLiveMap.contains(4));
		QCOMPARE(neurLiveMap[4], 1.0);

		//Train weightless neuron 4 with XOR function
		weightlessNeuronMap[4]->resetTraining();
		Util::addTraining(*weightlessNeuronMap[4], "000", 0);
		Util::addTraining(*weightlessNeuronMap[4], "001", 1);
		Util::addTraining(*weightlessNeuronMap[4], "010", 1);
		Util::addTraining(*weightlessNeuronMap[4], "100", 1);
		Util::addTraining(*weightlessNeuronMap[4], "011", 0);
		Util::addTraining(*weightlessNeuronMap[4], "101", 0);
		Util::addTraining(*weightlessNeuronMap[4], "110", 0);
		Util::addTraining(*weightlessNeuronMap[4], "111", 1);

		//Test liveliness with no neurons firing - all 3 should be lively
		firingNeuronMap.clear();;
		weiLivAna.setFiringNeuronMap(firingNeuronMap);
		weiLivAna.calculateConnectionLiveliness();
		conLiveMap = weiLivAna.getFromConnectionLivelinessMap();
		neurLiveMap = weiLivAna.getNeuronLivelinessMap();
		QVERIFY(conLiveMap.contains(1) && conLiveMap[1].contains(4));
		QVERIFY(conLiveMap.contains(2) && conLiveMap[2].contains(4));
		QVERIFY(conLiveMap.contains(3) && conLiveMap[3].contains(4));
		QCOMPARE(conLiveMap[1][4], 1.0);
		QCOMPARE(conLiveMap[2][4], 1.0);
		QCOMPARE(conLiveMap[3][4], 1.0);
		QCOMPARE(neurLiveMap.size(), (int)4);
		QVERIFY(neurLiveMap.contains(4));
		QCOMPARE(neurLiveMap[4], 3.0);
	}
	catch(SpikeStreamException& ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("Unknown exception");
	}
}


void TestWeightlessLivelinessAnalyzer::testFillInputArray(){
	/* Create weightless neuron with 17 neurons connecting to it. */
	QHash<unsigned int, QList<unsigned int> > conMap;
	for(int i=0; i<17; ++i){
		conMap[i+1].append(i);
	}
	WeightlessNeuron* weiNeur = new WeightlessNeuron(conMap, 23);

	//Set up analyzer. Neurons 2,3,4 and 15 are firing
	WeightlessLivelinessAnalyzer weiLivAna;
	QHash<unsigned int, bool> firingNeuronMap;
	firingNeuronMap[2] = true;
	firingNeuronMap[3] = true;
	firingNeuronMap[4] = true;
	firingNeuronMap[15] = true;
	weiLivAna.setFiringNeuronMap(firingNeuronMap);

	//Invoke test method
	try {
		byte* inPatArr;
		int inPatArrLen;
		weiLivAna.fillInputArray(weiNeur, inPatArr, inPatArrLen);

		//Check that array has been filled correctly
		QCOMPARE(inPatArrLen, (int)3);
		QVERIFY(inPatArr[1/8] & 1<<(1%8));//Neuron 2 with index 1 should be firing
		QVERIFY(inPatArr[2/8] & 1<<(2%8));//Neuron 3 with index 2 should be firing
		QVERIFY(inPatArr[3/8] & 1<<(3%8));//Neuron 4 with index 3 should be firing
		QVERIFY( !( inPatArr[4/8] & 1<<(4%8) ) );//Neuron 5 with index 4 should not be firing
		QVERIFY(inPatArr[14/8] & 1<<(14%8));//Neuron 15 with index 14 should be firing
	}
	catch(SpikeStreamException& ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("Unknown exception");
	}

	//Clean up
	delete weiNeur;
}


void TestWeightlessLivelinessAnalyzer::testFlipBits(){
	//Build array with 24 bits
	byte inPatArr[3];
	for(int i=0; i<3; ++i)
		inPatArr[i] = 0;

	//List of indexes to flip
	QList<unsigned int> flipIndxList;
	flipIndxList.append(1);
	flipIndxList.append(5);
	flipIndxList.append(22);

	//Test the method
	WeightlessLivelinessAnalyzer weiLivAna;
	weiLivAna.flipBits(inPatArr, 3, flipIndxList);
	QVERIFY(inPatArr[1/8] & 1<<(1%8));//Index 1 should be set to 1
	QVERIFY( !( inPatArr[3/8] & 1<<(3%8) ) );//Index 3 should be set to 0
	QVERIFY(inPatArr[5/8] & 1<<(5%8));//Index 5 should be set to 1
	QVERIFY(inPatArr[22/8] & 1<<(22%8));//Index 22 should be set to 1
}


void TestWeightlessLivelinessAnalyzer::testIdentifyClusters(){
	/* Create weightless neuron analyzer with five weightless neurons.
		Connections within weightless neurons don't matter for this part of the analysis.  */
	QHash<unsigned int, QList<unsigned int> > conMap;
	QHash<unsigned int, WeightlessNeuron*> weiNeurMap;
	QHash<unsigned int, double> neurLivMap;
	for(int i=0; i<5; ++i){
		weiNeurMap[i+1] = new WeightlessNeuron(conMap, i+1);
	}
	WeightlessLivelinessAnalyzer weiLivAna;
	weiLivAna.setWeightlessNeuronMap(weiNeurMap);

	//Inject duck dao so we can access results
	LivelinessDaoDuck* daoDuck = new LivelinessDaoDuck();
	weiLivAna.setLivelinessDao(daoDuck);

	try{
		// Build lively connections 1->2, 3->4, 4->5
		QHash<unsigned int, QHash<unsigned int, double> > fromConLivMap;
		QHash<unsigned int, QHash<unsigned int, double> > toConLivMap;
		fromConLivMap[1][2] = 1.0;
		fromConLivMap[3][4] = 1.0;
		fromConLivMap[4][5] = 1.0;
		toConLivMap[1][2] = 1.0;
		toConLivMap[3][4] = 1.0;
		toConLivMap[4][5] = 1.0;
		neurLivMap[1] = 0.0;
		neurLivMap[2] = 1.0;
		neurLivMap[3] = 0.0;
		neurLivMap[4] = 1.0;
		neurLivMap[5] = 1.0;
		weiLivAna.setFromConnectionLivelinessMap(fromConLivMap);
		weiLivAna.setToConnectionLivelinessMap(toConLivMap);
		weiLivAna.setNeuronLivelinessMap(neurLivMap);

		//Run analysis and check results
		weiLivAna.identifyClusters();
		checkClusters(daoDuck->getClusterList(), "1,2;3,4,5", "0.25;0.444");

		/* Build lively connections 1<->2, 2<->3, 3<->1, 4<->5
			Connections are symmetrical so only need to use one map */
		fromConLivMap.clear();
		neurLivMap.clear();
		fromConLivMap[1][2] = 1.0;
		fromConLivMap[2][1] = 1.0;
		fromConLivMap[2][3] = 1.0;
		fromConLivMap[3][2] = 1.0;
		fromConLivMap[1][3] = 1.0;
		fromConLivMap[3][1] = 1.0;
		fromConLivMap[4][5] = 1.0;
		fromConLivMap[5][4] = 1.0;
		neurLivMap[1] = 2.0;
		neurLivMap[2] = 2.0;
		neurLivMap[3] = 2.0;
		neurLivMap[4] = 1.0;
		neurLivMap[5] = 1.0;
		weiLivAna.setFromConnectionLivelinessMap(fromConLivMap);
		weiLivAna.setToConnectionLivelinessMap(fromConLivMap);
		weiLivAna.setNeuronLivelinessMap(neurLivMap);

		//Run analysis and check results
		daoDuck->reset();
		weiLivAna.identifyClusters();
		checkClusters(daoDuck->getClusterList(), "1,2,3;4,5", "4.0;1.0");

		// Build lively connections 1->2, 3->4
		fromConLivMap.clear();
		toConLivMap.clear();
		neurLivMap.clear();
		fromConLivMap[1][2] = 1.0;
		fromConLivMap[3][4] = 1.0;
		toConLivMap[2][1] = 1.0;
		toConLivMap[4][3] = 1.0;
		neurLivMap[1] = 0.0;
		neurLivMap[2] = 1.0;
		neurLivMap[3] = 0.0;
		neurLivMap[4] = 1.0;
		neurLivMap[5] = 0.0;
		weiLivAna.setFromConnectionLivelinessMap(fromConLivMap);
		weiLivAna.setToConnectionLivelinessMap(toConLivMap);
		weiLivAna.setNeuronLivelinessMap(neurLivMap);

		//Run analysis and check results
		daoDuck->reset();
		weiLivAna.identifyClusters();
		checkClusters(daoDuck->getClusterList(), "1,2;3,4", "0.25;0.25");

		// Build lively connections 2->1, 2->3
		fromConLivMap.clear();
		toConLivMap.clear();
		neurLivMap.clear();
		fromConLivMap[2][1] = 1.0;
		fromConLivMap[2][3] = 1.0;
		toConLivMap[1][2] = 1.0;
		toConLivMap[3][2] = 1.0;
		neurLivMap[1] = 1.0;
		neurLivMap[2] = 0.0;
		neurLivMap[3] = 1.0;
		neurLivMap[4] = 0.0;
		neurLivMap[5] = 0.0;
		weiLivAna.setFromConnectionLivelinessMap(fromConLivMap);
		weiLivAna.setToConnectionLivelinessMap(toConLivMap);
		weiLivAna.setNeuronLivelinessMap(neurLivMap);

		//Run analysis and check results
		daoDuck->reset();
		weiLivAna.identifyClusters();
		checkClusters(daoDuck->getClusterList(), "1,2,3", "0.444");

	}
	catch(SpikeStreamException& ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("Unknown exception");
	}

}


/*-------------------------------------------------------------*/
/*-------                PRIVATE METHODS                 ------*/
/*-------------------------------------------------------------*/

/*! Checks that the clusters in the string match the clusters in the list.
	Neuron ids are separated by commas; clusters are separated by semi colons */
void TestWeightlessLivelinessAnalyzer::checkClusters(QList<Cluster>& clusterList, QString clusterStr, QString clusterLivelinessStr){
	QStringList clusterStrList = clusterStr.split(";", QString::SkipEmptyParts);
	QStringList clusterLivelinessStrList = clusterLivelinessStr.split(";", QString::SkipEmptyParts);

	//Check lists are the same size
	if(clusterList.size() != clusterStrList.size() || clusterList.size() != clusterLivelinessStrList.size()){
		QFAIL("Expected cluster list size does not match actual cluster list size");
		return;
	}

	//Check that each cluster specified in the string can be found in the list
	for(int i=0; i<clusterStrList.size(); ++i){
		QString clstrStr = clusterStrList.at(i);
		double clstrLiveliness = Util::getDouble(clusterLivelinessStrList.at(i));

		//Look for a cluster that has this neuron id string
		bool clusterFound = false;
		foreach(Cluster clstr, clusterList){
			//qDebug()<<"COMPARING "<<clstrStr<<" WITH "<<clstr.getNeuronIDString();
			if(clstr.getNeuronIDString() == clstrStr){
				QCOMPARE( Util::rDouble(clstr.getLiveliness(), 3), clstrLiveliness );
				clusterFound = true;
				break;
			}
		}
		if(!clusterFound){
			qDebug()<<"Cluster string: '"<<clstrStr<<"'";
			QFAIL("Cluster in cluster string cannot be found in cluster list");
			return;
		}
	}
}

/*! Builds an analysis info with the appropriate parameters */
AnalysisInfo TestWeightlessLivelinessAnalyzer::getAnalysisInfo(){
	AnalysisInfo info;

	//Set parameters
	info.getParameterMap()["generalization"] = 1.0;
	info.getParameterMap()["store_connection_liveliness_as_temporary_weights"] = 0.0;

	//Return
	return info;
}






