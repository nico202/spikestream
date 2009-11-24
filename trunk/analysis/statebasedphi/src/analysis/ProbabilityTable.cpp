//SpikeStream includes
#include "ProbabilityTable.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>


/*! Constructor */
ProbabilityTable::ProbabilityTable(int size){
    this->numElements = size;
    buildProbabilityTable();
}


/*! Destructor */
ProbabilityTable::~ProbabilityTable(){
}


/*-------------------------------------------------------------*/
/*-------                 PUBLIC METHODS                 ------*/
/*-------------------------------------------------------------*/

/*! Returns the probability associated with the specifed key */
double ProbabilityTable::get(const QString& key){
    if(!probValueMap.contains(key)){
	qDebug()<<"Probability table key cannot be found: "<<key;
	throw SpikeStreamAnalysisException("Probability table key cannot be found");
    }

    //Return the value associated with the key
    return probValueMap[key];
}


/*! Sets the entry in the probability table */
void ProbabilityTable::set(const QString& key, double value){
    if(!probValueMap.contains(key)){
	qDebug()<<"Probability table key cannot be found: "<<key;
	throw SpikeStreamAnalysisException("Probability table key cannot be found");
    }

    //Store the value
    probValueMap[key] = value;
}


/*-------------------------------------------------------------*/
/*-------                 PRIVATE METHODS                ------*/
/*-------------------------------------------------------------*/

/*! Constructs the complete probability table */
void ProbabilityTable::buildProbabilityTable(){
    //Create array to carry out selection of all possibilites
    bool selectionArray[numElements];
    int numOnes = 0;
    while (numOnes <= numElements){
	Util::fillSelectionArray(&selectionArray, numElements, numOnes);
	bool permutationsComplete = false;
	while(!permutationsComplete){

	    //Create string for probability entry
	    QString tmpKeyStr = "";
	    for(int i=0; i<numElements; ++i){
		if(selectionArray[i])
		    tmpKeyStr += "1";
		else
		    tmpKeyStr += "0";
	    }

	    //Add string to hash map
	    probValueMap[tmpKeyStr] = 0.0;

	   //Change the selection array
	   permutationsComplete = !next_permutation(&selectionArray[0], &selectionArray[numElements]);
	}

	//Increase the number of ones in the selection
	++numOnes;
}


