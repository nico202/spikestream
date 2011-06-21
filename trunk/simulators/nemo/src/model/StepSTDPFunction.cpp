//SpikeStream includes
#include "SpikeStreamException.h"
#include "StepSTDPFunction.h"
using namespace spikestream;

//Other includes
#include <cmath>
#include <iostream>
using namespace std;

//Prints out the arrays when enabled
//#define DEBUG


/*! Constructor */
StepSTDPFunction::StepSTDPFunction() : AbstractSTDPFunction() {
	//Information about parameters
	parameterInfoList.append(ParameterInfo("pre_length", "Length of the pre array", ParameterInfo::INTEGER));
	parameterInfoList.append(ParameterInfo("post_length", "Length of the post array", ParameterInfo::INTEGER));
	parameterInfoList.append(ParameterInfo("pre_y_value", "Location of the pre step function on the Y axis. Can be positive or negative", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("post_y_value", "Location of the post step function on the Y axis. Can be positive or negative", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("min_excitatory_weight", "Minimum weight that excitatory synapse can reach with learning.", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("max_excitatory_weight", "Maximum weight that excitatory synapse can reach with learning.", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("min_inhibitory_weight", "Minimum weight that inhibitory synapse can reach with learning. NOTE: The minimum is defined in ABSOLUTE terms, but the value is negative.", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("max_inhibitory_weight", "Maximum weight that inhibitory synapse can reach with learning. NOTE: The maximum is defined in ABSOLUTE terms, but the value is negative.", ParameterInfo::DOUBLE));

	//Default values of parameters
	defaultParameterMap["pre_length"] = 1;
	defaultParameterMap["post_length"] = 1;
	defaultParameterMap["pre_y_value"] = 0.10;
	defaultParameterMap["post_y_value"] = -0.15;
	defaultParameterMap["min_excitatory_weight"] = 0.0001;
	defaultParameterMap["max_excitatory_weight"] = 1.0;
	defaultParameterMap["min_inhibitory_weight"] = -0.0001;
	defaultParameterMap["max_inhibitory_weight"] = -1.0;


	//Initialize arrays
	preLength = 0;
	postLength = 0;
	preArray = NULL;
	postArray = NULL;

	//Initialise current parameter map with default values
	parameterMap = defaultParameterMap;
}


/*! Destructor */
StepSTDPFunction::~StepSTDPFunction(){
	cleanUp();
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Returns the pre array for the specified function.
	Builds the function arrays if this has not been done already. */
float* StepSTDPFunction::getPreArray(){
	checkFunctionUpToDate();
	return preArray;
}


/*! Returns the length of the pre array for the specified function. */
int StepSTDPFunction::getPreLength(){
	checkFunctionUpToDate();
	return preLength;
}


/*! Returns the post array for the specified function.
	Builds the function arrays if this has not been done already. */
float* StepSTDPFunction::getPostArray(){
	checkFunctionUpToDate();
	return postArray;
}


/*! Returns the length of the post array for the specified function. */
int StepSTDPFunction::getPostLength(){
	checkFunctionUpToDate();
	return postLength;
}


/*! Returns the minimum excitatory weight for the specified function. */
float StepSTDPFunction::getMinExcitatoryWeight(){
	return getParameter("min_excitatory_weight");
}


/*! Returns the maximum excitatory weight for the specified function. */
float StepSTDPFunction::getMaxExcitatoryWeight(){
	return getParameter("max_excitatory_weight");
}


/*! Returns the minimum weight for the specified function. */
float StepSTDPFunction::getMinInhibitoryWeight(){
	return getParameter("min_inhibitory_weight");
}


/*! Returns the maximum weight for the specified function. */
float StepSTDPFunction::getMaxInhibitoryWeight(){
	return getParameter("max_inhibitory_weight");
}


/*! Prints out the function */
void StepSTDPFunction::print(){
	checkFunctionUpToDate();

	//Extract parameters
	double preY = getParameter("pre_y_value");
	double postY = getParameter("post_y_value");

	cout<<"Step STDP Function"<<endl;
	cout<<"Parameters. Pre length: "<<preLength<<"; post length: "<<postLength<<"; preY: "<<preY<<"; postY: "<<postY<<endl;
	for(int i=0; i<preLength; ++i)
		cout<<"Pre array ["<<i<<"]: "<<preArray[i]<<endl;
	cout<<endl;
	for(int i=0; i<postLength; ++i)
		cout<<"Post array ["<<i<<"]: "<<postArray[i]<<endl;
	cout<<"Min excitatory weight: "<<getMinExcitatoryWeight()<<"; Max excitatory weight: "<<getMaxExcitatoryWeight()<<"; Min inhibitory weight: "<<getMinInhibitoryWeight()<<"; Max inhibitory weight: "<<getMaxInhibitoryWeight()<<endl;
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Builds the standard STDP function and adds it to the maps. */
void StepSTDPFunction::buildStepSTDPFunction(){
	//Delete previous arrays if allocated
	cleanUp();

	//Extract parameters
	preLength = (int)getParameter("pre_length");
	postLength = (int)getParameter("post_length");
	double preY = getParameter("pre_y_value");
	double postY = getParameter("post_y_value");

	//Build the arrays specifying the function
	preArray = new float[preLength];
	for(int i = 0; i < preLength; ++i) {
		preArray[i] = preY;
	}
	postArray = new float[postLength];
	for(int i = 0; i < postLength; ++i) {
		postArray[i] = postY;
	}

	#ifdef DEBUG
		print();
	#endif//DEBUG
}


/*! Checks to see if functions have been built and builds them if not. */
void StepSTDPFunction::checkFunctionUpToDate(){
	if(functionUpToDate)
		return;
	buildStepSTDPFunction();
	functionUpToDate = true;
}


/*! Deletes dynamically allocated memory */
void StepSTDPFunction::cleanUp(){
	if(preArray != NULL)
		delete [] preArray;
	preArray = NULL;
	if(postArray != NULL)
		delete [] postArray;
	postArray = NULL;
}





