//SpikeStream includes
#include "ProbabilityTable.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>


/*! Constructor */
ProbabilityTable::ProbabilityTable(int size){
    this->numEntries = size;

    buildProbabilityTable();
}


/*! Destructor */
ProbabilityTable::~ProbabilityTable(){
}


/*-------------------------------------------------------------*/
/*-------                 PUBLIC METHODS                 ------*/
/*-------------------------------------------------------------*/

/*! Returns the probability associated with the specifed key */
double ProbabilityTable::getProbability(const QString& key){
    if(!probValueMap.contains(key)){
	qDebug()<<"Probability table key cannot be found: "<<key;
	throw SpikeStreamAnalysisException("Probability table key cannot be found");
    }

    //Return the value associated with the key
    return probValueMap[key];
}


/*! Returns a pointer to the map containing the keys and entries.
    NOTE: Not safe, but enables speedy access to data in probability table. */
QHash<QString, double>* ProbabilityTable::getProbabilityValueMap(){
    return &probValueMap;
}


/*! Sets the entry in the probability table */
void ProbabilityTable::setProbability(const QString& key, double value){
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
    bool selectionArray[numEntries];
    int numOnes = 0;
    while (numOnes <= numEntries){
	Util::fillSelectionArray(&selectionArray, numEntries, numOnes);

	bool permutationsComplete = false;
	while(!permutationsComplete){
	    //Create string for probability entry
	    QString tmpKeyStr = "";
	    for(int i=0; i<numEntries; ++i){
		if(selectionArray[i])
		    tmpKeyStr += "1";
		else
		    tmpKeyStr += "0";
	    }

	    //Add string to hash map
	    probValueMap[tmpKeyStr] = 0.0;

	   //Change the selection array
	    permutationsComplete = !next_permutation(&selectionArray[0], &selectionArray[numEntries]);
	}

	//Increase the number of ones in the selection
	++numOnes;
}


