//SpikeStream includes
#include "PhiCalculator.h"
#include "ProbabilityTable.h"
#include "SpikeStreamAnalysisException.h"
#include "Util.h"
using namespace spikestream;

//Other includes
#include <algorithm>
#include <iostream>
using namespace std;


/*! Constructor */
PhiCalculator::PhiCalculator(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo, const AnalysisInfo& anaInfo, unsigned int timeStep, const bool* stop){
    //Store variables
    this->analysisInfo = anaInfo;
    this->timeStep = timeStep;
    this->stop = stop;

    //Create data access objects
    networkDao = new NetworkDao(netDBInfo);
    archiveDao = new ArchiveDao(archDBInfo);
    stateDao = new StateBasedPhiAnalysisDao(anaDBInfo);

    //Load up the complete set of weightless neurons and the neurons firing at this time step
    loadWeightlessNeurons();
    loadFiringNeurons();
}


/*! Destructor */
PhiCalculator::~PhiCalculator(){
    for(QHash<unsigned int, WeightlessNeuron*>::iterator iter = weightlessNeuronMap.begin(); iter != weightlessNeuronMap.end(); ++iter){
	delete iter.value();
    }
}


/*-------------------------------------------------------------*/
/*-------                 PUBLIC METHODS                 ------*/
/*-------------------------------------------------------------*/

/*! Calculates and returns the phi of the specified subset.
    FIXME: NEED TO HANDLE THE NORMALIZATION FOR THE TOTAL PARTITION AND
    POSSIBILITY THAT MULTIPLE PARTITIONS HAVE THE SAME MINIMUM NORMALIZED PHI. */
double PhiCalculator::getSubsetPhi(QList<unsigned int>& subsetNeurIDs){
    //Convenience variable storing subset size
    int subsetSize = subsetNeurIDs.size();

    //Create array to select the different bipartitions
    bool* partitionArray = new bool[subsetSize];

    //Lists holding the two partitions
    QList<unsigned int> aPartition, bPartition;

    //Variables used during calculation
    double newPhi, minimumPhi, tmpNormFact, normalizationFactor;

    //Work through the bipartitions of the subset
    int aPartitionSize = subsetSize / 2;
    while(aPartitionSize >= 1 && !*stop){

	//Fill selectionArray with 1s and 0s corresponding to the partition size
	Util::fillSelectionArray(partitionArray, subsetSize, aPartitionSize);

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

	    /* Normalize the new phi
	       For a partition into two parts the normalization is the size of the smaller a priori
	       repertoire, which has 2^n entries */
	    if(aPartition.size() <= bPartition.size())
		tmpNormFact = exp2((double)aPartition.size());
	    else
		tmpNormFact = exp2((double)bPartition.size());
	    newPhi /= tmpNormFact;

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
	--aPartitionSize;

    }//Finished working through the partition sizes

    //Clean up the selection array
    delete [] partitionArray;

    //Return the non-normalized phi
    return minimumPhi *= normalizationFactor;
}


/*! Converts a causal probability table, p(x1|x0) into the reverse, p(x0|x1)
    using equation:
		     p(s0->s1) = p(s1|s0) * pmax(s0)
				--------------------
					p(s1)
    where p(s1) is given by:
	    sum(s0)[  p(s1|s0) * pmax(s0)]

*/
void PhiCalculator::calculateReverseProbability(ProbabilityTable& causalTable, ProbabilityTable& reverseTable){
    //Run a couple of checks
    if(causalTable.getNumberOfElements() != reverseTable.getNumberOfElements())
	throw SpikeStreamAnalysisException("Size of causal and reverse tables do not match.");


    //Work out pmax(s0)
    double pMaxS0 = 1 / exp2(causalTable.getNumberOfElements());

    //Work out p(s1)
    double pS1 = 0.0;
    for(QHash<QString, double>::iterator iter = causalTable.begin(); iter != causalTable.end(); ++iter){
	pS1 += iter.value() * pMaxS0;
    }

    //Populate reverse table
    double newProb;
    for(QHash<QString, double>::iterator iter = causalTable.begin(); iter != causalTable.end(); ++iter){
	newProb = (iter.value() * pMaxS0) / pS1;
	reverseTable.set(iter.key(), newProb);
    }
}


/*! Fills probability table with p(X0->x1) or equivalently, p(x0|x1)
    Supply list of neurons and a string of 1's or 0's that indicates the corresponding neuron's firing state. */
void  PhiCalculator::fillProbabilityTable(ProbabilityTable& probTable, QList<unsigned int> neurIDList){
    //Run some checks on the data
    if(probTable.getNumberOfElements() != neurIDList.size())
	throw SpikeStreamAnalysisException("Probability table size (" + QString::number(probTable.getNumberOfElements()) + ") does not match neuron id list size  (" + QString::number(neurIDList.size()) + ")");

    //Create table to hold p(x1|x0). This is needed to calculate the reverse probability p(x0|x1) via Bayes
    ProbabilityTable causalProbTable(probTable.getNumberOfElements());

    //Fill p(x1|x0) table
    for(QHash<QString, double>::iterator iter = probTable.begin(); iter != probTable.end(); ++iter){
	//Get probability that this state leads to the current state of the network
	double causalProb = getCausalProbability(neurIDList, iter.key());
	causalProbTable.set(iter.key(), causalProb);
    }

    //Calculate the reverse probability table using Bayes, p(x0|x1)
    calculateReverseProbability(causalProbTable, probTable);
}


/*! Calculates the probability that the first specified firing pattern leads to the second, or p(x1|x0) */
double PhiCalculator::getCausalProbability(QList<unsigned int>& neurIDList, const QString& x0Pattern){
    /* Probability of the first state leading to the second is the product of the probabilities of
	the  transition for each neuron */
    double totalProb = 1.0;

    //Work through each neuron to get the probability that it is in state
    foreach(unsigned int tmpNeurID,  neurIDList){
	int firingState = getFiringState(tmpNeurID);

	//Get the probability that this pattern led to this firing state
	double transitionProb = weightlessNeuronMap[tmpNeurID]->getTransitionProbability(neurIDList, x0Pattern, firingState);
	totalProb *= transitionProb;
    }

    //Return the product of the transition probabilities for each neuron
    return totalProb;
}


/*! Calculates the phi for the specified partition */
double PhiCalculator::getPartitionPhi(QList<unsigned int>& aPartition, QList<unsigned int>& bPartition){
    //Create a list of all the neurons in the subset
    QList<unsigned int> subsetList;
    foreach(unsigned int tmpNeurID, aPartition)
	subsetList.append(tmpNeurID);
    foreach(unsigned int tmpNeurID, bPartition)
	subsetList.append(tmpNeurID);

    //Get p(X0->x1) for the whole subset
    ProbabilityTable subProbTable(subsetList.size());
    fillProbabilityTable(subProbTable, subsetList);

    //Get p(X0->x1) for the A partition
    ProbabilityTable aProbTable(aPartition.size());
    fillProbabilityTable(aProbTable, aPartition);

    //Get p(X0->x1) for the B partition
    ProbabilityTable bProbTable(bPartition.size());
    fillProbabilityTable(bProbTable, bPartition);

    //Calculate the relative entropy
    double result = 0.0, subsetProb;
    //Work through the A partition
    for(QHash<QString, double>::iterator aIter = aProbTable.begin(); aIter != aProbTable.end(); ++aIter){
	//Work through the B partition
	for(QHash<QString, double>::iterator bIter =bProbTable.begin(); bIter != bProbTable.end(); ++bIter){
	    /* Calculation is
		SUM[ p(i)log2( p(i) / (pA(i) * pB(i)))
	    */
	    subsetProb = subProbTable.get(aIter.key() + bIter.key());//Combine the A and B keys to get the p(s0->s1) for whole subset
	    result += subsetProb * log2( subsetProb / (aIter.value() * bIter.value()));
	}
    }

    //Return the result
    return result;
}


/*! Loads up the neurons firing at this time step. */
void PhiCalculator::loadFiringNeurons(){
    firingNeuronMap.clear();
    QStringList neurIDStrList = archiveDao->getFiringNeuronIDs(analysisInfo.getArchiveID(), timeStep);
    QStringListIterator iter(neurIDStrList);
    while (iter.hasNext()){
	firingNeuronMap[ Util::getUInt(iter.next()) ] = true;
    }
}


/*! Loads up all of the weightless neurons */
void PhiCalculator::loadWeightlessNeurons(){
    weightlessNeuronMap.clear();
    QList<unsigned int> neuronIDList = networkDao->getNeuronIDs(analysisInfo.getNetworkID());
    foreach(unsigned int neurID, neuronIDList){
	//Store neuron
	weightlessNeuronMap[neurID] = networkDao->getWeightlessNeuron(neurID);

	//Set parameters in neuron
	weightlessNeuronMap[neurID]->setGeneralization(analysisInfo.getParameter("Generalization"));
    }
}


/*! Converts the selection array into lists of the neuron ids in the A and B partitions */
void PhiCalculator::fillPartitionLists(QList<unsigned int>& aPartition, QList<unsigned int>& bPartition, bool* partitionArray, int partitionArrayLength, QList<unsigned int>& subsetNeurIDs){
    for(int i=0; i<partitionArrayLength; ++i){
	if(partitionArray[i] == 1)
	    aPartition.append(subsetNeurIDs[i]);
	else
	    bPartition.append(subsetNeurIDs[i]);
    }

    //Sanity check
    if( (aPartition.size() + bPartition.size()) != subsetNeurIDs.size())
	throw SpikeStreamAnalysisException("The size of the two partitions does not match the size of the entire subset.");
}


int PhiCalculator::getFiringState(unsigned int neurID){
    //Run sanity check
    if(!weightlessNeuronMap.contains(neurID))
	throw SpikeStreamAnalysisException("Neuron ID cannot be found: " + QString::number(neurID));

    //Get firing state, check it and return
    if(firingNeuronMap.contains(neurID))
	return 1;
    return 0;
}
