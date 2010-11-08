//SpikeStream includes
#include "PartitionedNetworksBuilder.h"
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
PartitionedNetworksBuilder::PartitionedNetworksBuilder() : NetworksBuilder(){
}


/*! Destructor */
PartitionedNetworksBuilder::~PartitionedNetworksBuilder(){
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Adds all of the fully connected networks */
void PartitionedNetworksBuilder::prepareAddNetworks(const QString& networkName, const QString& networkDescription, const bool* stop){
	this->networkName = networkName;
	this->networkDescription = networkDescription;
	this->stop = stop;
}


/*! Run method inherited from QThread */
void PartitionedNetworksBuilder::run(){
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
void PartitionedNetworksBuilder::addBasicNetwork(const QString& networkName, const QString& networkDescription){
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
	neuronList.append(neuronGroup.addNeuron(2, 1, 1));
	neuronList.append(neuronGroup.addNeuron(3, 1, 1));
	neuronList.append(neuronGroup.addNeuron(4, 2, 1));
	neuronList.append(neuronGroup.addNeuron(3, 3, 1));
	neuronList.append(neuronGroup.addNeuron(2, 3, 1));
	neuronList.append(neuronGroup.addNeuron(1, 2, 1));
	neuronList.append(neuronGroup.addNeuron(8, 1, 1));
	neuronList.append(neuronGroup.addNeuron(9, 1, 1));
	neuronList.append(neuronGroup.addNeuron(10, 2, 1));
	neuronList.append(neuronGroup.addNeuron(9, 3, 1));
	neuronList.append(neuronGroup.addNeuron(8, 3, 1));
	neuronList.append(neuronGroup.addNeuron(7, 2, 1));

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
void PartitionedNetworksBuilder::addConnections(){
	//Build the connection group that is to be added
	QHash<QString, double> paramMap;
	ConnectionGroupInfo connGrpInfo(0, "Connection Group", neuronGroupID, neuronGroupID,  paramMap, networkDao->getSynapseType(SYNAPSE_TYPE_ID));
	ConnectionGroup connGrp(connGrpInfo);

	/* Add connections
		Each neuron receives connections from a random selection of three other neurons in the
		same cluster. */

	//Array of lists to hold connections
	int numConnections = 3;
	QList<unsigned> conListArray[12];

	//Add connections. Each neuron receives connections from a random selection of six other neurons not including itself
	for(unsigned int toNeurID=1; toNeurID<=12; ++toNeurID){
		//Get random selection of from neuron ids
		QList<unsigned int> fromNeurIDList = getRandomFromNeuronIDs(toNeurID, numConnections);
		if(fromNeurIDList.size() != numConnections)
			throw SpikeStreamException("From neuron id list not correct size");

		//Create connections
		foreach(unsigned int fromNeurID, fromNeurIDList){
			//Add the connection to the neuron
			conListArray[toNeurID-1].append( connGrp.addConnection(neuronMap[fromNeurID], neuronMap[toNeurID],  0,  0) );
		}
	}

	//Add connection group to the database
	addConnectionGroup(networkID, connGrp);

	//Add the weightless index of the connection each runs from 0 to 12
	for(unsigned int toNeurID=1; toNeurID <= 12; ++toNeurID){
		if(conListArray[toNeurID-1].size() != numConnections)
			throw SpikeStreamException("Connection list not correct size");

		//Add pattern indexes for this to neuron
		for(int patternIndx=0; patternIndx < numConnections; ++patternIndx)
			networkDao->addWeightlessConnection( connGrp[conListArray[toNeurID-1].at(patternIndx)].getID(), patternIndx );
	}
}


/*! Adds the firing patterns for the network */
void PartitionedNetworksBuilder::addFiringPatterns(){
	if(firingNeuronStr25.isEmpty() || firingNeuronStr50.isEmpty() || firingNeuronStr75.isEmpty() || firingNeuronStr100.isEmpty()){
		firingNeuronStr25 = getFiringNeuronStr(25);
		firingNeuronStr50 = getFiringNeuronStr(50);
		firingNeuronStr75 = getFiringNeuronStr(75);

		firingNeuronStr100 = "";
		for(int i=1; i<12; ++i)
			firingNeuronStr100 += QString::number(neuronMap[i]) + ",";
		firingNeuronStr100 += QString::number(neuronMap[12]);
	}

	//Create archive
	ArchiveInfo archiveInfo(0, networkID, QDateTime::currentDateTime().toTime_t(), "0%, 25%, 50%, 75% and 100% neurons randomly firing");
	archiveDao->addArchive(archiveInfo);

	//Add complete set of firing patterns
	archiveDao->addArchiveData(archiveInfo.getID(), 1, "");//0%
	archiveDao->addArchiveData(archiveInfo.getID(), 2, firingNeuronStr25);
	archiveDao->addArchiveData(archiveInfo.getID(), 3, firingNeuronStr50);
	archiveDao->addArchiveData(archiveInfo.getID(), 4, firingNeuronStr75);
	archiveDao->addArchiveData(archiveInfo.getID(), 5, firingNeuronStr100);
}


/*! Adds training to the network. */
void PartitionedNetworksBuilder::addTraining(unsigned int percentInputs){
	//Number of connections to each neuron
	int numberOfConnections = 3;

	//Neuron will output 1 if the number of input neurons firing is greater than or equal to the threshold
	int threshold = (int) rint( numberOfConnections * ((double)percentInputs / 100.0 ) );
	int output = 0;

	//Work through all permutations  of 1s and 0s on the 12 inputs to each neuron
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

			//Add the training to the neurons
			for(unsigned int tmpNeurID=1; tmpNeurID<=12; ++tmpNeurID)
				addTraining(neuronMap[tmpNeurID], trainingStr, output);

			//Get the next permutation
			permutationsComplete = !next_permutation(&selectionArray[0], &selectionArray[numberOfConnections]);
		}

		//Increase the selection size
		++numSelected;
	}
}


/*! Returns a string with the specified percentage of firing neurons selected at random */
QString PartitionedNetworksBuilder::getFiringNeuronStr(unsigned int percentNeurons){
	//Fill map with indexes of selected neurons
	int numNeurs = (int) rint( 12.0 * ((double)percentNeurons / 100.0 ) );
	QHash<unsigned int, bool> selectionMap;
	while(selectionMap.size() < numNeurs){
		int newIndex = rand() % 12 + 1;
		selectionMap[newIndex] = true;//Duplicates do not matter because they do not increase the size of the map
	}

	//Add neuron ids to the string and return it
	QString tmpStr = "";
	for(QHash<unsigned int, bool>::iterator iter = selectionMap.begin(); iter != selectionMap.end(); ++iter)
		tmpStr += QString::number(neuronMap[iter.key()]) + ",";
	tmpStr.truncate(tmpStr.length() - 1);
	return tmpStr;
}


QList<unsigned int> PartitionedNetworksBuilder::getRandomFromNeuronIDs(unsigned int toNeuronID, int numFromCons){
	if(numFromCons > neuronMap.size())
		throw SpikeStreamException("Trying to connect to more unique neurons than exist in network.");

	QHash<unsigned int, bool> selectionMap;
	while (selectionMap.size() < numFromCons){
		//Fill map with indexes of selected neurons. Connect 1st 6 neurons together and second 6 neurons together
		unsigned int tmpNeurID;
		if(toNeuronID >=7)
			tmpNeurID = rand() % 6 + 7;
		else
			tmpNeurID = rand() % 6 + 1;

		if(tmpNeurID != toNeuronID)
			selectionMap[tmpNeurID] = true;
	}
	return selectionMap.keys();
}


/*! Resets this class, except for the firing patterns */
void PartitionedNetworksBuilder::reset(){
	firingNeuronStr25 = "";
	firingNeuronStr50 = "";
	firingNeuronStr75 = "";
	firingNeuronStr100 = "";

	networkID = 0;
	neuronGroupID = 0;
	neuronMap.clear();

	clearError();
}

