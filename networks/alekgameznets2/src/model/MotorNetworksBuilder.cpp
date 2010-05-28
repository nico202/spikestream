//SpikeStream includes
#include "MotorNetworksBuilder.h"
#include "Globals.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QProgressDialog>

//Other includes
#include <algorithm>
using namespace std;

//Define the neuron and synapse types we are using
#define NEURON_TYPE_ID 3
#define SYNAPSE_TYPE_ID 2


/*! Constructor  */
MotorNetworksBuilder::MotorNetworksBuilder() : NetworksBuilder(){

	//Create array to store connections for each neuron to add weightless indexes later when we have the connection id
	conListArray = new QList<Connection*>[12];
}


/*! Destructor */
MotorNetworksBuilder::~MotorNetworksBuilder(){
	delete [] conListArray;
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Adds all of the fully connected networks */
void MotorNetworksBuilder::prepareAddNetworks(const QString& networkName, const QString& networkDescription, const bool* stop){
	this->networkName = networkName;
	this->networkDescription = networkDescription;
	this->stop = stop;
}


/*! Run method inherited from QThread */
void MotorNetworksBuilder::run(){
	clearError();

	// Set up the network and archive daos within the new thread
	networkDao = new NetworkDao(Globals::getNetworkDao()->getDBInfo());
	archiveDao = new ArchiveDao(Globals::getArchiveDao()->getDBInfo());

	try{
		emit progress(0, 4);

		//Add nework with training so each neuron fires when 100% of its input neurons are firing
		srand(123456789);//Seed the random number generator
		addBasicNetwork(networkName, networkDescription + " \n100% training");
		addTraining(100);

		emit progress(1, 4);
		if (*stop)
			return;

		//Add nework with training so each neuron fires when 75% of its input neurons are firing
		srand(123456789);//Seed the random number generator
		addBasicNetwork(networkName, networkDescription + " \n75% training");
		addTraining(75);

		emit progress(2, 4);
		if (*stop)
			return;

		//Add nework with training so each neuron fires when 50% of its input neurons are firing
		srand(123456789);//Seed the random number generator
		addBasicNetwork(networkName, networkDescription + " \n50% training");
		addTraining(50);

		emit progress(3, 4);
		if (*stop)
			return;

		//Add nework with training so each neuron fires when 25% of its input neurons are firing
		srand(123456789);//Seed the random number generator
		addBasicNetwork(networkName, networkDescription + " \n25% training");
		addTraining(25);
		emit progress(4, 4);

		//Clean up network and archive dao
		delete networkDao;
		delete archiveDao;
	}
	catch(SpikeStreamException& ex){
		setError(ex.getMessage());
	}
	catch(...){
		setError("Unknown Exception occurred.");
	}
}


/*--------------------------------------------------------*/
/*-------             PRIVATE METHODS              -------*/
/*--------------------------------------------------------*/

/*! Resets the class and adds the basic 12 neuron network along with the firing patterns */
void MotorNetworksBuilder::addBasicNetwork(const QString& networkName, const QString& networkDescription){
	//Reset this class
	reset();

	//Add network
	NetworkInfo netInfo(0, networkName, networkDescription);
	networkDao->addNetwork(netInfo);
	networkID = netInfo.getID();

	//Build neuron group - store neurons in list and then put ids in map
	QHash<QString, double> paramMap;
	NeuronGroup neuronGroup(NeuronGroupInfo(0, "Neuron group 1", "Main neuron group", paramMap, NEURON_TYPE_ID));
	QList<Neuron*> neuronList;
	neuronList.append(neuronGroup.addNeuron(3, 1, 1));
	neuronList.append(neuronGroup.addNeuron(5, 2, 1));
	neuronList.append(neuronGroup.addNeuron(5, 7, 1));
	neuronList.append(neuronGroup.addNeuron(3, 8, 1));
	neuronList.append(neuronGroup.addNeuron(1, 7, 1));
	neuronList.append(neuronGroup.addNeuron(1, 2, 1));
	neuronList.append(neuronGroup.addNeuron(3, 3, 1));
	neuronList.append(neuronGroup.addNeuron(4, 4, 1));
	neuronList.append(neuronGroup.addNeuron(4, 5, 1));
	neuronList.append(neuronGroup.addNeuron(3, 6, 1));
	neuronList.append(neuronGroup.addNeuron(2, 5, 1));
	neuronList.append(neuronGroup.addNeuron(2, 4, 1));

	//Add the neuron group
	DBInfo netDBInfo = networkDao->getDBInfo();
	NetworkDaoThread netDaoThread(netDBInfo);
	netDaoThread.prepareAddNeuronGroup(netInfo.getID(), &neuronGroup);
	runThread(netDaoThread);
	neuronGroupID = neuronGroup.getID();
	for(int i=0; i<neuronList.size(); ++i)
		neuronMap[i+1] = neuronList.at(i)->getID();

	//Add the connections
	addConnections();

	//Add archives
	addFiringPatterns();
}


/*! Adds the connections */
void MotorNetworksBuilder::addConnections(){
	//Build the connection group that is to be added
	QHash<QString, double> paramMap;
	ConnectionGroupInfo connGrpInfo(0, "Connection Group", neuronGroupID, neuronGroupID,  paramMap, SYNAPSE_TYPE_ID);
	ConnectionGroup connGrp(connGrpInfo);

	/* Add connections
		Center cluster is fully connected together; Each neuron at the periphery has a single connection
		from a neuron in the center cluster. */

	//Add connections within the center cluster
	for(unsigned int toNeurIndx = 7; toNeurIndx <=12; ++ toNeurIndx){
		for(unsigned int fromNeurIndx = 7; fromNeurIndx <=12; ++fromNeurIndx)
			if(fromNeurIndx != toNeurIndx)
				conListArray[toNeurIndx-1].append( connGrp.addConnection( new Connection(neuronMap[fromNeurIndx], neuronMap[toNeurIndx],  0,  0,  0) ) );
	}

	//Add connections from center to periphery
	conListArray[1-1].append( connGrp.addConnection( new Connection(neuronMap[7], neuronMap[1],  0,  0,  0) ) );
	conListArray[2-1].append( connGrp.addConnection( new Connection(neuronMap[8], neuronMap[2],  0,  0,  0) ) );
	conListArray[3-1].append( connGrp.addConnection( new Connection(neuronMap[9], neuronMap[3],  0,  0,  0) ) );
	conListArray[4-1].append( connGrp.addConnection( new Connection(neuronMap[10], neuronMap[4],  0,  0,  0) ) );
	conListArray[5-1].append( connGrp.addConnection( new Connection(neuronMap[11], neuronMap[5],  0,  0,  0) ) );
	conListArray[6-1].append( connGrp.addConnection( new Connection(neuronMap[12], neuronMap[6],  0,  0,  0) ) );

	//Add connection group to the database
	addConnectionGroup(networkID, connGrp);

	//Add the weightless index of the connection each runs from 0 to 12
	for(unsigned int toNeurID=1; toNeurID <= 12; ++toNeurID){
		//Add pattern indexes for this TO neuron
		for(int patternIndx=0; patternIndx < conListArray[toNeurID-1].size(); ++patternIndx)
			networkDao->addWeightlessConnection( conListArray[toNeurID-1].at(patternIndx)->getID(), patternIndx );
	}
}


/*! Adds the firing patterns for the network */
void MotorNetworksBuilder::addFiringPatterns(){
	//Create archive
	ArchiveInfo archiveInfo(0, networkID, QDateTime::currentDateTime().toTime_t(), "Combinations of center and perhiphery neurons firing.");
	archiveDao->addArchive(archiveInfo);

	//First firing pattern has 0 neurons firing
	archiveDao->addArchiveData(archiveInfo.getID(), 1, "");

	//Second firing pattern has 50% of center neurons firing
	QString firingStr = "";
	QHash<unsigned int, bool> selectionMap;
	while(selectionMap.size() < 3){
		selectionMap[ neuronMap[rand() % 6 + 7] ] = true;
	}
	for(QHash<unsigned int, bool>::iterator iter = selectionMap.begin(); iter != selectionMap.end(); ++iter)
		firingStr += QString::number(iter.key()) + ",";
	firingStr.truncate(firingStr.length() - 1);
	archiveDao->addArchiveData(archiveInfo.getID(), 2, firingStr);

	//Third firing pattern has 50% of peripheral neurons firing
	firingStr = "";
	selectionMap.clear();
	while(selectionMap.size() < 3){
		selectionMap[ neuronMap[rand() % 6 + 1] ] = true;
	}
	for(QHash<unsigned int, bool>::iterator iter = selectionMap.begin(); iter != selectionMap.end(); ++iter)
		firingStr += QString::number(iter.key()) + ",";
	firingStr.truncate(firingStr.length() - 1);
	archiveDao->addArchiveData(archiveInfo.getID(), 3, firingStr);

	//Fourth firing pattern has all center neurons firing and periphery quiescent.
	firingStr = "";
	for(unsigned int neurIndx = 7; neurIndx <=11; ++neurIndx)
		firingStr += QString::number(neuronMap[neurIndx]) + ",";
	firingStr += QString::number(neuronMap[12]);
	archiveDao->addArchiveData(archiveInfo.getID(), 4, firingStr);

	//Fifth firing pattern has all perphery neurons firing and center quiescent.
	firingStr = "";
	for(unsigned int neurIndx = 1; neurIndx <=5; ++neurIndx)
		firingStr += QString::number(neuronMap[neurIndx]) + ",";
	firingStr += QString::number(neuronMap[6]);
	archiveDao->addArchiveData(archiveInfo.getID(), 5, firingStr);

	//Sixth firing pattern has 50% neurons firing at random
	firingStr = "";
	selectionMap.clear();
	while(selectionMap.size() < 6){
		selectionMap[ neuronMap[rand() % 12 + 1] ] = true;
	}
	for(QHash<unsigned int, bool>::iterator iter = selectionMap.begin(); iter != selectionMap.end(); ++iter)
		firingStr += QString::number(iter.key()) + ",";
	firingStr.truncate(firingStr.length() - 1);
	archiveDao->addArchiveData(archiveInfo.getID(), 6, firingStr);
}


/*! Adds training to the network. */
void MotorNetworksBuilder::addTraining(unsigned int percentInputs){

	//Neurons have different numbers of connections so work through them in turn
	for(unsigned int toNeurIndx = 1; toNeurIndx <=12; ++toNeurIndx){

		//Number of connections varies with each neuron
		int numberOfConnections = conListArray[toNeurIndx-1].size();

		//Neuron will output 1 if the number of input neurons firing is greater than or equal to the threshold
		int threshold = (int) rint( numberOfConnections * ((double)percentInputs / 100.0 ) );
		int output = 0;

		//Work through all permutations  of 1s and 0s on the inputs to the neuron
		bool selectionArray[numberOfConnections];
		int numSelected = 0;
		while(numSelected <= numberOfConnections){
			Util::fillSelectionArray(selectionArray, numberOfConnections, numSelected);

			//Work through all permutations at this selection size
			bool permutationsComplete = false;
			while(!permutationsComplete){

				//Build the training string from the selection array
				QString trainingStr = "";
				for(int i=0; i<numberOfConnections; ++i){
					if(selectionArray[i])
						trainingStr += "1";
					else
						trainingStr += "0";
				}

				//Output is 1 if the number of neurons firing is greater than or equal to the threshold
				if(numSelected >= threshold)
					output = 1;
				else
					output = 0;

				//Add the training to the neuron
				addTraining(neuronMap[toNeurIndx], trainingStr, output);

				//Get the next permutation
				permutationsComplete = !next_permutation(&selectionArray[0], &selectionArray[numberOfConnections]);
			}

			//Increase the selection size
			++numSelected;
		}
	}
}


/*! Resets this class, except for the firing patterns */
void MotorNetworksBuilder::reset(){
	networkID = 0;
	neuronGroupID = 0;
	neuronMap.clear();

	for(int i=0; i<12; ++i)
		conListArray[i].clear();

	clearError();
}

