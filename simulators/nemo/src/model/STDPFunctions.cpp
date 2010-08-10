//SpikeStream includes
#include "SpikeStreamException.h"
#include "STDPFunctions.h"
using namespace spikestream;

//Other includes
#include <cmath>


//Declare static variables
unsigned STDPFunctions::STANDARD_STDP = 0;
bool STDPFunctions::initialized = false;
QHash<unsigned, float*> STDPFunctions::preArrayMap;
QHash<unsigned, int> STDPFunctions::preArrayLengthMap;
QHash<unsigned, float* > STDPFunctions::postArrayMap;
QHash<unsigned, int> STDPFunctions::postArrayLengthMap;
QHash<unsigned, float> STDPFunctions::minWeightMap;
QHash<unsigned, float> STDPFunctions::maxWeightMap;


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Returns a description of the function with the specified ID.
	Throws a SpikeStreamException if the ID is not recognized. */
QString STDPFunctions::getFunctionDescription(unsigned functionID){
	if(functionID == STANDARD_STDP)
		return "Standard STDP function.";
	throw SpikeStreamException("STDP Function ID not recognized: " + QString::number(functionID));
}


/*! Returns a list of the IDs of the currently available functions. */
QList<unsigned> STDPFunctions::getFunctionIDs(){
	QList<unsigned> tmpList;
	tmpList.append(STANDARD_STDP);
	return tmpList;
}


/*! Returns the pre array for the specified function.
	Throws an exception if the function ID is not recognized.
	Builds the function arrays if this has not been done already. */
float* STDPFunctions::getPre(unsigned functionID){
	checkInitialization();
	if(!preArrayMap.contains(functionID))
		throw SpikeStreamException("Pre array request. Function ID not recognized: " + QString::number(functionID));
	return preArrayMap[functionID];
}


/*! Returns the length of the pre array for the specified function.
	Throws an exception if the function ID is not recognized.
	Builds the function arrays if this has not been done already. */
int STDPFunctions::getPreLength(unsigned functionID){
	checkInitialization();
	if(!preArrayLengthMap.contains(functionID))
		throw SpikeStreamException("Pre array length request. Function ID not recognized: " + QString::number(functionID));
	return preArrayLengthMap[functionID];
}


/*! Returns the post array for the specified function.
	Throws an exception if the function ID is not recognized.
	Builds the function arrays if this has not been done already. */
float* STDPFunctions::getPost(unsigned functionID){
	checkInitialization();
	if(!postArrayMap.contains(functionID))
		throw SpikeStreamException("Post array request. Function ID not recognized: " + QString::number(functionID));
	return postArrayMap[functionID];
}


/*! Returns the length of the post array for the specified function.
	Throws an exception if the function ID is not recognized.
	Builds the function arrays if this has not been done already. */
int STDPFunctions::getPostLength(unsigned functionID){
	checkInitialization();
	if(!postArrayLengthMap.contains(functionID))
		throw SpikeStreamException("Post array length request. Function ID not recognized: " + QString::number(functionID));
	return postArrayLengthMap[functionID];
}


/*! Returns the minimum weight for the specified function.
	Throws an exception if the function ID is not recognized.
	Builds the maps of weights if this has not been done already. */
float STDPFunctions::getMinWeight(unsigned functionID){
	checkInitialization();
	if(!minWeightMap.contains(functionID))
		throw SpikeStreamException("Min weight request. Function ID not recognized: " + QString::number(functionID));
	return minWeightMap[functionID];
}


/*! Returns the maximum weight for the specified function.
	Throws an exception if the function ID is not recognized.
	Builds the maps of weights if this has not been done already. */
float STDPFunctions::getMaxWeight(unsigned functionID){
	checkInitialization();
	if(!maxWeightMap.contains(functionID))
		throw SpikeStreamException("Post vector request. Function ID not recognized: " + QString::number(functionID));
	return maxWeightMap[functionID];
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Builds the standard STDP function and adds it to the maps. */
void STDPFunctions::buildStandardStdpFunction(){
	float* pre = new float[20];
	float* post = new float[20];
	for(unsigned i = 0; i < 20; ++i) {
		float dt = float(i + 1);
		pre[i] = 1.0f * expf(-dt / 20.0f);
		post[i] = -0.8f * expf(-dt / 20.0f);
	}
	preArrayMap[STANDARD_STDP] = pre;
	preArrayLengthMap[STANDARD_STDP] = 20;
	postArrayMap[STANDARD_STDP] = post;
	postArrayLengthMap[STANDARD_STDP] = 20;
	minWeightMap[STANDARD_STDP] = -10.0f;
	maxWeightMap[STANDARD_STDP] = 10.0f;
}


/*! Checks to see if functions have been built and builds them if not. */
void STDPFunctions::checkInitialization(){
	if(initialized)
		return;
	initialize();
}


/*! Builds the functions and stores them in the maps. */
void STDPFunctions::initialize(){
	//Clean up any existing data
	initialized = false;
	for(QHash<unsigned, float*>::iterator iter = preArrayMap.begin(); iter != preArrayMap.end(); ++iter)
		delete iter.value();
	preArrayMap.clear();
	preArrayLengthMap.clear();
	for(QHash<unsigned, float*>::iterator iter = postArrayMap.begin(); iter != postArrayMap.end(); ++iter)
		delete iter.value();
	postArrayMap.clear();
	postArrayLengthMap.clear();
	minWeightMap.clear();
	maxWeightMap.clear();

	//Build the functions
	buildStandardStdpFunction();

	//Record the fact that initialization has now been carried out.
	initialized = true;
}






