//SpikeStream includes
#include "Globals.h"
#include "NetworkDao.h"
#include "NetworkInfo.h"
#include "NeuronGroupInfo.h"
#include "NeuronGroup.h"
#include "TononiNetworkBuilder.h"
#include "SpikeStreamException.h"
using namespace spikestream;


/*! Constructor */
TononiNetworkBuilder::TononiNetworkBuilder(){
}


/*! Destructor */
TononiNetworkBuilder::~TononiNetworkBuilder(){
}


/*! Adds Balduzzi and Tononi (2008), Figure 6 */
void TononiNetworkBuilder::addBalduzziTononiFigure6(const QString& networkName){
    //Add a network
    NetworkInfo netInfo(0, networkName, "Balduzzi and Tononi(2008) Figure 6", false);
    Globals::getNetworkDao()->addNetwork(netInfo);

    //Build neuron group - store neurons in order in a list
    QHash<QString, double> paramMap;
    NeuronGroup neurGrp(NeuronGroupInfo(0, "Neuron group 1", "Main neuron group", paramMap, 2));
    NeuronMap neurMap;
    neurMap[1] = neurGrp.addNeuron(1, 2, 1);
    neurMap[2] = neurGrp.addNeuron(3, 2, 1);
    neurMap[3] = neurGrp.addNeuron(2, 1, 1);
    neurMap[4] = neurGrp.addNeuron(1, 3, 1);
    neurMap[5] = neurGrp.addNeuron(3, 3, 1);
    neurMap[6] = neurGrp.addNeuron(4, 1, 1);

    //Add the neuron group
    DBInfo netDBInfo = Globals::getNetworkDao()->getDBInfo();
    NetworkDaoThread netDaoThread(netDBInfo);
    netDaoThread.prepareAddNeuronGroup(netInfo.getID(), &neurGrp);
    runThread(netDaoThread);

    //Build the connection group that is to be added
    ConnectionGroupInfo connGrpInfo(0, "Connection Group", neurGrp.getID(), neurGrp.getID(),  paramMap, 2);
    ConnectionGroup connGrp(connGrpInfo);

    //Add connections
    connGrp.addConnection(new Connection(neurMap[1]->getID(), neurMap[2]->getID(),  0,  0,  0));
    connGrp.addConnection(new Connection(neurMap[1]->getID(), neurMap[3]->getID(),  0,  0,  0));
    connGrp.addConnection(new Connection(neurMap[1]->getID(), neurMap[4]->getID(),  0,  0,  0));
    connGrp.addConnection(new Connection(neurMap[2]->getID(), neurMap[1]->getID(),  0,  0,  0));
    connGrp.addConnection(new Connection(neurMap[2]->getID(), neurMap[3]->getID(),  0,  0,  0));
    connGrp.addConnection(new Connection(neurMap[2]->getID(), neurMap[5]->getID(),  0,  0,  0));
    connGrp.addConnection(new Connection(neurMap[3]->getID(), neurMap[1]->getID(),  0,  0,  0));
    connGrp.addConnection(new Connection(neurMap[3]->getID(), neurMap[2]->getID(),  0,  0,  0));
    connGrp.addConnection(new Connection(neurMap[3]->getID(), neurMap[6]->getID(),  0,  0,  0));

    netDaoThread.prepareAddConnectionGroup(netInfo.getID(), &connGrp);
    runThread(netDaoThread);






//     //Build the connection group that is to be added
//    QHash<QString, double> paramMap;
//    ConnectionGroupInfo connGrpInfo(0, "Main connection group", neurGrp.getID(), neurGrp.getID(),  paramMap, 2);
//    ConnectionGroup connGrp(connGrpInfo);
//
//    //Add connections
//    for(int i=0; i<4; ++i){
//	Connection* newConn = new Connection(fromNeuronIDs[i], toNeuronIDs[i],  30,  0.1,  0.4);
//	connGrp.addConnection(newConn);
//    }
//
//    //Add the connection group to the database
//    netDaoThread.prepareAddConnectionGroup(netInfo.getID(), &connGrp);
//    runThread(netDaoThread);

//	//Same con map can be used for all neurons since the neurons copy its contents
//    QHash<unsigned int, unsigned int> conMap;
//    QHash<unsigned int, WeightlessNeuron*> weightlessNeuronMap;
//
//    //Build neuron 1 - it copies the output from neuron 2
//    conMap[2] = 0;
//    weightlessNeuronMap[1] = new WeightlessNeuron(conMap);
//    addTraining(*weightlessNeuronMap[1], "0", 0);
//    addTraining(*weightlessNeuronMap[1], "1", 1);
//
//    //Build neuron 2 - it copies the output from neuron 1
//    conMap.clear();
//    conMap[1] = 0;
//    weightlessNeuronMap[2] = new WeightlessNeuron(conMap);
//    addTraining(*weightlessNeuronMap[2], "0", 0);
//    addTraining(*weightlessNeuronMap[2], "1", 1);
//
//    //Build neuron 3 - it copies the output from neuron 4
//    conMap.clear();
//    conMap[4] = 0;
//    weightlessNeuronMap[3] = new WeightlessNeuron(conMap);
//    addTraining(*weightlessNeuronMap[3], "0", 0);
//    addTraining(*weightlessNeuronMap[3], "1", 1);
//
//    //Build neuron 4 - it copies the output from neuron 3
//    conMap.clear();
//    conMap[3] = 0;
//    weightlessNeuronMap[4] = new WeightlessNeuron(conMap);
//    addTraining(*weightlessNeuronMap[4], "0", 0);
//    addTraining(*weightlessNeuronMap[4], "1", 1);
//
//    //Create firing neuron map with current state of the network
//    QHash<unsigned int, bool> firingNeuronMap;
//    firingNeuronMap[2] = true;
//    firingNeuronMap[3] = true;

}


/*! Runs the supplied thread and checks for errors */
void TononiNetworkBuilder::runThread(NetworkDaoThread& thread){
    thread.start();
    thread.wait();
    if(thread.isError()){
	throw SpikeStreamException(thread.getErrorMessage());
    }
}


