//SpikeStream includes
#include "SensoryMotorNetworksBuilder.h"
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
SensoryMotorNetworksBuilder::SensoryMotorNetworksBuilder() : NetworksBuilder(){
	//Create array to store connections for each neuron to add weightless indexes later when we have the connection id
	conListArray = new QList<unsigned>[12];
}


/*! Destructor */
SensoryMotorNetworksBuilder::~SensoryMotorNetworksBuilder(){
}



/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Adds all of the fully connected networks */
void SensoryMotorNetworksBuilder::prepareAddNetworks(const QString& networkName, const QString& networkDescription, const bool* stop){
	this->networkName = networkName;
	this->networkDescription = networkDescription;
	this->stop = stop;
}


/*! Run method inherited from QThread */
void SensoryMotorNetworksBuilder::run(){
	clearError();

	// Set up the network and archive daos within the new thread
	networkDao = new NetworkDao(Globals::getNetworkDao()->getDBInfo());
	archiveDao = new ArchiveDao(Globals::getArchiveDao()->getDBInfo());

	try{
		emit progress(0, 1);

		//Add nework
		srand(123456789);//Seed the random number generator
		addBasicNetwork(networkName, networkDescription);

		emit progress(1, 1);

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
void SensoryMotorNetworksBuilder::addBasicNetwork(const QString& networkName, const QString& networkDescription){
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
	//Bottom input layer
	neuronList.append(neuronGroup.addNeuron(1.5, 1, 1));//1
	neuronList.append(neuronGroup.addNeuron(2.5, 1, 1));//2
	neuronList.append(neuronGroup.addNeuron(3.5, 1, 1));//3

	//Middle recurrent layer
	neuronList.append(neuronGroup.addNeuron(2, 1, 2.5));//4
	neuronList.append(neuronGroup.addNeuron(3, 1, 2.5));//5
	neuronList.append(neuronGroup.addNeuron(1.25, 1, 3.5));//6
	neuronList.append(neuronGroup.addNeuron(3.75, 1, 3.5));//7
	neuronList.append(neuronGroup.addNeuron(2, 1, 4.5));//8
	neuronList.append(neuronGroup.addNeuron(3, 1, 4.5));//9

	//Top output layer
	neuronList.append(neuronGroup.addNeuron(1.5, 1, 6));//10
	neuronList.append(neuronGroup.addNeuron(2.5, 1, 6));//11
	neuronList.append(neuronGroup.addNeuron(3.5, 1, 6));//12

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

	//Add the training
	addTraining();

	//Add archives
	addFiringPatterns();
}


/*! Adds the connections */
void SensoryMotorNetworksBuilder::addConnections(){
	//Build the connection group that is to be added
	QHash<QString, double> paramMap;
	ConnectionGroupInfo connGrpInfo(0, "Connection Group", neuronGroupID, neuronGroupID,  paramMap, networkDao->getSynapseType(SYNAPSE_TYPE_ID));
	ConnectionGroup connGrp(connGrpInfo);

	/* Add connections
		Center cluster is fully connected together;
		Each neuron in the center cluster receives connections from all of the input neurons;
		Each output neuron receives connections from all of the neurons in the center cluster.
	*/

	//Add connections from input layer to center
	for(unsigned fromNeurIdx = 1; fromNeurIdx <=3; ++fromNeurIdx){
		for(unsigned toNeurIdx = 4; toNeurIdx <= 9; ++toNeurIdx){
			conListArray[toNeurIdx-1].append( connGrp.addConnection(neuronMap[fromNeurIdx], neuronMap[toNeurIdx],  0,  0) );
		}
	}

	//Add connections within the center cluster
	for(unsigned int toNeurIdx = 4; toNeurIdx <=9; ++ toNeurIdx){
		for(unsigned int fromNeurIdx = 4; fromNeurIdx <=9; ++fromNeurIdx)
			if(fromNeurIdx != toNeurIdx)
				conListArray[toNeurIdx-1].append( connGrp.addConnection(neuronMap[fromNeurIdx], neuronMap[toNeurIdx],  0,  0) );
	}

	//Add connections from center to output layer
	for(unsigned fromNeurIdx = 4; fromNeurIdx <=9; ++fromNeurIdx){
		for(unsigned toNeurIdx = 10; toNeurIdx <= 12; ++toNeurIdx){
			conListArray[toNeurIdx-1].append( connGrp.addConnection(neuronMap[fromNeurIdx], neuronMap[toNeurIdx],  0,  0) );
		}
	}

	//Add connection group to the database
	addConnectionGroup(networkID, connGrp);

	//Add the weightless index of the connection 
	for(unsigned int toNeurID=1; toNeurID <= 12; ++toNeurID){
		//Add pattern indexes for this TO neuron
		for(int patternIndx=0; patternIndx < conListArray[toNeurID-1].size(); ++patternIndx)
			networkDao->addWeightlessConnection( connGrp[conListArray[toNeurID-1].at(patternIndx)].getID(), patternIndx );
	}
}


/*! Adds the firing patterns for the network */
void SensoryMotorNetworksBuilder::addFiringPatterns(){
	//Create archive
	ArchiveInfo archiveInfo(0, networkID, QDateTime::currentDateTime().toTime_t(), "Sensory data being processed by the network.");
	archiveDao->addArchive(archiveInfo);

	//No activity
	archiveDao->addArchiveData(archiveInfo.getID(), 1, "");

	//Sensory network with input pattern
	QString firingNeuronStr = QString::number(neuronMap[1]) + "," + QString::number(neuronMap[3]);
	archiveDao->addArchiveData(archiveInfo.getID(), 2, firingNeuronStr);

	//First processing of input pattern by centre cluster
	firingNeuronStr = QString::number(neuronMap[4]) + "," + QString::number(neuronMap[7])+ "," + QString::number(neuronMap[8]);
	archiveDao->addArchiveData(archiveInfo.getID(), 3, firingNeuronStr);

	//Second processing of input pattern by centre cluster
	firingNeuronStr = QString::number(neuronMap[5]) + "," + QString::number(neuronMap[6])+ "," + QString::number(neuronMap[9]);
	archiveDao->addArchiveData(archiveInfo.getID(), 4, firingNeuronStr);

	//Final output pattern
	firingNeuronStr = QString::number(neuronMap[11]);
	archiveDao->addArchiveData(archiveInfo.getID(), 5, firingNeuronStr);
}


/*! Adds training to the network. */
void SensoryMotorNetworksBuilder::addTraining(){

	//Neurons 1,2,3 output 0 all the time unless they are externally constrained to a particular state
	addTraining(neuronMap[1], "", 0);
	addTraining(neuronMap[2], "", 0);
	addTraining(neuronMap[3], "", 0);

	//Neurons have different numbers of connections so work through the rest of the neurons in turn
	for(unsigned int toNeurIndx = 4; toNeurIndx <=12; ++toNeurIndx){

		//Number of connections varies with each neuron
		int numberOfConnections = conListArray[toNeurIndx-1].size();

		//Work through all permutations  of 1s and 0s on the inputs to the neuron
		bool selectionArray[numberOfConnections];
		int numSelected = 0;
		while(numSelected <= numberOfConnections){
			Util::fillSelectionArray(selectionArray, numberOfConnections, numSelected);

			//Work through all permutations
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

				//Calculate the output, which is specific to each neuron
				unsigned output=0;
				if(toNeurIndx == 4 || toNeurIndx == 7 || toNeurIndx == 8){
					if(trainingStr.startsWith("101"))
						output = 1;
				}
				else if(toNeurIndx == 5){
					if(trainingStr.endsWith("10110"))
						output = 1;
				}
				else if(toNeurIndx == 6){
					if(trainingStr.endsWith("10110"))
						output = 1;
				}
				else if(toNeurIndx == 9){
					if(trainingStr.endsWith("10011"))
						output = 1;
				}
				else if(toNeurIndx == 11){
					if(trainingStr == "011001")
						output = 1;
				}

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
void SensoryMotorNetworksBuilder::reset(){
	networkID = 0;
	neuronGroupID = 0;
	neuronMap.clear();

	clearError();
}

