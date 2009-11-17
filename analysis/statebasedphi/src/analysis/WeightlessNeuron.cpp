#include WeightlessNeuron.h"
using namespace spikestream;


/*! Constructor */
WeightlessNeuron::WeightlessNeuron(){

    //Set default variable values
    hammingDistance = 0;
}


/*! Destructor */
WeightlessNeuron::~WeightlessNeuron(){
}


void WeightlessNeuron::setTrainingData(QList<QByteArray> data){

}

/*! Returns the probability that the initial pattern led the neuron to be in the specified firing state */
double WeightlessNeuron::getProbability(const QList<unsigned int>& neurIDList, const QString& initialPattern, bool firingState){
    //Run checks on the data
    if(neurIDList.size(); != initialPattern.size())
	throw SpikeStreamAnalysisException("Neuron ID list size does not match the size of the initial pattern.");

    //Build initial pattern string and initialize it
    QString inputPattern = "";
    for(int i=0; i<connectionMap.size(); ++i){
	inputPattern += "?";
    }

    //Set the known neuron IDs, which are in the initial pattern.
    int numberOfMissingNeurons =
    for(int i=0; i<neurIDList.size(); ++i){
	//Neuron in list is connected to this neuron
	if(connectionMap.contains(neurIDList[i])){
	    //Set the part of the input pattern to the state corresponding to the neuron's state
	    int neuronIndex = connectionMap[neurIDList[i]];
	    inputPattern[neuronIndex] = initialPattern[i];
	}
    }

    /* Now have a string which is the input pattern
       Many of the places in this input pattern will be missing, so have to do an appropriate probabilistic match
       with the training patterns. */

}


/*! From NRM HamNeu.cpp.
    */
byte HamNeu::RecallNeuron(byte* inPatArr, int patByteLen, int curDist, int colPlanes, bool& uVal, int altParam) {
    int n;
    byte resp = NOT_SET;
    uVal = false;

    if ( firstPatStruct ) {
	int dist;
	pattern* ps = firstPatStruct;

	while ( true ) {
	    dist = 0;
	    for (n = 0; n < patByteLen; n++)
		dist += lookup[inPatArr[n] ^ ps->patArr[n + 1]];

	    if ( dist < curDist ) {
		resp = ps->patArr[0];
		curDist = dist;
	    }
	    else if ( dist == curDist ) {
		if ( resp == NOT_SET )
		    resp = ps->patArr[0];
		else if ( resp != U_VAL && ps->patArr[0] != resp )
		    resp = U_VAL;
	    }

	    if ( !ps->next )
		break;
	    ps = ps->next;
	}

	if ( resp < 8 )  //  NOT (  U_VAL OR NOT_SET  )
	    return resp;

	uVal = true;

    }

    if ( colPlanes > 1 )
	return RandNum(8);
    else {
	if ( RandNum(2) )
	    return 7;
	else
	    return 0;
    }
}
