//SpikeStream includes
#include "ModularNetworksBuilder.h"
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
ModularNetworksBuilder::ModularNetworksBuilder() : NetworksBuilder(){

	//Create array to store connections for each neuron to add weightless indexes later when we have the connection id
	conListArray = new QList<unsigned>[12];
}


/*! Destructor */
ModularNetworksBuilder::~ModularNetworksBuilder(){
	delete [] conListArray;
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Adds all of the fully connected networks */
void ModularNetworksBuilder::prepareAddNetworks(const QString& networkName, const QString& networkDescription, const bool* stop){
	this->networkName = networkName;
	this->networkDescription = networkDescription;
	this->stop = stop;
}


/*! Run method inherited from QThread */
void ModularNetworksBuilder::run(){
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
void ModularNetworksBuilder::addBasicNetwork(const QString& networkName, const QString& networkDescription){
	//Reset this class
	reset();

	//Add network
	NetworkInfo netInfo(0, networkName, networkDescription);
	networkDao->addNetwork(netInfo);
	networkID = netInfo.getID();

	//Build neuron group - store neurons in list and then put ids in map
	QHash<QString, double> paramMap;
	NeuronGroup neuronGroup(NeuronGroupInfo(0, "Neuron group 1", "Main neuron group", paramMap, networkDao->getNeuronType(NEURON_TYPE_ID)));
	QList<Neuron*> neuronList;
	neuronList.append(neuronGroup.addNeuron(1, 5, 1));
	neuronList.append(neuronGroup.addNeuron(2, 5, 1));
	neuronList.append(neuronGroup.addNeuron(2, 6, 1));
	neuronList.append(neuronGroup.addNeuron(1, 6, 1));
	neuronList.append(neuronGroup.addNeuron(5, 1, 1));
	neuronList.append(neuronGroup.addNeuron(6, 2, 1));
	neuronList.append(neuronGroup.addNeuron(5, 3, 1));
	neuronList.append(neuronGroup.addNeuron(4, 2, 1));
	neuronList.append(neuronGroup.addNeuron(8, 5, 1));
	neuronList.append(neuronGroup.addNeuron(9, 5, 1));
	neuronList.append(neuronGroup.addNeuron(9, 6, 1));
	neuronList.append(neuronGroup.addNeuron(8, 6, 1));

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
void ModularNetworksBuilder::addConnections(){
	//Build the connection group that is to be added
	QHash<QString, double> paramMap;
	ConnectionGroupInfo connGrpInfo(0, "Connection Group", neuronGroupID, neuronGroupID,  paramMap, networkDao->getSynapseType(SYNAPSE_TYPE_ID));
	ConnectionGroup connGrp(connGrpInfo);

	/* Add connections
		Each neuron receives connections from all neurons within the same cluster */

	//FIRST MODULE
	//Add connections within the module
	for(unsigned int toNeurIndx = 1; toNeurIndx <=4; ++ toNeurIndx){
		for(unsigned int fromNeurIndx = 1; fromNeurIndx <=4; ++fromNeurIndx)
			if(fromNeurIndx != toNeurIndx)
				conListArray[toNeurIndx-1].append( connGrp.addConnection( neuronMap[fromNeurIndx], neuronMap[toNeurIndx],  0,  0) );
	}
	//Add single connection from other module
	conListArray[3-1].append( connGrp.addConnection(neuronMap[12], neuronMap[3],  0,  0) );

	//SECOND MODULE
	//Add connections within the module
	for(unsigned int toNeurIndx = 5; toNeurIndx <=8; ++ toNeurIndx){
		for(unsigned int fromNeurIndx = 5; fromNeurIndx <=8; ++fromNeurIndx)
			if(fromNeurIndx != toNeurIndx)
				conListArray[toNeurIndx-1].append( connGrp.addConnection(neuronMap[fromNeurIndx], neuronMap[toNeurIndx],  0,  0) );
	}
	//Add single connection from other module
	conListArray[8-1].append( connGrp.addConnection(neuronMap[1], neuronMap[8],  0,  0) );

	//THIRD MODULE
	//Add connections within the module
	for(unsigned int toNeurIndx = 9; toNeurIndx <=12; ++ toNeurIndx){
		for(unsigned int fromNeurIndx = 9; fromNeurIndx <=12; ++fromNeurIndx)
			if(fromNeurIndx != toNeurIndx)
				conListArray[toNeurIndx-1].append( connGrp.addConnection(neuronMap[fromNeurIndx], neuronMap[toNeurIndx],  0,  0) );
	}
	//Add single connection from other module
	conListArray[10-1].append( connGrp.addConnection(neuronMap[6], neuronMap[10],  0,  0) );

	//Add connection group to the database
	addConnectionGroup(networkID, connGrp);

	//Add the weightless index of the connection each runs from 0 to 12
	for(unsigned int toNeurID=1; toNeurID <= 12; ++toNeurID){
		//Add pattern indexes for this TO neuron
		for(int patternIndx=0; patternIndx < conListArray[toNeurID-1].size(); ++patternIndx)
			networkDao->addWeightlessConnection( conListArray[toNeurID-1].at(patternIndx), patternIndx );
	}
}


/*! Adds the firing patterns for the network */
void ModularNetworksBuilder::addFiringPatterns(){
	//Create archive
	ArchiveInfo archiveInfo(0, networkID, QDateTime::currentDateTime().toTime_t(), "0,1,2,3 or 4 randomly selected neurons firing per module");
	archiveDao->addArchive(archiveInfo);

	//First firing pattern has 0 neurons firing
	archiveDao->addArchiveData(archiveInfo.getID(), 1, "");//0%

	//Second firing pattern has 1 randomly selected neuron firing in each module
	QString firingStr = "";
	firingStr += QString::number(neuronMap[rand() % 4 + 1]) + ",";
	firingStr += QString::number(neuronMap[rand() % 4 + 5]) + ",";
	firingStr += QString::number(neuronMap[rand() % 4 + 9]);
	archiveDao->addArchiveData(archiveInfo.getID(), 2, firingStr);

	//Third firing pattern has 2 randomly selected neurons firing in each module
	firingStr = QString::number(neuronMap[rand() % 2 + 1]) + "," + QString::number(neuronMap[rand() % 2 + 3]) + ",";
	firingStr += QString::number(neuronMap[rand() % 2 + 5]) + "," + QString::number(neuronMap[rand() % 2 + 7]) + ",";
	firingStr += QString::number(neuronMap[rand() % 2 + 9]) + "," + QString::number(neuronMap[rand() % 2 + 11]);
	archiveDao->addArchiveData(archiveInfo.getID(), 3, firingStr);

	//Fourth firing pattern has 1 randomly selected neuron that does NOT fire in each module
	unsigned int notFire1 = rand() % 4 + 1, notFire2 =  rand() % 4 + 5, notFire3 =  rand() % 4 + 11;
	firingStr = "";
	for(unsigned int neurIndx = 1; neurIndx <= 12; ++neurIndx){
		if(neurIndx != notFire1 && neurIndx != notFire2 && neurIndx != notFire3)
			firingStr += QString::number(neuronMap[neurIndx]) + ",";
	}
	firingStr.truncate(firingStr.length() - 1);
	archiveDao->addArchiveData(archiveInfo.getID(), 4, firingStr);

	//Fifth firing pattern has all neurons firing
	firingStr = "";
	for(unsigned int neurIndx = 1; neurIndx <= 12; ++neurIndx){
		firingStr += QString::number(neuronMap[neurIndx]) + ",";
	}
	firingStr.truncate(firingStr.length() - 1);
	archiveDao->addArchiveData(archiveInfo.getID(), 5, firingStr);
}


/*! Adds training to the network. */
void ModularNetworksBuilder::addTraining(unsigned int percentInputs){

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
void ModularNetworksBuilder::reset(){
	networkID = 0;
	neuronGroupID = 0;
	neuronMap.clear();

	for(int i=0; i<12; ++i)
		conListArray[i].clear();

	clearError();
}

