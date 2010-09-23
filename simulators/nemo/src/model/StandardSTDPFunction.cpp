//SpikeStream includes
#include "SpikeStreamException.h"
#include "StandardSTDPFunction.h"
using namespace spikestream;

//Other includes
#include <cmath>
#include <iostream>
using namespace std;

//Prints out the arrays when enabled
#define DEBUG


/*! Constructor */
StandardSTDPFunction::StandardSTDPFunction() : AbstractSTDPFunction() {
	//Information about parameters
	parameterInfoList.append(ParameterInfo("A+", "A+ description", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("A-", "A- description", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("T+", "T+ description", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("T-", "T- description", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("min_weight", "Minimum weight that synapse can reach with learning.", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("max_weight", "Maximum weight that synapse can reach with learning.", ParameterInfo::DOUBLE));

	//Default values of parameters
	defaultParameterMap["A+"] = 0.005;
	defaultParameterMap["A-"] = 0.00525;
	defaultParameterMap["T+"] = 20.0;
	defaultParameterMap["T-"] = 20.0;
	defaultParameterMap["min_weight"] = -1.0;
	defaultParameterMap["max_weight"] = 1.0;

	//Initialise current parameter map with default values
	parameterMap = defaultParameterMap;

	//Create arrays
	preArray = new float[ARRAY_LENGTH];
	postArray = new float[ARRAY_LENGTH];
}


/*! Destructor */
StandardSTDPFunction::~StandardSTDPFunction(){
	delete [] preArray;
	delete [] postArray;
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
	return ARRAY_LENGTH;
}


/*! Returns the post array for the specified function.
	Builds the function arrays if this has not been done already. */
float* StandardSTDPFunction::getPostArray(){
	checkFunctionUpToDate();
	return postArray;
}


/*! Returns the length of the post array for the specified function. */
int StandardSTDPFunction::getPostLength(){
	return ARRAY_LENGTH;
}


/*! Returns the minimum weight for the specified function. */
float StandardSTDPFunction::getMinWeight(){
	return getParameter("min_weight");
}


/*! Returns the maximum weight for the specified function. */
float StandardSTDPFunction::getMaxWeight(){
	return getParameter("max_weight");
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Builds the standard STDP function and adds it to the maps. */
void StandardSTDPFunction::buildStandardSTDPFunction(){
	//Extract parameters
	double aPlus = getParameter("A+");
	double aMinus = getParameter("A-");
	double tPlus = getParameter("T+");
	double tMinus = getParameter("T-");

	//Build the arrays specifying the function
	for(int i = 0; i < ARRAY_LENGTH; ++i) {
		float dt = float(i + 1);
		preArray[i] = aPlus * expf( (-1.0 * dt) / tPlus);
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


/*! Prints out the function */
void StandardSTDPFunction::print(){
	//Extract parameters
	double aPlus = getParameter("A+");
	double aMinus = getParameter("A-");
	double tPlus = getParameter("T+");
	double tMinus = getParameter("T-");

	cout<<"Standard STDP Function"<<endl;
	cout<<"Parameters. Array length: "<<ARRAY_LENGTH<<"; A+: "<<aPlus<<"; A-: "<<aMinus<<"; T+: "<<tPlus<<"; T-: "<<tMinus<<endl;
	for(int i=0; i<ARRAY_LENGTH; ++i)
		cout<<"Pre array ["<<i<<"]: "<<preArray[i]<<endl;
	cout<<endl;
	for(int i=0; i<ARRAY_LENGTH; ++i)
		cout<<"Post array ["<<i<<"]: "<<postArray[i]<<endl;
}





