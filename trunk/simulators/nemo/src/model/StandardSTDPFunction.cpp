//SpikeStream includes
#include "SpikeStreamException.h"
#include "StandardSTDPFunction.h"
using namespace spikestream;

//Other includes
#include <cmath>
#include <iostream>
using namespace std;

//Prints out the arrays when enabled
//#define DEBUG


/*! Constructor */
StandardSTDPFunction::StandardSTDPFunction() : AbstractSTDPFunction() {
	//Information about parameters
	parameterInfoList.append(ParameterInfo("pre_length", "Length of the pre array", ParameterInfo::INTEGER));
	parameterInfoList.append(ParameterInfo("post_length", "Length of the post array", ParameterInfo::INTEGER));
	parameterInfoList.append(ParameterInfo("A+", "A+ description", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("A-", "A- description", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("T+", "T+ description", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("T-", "T- description", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("min_excitatory_weight", "Minimum weight that excitatory synapse can reach with learning.", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("max_excitatory_weight", "Maximum weight that excitatory synapse can reach with learning.", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("min_inhibitory_weight", "Minimum weight that inhibitory synapse can reach with learning. NOTE: The minimum is defined in ABSOLUTE terms, but the value is negative.", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("max_inhibitory_weight", "Maximum weight that inhibitory synapse can reach with learning. NOTE: The maximum is defined in ABSOLUTE terms, but the value is negative.", ParameterInfo::DOUBLE));

	//Default values of parameters
	defaultParameterMap["pre_length"] = 20;
	defaultParameterMap["post_length"] = 20;
	defaultParameterMap["A+"] = 0.005;
	defaultParameterMap["A-"] = 0.00525;
	defaultParameterMap["T+"] = 20.0;
	defaultParameterMap["T-"] = 20.0;
	defaultParameterMap["min_excitatory_weight"] = 0.0001;
	defaultParameterMap["max_excitatory_weight"] = 1.0;
	defaultParameterMap["min_inhibitory_weight"] = -0.0001;
	defaultParameterMap["max_inhibitory_weight"] = -1;

	//Initialise current parameter map with default values
	parameterMap = defaultParameterMap;

	//Initialize arrays
	preLength = 0;
	postLength = 0;
	preArray = NULL;
	postArray = NULL;
}


/*! Destructor */
StandardSTDPFunction::~StandardSTDPFunction(){
	cleanUp();
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Returns the pre array for the specified function.
	Builds the function arrays if this has not been done already. */
float* StandardSTDPFunction::getPreArray(){
	checkFunctionUpToDate();
	return preArray;
}


/*! Returns the length of the pre array for the specified function. */
int StandardSTDPFunction::getPreLength(){
	checkFunctionUpToDate();
	return preLength;
}


/*! Returns the post array for the specified function.
	Builds the function arrays if this has not been done already. */
float* StandardSTDPFunction::getPostArray(){
	checkFunctionUpToDate();
	return postArray;
}


/*! Returns the length of the post array for the specified function. */
int StandardSTDPFunction::getPostLength(){
	checkFunctionUpToDate();
	return postLength;
}


/*! Returns the minimum excitatory weight for the specified function. */
float StandardSTDPFunction::getMinExcitatoryWeight(){
	return getParameter("min_excitatory_weight");
}


/*! Returns the maximum excitatory weight for the specified function. */
float StandardSTDPFunction::getMaxExcitatoryWeight(){
	return getParameter("max_excitatory_weight");
}


/*! Returns the minimum weight for the specified function. */
float StandardSTDPFunction::getMinInhibitoryWeight(){
	return getParameter("min_inhibitory_weight");
}


/*! Returns the maximum weight for the specified function. */
float StandardSTDPFunction::getMaxInhibitoryWeight(){
	return getParameter("max_inhibitory_weight");
}


/*! Prints out the function */
void StandardSTDPFunction::print(){
	checkFunctionUpToDate();

	//Extract parameters
	double aPlus = getParameter("A+");
	double aMinus = getParameter("A-");
	double tPlus = getParameter("T+");
	double tMinus = getParameter("T-");

	cout<<"Standard STDP Function"<<endl;
	cout<<"Parameters. Pre length: "<<preLength<<"; postLength: "<<postLength<<"; A+: "<<aPlus<<"; A-: "<<aMinus<<"; T+: "<<tPlus<<"; T-: "<<tMinus<<endl;
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
void StandardSTDPFunction::buildStandardSTDPFunction(){
	//Delete previous arrays if allocated
	cleanUp();

	//Extract parameters
	preLength = (int)getParameter("pre_length");
	postLength = (int)getParameter("post_length");
	double aPlus = getParameter("A+");
	double aMinus = getParameter("A-");
	double tPlus = getParameter("T+");
	double tMinus = getParameter("T-");

	//Build the arrays specifying the function
	preArray = new float[preLength];
	for(int i = 0; i < preLength; ++i) {
		float dt = float(i + 1);
		preArray[i] = aPlus * expf( (-1.0 * dt) / tPlus);
	}
	postArray = new float[postLength];
	for(int i = 0; i < postLength; ++i) {
		float dt = float(i + 1);
		postArray[i] = -1.0 * aMinus * expf( (-1.0 * dt) / tMinus);
	}

	#ifdef DEBUG
		print();
	#endif//DEBUG
}


/*! Checks to see if functions have been built and builds them if not. */
void StandardSTDPFunction::checkFunctionUpToDate(){
	if(functionUpToDate)
		return;
	buildStandardSTDPFunction();
	functionUpToDate = true;
}


/*! Deletes dynamically allocated memory */
void StandardSTDPFunction::cleanUp(){
	if(preArray != NULL)
		delete [] preArray;
	preArray = NULL;
	if(postArray != NULL)
		delete [] postArray;
	postArray = NULL;
}






