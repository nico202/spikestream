//SpikeStream includes
#include "Globals.h"
#include "NetworkDao.h"
#include "NetworkInfo.h"
#include "NeuronGroupInfo.h"
#include "AleksanderNetworksBuilder.h"
#include "SpikeStreamException.h"
using namespace spikestream;


/*! Constructor */
AleksanderNetworksBuilder::AleksanderNetworksBuilder(){
}


/*! Destructor */
AleksanderNetworksBuilder::~AleksanderNetworksBuilder(){
    reset();
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

void AleksanderNetworksBuilder::add4NeuronNetwork1(const QString& networkName, const QString& networkDescription){
    //Add neurons and archives
    add4NeuronBasicNetwork(networkName, networkDescription);

    //Build the connection group that is to be added
    QHash<QString, double> paramMap;
    ConnectionGroupInfo connGrpInfo(0, "Connection Group", neuronGroup->getID(), neuronGroup->getID(),  paramMap, 2);
    ConnectionGroup connGrp(connGrpInfo);

    //Add connections
    Connection* abCon = connGrp.addConnection( new Connection(neuronMap[1]->getID(), neuronMap[2]->getID(),  0,  0,  0) );//A->B
    Connection* baCon = connGrp.addConnection( new Connection(neuronMap[2]->getID(), neuronMap[1]->getID(),  0,  0,  0) );//B->A
    Connection* cdCon = connGrp.addConnection( new Connection(neuronMap[3]->getID(), neuronMap[4]->getID(),  0,  0,  0) );//C->D
    Connection* dcCon = connGrp.addConnection( new Connection(neuronMap[4]->getID(), neuronMap[3]->getID(),  0,  0,  0) );//D->C
    addConnectionGroup(networkID, connGrp);

    //Add weightless connections. Only 1 connection per neuron, so pattern index is zero in all cases
    NetworkDao* networkDao = Globals::getNetworkDao();
    networkDao->addWeightlessConnection(baCon->getID(), 0);
    networkDao->addWeightlessConnection(abCon->getID(), 0);
    networkDao->addWeightlessConnection(dcCon->getID(), 0);
    networkDao->addWeightlessConnection(cdCon->getID(), 0);

    //Add training
    addTraining(neuronMap[1]->getID(), "0", 0);
    addTraining(neuronMap[1]->getID(), "1", 1);
    addTraining(neuronMap[2]->getID(), "0", 0);
    addTraining(neuronMap[2]->getID(), "1", 1);
    addTraining(neuronMap[3]->getID(), "0", 0);
    addTraining(neuronMap[3]->getID(), "1", 1);
    addTraining(neuronMap[4]->getID(), "0", 0);
    addTraining(neuronMap[4]->getID(), "1", 1);
}


/*! Adds network with 4 neurons. Connections: A<->B; A->C; C<->D
     and an AND function in C. */
void AleksanderNetworksBuilder::add4NeuronNetwork2_AND(const QString& networkName, const QString& networkDescription){
    //Add neurons and archives
    add4NeuronBasicNetwork(networkName, networkDescription);

    //Build the connection group that is to be added
    QHash<QString, double> paramMap;
    ConnectionGroupInfo connGrpInfo(0, "Connection Group", neuronGroup->getID(), neuronGroup->getID(),  paramMap, 2);
    ConnectionGroup connGrp(connGrpInfo);

    //Add connections
    Connection* abCon = connGrp.addConnection( new Connection(neuronMap[1]->getID(), neuronMap[2]->getID(),  0,  0,  0) );//A->B
    Connection* acCon = connGrp.addConnection( new Connection(neuronMap[1]->getID(), neuronMap[3]->getID(),  0,  0,  0) );//A->C
    Connection* baCon = connGrp.addConnection( new Connection(neuronMap[2]->getID(), neuronMap[1]->getID(),  0,  0,  0) );//B->A
    Connection* cdCon = connGrp.addConnection( new Connection(neuronMap[3]->getID(), neuronMap[4]->getID(),  0,  0,  0) );//C->D
    Connection* dcCon = connGrp.addConnection( new Connection(neuronMap[4]->getID(), neuronMap[3]->getID(),  0,  0,  0) );//D->C
    addConnectionGroup(networkID, connGrp);

    //Add weightless connections.1 connection to A, B and D
    NetworkDao* networkDao = Globals::getNetworkDao();
    networkDao->addWeightlessConnection(baCon->getID(), 0);
    networkDao->addWeightlessConnection(abCon->getID(), 0);
    networkDao->addWeightlessConnection(cdCon->getID(), 0);

    //2 connections to C;
    networkDao->addWeightlessConnection(acCon->getID(), 0);
    networkDao->addWeightlessConnection(dcCon->getID(), 1);

    //Add AND training. C has two inputs, the rest have one.
    addTraining(neuronMap[1]->getID(), "0", 0);
    addTraining(neuronMap[1]->getID(), "1", 1);
    addTraining(neuronMap[2]->getID(), "0", 0);
    addTraining(neuronMap[2]->getID(), "1", 1);
    addTraining(neuronMap[3]->getID(), "00", 0);
    addTraining(neuronMap[3]->getID(), "01", 0);
    addTraining(neuronMap[3]->getID(), "10", 0);
    addTraining(neuronMap[3]->getID(), "11", 1);
    addTraining(neuronMap[4]->getID(), "0", 0);
    addTraining(neuronMap[4]->getID(), "1", 1);
}


/*! Adds network with 4 neurons. Connections: A<->B; A->C; C<->D
     and an AND function in C. */
void AleksanderNetworksBuilder::add4NeuronNetwork2_XOR(const QString& networkName, const QString& networkDescription){
    //Add neurons and archives
    add4NeuronBasicNetwork(networkName, networkDescription);

    //Build the connection group that is to be added
    QHash<QString, double> paramMap;
    ConnectionGroupInfo connGrpInfo(0, "Connection Group", neuronGroup->getID(), neuronGroup->getID(),  paramMap, 2);
    ConnectionGroup connGrp(connGrpInfo);

    //Add connections
    Connection* abCon = connGrp.addConnection( new Connection(neuronMap[1]->getID(), neuronMap[2]->getID(),  0,  0,  0) );//A->B
    Connection* acCon = connGrp.addConnection( new Connection(neuronMap[1]->getID(), neuronMap[3]->getID(),  0,  0,  0) );//A->C
    Connection* baCon = connGrp.addConnection( new Connection(neuronMap[2]->getID(), neuronMap[1]->getID(),  0,  0,  0) );//B->A
    Connection* cdCon = connGrp.addConnection( new Connection(neuronMap[3]->getID(), neuronMap[4]->getID(),  0,  0,  0) );//C->D
    Connection* dcCon = connGrp.addConnection( new Connection(neuronMap[4]->getID(), neuronMap[3]->getID(),  0,  0,  0) );//D->C
    addConnectionGroup(networkID, connGrp);

    //Add weightless connections.1 connection to A, B and D
    NetworkDao* networkDao = Globals::getNetworkDao();
    networkDao->addWeightlessConnection(baCon->getID(), 0);
    networkDao->addWeightlessConnection(abCon->getID(), 0);
    networkDao->addWeightlessConnection(cdCon->getID(), 0);

    //2 connections to C;
    networkDao->addWeightlessConnection(acCon->getID(), 0);
    networkDao->addWeightlessConnection(dcCon->getID(), 1);

    //Add XOR training. C has two inputs, the rest have one.
    addTraining(neuronMap[1]->getID(), "0", 0);
    addTraining(neuronMap[1]->getID(), "1", 1);
    addTraining(neuronMap[2]->getID(), "0", 0);
    addTraining(neuronMap[2]->getID(), "1", 1);
    addTraining(neuronMap[3]->getID(), "00", 0);
    addTraining(neuronMap[3]->getID(), "01", 1);
    addTraining(neuronMap[3]->getID(), "10", 1);
    addTraining(neuronMap[3]->getID(), "11", 0);
    addTraining(neuronMap[4]->getID(), "0", 0);
    addTraining(neuronMap[4]->getID(), "1", 1);
}


/*! Adds network with 4 neurons. Connections: A<->B; A<->C; C<->D
     and an AND functions in A and C. */
void AleksanderNetworksBuilder::add4NeuronNetwork3_AND(const QString& networkName, const QString& networkDescription){
    //Add neurons and archives
    add4NeuronBasicNetwork(networkName, networkDescription);

    //Build the connection group that is to be added
    QHash<QString, double> paramMap;
    ConnectionGroupInfo connGrpInfo(0, "Connection Group", neuronGroup->getID(), neuronGroup->getID(),  paramMap, 2);
    ConnectionGroup connGrp(connGrpInfo);

    //Add connections
    Connection* abCon = connGrp.addConnection( new Connection(neuronMap[1]->getID(), neuronMap[2]->getID(),  0,  0,  0) );//A->B
    Connection* acCon = connGrp.addConnection( new Connection(neuronMap[1]->getID(), neuronMap[3]->getID(),  0,  0,  0) );//A->C
    Connection* baCon = connGrp.addConnection( new Connection(neuronMap[2]->getID(), neuronMap[1]->getID(),  0,  0,  0) );//B->A
    Connection* caCon = connGrp.addConnection( new Connection(neuronMap[3]->getID(), neuronMap[1]->getID(),  0,  0,  0) );//C->A
    Connection* cdCon = connGrp.addConnection( new Connection(neuronMap[3]->getID(), neuronMap[4]->getID(),  0,  0,  0) );//C->D
    Connection* dcCon = connGrp.addConnection( new Connection(neuronMap[4]->getID(), neuronMap[3]->getID(),  0,  0,  0) );//D->C
    addConnectionGroup(networkID, connGrp);

    //Add weightless connections.1 connection to B and D
    NetworkDao* networkDao = Globals::getNetworkDao();
    networkDao->addWeightlessConnection(abCon->getID(), 0);
    networkDao->addWeightlessConnection(cdCon->getID(), 0);

    //2 connections to A and C;
    networkDao->addWeightlessConnection(baCon->getID(), 0);
    networkDao->addWeightlessConnection(caCon->getID(), 1);
    networkDao->addWeightlessConnection(acCon->getID(), 0);
    networkDao->addWeightlessConnection(dcCon->getID(), 1);

    //Add AND training.A and C have two inputs, the rest have one.
    addTraining(neuronMap[1]->getID(), "00", 0);
    addTraining(neuronMap[1]->getID(), "01", 0);
    addTraining(neuronMap[1]->getID(), "10", 0);
    addTraining(neuronMap[1]->getID(), "11", 1);
    addTraining(neuronMap[2]->getID(), "0", 0);
    addTraining(neuronMap[2]->getID(), "1", 1);
    addTraining(neuronMap[3]->getID(), "00", 0);
    addTraining(neuronMap[3]->getID(), "01", 0);
    addTraining(neuronMap[3]->getID(), "10", 0);
    addTraining(neuronMap[3]->getID(), "11", 1);
    addTraining(neuronMap[4]->getID(), "0", 0);
    addTraining(neuronMap[4]->getID(), "1", 1);
}


/*! Adds network with 4 neurons. Connections: A<->B; A<->C; C<->D
     and XOR functions in A and C. */
void AleksanderNetworksBuilder::add4NeuronNetwork3_XOR(const QString& networkName, const QString& networkDescription){
    //Add neurons and archives
    add4NeuronBasicNetwork(networkName, networkDescription);

    //Build the connection group that is to be added
    QHash<QString, double> paramMap;
    ConnectionGroupInfo connGrpInfo(0, "Connection Group", neuronGroup->getID(), neuronGroup->getID(),  paramMap, 2);
    ConnectionGroup connGrp(connGrpInfo);

    //Add connections
    Connection* abCon = connGrp.addConnection( new Connection(neuronMap[1]->getID(), neuronMap[2]->getID(),  0,  0,  0) );//A->B
    Connection* acCon = connGrp.addConnection( new Connection(neuronMap[1]->getID(), neuronMap[3]->getID(),  0,  0,  0) );//A->C
    Connection* baCon = connGrp.addConnection( new Connection(neuronMap[2]->getID(), neuronMap[1]->getID(),  0,  0,  0) );//B->A
    Connection* caCon = connGrp.addConnection( new Connection(neuronMap[3]->getID(), neuronMap[1]->getID(),  0,  0,  0) );//C->A
    Connection* cdCon = connGrp.addConnection( new Connection(neuronMap[3]->getID(), neuronMap[4]->getID(),  0,  0,  0) );//C->D
    Connection* dcCon = connGrp.addConnection( new Connection(neuronMap[4]->getID(), neuronMap[3]->getID(),  0,  0,  0) );//D->C
    addConnectionGroup(networkID, connGrp);

    //Add weightless connections.1 connection to B and D
    NetworkDao* networkDao = Globals::getNetworkDao();
    networkDao->addWeightlessConnection(abCon->getID(), 0);
    networkDao->addWeightlessConnection(cdCon->getID(), 0);

    //2 connections to A and C;
    networkDao->addWeightlessConnection(baCon->getID(), 0);
    networkDao->addWeightlessConnection(caCon->getID(), 1);
    networkDao->addWeightlessConnection(acCon->getID(), 0);
    networkDao->addWeightlessConnection(dcCon->getID(), 1);

    //Add XOR training.A and C have two inputs, the rest have one.
    addTraining(neuronMap[1]->getID(), "00", 0);
    addTraining(neuronMap[1]->getID(), "01", 1);
    addTraining(neuronMap[1]->getID(), "10", 1);
    addTraining(neuronMap[1]->getID(), "11", 0);
    addTraining(neuronMap[2]->getID(), "0", 0);
    addTraining(neuronMap[2]->getID(), "1", 1);
    addTraining(neuronMap[3]->getID(), "00", 0);
    addTraining(neuronMap[3]->getID(), "01", 1);
    addTraining(neuronMap[3]->getID(), "10", 1);
    addTraining(neuronMap[3]->getID(), "11", 0);
    addTraining(neuronMap[4]->getID(), "0", 0);
    addTraining(neuronMap[4]->getID(), "1", 1);
}



/*! Adds network with 4 neurons. Connections: A<->B; A<->C; B<->D; C<->D
     and XOR functions in A and C. */
void AleksanderNetworksBuilder::add4NeuronNetwork4_AND(const QString& networkName, const QString& networkDescription){
    //Add neurons and archives
    add4NeuronBasicNetwork(networkName, networkDescription);

    //Build the connection group that is to be added
    QHash<QString, double> paramMap;
    ConnectionGroupInfo connGrpInfo(0, "Connection Group", neuronGroup->getID(), neuronGroup->getID(),  paramMap, 2);
    ConnectionGroup connGrp(connGrpInfo);

    //Add connections
    Connection* abCon = connGrp.addConnection( new Connection(neuronMap[1]->getID(), neuronMap[2]->getID(),  0,  0,  0) );//A->B
    Connection* acCon = connGrp.addConnection( new Connection(neuronMap[1]->getID(), neuronMap[3]->getID(),  0,  0,  0) );//A->C
    Connection* baCon = connGrp.addConnection( new Connection(neuronMap[2]->getID(), neuronMap[1]->getID(),  0,  0,  0) );//B->A
    Connection* bdCon = connGrp.addConnection( new Connection(neuronMap[2]->getID(), neuronMap[4]->getID(),  0,  0,  0) );//B->D
    Connection* caCon = connGrp.addConnection( new Connection(neuronMap[3]->getID(), neuronMap[1]->getID(),  0,  0,  0) );//C->A
    Connection* cdCon = connGrp.addConnection( new Connection(neuronMap[3]->getID(), neuronMap[4]->getID(),  0,  0,  0) );//C->D
    Connection* dbCon = connGrp.addConnection( new Connection(neuronMap[4]->getID(), neuronMap[2]->getID(),  0,  0,  0) );//D->B
    Connection* dcCon = connGrp.addConnection( new Connection(neuronMap[4]->getID(), neuronMap[3]->getID(),  0,  0,  0) );//D->C
    addConnectionGroup(networkID, connGrp);

    //Add weightless connections. All neurons have two connections
    NetworkDao* networkDao = Globals::getNetworkDao();
    networkDao->addWeightlessConnection(baCon->getID(), 0);
    networkDao->addWeightlessConnection(caCon->getID(), 1);
    networkDao->addWeightlessConnection(abCon->getID(), 0);
    networkDao->addWeightlessConnection(dbCon->getID(), 1);
    networkDao->addWeightlessConnection(acCon->getID(), 0);
    networkDao->addWeightlessConnection(dcCon->getID(), 1);
    networkDao->addWeightlessConnection(bdCon->getID(), 0);
    networkDao->addWeightlessConnection(cdCon->getID(), 1);

    //Add AND training.A and C have two inputs, the rest have one.
    addTraining(neuronMap[1]->getID(), "00", 0);
    addTraining(neuronMap[1]->getID(), "01", 0);
    addTraining(neuronMap[1]->getID(), "10", 0);
    addTraining(neuronMap[1]->getID(), "11", 1);
    addTraining(neuronMap[2]->getID(), "0", 0);
    addTraining(neuronMap[2]->getID(), "1", 1);
    addTraining(neuronMap[3]->getID(), "00", 0);
    addTraining(neuronMap[3]->getID(), "01", 0);
    addTraining(neuronMap[3]->getID(), "10", 0);
    addTraining(neuronMap[3]->getID(), "11", 1);
    addTraining(neuronMap[4]->getID(), "0", 0);
    addTraining(neuronMap[4]->getID(), "1", 1);
}


void AleksanderNetworksBuilder::add4NeuronNetwork4_XOR(const QString& networkName, const QString& networkDescription){

}


void AleksanderNetworksBuilder::add4NeuronNetwork5_AND(const QString& networkName, const QString& networkDescription){

}


void AleksanderNetworksBuilder::add4NeuronNetwork5_XOR(const QString& networkName, const QString& networkDescription){

}


/*! Resets all of the variables in the class ready to add another network */
void AleksanderNetworksBuilder::reset(){
    networkID = 0;

    //Clean up neuron group
    if(neuronGroup != NULL){
	delete neuronGroup;
	neuronGroup = NULL;
    }

    //Clean up neuron map
    for(QHash<unsigned int, Neuron*>::iterator iter = neuronMap.begin(); iter != neuronMap.end(); ++iter)
	delete iter.value();
    neuronMap.clear();
}


/*--------------------------------------------------------*/
/*-------             PRIVATE METHODS              -------*/
/*--------------------------------------------------------*/

void AleksanderNetworksBuilder::addConnectionGroup(unsigned int networkID, ConnectionGroup& connGrp){
    DBInfo netDBInfo = Globals::getNetworkDao()->getDBInfo();
    NetworkDaoThread netDaoThread(netDBInfo);
    netDaoThread.prepareAddConnectionGroup(networkID, &connGrp);
    runThread(netDaoThread);
}


/*! Adds a series of progressively interconnected 4 neuron networks with associated archives */
void AleksanderNetworksBuilder::add4NeuronBasicNetwork(const QString& networkName, const QString& networkDescription){
    //Reset class before adding network
    reset();

    //Local reference to network dao
    NetworkDao* networkDao = Globals::getNetworkDao();

    //Add first network
    NetworkInfo netInfo(0, networkName, networkDescription);
    networkDao->addNetwork(netInfo);
    networkID = netInfo.getID();

    //Build neuron group - store neurons in map
    QHash<QString, double> paramMap;
    neuronGroup = new NeuronGroup(NeuronGroupInfo(0, "Neuron group 1", "Main neuron group", paramMap, 2));
    neuronMap[1] = neuronGroup->addNeuron(1, 2, 1);//A
    neuronMap[2] = neuronGroup->addNeuron(1, 1, 1);//B
    neuronMap[3] = neuronGroup->addNeuron(2, 2, 1);//C
    neuronMap[4] = neuronGroup->addNeuron(2, 1, 1);//D

    //Add the neuron group
    DBInfo netDBInfo = Globals::getNetworkDao()->getDBInfo();
    NetworkDaoThread netDaoThread(netDBInfo);
    netDaoThread.prepareAddNeuronGroup(netInfo.getID(), neuronGroup);
    runThread(netDaoThread);

    //Add archives
    add4NeuronFiringPatterns(netInfo.getID(), neuronMap);
}


void AleksanderNetworksBuilder::add4NeuronFiringPatterns(unsigned int networkID, NeuronMap& neuronMap){
    //Local reference to archive dao
    ArchiveDao* archiveDao = Globals::getArchiveDao();

    //Easy to use neuron ids
    QString neurID1 = QString::number(neuronMap[1]->getID()), neurID2 = QString::number(neuronMap[2]->getID());
    QString neurID3 = QString::number(neuronMap[3]->getID()), neurID4 = QString::number(neuronMap[4]->getID());

    //Create archive
    ArchiveInfo archiveInfo(0, networkID, QDateTime::currentDateTime().toTime_t(), "Complete set of 4 neuron firing patterns");
    archiveDao->addArchive(archiveInfo);

    //Add complete set of firing patterns
    archiveDao->addArchiveData(archiveInfo.getID(), 0, "");//0000

    archiveDao->addArchiveData(archiveInfo.getID(), 1, neurID4);//0001
    archiveDao->addArchiveData(archiveInfo.getID(), 2, neurID3);//0010
    archiveDao->addArchiveData(archiveInfo.getID(), 3, neurID2);//0100
    archiveDao->addArchiveData(archiveInfo.getID(), 4, neurID1);//1000

    archiveDao->addArchiveData(archiveInfo.getID(), 5, neurID3 + "," + neurID4);//0011
    archiveDao->addArchiveData(archiveInfo.getID(), 6, neurID2 + "," + neurID4);//0101
    archiveDao->addArchiveData(archiveInfo.getID(), 7, neurID1 + "," + neurID4);//1001
    archiveDao->addArchiveData(archiveInfo.getID(), 8, neurID2 + "," + neurID3);//0110
    archiveDao->addArchiveData(archiveInfo.getID(), 9, neurID1 + "," + neurID2);//1100
    archiveDao->addArchiveData(archiveInfo.getID(), 10, neurID1 + "," + neurID3);//1010

    archiveDao->addArchiveData(archiveInfo.getID(), 11, neurID1 + "," + neurID2 + "," + neurID3);//1110
    archiveDao->addArchiveData(archiveInfo.getID(), 12, neurID1 + "," + neurID2 + "," + neurID4);//1101
    archiveDao->addArchiveData(archiveInfo.getID(), 13, neurID1 + "," + neurID3 + "," + neurID4);//1011
    archiveDao->addArchiveData(archiveInfo.getID(), 14, neurID2 + "," + neurID3 + "," + neurID4);//0111

    archiveDao->addArchiveData(archiveInfo.getID(), 15, neurID1 + "," + neurID2 + "," + neurID3 + "," + neurID4);//1111

}

