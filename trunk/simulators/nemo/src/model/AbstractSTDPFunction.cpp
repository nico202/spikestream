//SpikeStream includes
#include "AbstractSTDPFunction.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;


/*! Constructor */
AbstractSTDPFunction::AbstractSTDPFunction(){
	functionUpToDate = false;

	//Create parameters common to all STDP functions
	defaultParameterMap["reward"] = 1.0;
	parameterInfoList.append(ParameterInfo("reward", "Reward applied for each iteration of STDP learning.", ParameterInfo::DOUBLE));
	defaultParameterMap["apply_stdp_interval"] = 1.0;
	parameterInfoList.append(ParameterInfo("apply_stdp_interval", "Time in between each call to nemo_apply_stdp.", ParameterInfo::UNSIGNED_INTEGER));
}


/*! Destructor */
AbstractSTDPFunction::~AbstractSTDPFunction(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Returns the interval between applying STDP */
timestep_t AbstractSTDPFunction::getApplySTDPInterval(){
	return Util::rUInt(getParameter("apply_stdp_interval"));
}


/*! Returns the parameters for this function. */
QHash<QString, double> AbstractSTDPFunction::getParameters(){
	return parameterMap;
}


/*! Sets the parameters for this function */
void AbstractSTDPFunction::setParameters(QHash<QString, double>& newParamMap){
	if(parameterMap.size() != newParamMap.size())
		throw SpikeStreamException("Size of new parameter map does not match that of current parameters");
	for(QHash<QString,double>::iterator iter = newParamMap.begin(); iter != newParamMap.end(); ++iter){
		if(!parameterMap.contains(iter.key()))
			throw SpikeStreamException("Parameter " + iter.key() + " is not valid for this STDP function.");
		parameterMap[iter.key()] = iter.value();
	}

	//Need to rebuild function
	functionUpToDate = false;
}


/*! Returns the default parameters for this STDP function */
QHash<QString, double> AbstractSTDPFunction::getDefaultParameters(){
	return defaultParameterMap;
}


/*! Returns information about the parameters for this STDP function. */
QList<ParameterInfo> AbstractSTDPFunction::getParameterInfoList(){
	return parameterInfoList;
}


/*! Returns reward parameter, which is common to all STDP functions. */
float AbstractSTDPFunction::getReward(){
	return getParameter("reward");
}


/*----------------------------------------------------------*/
/*-----               PROTECTED METHODS                -----*/
/*----------------------------------------------------------*/

/*! Returns the current value of the specified parameter.
	Throws an exception if the parameter cannot be found */
double AbstractSTDPFunction::getParameter(QString parameterName){
	if(!parameterMap.contains(parameterName))
		throw SpikeStreamException("Parameter " + parameterName + " cannot be found.");
	return parameterMap[parameterName];
}


