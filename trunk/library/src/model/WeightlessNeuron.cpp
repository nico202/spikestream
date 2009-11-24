//SpikeStream includes
#include "SpikeStreamException.h"
#include "Util.h"
#include "WeightlessNeuron.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

//Other includes
#include <algorithm>
#include <iostream>
using namespace std;

/*! I think this is an array where each number represents the Hamming distance
    between XORed bytes. So for example, if the two bytes 10000111 and 00000111
    are XORed, you get the number 10000000, or 128, which should result in 1 when
    looked up in the array.
    FIXME: CHECK THIS ARRAY */
byte lookup[] = {
  0,1,1,2, 1,2,2,3, 1,2,2,3, 2,3,3,4,
  1,2,2,3, 2,3,3,4, 2,3,3,4, 3,4,4,5,
  1,2,2,3, 2,3,3,4, 2,3,3,4, 3,4,4,5,
  2,3,3,4, 3,4,4,5, 3,4,4,5, 4,5,5,6,
  1,2,2,3, 2,3,3,4, 2,3,3,4, 3,4,4,5,
  2,3,3,4, 3,4,4,5, 3,4,4,5, 4,5,5,6,
  2,3,3,4, 3,4,4,5, 3,4,4,5, 4,5,5,6,
  3,4,4,5, 4,5,5,6, 4,5,5,6, 5,6,6,7,
  1,2,2,3, 2,3,3,4, 2,3,3,4, 3,4,4,5,
  2,3,3,4, 3,4,4,5, 3,4,4,5, 4,5,5,6,
  2,3,3,4, 3,4,4,5, 3,4,4,5, 4,5,5,6,
  3,4,4,5, 4,5,5,6, 4,5,5,6, 5,6,6,7,
  2,3,3,4, 3,4,4,5, 3,4,4,5, 4,5,5,6,
  3,4,4,5, 4,5,5,6, 4,5,5,6, 5,6,6,7,
  3,4,4,5, 4,5,5,6, 4,5,5,6, 5,6,6,7,
  4,5,5,6, 5,6,6,7, 5,6,6,7, 6,7,7,8
};


/*! Constructor */
WeightlessNeuron::WeightlessNeuron(QHash<unsigned int, unsigned int>& connectionMap){
    //Store variables
    this->connectionMap = connectionMap;

    /* Calculate training data length
       Each bit stores an input with a byte at the beginning for the result */
    if(connectionMap.size() % 8 == 0)
	trainingDataLength = connectionMap.size() / 8 + 1;
    else
	trainingDataLength = connectionMap.size() / 8 + 2;

    //Default setting for hamming threshold
    hammingThreshold = 0;
}


/*! Destructor */
WeightlessNeuron::~WeightlessNeuron(){
   //Delete the training data
   foreach(byte* array, trainingData)
       delete array;
}


/*-------------------------------------------------------------*/
/*-------                  PUBLIC METHODS                ------*/
/*-------------------------------------------------------------*/

/*! Adds the array of bytes to the training data set */
void WeightlessNeuron::addTraining(QByteArray& newData, unsigned int output){
    if( (newData.size() + 1) != trainingDataLength)
	throw SpikeStreamException("New  training data length " + QString::number(newData.size() + 1) + " does not match current training data length " + QString::number(trainingDataLength));

    //Store the data
    byte* newTrainingArray = new byte[trainingDataLength];
    newTrainingArray[0] = output;
    for(int i=1; i<trainingDataLength; ++i){
	newTrainingArray[i] = newData[i-1];
    }
    trainingData.append(newTrainingArray);
}


/*! Compares the supplied pattern with the stored patterns and returns the probability of the
    specified output. If there is a single match, then the probability of the output is either
    1 or 0 depending on whether the stored output matches the specified output.
    If there is more than one match within the Hamming distance, then a random series of 1's
    and 0's is output, so the probability of 1 and 0 are both 0.5 */
double WeightlessNeuron::getFiringStateProbability(byte inPatArr[], int inPatArrLen, int firingState){
    if(inPatArrLen != (trainingDataLength - 1) )
	throw SpikeStreamException("Training data length " + QString::number(trainingDataLength-1) + " does not match pattern length " + QString::number(inPatArrLen));

    //Work through all of the training patterns
    bool firstTime = true;
    byte currentResponse;
    unsigned int minDist;
    byte* trainingPattern;
    QHash<int, byte> minDistIndxMap;
    for(int listIndx=0; listIndx<trainingData.size(); ++listIndx){
	trainingPattern = trainingData[listIndx];

	/* Work through all of the bytes in the input pattern and calculate
	    the total hamming distance between the input pattern and the stored pattern */
	unsigned int hamDist = 0;
	for(int i=0; i<inPatArrLen; ++i){
	    hamDist += lookup[inPatArr[i] ^ trainingPattern[i + 1]];
	    if(hamDist > hammingThreshold)
		break;
	}
	currentResponse = trainingPattern[0];

	if(firstTime){
	    minDist = hamDist;
	    minDistIndxMap[listIndx] = currentResponse;
	    firstTime = false;
	}
	else{
	    //Set this pattern to be the current minimum
	    if(hamDist < minDist){
		minDist = hamDist;
		minDistIndxMap.clear();
		minDistIndxMap[listIndx] = currentResponse;
	    }
	    //Add the index of this pattern to the map if it has the same Hamming distance
	    else if(hamDist == minDist){
		minDistIndxMap[listIndx] = currentResponse;
	    }
	}
    }
    //Return 0.5 if there is  no match within the minimum hamming distance
    if(minDist > hammingThreshold)
	return 0.5;

    //Return 0.5 if there are multiple contradicting matches
    bool zeroFound = false, oneFound = false;
    for(QHash<int, byte>::iterator iter = minDistIndxMap.begin(); iter != minDistIndxMap.end(); ++iter){
	if(iter.value() == 0)
	    zeroFound = true;
	else if(iter.value() == 1)
	    oneFound = true;
	else
	    throw SpikeStreamException("Entry in training data not recognized. It should be 1 or 0: " + QString::number(iter.value()));

	//Responses contradict one another
	if(oneFound && zeroFound)
	    return 0.5;
    }

    /*One or more best matches have been found with the same output.
      Return 1.0 if the output matches the specified firing state or 0.0 otherwise */
    if(minDistIndxMap.begin().value() == firingState)
	return 1.0;
    return 0.0;
}


/*! Returns the probability that the initial pattern led the neuron to be in the specified firing state */
double WeightlessNeuron::getTransitionProbability(const QList<unsigned int>& neurIDList, const QString& s0Pattern, int firingState){
    //Run checks on the data
    if(neurIDList.size() != s0Pattern.size())
	throw SpikeStreamException("Neuron ID list size does not match the size of the s0 pattern.");

    /* Start pattern corresponds to a list of neuron ids, but need the pattern that corresponds
	to the connections to this neuron. FiringNeuronIndexMap links an index in the input pattern
	to the firing state of that neuron*/
    QHash<unsigned int, byte> firingNeuronIndexMap;

    //Set the state of the known neuron IDs in the input pattern
    for(int i=0; i<neurIDList.size(); ++i){
	//Neuron in list is connected to this neuron
	if(connectionMap.contains(neurIDList[i])){
	    //Set the part of the input pattern to the state corresponding to the neuron's state
	    int neuronIndex = connectionMap[neurIDList[i]];
	    firingNeuronIndexMap[neuronIndex] = Util::getUInt(s0Pattern[i]);
	}
    }
    int missingNeuronCount = connectionMap.size() - firingNeuronIndexMap.size();
    if(missingNeuronCount <0)
	throw SpikeStreamException("Error in transition probability calculation. Missing neuron count is less than zero.");

    //Array used to select all combinations of missing neurons
    bool missingNeurSelectionArr[missingNeuronCount];

    //Create array used in the comparison with the stored data in the neuron
    int inPattArrSize = trainingDataLength - 1;
    byte inPattArr [inPattArrSize];

    //Variable to sum the output probabilities for each partially random input string
    double tmpProbOut = 0.0;
    int randomStringCount = 0;

    //Work through all firing combinations of the undefined input neurons
    int numOnes = 0;
    while(numOnes <= missingNeuronCount){

	//Initialize selection array with first combination of 1s and 0s
	Util::fillSelectionArray(missingNeurSelectionArr, missingNeuronCount, numOnes);

	bool permutationsComplete = false;
	while(!permutationsComplete){
	    //Build the input pattern
	    buildInputPattern(inPattArr, inPattArrSize, missingNeurSelectionArr, missingNeuronCount, firingNeuronIndexMap);

	    //Sum the probability that it has the specified output and keep track of the number of calculations
	    tmpProbOut += getFiringStateProbability(inPattArr, inPattArrSize, firingState);
	    ++randomStringCount;

	    //Change the selection array
	    permutationsComplete = !next_permutation(&missingNeurSelectionArr[0], &missingNeurSelectionArr[missingNeuronCount]);
	}
	++numOnes;
    }

    /* Unknown neuron states are random, so each string has the same probability of occurring
       So probability of the firing state is the sum of the prob(state occurring) * prob(firing state for that string) */
    tmpProbOut /= randomStringCount;
    return tmpProbOut;
}


/*! Sets the generalization of the neuron */
void WeightlessNeuron::setGeneralization(double generalization){
    hammingThreshold = Util::rUInt((double)connectionMap.size() * (1.0 - generalization));
}


/*-------------------------------------------------------------*/
/*-------                 PRIVATE METHODS                ------*/
/*-------------------------------------------------------------*/

/*! Constructs a byte array that can be used to compare with the trained arrays stored
    in the neuron */
void WeightlessNeuron::buildInputPattern(byte inPatArr[], int inPatArrSize, bool selArr[], int selArrSize, QHash<unsigned int, byte>& firingNeuronIndexMap){
    int numberOfConnections = connectionMap.size();

    //Clear inPatArr
    for(int i=0; i<inPatArrSize; ++i)
	inPatArr[i] = 0;

    /* Work through each connection and either populate the input array with the permuted random selection
	or with the actual firing state of the neuron, which is stored in the firingNeuronIndexMap */
    int selIndx = 0;
    for(int inputIndx=0; inputIndx < numberOfConnections; ++inputIndx){
	if(inputIndx / 8 > inPatArrSize)
	    throw SpikeStreamException("Input pattern array index out of range.");

	if(firingNeuronIndexMap.contains(inputIndx)){
	    //Set bit in the byte array to 1 or 0 depending on whether it is 1 or 0 in the map of relevant neuron firing states
	    if(firingNeuronIndexMap[inputIndx]){
		inPatArr[ inputIndx/8 ] |= 1<<(inputIndx % 8);
	    }
	}
	else{
	    if(selIndx >= selArrSize)
		throw SpikeStreamException("Selection index out of range. Actual=" + QString::number(selIndx) + "; maximum=" + QString::number(selArrSize));

	    //Set bit in the byte array to 1 if it is set to 1 in selection array
	    if(selArr[selIndx]){
		inPatArr[ inputIndx/8 ] |= 1<<(inputIndx % 8);
	    }
	    ++selIndx;
	}
    }
}

void WeightlessNeuron::printSelectionArray(bool selArr[], int arrSize){
    for(int i=0; i<arrSize; ++i){
	if(selArr[i])
	    cout<<"1";
	else
	    cout<<"0";
    }
    cout<<endl;
}


void WeightlessNeuron::printTraining(){
    foreach(byte* byteArr, trainingData)
	Util::printByteArray(byteArr, trainingDataLength);
}

