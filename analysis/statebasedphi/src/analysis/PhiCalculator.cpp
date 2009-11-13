#include "PhiCalculator.h"
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
	    fillPartitionLists(aPartition, bPartition, partitionArray);

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

/*! Fills an array to select k neurons out of n.
	Need to fill it with the lowest value + 1 so that next_permutation runs through all values
	before returning false. */
void PhiCalculator::fillSelectionArray(bool* array, int arraySize, int selectionSize){
    int nonSelectionSize = arraySize - selectionSize;

    //Add zeros at start of array up to the non-selection size
    for(int i=0; i<nonSelectionSize; ++i)
	selectionArray[i] = 0;

    //Add 1s to the rest of the array
    for(int i=nonSelectionSize; i<arraySize; ++i)
	selectionArray[i] = 1;
}


/*! Calculates the phi for the specified partition */
double PhiCalculator::getPartitionPhi(QList<unsigned int>& aPartition, QList<unsigned int>& bPartition){


}


