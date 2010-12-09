//SpikeStream includes
#include "NRMNeuron.h"
#include "NRMException.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;


/*! Constructor */
NRMNeuron::NRMNeuron(){
	//Initialize variables
	trainingArrayLength = 0;
}


/*! Destructor */
NRMNeuron::~NRMNeuron(){
}


/*-------------------------------------------------------*/
/*-----                PUBLIC METHODS               -----*/
/*-------------------------------------------------------*/

/*! Adds training to the neuron */
void NRMNeuron::addTraining(unsigned char* inBitArray, unsigned int inBitArrLen, unsigned char output){
	if(trainingList.size() == 0)
		trainingArrayLength = inBitArrLen + 1;
	else if( (inBitArrLen + 1) != trainingArrayLength)
		throw NRMException("Length of current training array does not match new training data.");

	//Copy training information into a new dynamic array
	unsigned char* tmpArr = new unsigned char[trainingArrayLength];
	tmpArr[0] = output;
	for(unsigned int i=0; i<inBitArrLen; ++i)
		tmpArr[i+1] = inBitArray[i];

	//Add array to the training list
	trainingList.append(tmpArr);
}


/*! Returns the neuron's training */
QList<unsigned char*> NRMNeuron::getTraining(){
	return trainingList;
}


/*! Prints out the training */
void NRMNeuron::printTraining(){
	cout<<"========= Neuron Training =============="<<endl;
	for(QList<unsigned char*>::iterator iter = trainingList.begin(); iter != trainingList.end(); ++iter){
		unsigned char* tmpArray = *iter;
		for(unsigned int byteCount=1; byteCount<trainingArrayLength; ++byteCount){//Work through the bytes starting at position 1
			unsigned char currentByte = tmpArray[byteCount];
			unsigned char andByte = 128;//Should be 10000000
			for(unsigned int bitCount = 0; bitCount < 8; ++bitCount){//Work through the bits
				if(andByte & currentByte)
					cout<<"1";
				else
					cout<<"0";
				andByte >>= 1;//Move the 1 one position to the right
			}
			cout<<" ";
		}
		if(tmpArray[0])
			cout<<": Output 1"<<endl;
		else
			cout<<": Output 0"<<endl;
	}
}



