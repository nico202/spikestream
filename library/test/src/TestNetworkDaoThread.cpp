//SpikeStream includes
#include "TestNetworkDaoThread.h"
#include "NetworkDao.h"
#include "NetworkDaoThread.h"
#include "SpikeStreamException.h"
#include "NetworkInfo.h"
#include "Neuron.h"
using namespace spikestream;

#include <iostream>
using namespace std;


/*----------------------------------------------------------*/
/*-----                     TESTS                      -----*/
/*----------------------------------------------------------*/

void TestNetworkDaoThread::testAddConnectionGroup(){
    try{
		/* Add network - slightly sloppy to use the network dao method, but it has been tested elsewhere
	    and we need a network because of foreign key constraints */
		NetworkInfo netInfo(0, "test3Name", "test3Description");
		NetworkDao networkDao (dbInfo);
		networkDao.addNetwork(netInfo);

		/* Add neuron groups - again, slightly sloppy, but it has been tested elsewhere
			and we need neuron groups because of foreign key constraints */
		//Add from neuron group
		NeuronGroup fromGrp( NeuronGroupInfo(0, "fromNeuronGroup1Name", "fromNeuronGroup1Desc", QHash<QString, double>(), networkDao.getNeuronType(1)));
		NeuronMap* neurMap = new NeuronMap();
		(*neurMap)[1] = new Neuron(0, 0, 0);
		(*neurMap)[2] = new Neuron(0, 1, 0);
		(*neurMap)[3] = new Neuron(1, 0, 0);
		(*neurMap)[4] = new Neuron(1, 1, 0);
		fromGrp.setNeuronMap(neurMap);
		NetworkDaoThread netDaoThread(dbInfo);
		netDaoThread.prepareAddNeuronGroup(netInfo.getID(), &fromGrp);
		runThread(netDaoThread);

		//Add to neuron group
		NeuronGroup toGrp( NeuronGroupInfo(0, "toNeuronGroup1Name", "toNeuronGroup1Desc", QHash<QString, double>(), networkDao.getNeuronType(1)) );
		neurMap = new NeuronMap();
		(*neurMap)[1] = new Neuron(0, 0, 10);
		(*neurMap)[2] = new Neuron(0, 3, 10);
		(*neurMap)[3] = new Neuron(2, 0, 10);
		(*neurMap)[4] = new Neuron(1, 7, 10);
		toGrp.setNeuronMap(neurMap);
		netDaoThread.prepareAddNeuronGroup(netInfo.getID(), &toGrp);
		runThread(netDaoThread);

		//Get lists of the from and to neuron ids
		QList<unsigned int> fromNeuronIDs = fromGrp.getNeuronIDs();
		QList<unsigned int> toNeuronIDs = toGrp.getNeuronIDs();
		QCOMPARE(fromNeuronIDs.size(), 4);
		QCOMPARE(toNeuronIDs.size(), 4);

		//Build the connection group that is to be added
		QHash<QString, double> paramMap;
		paramMap["param3"] = 0.7;
		paramMap["param4"] = 0.8;
		ConnectionGroupInfo connGrpInfo(0, "testConnGroup1Desc", fromGrp.getID(), toGrp.getID(),  paramMap, networkDao.getSynapseType(1));
		ConnectionGroup connGrp(connGrpInfo);

		//Add connections
		for(int i=0; i<4; ++i){
			connGrp.addConnection(fromNeuronIDs[i], toNeuronIDs[i],  30,  0.1);
		}

		//Add the connection group to the database
		netDaoThread.prepareAddConnectionGroup(netInfo.getID(), &connGrp);
		runThread(netDaoThread);

		//Check that the connection group was added correctly
		QSqlQuery query = getQuery("SELECT NetworkID, Description, FromNeuronGroupID, ToNeuronGroupID, Parameters, SynapseTypeID FROM ConnectionGroups");
		executeQuery(query);
		QCOMPARE(1, query.size());
		query.next();
		QCOMPARE(query.value(0).toString(), QString::number(netInfo.getID()));//Check network id
		QCOMPARE(query.value(1).toString(), connGrpInfo.getDescription());//Check description
		QCOMPARE(query.value(2).toString(), QString::number(fromGrp.getID()));//Check from neuron group id
		QCOMPARE(query.value(3).toString(), QString::number(toGrp.getID()));//Check to neuron group id
		QCOMPARE(query.value(4).toString(), connGrpInfo.getParameterXML());//Check parameters
		QCOMPARE(query.value(5).toString(), QString::number(connGrpInfo.getSynapseTypeID()));//Check synapse type
		QVERIFY(connGrp.getID() != 0);

		//Check that connections were added correctly
		for(ConnectionIterator iter = connGrp.begin(); iter != connGrp.end(); ++iter){
			query = getQuery("SELECT ConnectionGroupID, FromNeuronID, ToNeuronID, Delay, Weight FROM Connections WHERE ConnectionID = " + QString::number(iter->getID()));
			executeQuery(query);
			query.next();
			QCOMPARE( query.value(0).toUInt(), connGrp.getID() );//Check connection group id
			QCOMPARE( query.value(1).toUInt(), iter->getFromNeuronID() );
			QCOMPARE( query.value(2).toUInt(), iter->getToNeuronID() );
			QCOMPARE( query.value(3).toString().toFloat(), iter->getDelay() );
			QCOMPARE( query.value(4).toString().toFloat(), iter->getWeight() );
		}
    }
    catch(SpikeStreamException& ex){
		QFAIL(ex.getMessage().toAscii());
    }
	catch (...){
		QFAIL("An unknown exception occurred.");
	}
}


/*! Tests the addition of a neuron group to the network */
void TestNetworkDaoThread::testAddNeuronGroup(){
    try{
		/* Add network to get a valid id.
			Slightly sloppy to use the network dao method, but it has been tested elsewhere
			and we need a network because of foreign key constraints */
		NetworkInfo netInfo(0, "test2Name","test3Description");
		NetworkDao networkDao(dbInfo);
		networkDao.addNetwork(netInfo);

		//Build the neuron group that is to be added
		QHash<QString, double> paramMap;
		paramMap["param1"] = 0.5;
		paramMap["param2"] = 0.6;
		NeuronType neurType(1, "neur type description", "neur type paramTableName", "");
		NeuronGroup neurGrp(NeuronGroupInfo(0, "testNeuronGroup1Name", "testNeuronGroup1Desc", paramMap, neurType));
		NeuronMap* neurMap = neurGrp.getNeuronMap();
		(*neurMap)[1] = new Neuron(0, 0, 0);
		(*neurMap)[2] = new Neuron(0, 1, 0);
		(*neurMap)[3] = new Neuron(1, 0, 0);
		(*neurMap)[4] = new Neuron(1, 1, 1);

		//Add the neuron group
		NetworkDaoThread netDaoThread(dbInfo);
		netDaoThread.prepareAddNeuronGroup(netInfo.getID(), &neurGrp);
		runThread(netDaoThread);

		//Check that the neuron group was added correctly
		QSqlQuery query = getQuery("SELECT NetworkID, Name, Description, Parameters, NeuronTypeID FROM NeuronGroups");
		executeQuery(query);
		QCOMPARE(1, query.size());
		query.next();
		NeuronGroupInfo neurGrpInfo = neurGrp.getInfo();
		QCOMPARE(query.value(0).toString(), QString::number(netInfo.getID()));//Check network id
		QCOMPARE(query.value(1).toString(), neurGrpInfo.getName());//Check name
		QCOMPARE(query.value(2).toString(), neurGrpInfo.getDescription());//Check description
		QCOMPARE(query.value(3).toString(), neurGrpInfo.getParameterXML());//Check parameters
		QCOMPARE(query.value(4).toString(), QString::number(neurGrpInfo.getNeuronTypeID()));//Check neuron type
		QVERIFY( neurGrpInfo.getID() != 0);

		//Check that the neurons were added correctly
		query = getQuery("SELECT NeuronID, X, Y, Z FROM Neurons WHERE NeuronGroupID = " + QString::number(neurGrp.getID()));
		executeQuery(query);
		QCOMPARE(4, query.size());//4 neurons should have been added
		query.next();
		QVERIFY( neurGrp.contains(
				query.value(0).toUInt(),
				query.value(1).toString().toFloat(),
				query.value(2).toString().toFloat(),
				query.value(3).toString().toFloat()
				) );
		query.next();
		QVERIFY( neurGrp.contains(
				query.value(0).toUInt(),
				query.value(1).toString().toFloat(),
				query.value(2).toString().toFloat(),
				query.value(3).toString().toFloat()
				) );
		query.next();
		QVERIFY( neurGrp.contains(
				query.value(0).toUInt(),
				query.value(1).toString().toFloat(),
				query.value(2).toString().toFloat(),
				query.value(3).toString().toFloat()
				) );
		query.next();
		//Check the last one in a slightly different way
		QCOMPARE(query.value(1).toString(), QString::number(1));
		QCOMPARE(query.value(2).toString(), QString::number(1));
		QCOMPARE(query.value(3).toString(), QString::number(1));

    }
    catch(SpikeStreamException& ex){
		QFAIL(ex.getMessage().toAscii());
    }
}


void TestNetworkDaoThread::testDeleteConnectionGroups(){
	try{
		//Add test network
		addTestNetwork1();

		//Add second connection group in opposite direction
		QString queryStr = "INSERT INTO ConnectionGroups (NetworkID, Description, FromNeuronGroupID, ToNeuronGroupID, Parameters, SynapseTypeID ) VALUES (";
		queryStr += QString::number(testNetID) + ", 'conngrpdesc1', " + QString::number(neurGrp2ID) + ", " + QString::number(neurGrp1ID) + ", '" + getConnectionParameterXML() + "', 1)";
		QSqlQuery query = getQuery(queryStr);
		executeQuery(query);
		unsigned int connGrp2ID = query.lastInsertId().toUInt();

		//Run method to delete the second neuron group
		QList<unsigned int> deleteList;
		deleteList.append(connGrp2ID);
		NetworkDaoThread netDaoThread(dbInfo);
		netDaoThread.prepareDeleteConnectionGroups(testNetID, deleteList);
		runThread(netDaoThread);

		//Check that the neuron group was deleted correctly
		query = getQuery("SELECT ConnectionGroupID FROM ConnectionGroups WHERE NetworkID=" + QString::number(testNetID));
		executeQuery(query);
		QCOMPARE(1, query.size());
		query.next();
		QCOMPARE(query.value(0).toString(), QString::number(connGrp1ID));//Check correct connection group was deleted
	}
	catch(SpikeStreamException& ex){
		QFAIL(ex.getMessage().toAscii());
	}
}


void TestNetworkDaoThread::testDeleteNetwork(){
	//Add test network
	addTestNetwork1();

	//Check that test network is in database
	QSqlQuery query = getQuery("SELECT * FROM Networks WHERE NetworkID = " + QString::number(testNetID));
	executeQuery(query);

	//Should be a single network
	QCOMPARE(query.size(), (int)1);

	//Invoke method being tested
	NetworkDaoThread netDaoThread(dbInfo);
	netDaoThread.startDeleteNetwork(testNetID);
	netDaoThread.wait();

	//Check to see if network with this id has been removed from the database
	query = getQuery("SELECT * FROM Networks WHERE NetworkID = " + QString::number(testNetID));
	executeQuery(query);
	QCOMPARE(query.size(), (int)0);

	//Check neurons have gone
	query = getQuery("SELECT * FROM Neurons");
	executeQuery(query);
	QCOMPARE(query.size(), (int)0);

	//Check archive has gone
	query = getArchiveQuery("SELECT * FROM Archives");
	executeQuery(query);
	QCOMPARE(query.size(), (int)0);

	//Check analysis has gone
	query = getAnalysisQuery("SELECT * FROM Analyses");
	executeQuery(query);
	QCOMPARE(query.size(), (int)0);
}


void TestNetworkDaoThread::testDeleteNeuronGroups(){
	try{
		//Add test network
		addTestNetwork1();

		//Run method to delete the second neuron group
		QList<unsigned int> deleteList;
		deleteList.append(neurGrp2ID);
		NetworkDaoThread netDaoThread(dbInfo);
		netDaoThread.prepareDeleteNeuronGroups(testNetID, deleteList);
		runThread(netDaoThread);

		//Check that the neuron group was deleted correctly
		QSqlQuery query = getQuery("SELECT NeuronGroupID FROM NeuronGroups WHERE NetworkID=" + QString::number(testNetID));
		executeQuery(query);
		QCOMPARE(1, query.size());
		query.next();
		QCOMPARE(query.value(0).toString(), QString::number(neurGrp1ID));//Check correct neuron group was deleted
	}
	catch(SpikeStreamException& ex){
		QFAIL(ex.getMessage().toAscii());
	}
}


void TestNetworkDaoThread::testLoadConnections(){
	try{
		//Add test network
		addTestNetwork1();

		//Create a connection group with the appropriate id
		SynapseType synType(1, "syn type description", "syn type paramTableName", "");
		ConnectionGroup connGrp( ConnectionGroupInfo(connGrp1ID, "undefined", 0, 0, QHash<QString, double>(), synType) );

		//Load connections associated with this neuron group
		NetworkDaoThread networkDaoThread(dbInfo);
		networkDaoThread.prepareLoadConnections(&connGrp);
		runThread(networkDaoThread);

		//Check that connections were correctly loaded.
		QList<Connection*> allConnList = connGrp.getConnections();
		QCOMPARE(allConnList.size(), 6);

		//Should be three connections from the first neuron in the list
		QList<Connection*> fromList = connGrp.getFromConnections(testNeurIDList[0]);
		QCOMPARE(fromList.size(), 3);
		QCOMPARE(fromList[0]->getToNeuronID(), testNeurIDList[1]);
		QCOMPARE(fromList[1]->getToNeuronID(), testNeurIDList[2]);
		QCOMPARE(fromList[2]->getToNeuronID(), testNeurIDList[3]);
		QCOMPARE(fromList[2]->getWeight(), 0.3f);
		QCOMPARE(fromList[2]->getDelay(), 1.3f);

		//Should be two connections to the first neuron in the list
		QList<Connection*> toList = connGrp.getToConnections(testNeurIDList[2]);
		QCOMPARE(toList.size(), 2);
		QCOMPARE(toList[0]->getFromNeuronID(), testNeurIDList[0]);
		QCOMPARE(toList[1]->getFromNeuronID(), testNeurIDList[3]);
		QCOMPARE(toList[1]->getWeight(), 0.6f);
		QCOMPARE(toList[1]->getDelay(), 1.6f);
	}
	catch(SpikeStreamException ex){
		QFAIL(ex.getMessage().toAscii());
	}
	catch(...){
		QFAIL("Unrecognized exception thrown.");
	}
}


void TestNetworkDaoThread::testLoadNeurons(){
    //Add test network
    addTestNetwork1();

    //Create a list of neuron groups with the appropriate ids
	NeuronType neurType(1, "neur type description", "neur type paramTableName", "");
	NeuronGroup neurGrp1( NeuronGroupInfo(neurGrp1ID, "undefined name", "undefined desc", QHash<QString, double>(), neurType) );
	NeuronGroup neurGrp2( NeuronGroupInfo(neurGrp2ID, "undefined name", "undefined desc", QHash<QString, double>(), neurType) );
    QList<NeuronGroup*> neurGrpList;
    neurGrpList.append(&neurGrp1);
    neurGrpList.append(&neurGrp2);

    //Load neurons associated with this neuron group
    NetworkDaoThread networkDaoThread(dbInfo);
    networkDaoThread.prepareLoadNeurons(neurGrpList);
    runThread(networkDaoThread);

    //Check that neuron groups have correct number of neurons
    QCOMPARE(neurGrp1.size(), (int)3);
    QCOMPARE(neurGrp2.size(), (int)2);

    //Check the ids and location in neuron group 1 are correct
    QHash<unsigned int, Neuron*>* neurGrpMap = neurGrp1.getNeuronMap();
    QCOMPARE((*neurGrpMap)[ testNeurIDList[0] ]->getXPos(), -1.0f);
    QCOMPARE((*neurGrpMap)[ testNeurIDList[0] ]->getZPos(), -6.0f);
    QCOMPARE((*neurGrpMap)[ testNeurIDList[2] ]->getYPos(), -4.0f);

    //Check the ids and location in neuron group 2 are correct
    neurGrpMap = neurGrp2.getNeuronMap();
    QCOMPARE((*neurGrpMap)[ testNeurIDList[3] ]->getXPos(), 0.0f);
    QCOMPARE((*neurGrpMap)[ testNeurIDList[3] ]->getZPos(), 10.0f);
    QCOMPARE((*neurGrpMap)[ testNeurIDList[4] ]->getYPos(), -7.0f);
}


/*----------------------------------------------------------*/
/*-----               PRIVATE METHODS                  -----*/
/*----------------------------------------------------------*/


/*! Runs the supplied thread and checks for errors */
void TestNetworkDaoThread::runThread(NetworkDaoThread& thread){
    thread.start();
    thread.wait();
    if(thread.isError()){
		throw SpikeStreamException(thread.getErrorMessage());
    }
}








