//SpikeStream includes
#include "SpikeStreamException.h"
#include "StandardSTDPFunction.h"
#include "STDPFunctions.h"
using namespace spikestream;

//Declare static variables
unsigned STDPFunctions::STANDARD_STDP = 0;
bool STDPFunctions::initialized = false;
QHash<unsigned, AbstractSTDPFunction*> STDPFunctions::functionMap;


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Deletes dynamically allocated classes */
void STDPFunctions::cleanup(){
	for(QHash<unsigned, AbstractSTDPFunction*>::iterator iter = functionMap.begin(); iter != functionMap.end(); ++iter)
		delete iter.value();
	functionMap.clear();
	initialized = false;
}


/*! Returns the parameters associated with a particular STDP function. */
QHash<QString, double> STDPFunctions::getParameters(unsigned functionID){
	checkInitialization();
	checkFunctionID(functionID);
	return functionMap[functionID]->getParameters();
}


/*! Sets the parameters associated with a particular STDP function */
void STDPFunctions::setParameters(unsigned functionID, QHash<QString, double>& newParameterMap){
	checkInitialization();
	checkFunctionID(functionID);
	functionMap[functionID]->setParameters(newParameterMap);
}


/*! Returns the default parameters associated with a particular STDP function */
QHash<QString, double> STDPFunctions::getDefaultParameters(unsigned functionID){
	checkInitialization();
	checkFunctionID(functionID);
	return functionMap[functionID]->getDefaultParameters();
}


/*! Returns information about the parameters associated with a particular STDP function */
QList<ParameterInfo> STDPFunctions::getParameterInfoList(unsigned functionID){
	checkInitialization();
	checkFunctionID(functionID);
	return functionMap[functionID]->getParameterInfoList();
}


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
float* STDPFunctions::getPreArray(unsigned functionID){
	checkInitialization();
	checkFunctionID(functionID);
	return functionMap[functionID]->getPreArray();
}


/*! Returns the length of the pre array for the specified function.
	Throws an exception if the function ID is not recognized.
	Builds the function arrays if this has not been done already. */
int STDPFunctions::getPreLength(unsigned functionID){
	checkInitialization();
	checkFunctionID(functionID);
	return functionMap[functionID]->getPreLength();
}


/*! Returns the post array for the specified function.
	Throws an exception if the function ID is not recognized.
	Builds the function arrays if this has not been done already. */
float* STDPFunctions::getPostArray(unsigned functionID){
	checkInitialization();
	checkFunctionID(functionID);
	return functionMap[functionID]->getPostArray();
}


/*! Returns the length of the post array for the specified function.
	Throws an exception if the function ID is not recognized.
	Builds the function arrays if this has not been done already. */
int STDPFunctions::getPostLength(unsigned functionID){
	checkInitialization();
	checkFunctionID(functionID);
	return functionMap[functionID]->getPostLength();
}


/*! Returns the minimum weight for the specified function.
	Throws an exception if the function ID is not recognized.
	Builds the maps of weights if this has not been done already. */
float STDPFunctions::getMinWeight(unsigned functionID){
	checkInitialization();
	checkFunctionID(functionID);
	return functionMap[functionID]->getMinWeight();
}


/*! Returns the maximum weight for the specified function.
	Throws an exception if the function ID is not recognized.
	Builds the maps of weights if this has not been done already. */
float STDPFunctions::getMaxWeight(unsigned functionID){
	checkInitialization();
	checkFunctionID(functionID);
	return functionMap[functionID]->getMaxWeight();
}


/*! Returns the reward associated with the application of STDP learning. */
float STDPFunctions::getReward(unsigned functionID){
	checkInitialization();
	checkFunctionID(functionID);
	return functionMap[functionID]->getReward();
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Checks that the function ID is valid */
void STDPFunctions::checkFunctionID(unsigned functionID){
	if(!functionMap.contains(functionID))
		throw SpikeStreamException("Function ID not recognized: " + QString::number(functionID));
}


/*! Checks to see if functions have been built and builds them if not. */
void STDPFunctions::checkInitialization(){
	if(initialized)
		return;
	initialize();
}


/*! Creates a set of function classes.
	Need to call cleanup to delete these dynamically allocated classes. */
void STDPFunctions::initialize(){
	//Create a class for each STDP function
	functionMap[STANDARD_STDP] = new StandardSTDPFunction();

	//Record the fact that initialization has now been carried out.
	initialized = true;
}


