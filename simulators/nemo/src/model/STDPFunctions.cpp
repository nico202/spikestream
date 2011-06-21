//SpikeStream includes
#include "SpikeStreamException.h"
#include "StandardSTDPFunction.h"
#include "StepSTDPFunction.h"
#include "STDPFunctions.h"
using namespace spikestream;

//Declare static variables
unsigned STDPFunctions::STANDARD_STDP = 0;
unsigned STDPFunctions::STEP_STDP = 1;
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


/*! Returns the interval between applications of the STDP function. */
timestep_t STDPFunctions::getApplySTDPInterval(unsigned functionID){
	checkFunctionID(functionID);
	return functionMap[functionID]->getApplySTDPInterval();
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
	else if(functionID == STEP_STDP)
		return "Step STDP function.";
	throw SpikeStreamException("STDP Function ID not recognized: " + QString::number(functionID));
}


/*! Returns a list of the IDs of the currently available functions. */
QList<unsigned> STDPFunctions::getFunctionIDs(){
	QList<unsigned> tmpList;
	tmpList.append(STANDARD_STDP);
	tmpList.append(STEP_STDP);
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


/*! Returns the minimum excitatory weight for the specified function.
	Throws an exception if the function ID is not recognized.
	Builds the maps of weights if this has not been done already. */
float STDPFunctions::getMinExcitatoryWeight(unsigned functionID){
	checkInitialization();
	checkFunctionID(functionID);
	return functionMap[functionID]->getMinExcitatoryWeight();
}


/*! Returns the maximum excitatory weight for the specified function.
	Throws an exception if the function ID is not recognized.
	Builds the maps of weights if this has not been done already. */
float STDPFunctions::getMaxExcitatoryWeight(unsigned functionID){
	checkInitialization();
	checkFunctionID(functionID);
	return functionMap[functionID]->getMaxExcitatoryWeight();
}


/*! Returns the minimum inhibitory weight for the specified function.
 Throws an exception if the function ID is not recognized.
 Builds the maps of weights if this has not been done already. */
float STDPFunctions::getMinInhibitoryWeight(unsigned functionID){
	checkInitialization();
	checkFunctionID(functionID);
	return functionMap[functionID]->getMinInhibitoryWeight();
}


/*! Returns the maximum inhibitory weight for the specified function.
 Throws an exception if the function ID is not recognized.
 Builds the maps of weights if this has not been done already. */
float STDPFunctions::getMaxInhibitoryWeight(unsigned functionID){
	checkInitialization();
	checkFunctionID(functionID);
	return functionMap[functionID]->getMaxInhibitoryWeight();
}


/*! Returns the reward associated with the application of STDP learning. */
float STDPFunctions::getReward(unsigned functionID){
	checkInitialization();
	checkFunctionID(functionID);
	return functionMap[functionID]->getReward();
}


/*! Prints out the specified function */
void STDPFunctions::print(unsigned functionID){
	checkInitialization();
	checkFunctionID(functionID);
	functionMap[functionID]->print();
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
	functionMap[STEP_STDP] = new StepSTDPFunction();

	//Record the fact that initialization has now been carried out.
	initialized = true;
}


