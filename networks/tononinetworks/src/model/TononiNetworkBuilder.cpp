//SpikeStream includes
#include "Globals.h"
#include "NetworkDao.h"
#include "NetworkInfo.h"
#include "NeuronGroupInfo.h"
#include "NeuronGroup.h"
#include "TononiNetworkBuilder.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;

//Other includes
#include <algorithm>
using namespace std;

//Define the neuron and synapse types we are using
#define NEURON_TYPE_ID 3
#define SYNAPSE_TYPE_ID 2


/*! Constructor */
TononiNetworkBuilder::TononiNetworkBuilder(){
	/*Set up the network and archive dao.
		These can be created in the constructor because this builder does not run as a separate thread. */
	networkDao = new NetworkDao(Globals::getNetworkDao()->getDBInfo());
	archiveDao = new ArchiveDao(Globals::getArchiveDao()->getDBInfo());
}


/*! Destructor */
TononiNetworkBuilder::~TononiNetworkBuilder(){
	//Clean up database interfaces
	delete networkDao;
	delete archiveDao;
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Adds Balduzzi and Tononi (2008), Figure 5.
    Numbering of neurons starts with 1 at bottom left and across the figure,
    starting at the top of each column. The 28 connections are:
	2->1	3->2
	3->4	3->5	3->6
	4->3	4->5	4->6
	5->3	5->4	5->6
	6->3	6->4	6->5	6->7	6->8	6->10
	7->6	7->8	7->10
	8->6	8->7	8->10
	10->6	10->7	10->8
	9->7	11->9
*/
void TononiNetworkBuilder::addBalduzziTononiFigure5(const QString& networkName, const QString& networkDescription){
    //Add a network
    NetworkInfo netInfo(0, networkName, networkDescription);
	networkDao->addNetwork(netInfo);

    //Build neuron group - keep references to neurons
    QHash<QString, double> paramMap;
	NeuronGroup neurGrp(NeuronGroupInfo(0, "Neuron group 1", "Main neuron group", paramMap, NEURON_TYPE_ID));
    Neuron* neur1 = neurGrp.addNeuron(1, 1, 1);
    Neuron* neur2 = neurGrp.addNeuron(2, 2, 1);
    Neuron* neur3 = neurGrp.addNeuron(3, 3, 1);
    Neuron* neur4 = neurGrp.addNeuron(4, 4, 1);
    Neuron* neur5 = neurGrp.addNeuron(4, 2, 1);
    Neuron* neur6 = neurGrp.addNeuron(5, 3, 1);
    Neuron* neur7 = neurGrp.addNeuron(6, 4, 1);
    Neuron* neur8 = neurGrp.addNeuron(6, 2, 1);
    Neuron* neur9 = neurGrp.addNeuron(7, 5, 1);
    Neuron* neur10 = neurGrp.addNeuron(7, 3, 1);
    Neuron* neur11 = neurGrp.addNeuron(8, 6, 1);

    //Add the neuron group
    DBInfo netDBInfo = networkDao->getDBInfo();
    NetworkDaoThread netDaoThread(netDBInfo);
    netDaoThread.prepareAddNeuronGroup(netInfo.getID(), &neurGrp);
    runThread(netDaoThread);

    //Build the connection group that is to be added
	ConnectionGroupInfo connGrpInfo(0, "Connection Group", neurGrp.getID(), neurGrp.getID(),  paramMap, SYNAPSE_TYPE_ID);
    ConnectionGroup connGrp(connGrpInfo);

    //Add connections
    //2->1	3->2
	Connection* con21 = connGrp.addConnection(neur2->getID(), neur1->getID(),  0,  0);
	Connection* con32 = connGrp.addConnection(neur3->getID(), neur2->getID(),  0,  0);

    //3->4	3->5	3->6
	Connection* con34 = connGrp.addConnection(neur3->getID(), neur4->getID(),  0,  0);
	Connection* con35 = connGrp.addConnection(neur3->getID(), neur5->getID(),  0,  0);
	Connection* con36 = connGrp.addConnection(neur3->getID(), neur6->getID(),  0,  0);

    //4->3	4->5	4->6
	Connection* con43 = connGrp.addConnection(neur4->getID(), neur3->getID(),  0,  0);
	Connection* con45 = connGrp.addConnection(neur4->getID(), neur5->getID(),  0,  0);
	Connection* con46 = connGrp.addConnection(neur4->getID(), neur6->getID(),  0,  0);

    //5->3	5->4	5->6
	Connection* con53 = connGrp.addConnection(neur5->getID(), neur3->getID(),  0,  0);
	Connection* con54 = connGrp.addConnection(neur5->getID(), neur4->getID(),  0,  0);
	Connection* con56 = connGrp.addConnection(neur5->getID(), neur6->getID(),  0,  0);

    //6->3	6->4	6->5	6->7	6->8	6->10
	Connection* con63 = connGrp.addConnection(neur6->getID(), neur3->getID(),  0,  0);
	Connection* con64 = connGrp.addConnection(neur6->getID(), neur4->getID(),  0,  0);
	Connection* con65 = connGrp.addConnection(neur6->getID(), neur5->getID(),  0,  0);
	Connection* con67 = connGrp.addConnection(neur6->getID(), neur7->getID(),  0,  0);
	Connection* con68 = connGrp.addConnection(neur6->getID(), neur8->getID(),  0,  0);
	Connection* con610 = connGrp.addConnection(neur6->getID(), neur10->getID(),  0,  0);

    //7->6	7->8	7->10
	Connection* con76 = connGrp.addConnection(neur7->getID(), neur6->getID(),  0,  0);
	Connection* con78 = connGrp.addConnection(neur7->getID(), neur8->getID(),  0,  0);
	Connection* con710 = connGrp.addConnection(neur7->getID(), neur10->getID(),  0,  0);

    //8->6	8->7	8->10
	Connection* con86 = connGrp.addConnection(neur8->getID(), neur6->getID(),  0,  0);
	Connection* con87 = connGrp.addConnection(neur8->getID(), neur7->getID(),  0,  0);
	Connection* con810 = connGrp.addConnection(neur8->getID(), neur10->getID(),  0,  0);

    //10->6	10->7	10->8
	Connection* con106 = connGrp.addConnection(neur10->getID(), neur6->getID(),  0,  0);
	Connection* con107 = connGrp.addConnection(neur10->getID(), neur7->getID(),  0,  0);
	Connection* con108 = connGrp.addConnection(neur10->getID(), neur8->getID(),  0,  0);

    //9->7	11->9
	Connection* con97 = connGrp.addConnection(neur9->getID(), neur7->getID(),  0,  0);
	Connection* con119 = connGrp.addConnection(neur11->getID(), neur9->getID(),  0,  0);

    netDaoThread.prepareAddConnectionGroup(netInfo.getID(), &connGrp);
    runThread(netDaoThread);

    //Add weightless connections
    //1 connection to 1
    networkDao->addWeightlessConnection(con21->getID(), 0);

    //1 connection to 2
    networkDao->addWeightlessConnection(con32->getID(), 0);

    //3 connections to 3
    networkDao->addWeightlessConnection(con43->getID(), 0);
    networkDao->addWeightlessConnection(con53->getID(), 1);
    networkDao->addWeightlessConnection(con63->getID(), 2);

    //3 connections to 4
    networkDao->addWeightlessConnection(con34->getID(), 0);
    networkDao->addWeightlessConnection(con54->getID(), 1);
    networkDao->addWeightlessConnection(con64->getID(), 2);

    //3 connection to 5
    networkDao->addWeightlessConnection(con35->getID(), 0);
    networkDao->addWeightlessConnection(con45->getID(), 1);
    networkDao->addWeightlessConnection(con65->getID(), 2);

    //6 connections to 6
    networkDao->addWeightlessConnection(con36->getID(), 0);
    networkDao->addWeightlessConnection(con46->getID(), 1);
    networkDao->addWeightlessConnection(con56->getID(), 2);
    networkDao->addWeightlessConnection(con76->getID(), 3);
    networkDao->addWeightlessConnection(con86->getID(), 4);
    networkDao->addWeightlessConnection(con106->getID(), 5);

    //4 connections to 7
    networkDao->addWeightlessConnection(con67->getID(), 0);
    networkDao->addWeightlessConnection(con87->getID(), 1);
    networkDao->addWeightlessConnection(con97->getID(), 2);
    networkDao->addWeightlessConnection(con107->getID(), 3);

    //3 connections to 8
    networkDao->addWeightlessConnection(con68->getID(), 0);
    networkDao->addWeightlessConnection(con78->getID(), 1);
    networkDao->addWeightlessConnection(con108->getID(), 2);

    //1 connection to 9
    networkDao->addWeightlessConnection(con119->getID(), 0);

    //3 connections to 10
    networkDao->addWeightlessConnection(con610->getID(), 0);
    networkDao->addWeightlessConnection(con710->getID(), 1);
    networkDao->addWeightlessConnection(con810->getID(), 2);

    //No connections to 11

    //Add training - Elements are parity gates that fire when they receive an odd number of spikes
    addParityGateTraining(neur1->getID(), 1);
    addParityGateTraining(neur2->getID(), 1);
    addParityGateTraining(neur3->getID(), 3);
    addParityGateTraining(neur4->getID(), 3);
    addParityGateTraining(neur5->getID(), 3);
    addParityGateTraining(neur6->getID(), 6);
    addParityGateTraining(neur7->getID(), 4);
    addParityGateTraining(neur8->getID(), 3);
    addParityGateTraining(neur9->getID(), 1);
    addParityGateTraining(neur10->getID(), 3);

    //Add Archive to hold firing patterns
    ArchiveInfo archiveInfo(0, netInfo.getID(), QDateTime::currentDateTime().toTime_t(), "Single firing pattern present in Balduzzi and Tononi (2008), Figure 5");
	archiveDao->addArchive(archiveInfo);

    //Add archive data
	archiveDao->addArchiveData(
	    archiveInfo.getID(),
	    1,//Time step 1
	    "" //00000000000
    );
}


/*! Adds Balduzzi and Tononi (2008), Figure 6 */
void TononiNetworkBuilder::addBalduzziTononiFigure6(const QString& networkName, const QString& networkDescription){
    //Add a network
    NetworkInfo netInfo(0, networkName, networkDescription);
	networkDao->addNetwork(netInfo);

    //Build neuron group - store neurons in order in a list
    QHash<QString, double> paramMap;
	NeuronGroup neurGrp(NeuronGroupInfo(0, "Neuron group 1", "Main neuron group", paramMap, NEURON_TYPE_ID));
    NeuronMap neuronMap;
    neuronMap[1] = neurGrp.addNeuron(1, 2, 1);
    neuronMap[2] = neurGrp.addNeuron(3, 2, 1);
    neuronMap[3] = neurGrp.addNeuron(2, 1, 1);
    neuronMap[4] = neurGrp.addNeuron(1, 3, 1);
    neuronMap[5] = neurGrp.addNeuron(3, 3, 1);
    neuronMap[6] = neurGrp.addNeuron(4, 1, 1);

    //Add the neuron group
	DBInfo netDBInfo = networkDao->getDBInfo();
    NetworkDaoThread netDaoThread(netDBInfo);
    netDaoThread.prepareAddNeuronGroup(netInfo.getID(), &neurGrp);
    runThread(netDaoThread);

    //Build the connection group that is to be added
	ConnectionGroupInfo connGrpInfo(0, "Connection Group", neurGrp.getID(), neurGrp.getID(),  paramMap, SYNAPSE_TYPE_ID);
    ConnectionGroup connGrp(connGrpInfo);

    //Add connections
	Connection* con12 = connGrp.addConnection(neuronMap[1]->getID(), neuronMap[2]->getID(),  0,  0);
	Connection* con13 = connGrp.addConnection(neuronMap[1]->getID(), neuronMap[3]->getID(),  0,  0);
	Connection* con14 = connGrp.addConnection(neuronMap[1]->getID(), neuronMap[4]->getID(),  0,  0);
	Connection* con21 = connGrp.addConnection(neuronMap[2]->getID(), neuronMap[1]->getID(),  0,  0);
	Connection* con23 = connGrp.addConnection(neuronMap[2]->getID(), neuronMap[3]->getID(),  0,  0);
	Connection* con25 = connGrp.addConnection(neuronMap[2]->getID(), neuronMap[5]->getID(),  0,  0);
	Connection* con31 = connGrp.addConnection(neuronMap[3]->getID(), neuronMap[1]->getID(),  0,  0);
	Connection* con32 = connGrp.addConnection(neuronMap[3]->getID(), neuronMap[2]->getID(),  0,  0);
	Connection* con36 = connGrp.addConnection(neuronMap[3]->getID(), neuronMap[6]->getID(),  0,  0);

    netDaoThread.prepareAddConnectionGroup(netInfo.getID(), &connGrp);
    runThread(netDaoThread);

    //Add weightless connections
    //2 connections to 1
    networkDao->addWeightlessConnection(con21->getID(), 0);
    networkDao->addWeightlessConnection(con31->getID(), 1);

    //2 connections to 2
    networkDao->addWeightlessConnection(con12->getID(), 0);
    networkDao->addWeightlessConnection(con32->getID(), 1);

    //2 connections to 3
    networkDao->addWeightlessConnection(con13->getID(), 0);
    networkDao->addWeightlessConnection(con23->getID(), 1);

    //1 connection to 4
    networkDao->addWeightlessConnection(con14->getID(), 0);

    //1 connection to 5
    networkDao->addWeightlessConnection(con25->getID(), 0);

    //1 connection to 6
    networkDao->addWeightlessConnection(con36->getID(), 0);

    //Add training - 1,2,3 are ANDs; 4,5,6 copy the output of 1,2,3
    addTraining(neuronMap[1]->getID(), "00", 0);
    addTraining(neuronMap[1]->getID(), "01", 0);
    addTraining(neuronMap[1]->getID(), "10", 0);
    addTraining(neuronMap[1]->getID(), "11", 1);

    addTraining(neuronMap[2]->getID(), "00", 0);
    addTraining(neuronMap[2]->getID(), "01", 0);
    addTraining(neuronMap[2]->getID(), "10", 0);
    addTraining(neuronMap[2]->getID(), "11", 1);

    addTraining(neuronMap[3]->getID(), "00", 0);
    addTraining(neuronMap[3]->getID(), "01", 0);
    addTraining(neuronMap[3]->getID(), "10", 0);
    addTraining(neuronMap[3]->getID(), "11", 1);

    addTraining(neuronMap[4]->getID(), "0", 0);
    addTraining(neuronMap[4]->getID(), "1", 1);

    addTraining(neuronMap[5]->getID(), "0", 0);
    addTraining(neuronMap[5]->getID(), "1", 1);

    addTraining(neuronMap[6]->getID(), "0", 0);
    addTraining(neuronMap[6]->getID(), "1", 1);

    //Add Archive to hold firing patterns
    ArchiveInfo archiveInfo(0, netInfo.getID(), QDateTime::currentDateTime().toTime_t(), "Single firing pattern present in Balduzzi and Tononi (2008), Figure 6");
	archiveDao->addArchive(archiveInfo);

    //Add archive data
	archiveDao->addArchiveData(
	    archiveInfo.getID(),
	    1,//Time step 1
	    QString::number(neuronMap[3]->getID()) + "," + QString::number(neuronMap[4]->getID()) + "," + QString::number(neuronMap[5]->getID()) //001110
    );
}


/*! Adds Balduzzi and Tononi (2008), Figure 12.
    Numbering of neurons starts with 1 at top left and goes around clockwise.
    The 12 connections are:
	1->7	2->1	3->4	3->7
	3->8	4->2	5->4	6->2
	6->5	7->8	8->1	8->5
*/
void TononiNetworkBuilder::addBalduzziTononiFigure12(const QString& networkName, const QString& networkDescription){
    //Add a network
    NetworkInfo netInfo(0, networkName, networkDescription);
	networkDao->addNetwork(netInfo);

    //Build neuron group - keep references to neurons
    QHash<QString, double> paramMap;
	NeuronGroup neurGrp(NeuronGroupInfo(0, "Neuron group 1", "Main neuron group", paramMap, NEURON_TYPE_ID));
    Neuron* neur1 = neurGrp.addNeuron(2, 4, 1);
    Neuron* neur2 = neurGrp.addNeuron(3, 4, 1);
    Neuron* neur3 = neurGrp.addNeuron(4, 3, 1);
    Neuron* neur4 = neurGrp.addNeuron(4, 2, 1);
    Neuron* neur5 = neurGrp.addNeuron(3, 1, 1);
    Neuron* neur6 = neurGrp.addNeuron(2, 1, 1);
    Neuron* neur7 = neurGrp.addNeuron(1, 2, 1);
    Neuron* neur8 = neurGrp.addNeuron(1, 3, 1);

    //Add the neuron group
    DBInfo netDBInfo = networkDao->getDBInfo();
    NetworkDaoThread netDaoThread(netDBInfo);
    netDaoThread.prepareAddNeuronGroup(netInfo.getID(), &neurGrp);
    runThread(netDaoThread);

    //Build the connection group that is to be added
	ConnectionGroupInfo connGrpInfo(0, "Connection Group", neurGrp.getID(), neurGrp.getID(),  paramMap, SYNAPSE_TYPE_ID);
    ConnectionGroup connGrp(connGrpInfo);

    //Add connections
	Connection* con17 = connGrp.addConnection(neur1->getID(), neur7->getID(),  0,  0);
	Connection* con21 = connGrp.addConnection(neur2->getID(), neur1->getID(),  0,  0);
	Connection* con34 = connGrp.addConnection(neur3->getID(), neur4->getID(),  0,  0);
	Connection* con37 = connGrp.addConnection(neur3->getID(), neur7->getID(),  0,  0);

	Connection* con38 = connGrp.addConnection(neur3->getID(), neur8->getID(),  0,  0);
	Connection* con42 = connGrp.addConnection(neur4->getID(), neur2->getID(),  0,  0);
	Connection* con54 = connGrp.addConnection(neur5->getID(), neur4->getID(),  0,  0);
	Connection* con62 = connGrp.addConnection(neur6->getID(), neur2->getID(),  0,  0);

	Connection* con65 = connGrp.addConnection(neur6->getID(), neur5->getID(),  0,  0);
	Connection* con78 = connGrp.addConnection(neur7->getID(), neur8->getID(),  0,  0);
	Connection* con81 = connGrp.addConnection(neur8->getID(), neur1->getID(),  0,  0);
	Connection* con85 = connGrp.addConnection(neur8->getID(), neur5->getID(),  0,  0);

    netDaoThread.prepareAddConnectionGroup(netInfo.getID(), &connGrp);
    runThread(netDaoThread);

    //Add weightless connections
    //2 connections to 1
    networkDao->addWeightlessConnection(con21->getID(), 0);
    networkDao->addWeightlessConnection(con81->getID(), 1);

    //2 connections to 2
    networkDao->addWeightlessConnection(con42->getID(), 0);
    networkDao->addWeightlessConnection(con62->getID(), 1);

    //0 connections to 3

    //2 connections to 4
    networkDao->addWeightlessConnection(con34->getID(), 0);
    networkDao->addWeightlessConnection(con54->getID(), 1);

    //2 connection to 5
    networkDao->addWeightlessConnection(con65->getID(), 0);
    networkDao->addWeightlessConnection(con85->getID(), 1);

    //0 connections to 6

    //2 connections to 7
    networkDao->addWeightlessConnection(con17->getID(), 0);
    networkDao->addWeightlessConnection(con37->getID(), 1);

    //2 connections to 8
    networkDao->addWeightlessConnection(con38->getID(), 0);
    networkDao->addWeightlessConnection(con78->getID(), 1);

    //Add training - all are ANDs, but 3 and 6 don't receive any connections, so don't need training
    addTraining(neur1->getID(), "00", 0);
    addTraining(neur1->getID(), "01", 0);
    addTraining(neur1->getID(), "10", 0);
    addTraining(neur1->getID(), "11", 1);

    addTraining(neur2->getID(), "00", 0);
    addTraining(neur2->getID(), "01", 0);
    addTraining(neur2->getID(), "10", 0);
    addTraining(neur2->getID(), "11", 1);

    addTraining(neur4->getID(), "00", 0);
    addTraining(neur4->getID(), "01", 0);
    addTraining(neur4->getID(), "10", 0);
    addTraining(neur4->getID(), "11", 1);

    addTraining(neur5->getID(), "00", 0);
    addTraining(neur5->getID(), "01", 0);
    addTraining(neur5->getID(), "10", 0);
    addTraining(neur5->getID(), "11", 1);

    addTraining(neur7->getID(), "00", 0);
    addTraining(neur7->getID(), "01", 0);
    addTraining(neur7->getID(), "10", 0);
    addTraining(neur7->getID(), "11", 1);

    addTraining(neur8->getID(), "00", 0);
    addTraining(neur8->getID(), "01", 0);
    addTraining(neur8->getID(), "10", 0);
    addTraining(neur8->getID(), "11", 1);

    //Add Archive to hold firing patterns
    ArchiveInfo archiveInfo(0, netInfo.getID(), QDateTime::currentDateTime().toTime_t(), "Single firing pattern present in Balduzzi and Tononi (2008), Figure 12");
	archiveDao->addArchive(archiveInfo);

    //Add archive data
	archiveDao->addArchiveData(
	    archiveInfo.getID(),
	    1,//Time step 1
	    QString::number(neur4->getID()) + "," + QString::number(neur5->getID()) //000110000
    );
}


/*! Adds Balduzzi and Tononi (2008), Figure 13.
    Numbering of neurons is clockwise within each group, working through each group clockwise
    So neuron at top left has number 1; neuron at bottom right has number 8.
    The connections are as follows:
	1->2	1->3
	2->1	2->3
	3->1	3->2	3->6	3->7
	4->5	4->6
	5->4	5->6
	6->3	6->4	6->5	6->10
	7->3	7->8	7->9	7->10
	8->7	8->9
	9->7	9->8
	10->6	10->7	10->11	10->12
	11->10	11->12
	12->10	12->11
*/
void TononiNetworkBuilder::addBalduzziTononiFigure13(const QString& networkName, const QString& networkDescription){
    //Add a network
    NetworkInfo netInfo(0, networkName, networkDescription);
	networkDao->addNetwork(netInfo);

    //Build neuron group - keep references to neurons
    QHash<QString, double> paramMap;
	NeuronGroup neurGrp(NeuronGroupInfo(0, "Neuron group 1", "Main neuron group", paramMap, NEURON_TYPE_ID));
    Neuron* neur1 = neurGrp.addNeuron(1, 4, 1);
    Neuron* neur2 = neurGrp.addNeuron(3, 4, 1);
    Neuron* neur3 = neurGrp.addNeuron(2, 3, 1);
    Neuron* neur4 = neurGrp.addNeuron(5, 4, 1);
    Neuron* neur5 = neurGrp.addNeuron(7, 4, 1);
    Neuron* neur6 = neurGrp.addNeuron(6, 3, 1);
    Neuron* neur7 = neurGrp.addNeuron(6, 2, 1);
    Neuron* neur8 = neurGrp.addNeuron(7, 1, 1);
    Neuron* neur9 = neurGrp.addNeuron(5, 1, 1);
    Neuron* neur10 = neurGrp.addNeuron(2, 2, 1);
    Neuron* neur11 = neurGrp.addNeuron(3, 1, 1);
    Neuron* neur12 = neurGrp.addNeuron(1, 1, 1);

    //Add the neuron group
    DBInfo netDBInfo = networkDao->getDBInfo();
    NetworkDaoThread netDaoThread(netDBInfo);
    netDaoThread.prepareAddNeuronGroup(netInfo.getID(), &neurGrp);
    runThread(netDaoThread);

    //Build the connection group that is to be added
	ConnectionGroupInfo connGrpInfo(0, "Connection Group", neurGrp.getID(), neurGrp.getID(),  paramMap, SYNAPSE_TYPE_ID);
    ConnectionGroup connGrp(connGrpInfo);

    //1->2	1->3
	Connection* con12 = connGrp.addConnection(neur1->getID(), neur2->getID(),  0,  0);
	Connection* con13 = connGrp.addConnection(neur1->getID(), neur3->getID(),  0,  0);

    //2->1	2->3
	Connection* con21 = connGrp.addConnection(neur2->getID(), neur1->getID(),  0,  0);
	Connection* con23 = connGrp.addConnection(neur2->getID(), neur3->getID(),  0,  0);

    //3->1	3->2	3->6	3->7
	Connection* con31 = connGrp.addConnection(neur3->getID(), neur1->getID(),  0,  0);
	Connection* con32 = connGrp.addConnection(neur3->getID(), neur2->getID(),  0,  0);
	Connection* con36 = connGrp.addConnection(neur3->getID(), neur6->getID(),  0,  0);
	Connection* con37 = connGrp.addConnection(neur3->getID(), neur7->getID(),  0,  0);

    //4->5	4->6
	Connection* con45 = connGrp.addConnection(neur4->getID(), neur5->getID(),  0,  0);
	Connection* con46 = connGrp.addConnection(neur4->getID(), neur6->getID(),  0,  0);

    //5->4	5->6
	Connection* con54 = connGrp.addConnection(neur5->getID(), neur4->getID(),  0,  0);
	Connection* con56 = connGrp.addConnection(neur5->getID(), neur6->getID(),  0,  0);

    //6->3	6->4	6->5	6->10
	Connection* con63 = connGrp.addConnection(neur6->getID(), neur3->getID(),  0,  0);
	Connection* con64 = connGrp.addConnection(neur6->getID(), neur4->getID(),  0,  0);
	Connection* con65 = connGrp.addConnection(neur6->getID(), neur5->getID(),  0,  0);
	Connection* con610 = connGrp.addConnection(neur6->getID(), neur10->getID(),  0,  0);

    //7->3	7->8	7->9	7->10
	Connection* con73 = connGrp.addConnection(neur7->getID(), neur3->getID(),  0,  0);
	Connection* con78 = connGrp.addConnection(neur7->getID(), neur8->getID(),  0,  0);
	Connection* con79 = connGrp.addConnection(neur7->getID(), neur9->getID(),  0,  0);
	Connection* con710 = connGrp.addConnection(neur7->getID(), neur10->getID(),  0,  0);

    //8->7	8->9
	Connection* con87 = connGrp.addConnection(neur8->getID(), neur7->getID(),  0,  0);
	Connection* con89 = connGrp.addConnection(neur8->getID(), neur9->getID(),  0,  0);

    //9->7	9->8
	Connection* con97 = connGrp.addConnection(neur9->getID(), neur7->getID(),  0,  0);
	Connection* con98 = connGrp.addConnection(neur9->getID(), neur8->getID(),  0,  0);

    //10->6	10->7	10->11	10->12
	Connection* con106 = connGrp.addConnection(neur10->getID(), neur6->getID(),  0,  0);
	Connection* con107 = connGrp.addConnection(neur10->getID(), neur7->getID(),  0,  0);
	Connection* con1011 = connGrp.addConnection(neur10->getID(), neur11->getID(),  0,  0);
	Connection* con1012 = connGrp.addConnection(neur10->getID(), neur12->getID(),  0,  0);

    //11->10	11->12
	Connection* con1110 = connGrp.addConnection(neur11->getID(), neur10->getID(),  0,  0);
	Connection* con1112 = connGrp.addConnection(neur11->getID(), neur12->getID(),  0,  0);

    //12->10	12->11
	Connection* con1210 = connGrp.addConnection(neur12->getID(), neur10->getID(),  0,  0);
	Connection* con1211 = connGrp.addConnection(neur12->getID(), neur11->getID(),  0,  0);

    netDaoThread.prepareAddConnectionGroup(netInfo.getID(), &connGrp);
    runThread(netDaoThread);


    //Add weightless connections
    //2 connections to 1
    networkDao->addWeightlessConnection(con21->getID(), 0);
    networkDao->addWeightlessConnection(con31->getID(), 1);

    //2 connections to 2
    networkDao->addWeightlessConnection(con12->getID(), 0);
    networkDao->addWeightlessConnection(con32->getID(), 1);

    //4 connections to 3
    networkDao->addWeightlessConnection(con13->getID(), 0);
    networkDao->addWeightlessConnection(con23->getID(), 1);
    networkDao->addWeightlessConnection(con63->getID(), 2);
    networkDao->addWeightlessConnection(con73->getID(), 3);

    //2 connections to 4
    networkDao->addWeightlessConnection(con54->getID(), 0);
    networkDao->addWeightlessConnection(con64->getID(), 1);

    //2 connections to 5
    networkDao->addWeightlessConnection(con45->getID(), 0);
    networkDao->addWeightlessConnection(con65->getID(), 1);

    //4 connections to 6
    networkDao->addWeightlessConnection(con36->getID(), 0);
    networkDao->addWeightlessConnection(con46->getID(), 1);
    networkDao->addWeightlessConnection(con56->getID(), 2);
    networkDao->addWeightlessConnection(con106->getID(), 3);

    //4 connections to 7
    networkDao->addWeightlessConnection(con37->getID(), 0);
    networkDao->addWeightlessConnection(con87->getID(), 1);
    networkDao->addWeightlessConnection(con97->getID(), 2);
    networkDao->addWeightlessConnection(con107->getID(), 3);

    //2 connections to 8
    networkDao->addWeightlessConnection(con78->getID(), 0);
    networkDao->addWeightlessConnection(con98->getID(), 1);

    //2 connections to 9
    networkDao->addWeightlessConnection(con79->getID(), 0);
    networkDao->addWeightlessConnection(con89->getID(), 1);

    //4 connections to 10
    networkDao->addWeightlessConnection(con610->getID(), 0);
    networkDao->addWeightlessConnection(con710->getID(), 1);
    networkDao->addWeightlessConnection(con1110->getID(), 2);
    networkDao->addWeightlessConnection(con1210->getID(), 3);

    //2 connections to 11
    networkDao->addWeightlessConnection(con1011->getID(), 0);
    networkDao->addWeightlessConnection(con1211->getID(), 1);

    //2 connections to 12
    networkDao->addWeightlessConnection(con1012->getID(), 0);
    networkDao->addWeightlessConnection(con1112->getID(), 1);

    //Add training - Elements fire when they receive two or more spikes
    addTwoOrMoreTraining(neur1->getID(), 2);
    addTwoOrMoreTraining(neur2->getID(), 2);
    addTwoOrMoreTraining(neur3->getID(), 4);
    addTwoOrMoreTraining(neur4->getID(), 2);
    addTwoOrMoreTraining(neur5->getID(), 2);
    addTwoOrMoreTraining(neur6->getID(), 4);
    addTwoOrMoreTraining(neur7->getID(), 4);
    addTwoOrMoreTraining(neur8->getID(), 2);
    addTwoOrMoreTraining(neur9->getID(), 2);
    addTwoOrMoreTraining(neur10->getID(), 4);
    addTwoOrMoreTraining(neur11->getID(), 2);
    addTwoOrMoreTraining(neur12->getID(), 2);

    //Add Archive to hold firing patterns
    ArchiveInfo archiveInfo(0, netInfo.getID(), QDateTime::currentDateTime().toTime_t(), "Single firing pattern present in Balduzzi and Tononi (2008), Figure 5");
	archiveDao->addArchive(archiveInfo);

    //Add archive data
	archiveDao->addArchiveData(
	    archiveInfo.getID(),
	    1,//Time step 1
	    "" //00000000000
    );
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Adds parity gate training to the neuron.
    A parity gate fires if it receives an odd number of spikes. */
void TononiNetworkBuilder::addParityGateTraining(unsigned int neuronID, int numberOfConnections){
    //Create array to select subsets
    bool truthTableArray[numberOfConnections];

    for(int numOnes = 0; numOnes <= numberOfConnections; ++ numOnes){
	//Fill permutation array with initial selection
	Util::fillSelectionArray(truthTableArray, numberOfConnections, numOnes);

	//Work through all permutations at this subset size
	bool permutationsComplete = false;
	while(!permutationsComplete){

	    //Build training string from subset selection array
	    QString trainingStr = "";
	    for(int i=0; i<numberOfConnections; ++i){
		if(truthTableArray[i])
		    trainingStr += "1";
		else
		    trainingStr += "0";
	    }

	    //Add the training
	    if(numOnes % 2 == 0)
		addTraining(neuronID, trainingStr, 0);
	    else
		addTraining(neuronID, trainingStr, 1);

	    //Get the next permutation with this number of ones
    	    permutationsComplete = !next_permutation(&truthTableArray[0], &truthTableArray[numberOfConnections]);
	}
    }
}

void TononiNetworkBuilder::addTwoOrMoreTraining(unsigned int neuronID, int numberOfConnections){
    //Create array to select subsets
    bool truthTableArray[numberOfConnections];

    for(int numOnes = 0; numOnes <= numberOfConnections; ++ numOnes){
	//Fill permutation array with initial selection
	Util::fillSelectionArray(truthTableArray, numberOfConnections, numOnes);

	//Work through all permutations at this subset size
	bool permutationsComplete = false;
	while(!permutationsComplete){

	    //Build training string from subset selection array
	    QString trainingStr = "";
	    for(int i=0; i<numberOfConnections; ++i){
		if(truthTableArray[i])
		    trainingStr += "1";
		else
		    trainingStr += "0";
	    }

	    //Add the training
	    if(numOnes >= 2)
		addTraining(neuronID, trainingStr, 1);
	    else
		addTraining(neuronID, trainingStr, 0);

	    //Get the next permutation with this number of ones
    	    permutationsComplete = !next_permutation(&truthTableArray[0], &truthTableArray[numberOfConnections]);
	}
    }
}
