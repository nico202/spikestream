//SpikeStream includes
#include "SpikeStreamAnalysisException.h"
#include "SubsetManager.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

//Other includes
#include <algorithm>
#include <gmpxx.h>
#include <iostream>
#include <sstream>
using namespace std;

/*! Standard Constructor */
SubsetManager::SubsetManager(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo, const AnalysisInfo& anaInfo, unsigned int timeStep) : QObject() {
    //Store variables
    this->analysisInfo = anaInfo;
    this->timeStep = timeStep;

    //Create data access objects
    networkDao = new NetworkDao(netDBInfo);
    archiveDao = new ArchiveDao(archDBInfo);
    stateDao = new StateBasedPhiAnalysisDao(anaDBInfo);

    //Create phi calculator
    phiCalculator = new PhiCalculator(netDBInfo, archDBInfo, anaDBInfo, anaInfo, timeStep, stop);
}


/*! Empty constructor for unit testing */
SubsetManager::SubsetManager(){
    timeStep = -1;
    networkDao = NULL;
    archiveDao = NULL;
    stateDao = NULL;

    //Fix stop variable so that class is always running
    bool* tmpStop = new bool;
    *tmpStop = false;
    stop = tmpStop;

    //Set up progress so that it does not affect tests
    progressCounter = 0;
    numberOfProgressSteps = 0xffff;
}


/*! Destructor */
SubsetManager::~SubsetManager(){
    deleteSubsets();
    delete phiCalculator;

    if(networkDao != NULL)
	delete networkDao;
    if(archiveDao != NULL)
	delete archiveDao;
    if(stateDao != NULL)
	delete stateDao;
}


/*-------------------------------------------------------------*/
/*-------                 PUBLIC METHODS                 ------*/
/*-------------------------------------------------------------*/

/*! Builds a complete list of the possible subsets
    NOTE: It would be better to only consider connected subsets because disconnected subsets will have zero phi.
    However, identifying the connected subsets looked complicated, so brute force is used for now. */
void SubsetManager::buildSubsetList(){
    int networkSize = neuronIDList.size();

    //Clear the current list of subsets
    deleteSubsets();

    //Create array to select subsets
    bool subsetSelectionArray[networkSize];
    int subsetSize = networkSize;
    while(!*stop && subsetSize >= 2){
	//Fill permutation array with initial selection
	Util::fillSelectionArray(subsetSelectionArray, networkSize, subsetSize);

	//Work through all permutations at this subset size
	bool permutationsComplete = false;
	while(!*stop && !permutationsComplete){
	    addSubset(subsetSelectionArray, networkSize);
    	    permutationsComplete = !next_permutation(&subsetSelectionArray[0], &subsetSelectionArray[networkSize]);
	}

	//Decrease the subset size
	--subsetSize;

	//Inform other classes about progress
	updateProgress("Building subset list. " + QString::number(networkSize - subsetSize) + " out of " + QString::number(networkSize-1));
    }
}


/*! Works through the list of subsets and calculates the phi of each */
void SubsetManager::calculateSubsetsPhi(){
    //Calculate the phi of each subset
    for(int i=0; i<subsetList.size() && !*stop; ++i){
	//Calculate phi on the subset
	QList<unsigned int> tmpNeurIDs = subsetList[i]->getNeuronIDs();
	subsetList[i]->setPhi(phiCalculator->getSubsetPhi(tmpNeurIDs));

	//Inform main application about the progress
	updateProgress( "Calculating subset phi. " + QString::number(i+1) + " out of " + QString::number(subsetList.size()) );
    }
}


/*! Takes each subset and determines whether it is contained within another subset
    of higher phi. The subset is a complex if no enclosing higher phi subset
    can be found */
void SubsetManager::identifyComplexes(){
    //Check each subset to see if it contained within another subset of higher phi
    for(int tstIndx=0; tstIndx<subsetList.size() && !*stop; ++tstIndx){

	//Complexes must have phi greater than zero
	if(subsetList[tstIndx]->getPhi() > 0.0){
	    /* Work through all of the other subsets to see if the subset is contained within it and has
	       higher phi. If it cannot find an enclosing subset with higher phi, then it is a complex. */
	    bool isComplex = true;
	    for(int containsIndx=0; containsIndx<subsetList.size() && !*stop; ++containsIndx){

		//Is the first subset contained in the second?
		if(subsetList[containsIndx]->contains(subsetList[tstIndx])){
		    if(subsetList[tstIndx]->getPhi() < subsetList[containsIndx]->getPhi()){
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
		QList<unsigned int> subNeurIDs = subsetList[tstIndx]->getNeuronIDs();
		stateDao->addComplex(analysisInfo.getID(), timeStep, subNeurIDs, subsetList[tstIndx]->getPhi());

		//Inform other classes that a complex has been found
		emit complexFound();

		//qDebug()<<"COMPLEX FOUND: NeuronIDs="<<subNeurIDs<<"; phi="<<subsetList[tstIndx]->getPhi();
	    }
	}

	//Inform main application about progress
	updateProgress("Identifying complexes. " + QString::number(tstIndx + 1) + " out of " + QString::number(subsetList.size()));
    }
}


/*! Runs the phi calculations on the network for the specified time step */
void SubsetManager::runCalculation(const bool * const stop){
    //Store reference to stop in invoking class
    this->stop = stop;
    phiCalculator->setStop(stop);

    //Get a complete list of the neuron IDs in the network
    neuronIDList = networkDao->getNeuronIDs(analysisInfo.getNetworkID());

    //Get a complete map of the connections in the network, if required
    if( analysisInfo.getParameter("ignore_disconnected_subsets") ){
	networkDao->getAllFromConnections(analysisInfo.getNetworkID(), fromConnectionMap);
	networkDao->getAllToConnections(analysisInfo.getNetworkID(), toConnectionMap);
    }

    //Record the number of steps that need to be completed and initialize progress counter
    numberOfProgressSteps = 2 * getMaxSubsetListSize(neuronIDList.size()) + neuronIDList.size() - 1;
    progressCounter = 0;

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
void SubsetManager::addSubset(bool subsetSelectionArray[], int arrayLength){
    if(arrayLength != neuronIDList.size())
	throw SpikeStreamAnalysisException("Array length does not match network size.");

    //Create subset and add neurons
    Subset* tmpSubset = new Subset(&neuronIDList);
    for(int i=0; i<arrayLength; ++i){
	if(subsetSelectionArray[i])
	    tmpSubset->addNeuronIndex(i);
    }

    if(analysisInfo.getParameter("ignore_disconnected_subsets")){
	qDebug()<<"USING IGNORE DISCONNECTED SUBSETS PARAMETER";
	if( subsetConnected(tmpSubset->getNeuronIDs()) ){
	    //Store subset in class
	    subsetList.append(tmpSubset);
	}
    }
    else{
	//Store subset in class
	subsetList.append(tmpSubset);
    }
}


/*! Deletes the current subsets */
void SubsetManager::deleteSubsets(){
    for(int i=0; i<subsetList.size(); ++i)
	delete subsetList[i];
    subsetList.clear();
}


/*! Works out the maximum size of the subset list. Used for indicating progress with the calculation. */
unsigned int SubsetManager::getMaxSubsetListSize(int numberOfNeurons){

    //Work through all the possible subset sizes of the network
    mpf_class numberOfSubsets = 0;
    for(int subSize = 2; subSize <= numberOfNeurons; ++subSize){

	/* Each subset size can be selected in a number of different ways from the network. */
	numberOfSubsets += Util::factorial(numberOfNeurons) / ( Util::factorial(subSize) * Util::factorial(numberOfNeurons - subSize) );
    }

    if(numberOfSubsets > 0xffffffff){
	ostringstream strStream;
	strStream<<"This network has "<<numberOfSubsets<<" subsets. This exceeds the supported number and would take an extremely long time to run.";
	throw SpikeStreamAnalysisException(strStream.str().data());
    }
    return numberOfSubsets.get_si();
}


/*! Prints out the current list of subsets */
void SubsetManager::printSubsets(){
    cout<<"--------------------  SUBSETS  --------------------"<<endl;
    foreach(Subset* subset, subsetList){
	QList<unsigned int> neurIDList = subset->getNeuronIDs();
	foreach(unsigned int neurID, neurIDList)
	    cout<<neurID<<", ";
	cout<<" phi="<<subset->getPhi()<<endl;
    }
}


/*! Returns true if all of the neurons have at least one connection to the other neurons in the subset */
bool SubsetManager::subsetConnected(QList<unsigned int> neuronIDs){
    foreach(unsigned int neuronID, neuronIDs){
	if(fromConnectionMap[neuronID].isEmpty() && toConnectionMap[neuronID].isEmpty())
	    return false;
    }
    return true;
}


/*! Informs other classes about progess with the calculation */
void SubsetManager::updateProgress(const QString& msg){
    ++progressCounter;
    if(progressCounter > numberOfProgressSteps)
	throw SpikeStreamAnalysisException("Progress counter out of range.");

    emit progress(msg, timeStep, progressCounter, numberOfProgressSteps);
}

