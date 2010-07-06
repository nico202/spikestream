//SpikeStream includes
#include "Util.h"
#include "WeightlessLivelinessAnalyzer.h"
#include "SpikeStreamAnalysisException.h"
using namespace spikestream;


/*! Constructor */
WeightlessLivelinessAnalyzer::WeightlessLivelinessAnalyzer(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo, const AnalysisInfo& anaInfo, unsigned int timeStep){
	//Store variables
	this->analysisInfo = anaInfo;
	this->timeStep = timeStep;

	//Create data access objects
	networkDao = new NetworkDao(netDBInfo);
	archiveDao = new ArchiveDao(archDBInfo);
	livelinessDao = new LivelinessDao(anaDBInfo);

	//Initialize variables
	numberOfProgressSteps = 0;
}


/*! Empty constructor for testing. */
WeightlessLivelinessAnalyzer::WeightlessLivelinessAnalyzer(){
	timeStep = -1;
	networkDao = NULL;
	archiveDao = NULL;
	livelinessDao = NULL;

	//Set default values of parameters
	analysisInfo.setParameter("minimum_cluster_liveliness", 0.0);

	//Fix stop variable so that class is always running
	bool* tmpStop = new bool;
	*tmpStop = false;
	stop = tmpStop;

	//Set up progress so that it does not affect tests
	progressCounter = 0;
	numberOfProgressSteps = 0xffff;
}


/*! Destructor */
WeightlessLivelinessAnalyzer::~WeightlessLivelinessAnalyzer(){
	if(networkDao != NULL)
		delete networkDao;
	if(archiveDao != NULL)
		delete archiveDao;
	if(livelinessDao != NULL)
		delete livelinessDao;

	deleteWeightlessNeurons();
}


/*-------------------------------------------------------------*/
/*-------                 PUBLIC METHODS                 ------*/
/*-------------------------------------------------------------*/

/*! Runs the analysis for liveliness on the network */
void WeightlessLivelinessAnalyzer::runCalculation(const bool * const stop){
	//Store reference to stop in invoking class
	this->stop = stop;

	//Load up weightless neurons, firing neurons and connections
	loadWeightlessNeurons();
	loadFiringNeurons();

	//Have to work through neurons twice, once to calculate livelinesss and then again to identify clusters.
	numberOfProgressSteps = weightlessNeuronMap.size() * 2;
	progressCounter = 0;

	//Work through the stages in the analysis
	calculateConnectionLiveliness();
	saveNeuronLiveliness();
	identifyClusters();

}



/*! Identifies the liveliness of each connection and optionally stores it in the tmp
	weight field of the network database */
void WeightlessLivelinessAnalyzer::calculateConnectionLiveliness(){
	//Reset previous results
	fromConnectionLivelinessMap.clear();
	toConnectionLivelinessMap.clear();
	neuronLivelinessMap.clear();

	//Work through each weightless neuron
	for(QHash<unsigned int, WeightlessNeuron*>::iterator weiNeurIter = weightlessNeuronMap.begin(); !*stop && weiNeurIter != weightlessNeuronMap.end(); ++weiNeurIter){
		WeightlessNeuron* tmpWeiNeur = weiNeurIter.value();
		QHash<unsigned int, QList<unsigned int> > tmpConMap = tmpWeiNeur->getConnectionMap();

		//Initialize its liveliness value to zero
		neuronLivelinessMap[tmpWeiNeur->getID()] = 0.0;

		//Create input byte array that reflects the firing state of the connected neurons
		byte* inPatArr;
		int inPatArrLen;
		fillInputArray(tmpWeiNeur, inPatArr, inPatArrLen);

		//Work through all the connections to the neuron
		for(QHash<unsigned int, QList<unsigned int> >::iterator conIter = tmpConMap.begin(); !*stop && conIter != tmpConMap.end(); ++conIter){

			//Get firing probability with neuron in its current state. Firing state is set to 1, but could equally well be zero
			double firingProb1 = tmpWeiNeur->getFiringStateProbability(inPatArr, inPatArrLen, 1);

			//Flip the bits corresponding to the neuron under test
			flipBits(inPatArr, inPatArrLen, conIter.value());

			//Get probability again
			double firingProb2 = tmpWeiNeur->getFiringStateProbability(inPatArr, inPatArrLen, 1);

			//If probability has changed, neuron is lively
			if(firingProb1 != firingProb2){
				setConnectionLiveliness(conIter.key(), tmpWeiNeur->getID(), 1.0);
			}
			else{
				setConnectionLiveliness(conIter.key(), tmpWeiNeur->getID(), 0.0);
			}

			//Flip bit back ready to test next neuron in connection set
			flipBits(inPatArr, inPatArrLen, conIter.value());
		}

		//Clean up array
		delete [] inPatArr;

		//Inform other classes about progress
		updateProgress("Neuron " + QString::number(tmpWeiNeur->getID()) + " liveliness calculation complete.");
	}
}


/*! Creates an array and populates it with the firing pattern of neurons that connect to the
	specified weightless neuron. */
void WeightlessLivelinessAnalyzer::fillInputArray(WeightlessNeuron* weiNeuron, byte*& inPatArr, int& inPatArrLen){
	//Calculate the length of the array
	if(weiNeuron->getNumberOfConnections() % 8 == 0)
		inPatArrLen = weiNeuron->getNumberOfConnections() / 8;
	else
		inPatArrLen = weiNeuron->getNumberOfConnections() / 8 + 1;

	//Create pattern array and initialize with zeros
	inPatArr = new byte [inPatArrLen];
	for(int i=0; i<inPatArrLen; ++i)
		inPatArr[i] = 0;

	//Fill input array according to firing patterns of other neurons
	QHash<unsigned int, QList<unsigned int> > conMap = weiNeuron->getConnectionMap();
	for(QHash<unsigned int, QList<unsigned int> >::iterator conIter = conMap.begin(); conIter != conMap.end(); ++conIter){
		//Connected neuron is firing
		if(firingNeuronMap.contains(conIter.key())){
			//Set all indexes associated with this neuron to 1
			foreach(int indx, conIter.value()){
				if(indx / 8 > inPatArrLen)
					throw SpikeStreamAnalysisException("Input pattern array index out of range.");
				inPatArr[ indx/8 ] |= 1<<(indx % 8);
			}
		}
	}
}


/*! Flips the bits corresponding to the indexes associated with a neuron */
void WeightlessLivelinessAnalyzer::flipBits(byte inPatArr [], int inPatArrLen, QList<unsigned int>& indexList){
	//Flip the bits for each index entry for this neuron
	foreach(int indx, indexList){
		if(indx/8 >= inPatArrLen)
			throw SpikeStreamAnalysisException("Index out of range: " + QString::number(indx/8) );
		inPatArr[ indx/8 ] ^= 1<<(indx % 8);
	}
}


void WeightlessLivelinessAnalyzer::identifyClusters(){
	//Get a local copy of the minimum value of liveliness
	double minClusterLiveliness = analysisInfo.getParameter("minimum_cluster_liveliness");

	//Map containing all neurons that have been added to a cluster
	QHash<unsigned int, bool> addedNeuronsMap;

	//List of neurons to expand in the current cluster
	QList<unsigned int> expansionList;

	//Map to filter duplicates in the expansion list
	QHash<unsigned int, bool> expansionListMap;

	//Work through each weightless neuron
	for(QHash<unsigned int, WeightlessNeuron*>::iterator weiNeurIter = weightlessNeuronMap.begin(); !*stop && weiNeurIter != weightlessNeuronMap.end(); ++weiNeurIter){
		expansionListMap.clear();
		expansionList.clear();

		//Add seed neuron to list of neurons that are to be expanded
		if( !addedNeuronsMap.contains( weiNeurIter.key() ) ){
			expansionList.append(weiNeurIter.key());
			expansionListMap[weiNeurIter.key()] = true;
		}

		//Expand the connections to all the neurons in current cluster
		for(int clstrIndx=0; clstrIndx<expansionList.size(); ++clstrIndx){
			unsigned int expNeurID = expansionList.at(clstrIndx);

			//Add neurons that are connected FROM this neuron that are not in the current cluster
			if(fromConnectionLivelinessMap.contains(expNeurID)){
				for(QHash<unsigned int, double>::iterator conIter = fromConnectionLivelinessMap[expNeurID].begin(); conIter != fromConnectionLivelinessMap[expNeurID].end(); ++conIter){

					//Positive liveliness between this neuron and the other neuron
					if(conIter.value() > 0){

						//Abandon expansion if neuron is in a cluster we already have
						if(addedNeuronsMap.contains(conIter.key())){
							expansionList.clear();
							break;
						}

						//Add neuron to expansion list if it is not already in it
						else if(!expansionListMap.contains(conIter.key())){
							expansionList.append(conIter.key());
							expansionListMap[conIter.key()] = true;
						}
					}
				}
			}

			//Add neurons that connect TO this neuron that are not in the current cluster
			if(toConnectionLivelinessMap.contains(expNeurID) && !expansionList.isEmpty()){
				for(QHash<unsigned int, double>::iterator conIter = toConnectionLivelinessMap[expNeurID].begin(); conIter != toConnectionLivelinessMap[expNeurID].end(); ++conIter){

					//Positive liveliness between this neuron and the other neuron
					if(conIter.value() > 0){

						//Abandon expansion if neuron is in a cluster we already have
						if(addedNeuronsMap.contains(conIter.key())){
							expansionList.clear();
							break;
						}

						//Add neuron to expansion list if it is not already in it
						else if(!expansionListMap.contains(conIter.key())){
							expansionList.append(conIter.key());
							expansionListMap[conIter.key()] = true;
						}
					}
				}
			}
		}

		//Store cluster if there is more than one neuron in it and if the liveliness is greater than the threshold
		if(expansionList.size() > 1){
			double clstrLiveliness = getClusterLiveliness(expansionList);
			if(clstrLiveliness >= minClusterLiveliness){
				livelinessDao->addCluster(analysisInfo.getID(), timeStep, expansionList, clstrLiveliness);
			}

			//Add neurons to map of all expanded neurons
			foreach(unsigned int neurID, expansionList)
				addedNeuronsMap[neurID] = true;
		}

		//Inform other classes about progress
		updateProgress("Neuron " + QString::number(weiNeurIter.key()) + " cluster expansion complete.");
	}

	//Inform user that clusters have been found
	emit newResultsFound();
}


/*-------------------------------------------------------------*/
/*-------                 PRIVATE METHODS                ------*/
/*-------------------------------------------------------------*/

/*! Deletes all of the weightless neurons currently stored in this class */
void WeightlessLivelinessAnalyzer::deleteWeightlessNeurons(){
	for(QHash<unsigned int, WeightlessNeuron*>::iterator iter = weightlessNeuronMap.begin(); iter != weightlessNeuronMap.end(); ++iter){
		delete iter.value();
	}
	weightlessNeuronMap.clear();
	fromConnectionLivelinessMap.clear();
	toConnectionLivelinessMap.clear();
	neuronLivelinessMap.clear();
}


/*! Returns the liveliness of a cluster.
	This is the sum of the liveliness of each neuron in the cluster multiplied by
	this same sum divided by the maximum possible liveliness */
double WeightlessLivelinessAnalyzer::getClusterLiveliness(QList<unsigned int>& neuronIDs){
	//Work out the sum of the liveliness
	double totLiveliness = 0.0;
	foreach(unsigned int neurID, neuronIDs){
		if(!neuronLivelinessMap.contains(neurID))
			throw SpikeStreamAnalysisException("Neuron ID is missing from neuron liveliness map.");
		totLiveliness += neuronLivelinessMap[neurID];
	}

	//Return the sum weighted by the extent to which this is the maximum liveliness
	if( neuronIDs.size() > sqrt(0xffffffff) )
		throw SpikeStreamAnalysisException("Neuron list size will cause overflow of 32 bit calculation.");
	return totLiveliness * ( totLiveliness /  (double)( neuronIDs.size() * neuronIDs.size() ) );
}


/*! Loads up the neurons firing at this time step. */
void WeightlessLivelinessAnalyzer::loadFiringNeurons(){
	if(archiveDao == NULL){
		throw SpikeStreamAnalysisException("Archive dao has not been set. Empty constructor should only be used for unit testing.");
	}

	firingNeuronMap.clear();
	QStringList neurIDStrList = archiveDao->getFiringNeuronIDs(analysisInfo.getArchiveID(), timeStep);
	QStringListIterator iter(neurIDStrList);
	while (iter.hasNext()){
		firingNeuronMap[ Util::getUInt(iter.next()) ] = true;
	}
}


/*! Loads up all of the weightless neurons */
void WeightlessLivelinessAnalyzer::loadWeightlessNeurons(){
	if(networkDao == NULL){
		throw SpikeStreamAnalysisException("Network dao has not been set. Empty constructor should only be used for unit testing.");
	}

	//Clean up any previous data
	deleteWeightlessNeurons();

	//Load weightless neurons
	QList<unsigned int> neuronIDList = networkDao->getNeuronIDs(analysisInfo.getNetworkID());
	foreach(unsigned int neurID, neuronIDList){
		//Store neuron
		weightlessNeuronMap[neurID] = networkDao->getWeightlessNeuron(neurID);

		//Set parameters in neuron
		weightlessNeuronMap[neurID]->setGeneralization(analysisInfo.getParameter("generalization"));
	}
}


/*! Stores the connection liveliness in the database if requested, or just adds it to a map. */
void WeightlessLivelinessAnalyzer::setConnectionLiveliness(unsigned int fromNeurID, unsigned int toNeurID, double liveliness){
	//Store in the database if requested
	if(analysisInfo.getParameter("store_connection_liveliness_as_temporary_weights") == 1.0){
		networkDao->setTempWeight(fromNeurID, toNeurID, liveliness);
	}

	//Store in maps for analysis of clusters and neurons
	fromConnectionLivelinessMap[fromNeurID][toNeurID] = liveliness;
	toConnectionLivelinessMap[toNeurID][fromNeurID] = liveliness;
	neuronLivelinessMap[toNeurID] += liveliness;
}


/*! Informs other classes about progess with the calculation */
void WeightlessLivelinessAnalyzer::updateProgress(const QString& msg){
	++progressCounter;
	if(progressCounter > numberOfProgressSteps)
		throw SpikeStreamAnalysisException("Progress counter out of range. progressCounter=" + QString::number(progressCounter) + "; numberOfProgressSteps=" + QString::number(numberOfProgressSteps));

	emit progress(msg, timeStep, progressCounter, numberOfProgressSteps);
}


/*! Writes the liveliness of each neuron to the database */
void WeightlessLivelinessAnalyzer::saveNeuronLiveliness(){
	for(QHash<unsigned int, double>::iterator iter = neuronLivelinessMap.begin(); iter != neuronLivelinessMap.end(); ++iter){
		livelinessDao->setNeuronLiveliness(analysisInfo.getID(), timeStep, iter.key(), iter.value());
	}
}


