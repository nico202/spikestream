#include "SubsetManager.h"
using namespace spikestream;


/*! Constructor */
SubsetManager::SubsetManager(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo, const AnalysisInfo& anaInfo, unsigned int timeStep){
    //Store variables
    this->analysisInfo = anaInfo;
    this->timeStep = timeStep;

    //Create data access objects
    networkDao = new NetworkDao(netDBInfo);
    archiveDao = new ArchiveDao(archDBInfo);
    stateDao = new StateBasedPhiAnalysisDao(anaDBInfo);

    //Create phi calculator
    phiCalculator = new PhiCalculator();
}


/*! Destructor */
SubsetManager::~SubsetManager(){
}


/*-------------------------------------------------------------*/
/*-------                 PUBLIC METHODS                 ------*/
/*-------------------------------------------------------------*/

/*! Runs the phi calculations on the network for the specified time step */
void SubsetManager::runCalculation(const bool * const stop){
    //Store reference to stop in invoking class
    this->stop = stop;

    //Get a complete list of the neuron IDs in the network
    //FIXME: CHECK THAT THESE ARE SORTED IN ASCENDING ORDER
    neuronIDList = networkDao->getNeuronIDs(analysisInfo.getNetworkID());

    //Generate a list of all possible connected subsets
    buildSubsetList();

    //Calculate the phi of each of these subsets
    calculateSubsetsPhi();

    //Identify which of the subsets are complexes
    identifyComplexes();

}


/*-------------------------------------------------------------*/
/*-------                 PRIVATE METHODS                ------*/
/*-------------------------------------------------------------*/

/*! Adds subset corresponding to the current selection to the subset list */
void SubsetManager::addSubset(bool* subsetSelectionArray, int arrayLength){
    if(arrayLength != neuronIDList.size())
	throw SpikeStreamAnalysisException("Array length does not match network size.");

    //Create subset and add neurons
    Subset* tmpSubset = new Subset();
    for(int i=0; i<arrayLength; ++i){
	if(subsetSelectionArray[i])
	    subset->addNeuronIndex(i);
    }

    //Store subset in class
    subsetList.append(tmpSubset);
}


/*! Builds a complete list of the possible subsets
    Only connected subsets are considered because disconnected subsets will have zero phi.
    This method works along the connections, generating all the subsets involving the first neuron
    and then the second neuron and so on... */
void SubsetManager::buildSubsetList(){
    int networkSize = neuronIDList.size();

    //Clear the current list of subsets
    deleteSubsets();

    //Create array to select subsets
    bool subsetSelectionArray = new bool[networkSize];
    int subsetSize = networkSize;
    while(!*stop && subsetSize >= 2){
	//Fill permutation array with initial selection
	fillSelectionArray(subsetSelectionArray, networkSize, subsetSize);

	//Work through all permutations at this subset size
	bool permutationsComplete = false;
	while(!*stop && !permutationsComplete){
	    addSubset(subsetSelectionArray);
    	    permutationsComplete = !next_permutation(&subsetSelectionArray[0], &subsetSelectionArray[networkSize]);
	}

	//Decrease the subset size
	--subsetSize;
    }
}


/*! Works through the list of subsets and calculates the phi of each */
void SubsetManager::calculateSubsetsPhi(){
    //Calculate the phi of each subset
    for(int i=0; i<subsetList.size() && !*stop; ++i){
	//Calculate phi on the subset
	subsetList[i].phi = phiCalculator.getSubsetPhi(subsetList[i]->getNeuronIDs());
    }
}


/*! Deletes the current subsets */
void SubsetManager::deleteSubsets(){
    for(int i=0; i<subsetList.size(); ++i)
	delete subsetList[i];
    subsetList.clear();
}


/*! Takes each subset and determines whether it is contained within another subset
    of higher phi. The subset is a complex if no enclosing higher phi subset
    can be found */
void SubsetManager::identifyComplexes(){

    //Check each subset to see if it contained within another subset of higher phi
    for(int i=0; i<subsetList.size() && !*stop; ++i){

	/* Work through all of the other subsets to see if the subset is contained within it and has
	   higher phi. If it cannot find an enclosing subset with higher phi, then it is a complex. */
	bool isComplex = true;
	for(int j=0; j<subsetList.size() && !*stop; ++j){

	    //Is the first subset contained in the second?
	    if(subsetList[j].contains(subsetList[i])){
		if(subsetList[i].phi < subsetList[j].phi){
		    isComplex = false;//Will break out of outer loop and check next subset

		    //Break out of inner loop
		    break;//FIXME: CHECK THIS BREAK
		}
	    }
	}

	/* All the other subsets have been checked. If no enclosing subset has been found with higher
	   phi, then the current subset is a complex */
	if(isComplex){
	    //Store complex in database
	    analysisDao->addComplex(analysisInfo.getID(), timeStep, subsetList[i]->getNeuronIDs(), subsetList[i].phi);

	    //Inform other classes that a complex has been found
	    emit complexFound();
	}
    }
}


/*! Fills an array to select k neurons out of n.
	Need to fill it with the lowest value + 1 so that next_permutation runs through all values
	before returning false. */
void SubsetManager::fillSelectionArray(bool* array, int arraySize, int selectionSize){
    int nonSelectionSize = arraySize - selectionSize;

    //Add zeros at start of array up to the non-selection size
    for(int i=0; i<nonSelectionSize; ++i)
	selectionArray[i] = 0;

    //Add 1s to the rest of the array
    for(int i=nonSelectionSize; i<arraySize; ++i)
	selectionArray[i] = 1;
}

