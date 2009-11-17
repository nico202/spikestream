#include "PhiCalculator.h"
#include "ProbabilityTable.h"
#include "Util.h"
using namespace spikestream;


/*! Constructor */
PhiCalculator::PhiCalculator(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo, const AnalysisInfo& anaInfo, unsigned int timeStep, bool* stop){
    //Store variables
    this->analysisInfo = anaInfo;
    this->timeStep = timeStep;
    this->stop = stop;

    //Create data access objects
    networkDao = new NetworkDao(netDBInfo);
    archiveDao = new ArchiveDao(archDBInfo);
    stateDao = new StateBasedPhiAnalysisDao(anaDBInfo);
}


/*! Destructor */
PhiCalculator::~PhiCalculator(){
}


/*-------------------------------------------------------------*/
/*-------                 PUBLIC METHODS                 ------*/
/*-------------------------------------------------------------*/

/*! Calculates and returns the phi of the specified subset.
    FIXME: NEED TO HANDLE THE NORMALIZATION FOR THE TOTAL PARTITION AND
    POSSIBILITY THAT MULTIPLE PARTITIONS HAVE THE SAME MINIMUM NORMALIZED PHI. */
double SubsetManager::getSubsetPhi(QList<unsigned int>& subsetNeurIDs){
    //Convenience variable storing subset size
    int subsetSize = subsetNeurIDs.size();

    //Create array to select the different bipartitions
    bool* partitionArray = new bool[subsetSize];

    //Lists holding the two partitions
    QList<unsigned int> aPartition, bPartition;

    //Work through the bipartitions of the subset
    int aPartitionSize = subsetSize / 2;
    while(partitionSize >= 1 && !*stop){

	//Fill selectionArray with 1s and 0s corresponding to the partition size
	fillSelectionArray(partitionArray, subsetSize, aPartitionSize);

	//Work through all the combinations
	bool permutationsComplete = false;
	bool firstTime = true;
	while(!*stop && !permutationsComplete){

	    //Use selection array to fill the list of neurons in the two partitions
	    fillPartitionLists(aPartition, bPartition, partitionArray, subsetSize, subsetNeurIDs);

	    //Calculate phi on this bipartition
	    newPhi = getPartitionPhi(aPartition, bPartition);

	    //If phi is zero we have found the minimum information bipartion, so can return here
	    if(newPhi == 0.0){
		return 0.0;
	    }

	    //Normalize the new phi
	    if(aPartition.size() <= bPartition.size())
		tmpNormFact = aPartition.size();
	    else
		tmpNormFact = bPartition.size();
	    newPhi /= (double)tmpNormFact;

	    //First time this loop has run, so store newPhi as current minimum
	    if(firstTime){
		minimumPhi = newPhi;
		normalizationFactor = tmpNormFact;
		firstTime = false;
	    }

	    //If newPhi is less than the current minimum, store newPhi as the minimum
	    if(newPhi < minimumPhi){
		minimumPhi = newPhi;
		normalizationFactor = tmpNormFact;
	    }

	    //Change the selection array
	    permutationsComplete = !next_permutation(&partitionArray[0], &partitionArray[subsetSize]);
	}

	//Analyze the next partition size
	--partitionSize;

    }//Finished working through the partition sizes

    //Clean up the selection array
    delete [] subsetSelectionArray;

    //Return the non-normalized phi
    return minimumPhi *= normalizationFactor;
}


/*-------------------------------------------------------------*/
/*-------                 PRIVATE METHODS                ------*/
/*-------------------------------------------------------------*/

/*! Converts a causal probability table, p(x1|x0) into the reverse, p(x0|x1) */
void PhiCalculator::calculateReverseProbability(QList<unsigned int>& neuronIDList, const QString& firingPattern, ProbabilityTable& causalTable, ProbabilityTable& table){

}


/*! Converts the selection array into lists of the neuron ids in the A and B partitions */
void PhiCalculator::fillPartitionLists(QList<unsigned int>& aPartition, QList<unsigned int>& bPartition, bool* partitionArray, int arrayLength, QList<unsigned int>& subsetNeurIDs){
    for(int i=0; i<arrayLength; ++i){
	if(partitionArray[i] == 1)
	    aPartition.append(subsetNeurIDs[i]);
	else
	    bPartition.append(subsetNeurIDs[i]);
    }
}


/*! Fills probability table with p(X0->x1) or equivalently, p(x0|x1)
    Supply list of neurons and a string of 1's or 0's that indicates the corresponding neuron's firing state. */
void  PhiCalculator::fillProbabilityTable(ProbabilityTable& table, QList<unsigned int> neurIDList, const QString& firingPattern){
    //Run some checks on the data
    if(table.numNeurons() != neurIDList.size())
	throw SpikeStreamAnalysisException("Probability table size (" + QString::number(table.numNeurons()) + ") does not match neuron id list size  (" + QString::number(neurIDList.size()) + ")");
    if(table.numNeurons() != firingPattern.size())
	throw SpikeStreamAnalysisException("Probability table size (" + QString::number(table.numNeurons()) + ") does not match firing pattern size (" + QString::number(firingPattern.size()) + ")");

    //Create table to hold p(x1|x0). This is needed to calculate the reverse probability p(x0|x1) via Bayes
    ProbabilityTable causalProbTable(table.numNeurons());

    //Fill p(x1|x0) table
    QHash<QString, double>* probValTab = table.getProbabilityValueMap();
    for(QHash<QString, double>::iterator iter = probValTab->begin(); iter != probValTab->end(); ++iter){
	//Get probability that this state leads to the current state of the network
	double causalProb = getCausalProbability(neurIDList, iter.key(), firingPattern);
	causalProbTable.setProbability(iter.key(), causalProb);
    }

    //Calculate the reverse probability table using Bayes, p(x0|x1)
    calculateReverseProbability(causalProbTable, table);//FIXME: RENAME!!
}


/*! Calculates the probability that the first specified firing pattern leads to the second, or p(x1|x0) */
double PhiCalculator::getCausalProbability(QList<unsigned int>& neurIDList, const QString& initialPat, const QString& currentPat){
    /* Probability of the first state leading to the second is the product of the probabilities of
	the  transition for each neuron */
    double totalProb = 1.0;

    //Work through each neuron to get its probability
    for(int i=0; i<neurIDList.size(); ++i){
	unsigned int neurFiringState = Util::getUInt(secondPat.at(i));

	//Run a couple of sanity checks
	if(currentNeurFiringState != 0 && neurFiringState != 1)
	    throw SpikeStreamAnalysisException("Current neuron firing state must be either 1 or 0");
	if(!weightlessNeuronMap.contains(neurIDList[i]))
	    throw SpikeStreamAnalysisException("Neuron ID cannot be found: " + QString::number(neuronIDList[i]));

	//Get the probability that this pattern led to this firing state
	double transitionProb = weightlessNeuronMap[neurIDList[i]].getProbability(neurIDList, initialPattern, neurFiringState);
	totalProb *= transitionProb;
    }
}


/*! Calculates the phi for the specified partition */
double PhiCalculator::getPartitionPhi(QList<unsigned int>& aPartition, QList<unsigned int>& bPartition){
    //Get a complete map of neurons firing at the current time step
    QList<unsigned int> firingNeuronList = archiveDao->getFiringNeuronIDs(archiveID, timeStep);

    //Get a list of all the neurons in the subset
    QList<unsigned int> subsetList;
    subsetList.append(aPartition);
    subsetList.append(bPartition);

    //Get p(X0->x1) for the whole subset
    ProbabilityTable subsetProbTable(subsetList.size());
    fillProbabilityTable(subsetProbTable);

    //Get p(X0->x1) for the A partition
    ProbabilityTable aProbTable = getProbabilityTable(aPartition);

    //Get p(X0->x1) for the B partition
    ProbabilityTable bProbTable = getProbabilityTable(bPartition);

    //Calculate the relative entropy
    double result = 0.0;
    QHash<QString, double>* tmpProbMap = subsetProbTable.getValueMap();
    for(int i=0; i<subsetProbTable.size(); ++i){
	//Get the string of 1's and 0's corresponding to the A and B parts of the key
	ProbabilityKey aKey = subsetProbTable[i].getSubKey(0, aPartition.size() - 1);
	ProbabilityKey bKey = subsetProbTable[i].getSubKey(bPartition.size() - 1, subsetList.size() - 1);
	result += subsetProbTable[i] * log (subsetProbTable[i] / (aProbTable.getProbability(aKey) * bProbTable.getProbability(bKey)));
    }

    //Return the result
    return result;
}


