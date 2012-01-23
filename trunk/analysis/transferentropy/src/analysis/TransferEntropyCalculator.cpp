//SpikeStream includes
#include "TransferEntropyCalculator.h"
#include "SpikeStreamAnalysisException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

//Other includes
#include <cmath>
#include <iostream>
using namespace std;

/*! Number of bits in an unsigned variable */
#define NUM_BITS 32

/*! Output debug information about probabilities */
#define DEBUG_PROBABILITIES


/*! Constructor */
TransferEntropyCalculator::TransferEntropyCalculator(unsigned k_param, unsigned l_param, unsigned timeWindow){
	//Run some checks
	if(k_param == 0 || l_param == 0 || timeWindow == 0)
		throw SpikeStreamAnalysisException("Parameters cannot be zero");
	if(k_param < l_param)
		throw SpikeStreamAnalysisException("k_param must be greater than or equal to l_param");

	//Store parameters
	this->k_param = k_param;
	this->l_param = l_param;
	this->timeWindow = timeWindow;
}


/*! Destructor */
TransferEntropyCalculator::~TransferEntropyCalculator(){
}


/*----------------------------------------------------------*/
/*------                PUBLIC METHODS                ------*/
/*----------------------------------------------------------*/

/*! Calculates the transfer entropy between the two sets of data for the time window specified.
	fromNeuronData contains list of unsigned 1s and 0s indicating the spike activity in J.
	toNeuronData contains list of unsigned 1s and 0s indicating the spike activity in I. */
double TransferEntropyCalculator::getTransferEntropy(unsigned startTimeStep, vector<unsigned>& fromNeuronData, vector<unsigned>& toNeuronData){
	//Run a couple of checks
	if(startTimeStep + timeWindow > fromNeuronData.size())
		throw SpikeStreamAnalysisException("Start time step + time window exceeds available data");
	if(fromNeuronData.size() != toNeuronData.size())
		throw SpikeStreamAnalysisException("From neuron data size does not match to neuron data size");

	//Load all probabilities for the time sequence into hash maps
	load_I_k_probabilities(startTimeStep, toNeuronData);
	load_I_k_plus_1_probabilities(startTimeStep, toNeuronData);
	load_I_k_J_k_probabilities(startTimeStep, fromNeuronData, toNeuronData);
	load_I_k_plus_1_J_k_probabilities(startTimeStep, fromNeuronData, toNeuronData);

	//Work through sequence and calculate transfer entropy
	double sum=0.0;
	for(unsigned n= (startTimeStep+k_param-1); n < startTimeStep+timeWindow-1; ++n){
		//-------------------------------------------------------
		//Get joint probability p(i_n+1, i_n^k, j_n^k)
		//-------------------------------------------------------
		//Load i_n+1,i_n^k into integer
		unsigned tmpNum = 0;
		for(unsigned b = (n-k_param+1); b <= n+1; ++b){
			tmpNum <<= 1;
			tmpNum |= toNeuronData.at(b);
		}

		//Load j_n^l into integer
		for(unsigned b = (n-l_param+1); b <=n; ++b){
			tmpNum <<= 1;
			tmpNum |= fromNeuronData.at(b);
		}
		double p1 = I_k_plus_1_J_1_probs[tmpNum];


		//-------------------------------------------------------
		//Get joint probability p(i_n^k, j_n^k)
		//-------------------------------------------------------
		//Load I into integer
		tmpNum = 0;
		for(unsigned b = (n-k_param+1); b <=n; ++b){
			tmpNum <<= 1;
			tmpNum |= toNeuronData.at(b);
		}

		//Load J into integer
		for(unsigned b = (n-l_param+1); b <=n; ++b){
			tmpNum <<= 1;
			tmpNum |= fromNeuronData.at(b);
		}
		double p2a = I_k_J_l_probs[tmpNum];


		//-------------------------------------------------------
		// Calculate conditional probability p(i_n+1 | i_n^k, j_n^k)
		//-------------------------------------------------------
		double p2=0.0;
		if(p2a != 0)
			p2 = p1/p2a; //p(A|B) = p(A and B)/P(B)


		//-------------------------------------------------------
		// Get joint probability p(i_n+1, i_n^k)
		//-------------------------------------------------------
		//Load i_n+1,i_n^k into integer
		tmpNum = 0;
		for(unsigned b = (n-k_param+1); b <= n+1; ++b){
			tmpNum <<= 1;
			tmpNum |= toNeuronData.at(b);
		}
		double p3a = I_k_plus_1_probs[tmpNum];


		//-------------------------------------------------------
		// Get joint probability p(i_n^k)
		//-------------------------------------------------------
		//Load I into integer
		tmpNum = 0;
		for(unsigned b = (n-k_param+1); b <=n; ++b){
			tmpNum <<= 1;
			tmpNum |= toNeuronData.at(b);
		}
		double p3b = I_k_probs[tmpNum];


		//-------------------------------------------------------
		// Get conditional probability p(i_n+1 | i_n^k)
		//-------------------------------------------------------
		double p3 = 0.0;
		if(p3b != 0.0)
			p3 = p3a/p3b; //p(A|B) = p(A and B)/P(B)


		//Update sum using probabilities
		if(p3 != 0.0 && p2 != 0.0)
			sum += p1 * log2 (p2 / p3);
	}

	//Return sum
	return sum;
}


/*! Loads probabilites, p(i_n^k), into I_k_probs hash map.
	Works through entire sequence, totals the number of times that each pattern occurs, and converts this into a probability.
	Made public to facilitate unit testing. */
void TransferEntropyCalculator::load_I_k_probabilities(unsigned startTimeStep, vector<unsigned>& iVector){
	unsigned patternCount = 0;
	I_k_probs.clear();

	//Mask to clear bits after shift
	unsigned clearEndBits = 1<<k_param;
	--clearEndBits;

	//Load in first number and count it
	unsigned tmpNum = 0;
	for(unsigned b = startTimeStep; b < startTimeStep+k_param; ++b){
		tmpNum <<= 1;
		tmpNum |= iVector.at(b);
	}
	I_k_probs[tmpNum] = 1.0;
	++patternCount;

	//Load in subsequent numbers
	for(unsigned b = startTimeStep+k_param; b < startTimeStep+timeWindow; ++b){
		tmpNum <<= 1;
		tmpNum |= iVector.at(b);
		tmpNum &= clearEndBits;

		//Add to hash map
		if(!I_k_probs.contains(tmpNum))
			I_k_probs[tmpNum] = 1.0;
		else
			++I_k_probs[tmpNum];
		++patternCount;//Could work this out, but safer just to add
	}

	//Convert count of instances into probabilites
	for(QHash<unsigned, double>::iterator iter = I_k_probs.begin(); iter != I_k_probs.end(); ++iter)
		iter.value() /= patternCount;

	#ifdef DEBUG_PROBABILITIES
		printMap(I_k_probs, "I_k_probs");
	#endif//DEBUG_PROBABILITIES
}


/*! Loads probabilites, p(i_n+1^k), into I_k_plus_1_probs hash map.
	Works through entire sequence, totals the number of times that each pattern occurs, and converts this into a probability.
	Made public to facilitate unit testing. */
void TransferEntropyCalculator::load_I_k_plus_1_probabilities(unsigned startTimeStep, vector<unsigned>& iVector){
	unsigned patternCount = 0;
	I_k_plus_1_probs.clear();

	//Mask to clear bits after shift
	unsigned clearEndBits = 1<<(k_param+1);
	--clearEndBits;

	//Load in first number and count it
	unsigned tmpNum = 0;
	for(unsigned b = startTimeStep; b < startTimeStep+k_param+1; ++b){
		tmpNum <<= 1;
		tmpNum |= iVector.at(b);
	}
	I_k_plus_1_probs[tmpNum] = 1.0;
	++patternCount;

	//Load in subsequent numbers
	for(unsigned b = startTimeStep+k_param+1; b < startTimeStep+timeWindow; ++b){
		tmpNum <<= 1;
		tmpNum |= iVector.at(b);
		tmpNum &= clearEndBits;

		//Add to hash map
		if(!I_k_plus_1_probs.contains(tmpNum))
			I_k_plus_1_probs[tmpNum] = 1.0;
		else
			++I_k_plus_1_probs[tmpNum];
		++patternCount;//Could work this out, but safer just to add
	}

	//Convert count of instances into probabilites
	for(QHash<unsigned, double>::iterator iter = I_k_plus_1_probs.begin(); iter != I_k_plus_1_probs.end(); ++iter)
		iter.value() /= patternCount;

	#ifdef DEBUG_PROBABILITIES
		printMap(I_k_plus_1_probs, "I_k_plus_1_probs");
	#endif//DEBUG_PROBABILITIES
}


void TransferEntropyCalculator::load_I_k_J_k_probabilities(unsigned startTimeStep, vector<unsigned>& jVector, vector<unsigned>& iVector){
	unsigned patternCount = 0;
	I_k_J_l_probs.clear();

	//Mask to clear bits after shift. Needs to clear end bits as well as the least significant bit in i
	unsigned clearBits = 1<<(k_param+l_param);
	--clearBits;//This will now clear any bits after the end of i
	clearBits ^= (1<<l_param+1);//XOR to clear LSB value in i

	//Load in first number from i
	unsigned tmpNum = 0;
	for(unsigned b = startTimeStep; b < startTimeStep+k_param; ++b){
		tmpNum <<= 1;
		tmpNum |= iVector.at(b);
	}
	//Add first number from j
	for(unsigned b = startTimeStep; b < startTimeStep+l_param; ++b){
		tmpNum <<= 1;
		tmpNum |= jVector.at(b);
	}
	I_k_J_l_probs[tmpNum] = 1.0;
	++patternCount;

	//Load in subsequent numbers
	for(unsigned b = startTimeStep+k_param; b < startTimeStep+timeWindow; ++b){
		tmpNum <<= 1;
		tmpNum &= clearBits;

		//Add in i value
		tmpNum |= (iVector.at(b)<<(l_param+1));

		//Add in j value
		tmpNum |= jVector.at(b);

		//Add to hash map
		if(!I_k_J_l_probs.contains(tmpNum))
			I_k_J_l_probs[tmpNum] = 1.0;
		else
			++I_k_J_l_probs[tmpNum];
		++patternCount;//Could work this out, but safer just to add
	}

	//Convert count of instances into probabilites
	for(QHash<unsigned, double>::iterator iter = I_k_J_l_probs.begin(); iter != I_k_J_l_probs.end(); ++iter)
		iter.value() /= patternCount;

	#ifdef DEBUG_PROBABILITIES
		printMap(I_k_J_l_probs, "I_k_J_l_probs");
	#endif//DEBUG_PROBABILITIES
}



void TransferEntropyCalculator::load_I_k_plus_1_J_k_probabilities(unsigned startTimeStep, vector<unsigned>& jVector, vector<unsigned>& iVector){

}

/*! Converts the number to a string showing its 1's and 0's */
QString TransferEntropyCalculator::getBitString(unsigned num){
	//Starting mask
	unsigned mask = 1<<(NUM_BITS-1);

	QString tmpStr = "";
	for(int i=0; i<NUM_BITS; ++i){
		//Space out the bits to make them more legible
		if(i && i%8 == 0)
			tmpStr += " ";

		if(mask&num)
			tmpStr += "1";
		else
			tmpStr += "0";

		mask >>= 1;

	}
	return tmpStr;
}



/*----------------------------------------------------------*/
/*------                PRIVATE METHODS               ------*/
/*----------------------------------------------------------*/

/*! Prints out a map for debugging purposes */
void TransferEntropyCalculator::printMap(QHash<unsigned, double> &map, QString name){
	cout<<"-------------------"<<name.toStdString()<<"-------------------------"<<endl;
	for(QHash<unsigned, double>::iterator iter = map.begin(); iter != map.end(); ++iter){
		cout<<"Key: "<<iter.key()<<"/"<<getBitString(iter.key()).toStdString()<<"; value: "<<iter.value()<<endl;
	}
	cout<<endl;
}

