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
}


/*! Adds a series of progressively interconnected 4 neuron networks with associated archives */
void AleksanderNetworksBuilder::add4NeuronNetworks1(const QString& networkName){
    //Local reference to network dao
    NetworkDao* networkDao = Globals::getNetworkDao();

    //Add first network
    NetworkInfo netInfo(0, networkName, "Aleksander 4 neuron network 1. A<->B; C<->D");
    networkDao->addNetwork(netInfo);

    //Build neuron group - store neurons in order in a list
    QHash<QString, double> paramMap;
    NeuronGroup neurGrp(NeuronGroupInfo(0, "Neuron group 1", "Main neuron group", paramMap, 2));
    NeuronMap neurMap;
    neurMap[1] = neurGrp.addNeuron(1, 2, 1);//A
    neurMap[2] = neurGrp.addNeuron(1, 1, 1);//B
    neurMap[3] = neurGrp.addNeuron(2, 2, 1);//C
    neurMap[4] = neurGrp.addNeuron(2, 1, 1);//D

    //Add the neuron group
    DBInfo netDBInfo = Globals::getNetworkDao()->getDBInfo();
    NetworkDaoThread netDaoThread(netDBInfo);
    netDaoThread.prepareAddNeuronGroup(netInfo.getID(), &neurGrp);
    runThread(netDaoThread);

    //Build the connection group that is to be added
    ConnectionGroupInfo connGrpInfo(0, "Connection Group", neurGrp.getID(), neurGrp.getID(),  paramMap, 2);
    ConnectionGroup connGrp(connGrpInfo);

    //Add connections
    Connection* abCon = new Connection(neurMap[1]->getID(), neurMap[2]->getID(),  0,  0,  0);//A->B
    connGrp.addConnection(abCon);
    Connection* baCon = new Connection(neurMap[2]->getID(), neurMap[1]->getID(),  0,  0,  0);//B->A
    connGrp.addConnection(baCon);
    Connection* cdCon = new Connection(neurMap[3]->getID(), neurMap[4]->getID(),  0,  0,  0);//C->D
    connGrp.addConnection(cdCon);
    Connection* dcCon = new Connection(neurMap[4]->getID(), neurMap[3]->getID(),  0,  0,  0);//D->C
    connGrp.addConnection(dcCon);

    netDaoThread.prepareAddConnectionGroup(netInfo.getID(), &connGrp);
    runThread(netDaoThread);

    //Add weightless connections. Only 1 connection per neuron, so pattern index is zero in all cases
    networkDao->addWeightlessConnection(abCon->getID(), 0);
    networkDao->addWeightlessConnection(baCon->getID(), 0);
    networkDao->addWeightlessConnection(cdCon->getID(), 0);
    networkDao->addWeightlessConnection(dcCon->getID(), 0);

    //Add training
    addTraining(neurMap[1]->getID(), "0", 0);
    addTraining(neurMap[1]->getID(), "1", 1);
    addTraining(neurMap[2]->getID(), "0", 0);
    addTraining(neurMap[2]->getID(), "1", 1);
    addTraining(neurMap[3]->getID(), "0", 0);
    addTraining(neurMap[3]->getID(), "1", 1);
    addTraining(neurMap[4]->getID(), "0", 0);
    addTraining(neurMap[4]->getID(), "1", 1);

    //Add archives
    add4NeuronFiringPatterns(netInfo.getID(), neurMap);

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

