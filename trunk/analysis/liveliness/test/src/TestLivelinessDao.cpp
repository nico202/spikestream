#include "TestLivelinessDao.h"
#include "LivelinessDao.h"
#include "SpikeStreamException.h"
using namespace spikestream;


void TestLivelinessDao::testAddCluster(){
	//Add test analysis
	addTestAnalysis1();

	//Create class to be tested and test it
	LivelinessDao livelinessDao(analysisDBInfo);
	QList<unsigned int> neuronIDList;
	neuronIDList.append(23);
	neuronIDList.append(27);
	neuronIDList.append(35);
	livelinessDao.addCluster(testAnalysis1ID, 7, neuronIDList, 3.45);

	//Check complex was added correctly.
	QSqlQuery query = getAnalysisQuery("SELECT ClusterID, AnalysisID, TimeStep, Liveliness, Neurons FROM ClusterLiveliness");
	executeQuery(query);
	query.next();
	QCOMPARE(query.size(), (int)1);
	QVERIFY(query.value(0).toUInt() > (unsigned int)0);
	QCOMPARE(query.value(1).toUInt(), (unsigned int)testAnalysis1ID);
	QCOMPARE(query.value(2).toUInt(), (unsigned int)7);
	QCOMPARE(query.value(3).toDouble(), 3.45);
	QCOMPARE(query.value(4).toString(), QString("23,27,35"));
}


void TestLivelinessDao::testContainsAnalysisData(){
	//Add test analysis and data
	addTestAnalysis1();
	addTestAnalysis1Data();

	//Create class to be tested and test it
	LivelinessDao livelinessDao(analysisDBInfo);
	QVERIFY( livelinessDao.containsAnalysisData(testAnalysis1ID, 1, 2) );
	QVERIFY( !livelinessDao.containsAnalysisData(testAnalysis1ID, 3, 4) );
}


void TestLivelinessDao::testDeleteTimeSteps(){
	//Add test analysis and data
	addTestAnalysis1();
	addTestAnalysis1Data();

	//Create class to be tested
	LivelinessDao livelinessDao(analysisDBInfo);

	//Check that there are three time steps in database
	QSqlQuery query = getAnalysisQuery("SELECT COUNT(*) FROM ClusterLiveliness");
	executeQuery(query);
	query.next();
	QCOMPARE(query.value(0).toInt(), (int)3);

	//Delete 2 time steps
	livelinessDao.deleteTimeSteps(testAnalysis1ID, 1, 1);

	//Should be 1 cluster remaining
	query = getAnalysisQuery("SELECT COUNT(*) FROM ClusterLiveliness");
	executeQuery(query);
	query.next();
	QCOMPARE(query.value(0).toInt(), (int)1);

	//Should be 3 neuron entries remaining
	query = getAnalysisQuery("SELECT COUNT(*) FROM NeuronLiveliness");
	executeQuery(query);
	query.next();
	QCOMPARE(query.value(0).toInt(), (int)3);
}


void TestLivelinessDao::testGetClusters(){
	//Add test analysis and data
	addTestAnalysis1();
	addTestAnalysis1Data();

	try{
		//Invoke test method
		LivelinessDao livelinessDao(analysisDBInfo);
		QList<Cluster> clstrList = livelinessDao.getClusters(testAnalysis1ID);

		//Check that cluster information is correct
		QCOMPARE(clstrList.size(), 3);
		QCOMPARE(clstrList.at(0).getLiveliness(), 3.0);
		QCOMPARE(clstrList.at(2).getLiveliness(), 5.0);
		QCOMPARE(clstrList.at(2).getTimeStep(), (unsigned int)2);

		//Spot check some neuron ids
		QList<unsigned int> neuronIDList = clstrList[2].getNeuronIDs();
		QCOMPARE(neuronIDList.size(), 3);
		QCOMPARE(neuronIDList.at(0), testNeurIDList[2]);
		QCOMPARE(neuronIDList.at(1), testNeurIDList[3]);
		QCOMPARE(neuronIDList.at(2), testNeurIDList[4]);

		//Spot check neuron liveliness
		QHash<unsigned int, double> neurLivelinessMap = clstrList[1].getNeuronLivelinessMap();
		QCOMPARE(neurLivelinessMap[ testNeurIDList[0] ], 1.1);
		QCOMPARE(neurLivelinessMap[ testNeurIDList[1] ], 1.2);
		QCOMPARE(neurLivelinessMap[ testNeurIDList[2] ], 1.3);
	}
	catch(SpikeStreamException& ex){
		QFAIL(ex.getMessage().toAscii());
	}
}


void TestLivelinessDao::testGetNeuronLiveliness(){
	//Add test analysis and data
	addTestAnalysis1();
	addTestAnalysis1Data();

	//Invoke test method
	LivelinessDao livelinessDao(analysisDBInfo);
	QCOMPARE(livelinessDao.getNeuronLiveliness(testAnalysis1ID, 1, testNeurIDList[2]), 1.3);
	QCOMPARE(livelinessDao.getNeuronLiveliness(testAnalysis1ID, 2, testNeurIDList[2]), 1.6);
	QCOMPARE(livelinessDao.getNeuronLiveliness(testAnalysis1ID, 2, testNeurIDList[3]), 1.7);
	QCOMPARE(livelinessDao.getNeuronLiveliness(testAnalysis1ID, 2, testNeurIDList[4]), 1.8);
}


void TestLivelinessDao::testGetMaxNeuronLiveliness(){
	//Add test analysis and data
	addTestAnalysis1();
	addTestAnalysis1Data();

	//Invoke test method
	LivelinessDao livelinessDao(analysisDBInfo);
	QCOMPARE(livelinessDao.getMaxNeuronLiveliness(testAnalysis1ID), 1.8);
}


void TestLivelinessDao::testSetNeuronLiveliness(){
	//Add test analysis and data
	addTestAnalysis1();
	addTestAnalysis1Data();

	//Invoke test method
	LivelinessDao livelinessDao(analysisDBInfo);
	QCOMPARE(livelinessDao.getNeuronLiveliness(testAnalysis1ID, 1, testNeurIDList[2]), 1.3);
	livelinessDao.setNeuronLiveliness(testAnalysis1ID, 1, testNeurIDList[2], 7.22);
	QString queryStr = "SELECT Liveliness FROM NeuronLiveliness WHERE AnalysisID=" + QString::number(testAnalysis1ID);
	queryStr += " AND TimeStep=1 AND NeuronID=" + QString::number(testNeurIDList[2]);
	QSqlQuery query = getAnalysisQuery(queryStr);
	executeQuery(query);
	query.next();
	QCOMPARE(query.value(0).toDouble(), 7.22);
}


/*! Adds test data to analysis 1 */
void TestLivelinessDao::addTestAnalysis1Data(){
	//Create strings with actual neuron ids
	QString cluster1Str = QString::number(testNeurIDList[0]) + "," + QString::number(testNeurIDList[1]) + "," + QString::number(testNeurIDList[2]) + ",";
	cluster1Str += QString::number(testNeurIDList[3]) + "," + QString::number(testNeurIDList[4]);
	QString cluster2Str = QString::number(testNeurIDList[0]) + "," + QString::number(testNeurIDList[1]) + "," + QString::number(testNeurIDList[2]);
	QString cluster3Str = QString::number(testNeurIDList[2]) + "," + QString::number(testNeurIDList[3]) + "," + QString::number(testNeurIDList[4]);

	//Add clusters to database
	executeAnalysisQuery("INSERT INTO ClusterLiveliness (AnalysisID, TimeStep, Liveliness, Neurons) VALUES (" + QString::number(testAnalysis1ID) + ", 1, 3.0, '" + cluster1Str + "')");
	executeAnalysisQuery("INSERT INTO ClusterLiveliness (AnalysisID, TimeStep, Liveliness, Neurons) VALUES (" + QString::number(testAnalysis1ID) + ", 1, 4.0, '" + cluster2Str + "')");
	executeAnalysisQuery("INSERT INTO ClusterLiveliness (AnalysisID, TimeStep, Liveliness, Neurons) VALUES (" + QString::number(testAnalysis1ID) + ", 2, 5.0, '" + cluster3Str + "')");

	//Add neuron liveliness for time step 1
	executeAnalysisQuery("INSERT INTO NeuronLiveliness (AnalysisID, TimeStep, NeuronID, Liveliness) VALUES (" + QString::number(testAnalysis1ID) + ", 1, " +  QString::number(testNeurIDList[0]) + ", 1.1)");
	executeAnalysisQuery("INSERT INTO NeuronLiveliness (AnalysisID, TimeStep, NeuronID, Liveliness) VALUES (" + QString::number(testAnalysis1ID) + ", 1, " +  QString::number(testNeurIDList[1]) + ", 1.2)");
	executeAnalysisQuery("INSERT INTO NeuronLiveliness (AnalysisID, TimeStep, NeuronID, Liveliness) VALUES (" + QString::number(testAnalysis1ID) + ", 1, " +  QString::number(testNeurIDList[2]) + ", 1.3)");
	executeAnalysisQuery("INSERT INTO NeuronLiveliness (AnalysisID, TimeStep, NeuronID, Liveliness) VALUES (" + QString::number(testAnalysis1ID) + ", 1, " +  QString::number(testNeurIDList[3]) + ", 1.4)");
	executeAnalysisQuery("INSERT INTO NeuronLiveliness (AnalysisID, TimeStep, NeuronID, Liveliness) VALUES (" + QString::number(testAnalysis1ID) + ", 1, " +  QString::number(testNeurIDList[4]) + ", 1.5)");

	//Add neuron liveliness for time step 2
	executeAnalysisQuery("INSERT INTO NeuronLiveliness (AnalysisID, TimeStep, NeuronID, Liveliness) VALUES (" + QString::number(testAnalysis1ID) + ", 2, " +  QString::number(testNeurIDList[2]) + ", 1.6)");
	executeAnalysisQuery("INSERT INTO NeuronLiveliness (AnalysisID, TimeStep, NeuronID, Liveliness) VALUES (" + QString::number(testAnalysis1ID) + ", 2, " +  QString::number(testNeurIDList[3]) + ", 1.7)");
	executeAnalysisQuery("INSERT INTO NeuronLiveliness (AnalysisID, TimeStep, NeuronID, Liveliness) VALUES (" + QString::number(testAnalysis1ID) + ", 2, " +  QString::number(testNeurIDList[4]) + ", 1.8)");
}


