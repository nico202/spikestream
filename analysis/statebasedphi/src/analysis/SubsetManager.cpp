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
}


/*! Destructor */
SubsetManager::~SubsetManager(){
}


/*-------------------------------------------------------------*/
/*-------                 PUBLIC METHODS                 ------*/
/*-------------------------------------------------------------*/

/*! Runs the phi calculations on the network for the specified time step */
void SubsetManager::runCalculation(const bool * const stop){
    this->stop = stop;

    //Set up class for the calculation
    initialize();

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

/*!
void SubsetManager::addConnectedSubsets(neuronIDList[i], subsetList){
}



/*! Builds a complete list of the possible subsets
    Only connected subsets are considered because disconnected subsets will have zero phi.
    This method works along the connections, generating all the subsets involving the first neuron
    and then the second neuron and so on... */
void SubsetManager::buildSubsetList(){
    QList<Subset> subsetList;
    for(int i=0; i<neuronIDList.size() && !*stop; ++i){
	addConnectedSubsets(neuronIDList[i], subsetList);
    }
}


/*! Works through the list of subsets and calculates the phi of each */
void SubsetManager::calculateSubsetsPhi(){
    //Create a class to carry out the calculations
    PhiCalculator phiCalculator();

    //Calculate the phi of each subset
    for(int i=0; i<subsetList.size() && !*stop; ++i){
	//Convert the 1's and 0's to neuron ids
	QList<unsigned int> subsetNeurIDs;
	getNeuronIDs(subsetList[i].neurIdxs, subsetList[i].size, subsetNeurIDs);

	//Calculate phi on the subset
	subsetList[i].phi = phiCalculator.getSubsetPhi(subsetNeurIDs);
    }
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

	    //No point in checking whether the subset is contained within itself
	    if(j != i){

		//Second subset must be larger to contain it
		if(subsetList[i].size < subsetList[j].size){

		    //Check to see if first subset is contained in the second
		    if( (subsetList[i].neurIdxs & subsetList[j].neurIdxs) == subsetList[i].neurIdxs ){

			//Does the enclosing subset have a higher value of phi?
			if(subsetList[i].phi < subsetList[j].phi){
			    isComplex = false;
			    break;//FIXME: CHECK THIS BREAK
			}
		    }

		}
	    }
	}

	/* All the other subsets have been checked. If no enclosing subset has been found with higher
	   phi, then the current subset is a complex */
	if(isComplex){
	    //Store complex in database
	    analysisDao->addComplex(analysisInfo.getID(), timeStep, getNeuronIDs(subsetList[i].neurIdxs), subsetList[i].phi);

	    //Inform other classes that a complex has been found
	    emit complexFound();
	}
    }
}


/*! Set calculator into its initial state for the specified network */
void SubsetManager::initialize(){
    //Get a complete list of the neuron IDs in the network
    //FIXME: CHECK THAT THESE ARE SORTED IN ASCENDING ORDER
    neuronIDList = networkDao->getNeuronIDs(analysisInfo.getNetworkID());
}

/*! Fills the supplied list with neuron ids.
    The supplied mpz_class is interpreted as a list of 1's and zeros that correspond to positions
    in the neuronIDList. If there is a 1 in the mpz_class, the neuron id is added; otherwise
    the neuron id is not added. */
void SubsetManager::getNeuronIDs(mpz_class neuronIndexes, int numNeur, QList<unsigned int>& subsetNeurIDs){
    //Clear the list just in case
    subsetNeurIDs.clear();

    /* Variable used to work through the 1's and 0's in the neuronIndexes variable
	Start with the 1 in the extreme right of the variable */
    mpz_class andIndex = 1;
    andIndex << numNeur;

    for(int i=0; i<numNeur; ++i){
	if(neuronIndexes & andIndex == 1){
	    subsetNeurIDs.append(neuronIDList[i]);
	}
	//Shift position of the 1 to the right
	andIndex >> 1;
    }
}


