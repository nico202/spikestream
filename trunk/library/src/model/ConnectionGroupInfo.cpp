//SpikeStream includes
#include "ConnectionGroupInfo.h"
#include "GlobalVariables.h"
#include "SpikeStreamException.h"
using namespace spikestream;


/*! Empty constructor */
ConnectionGroupInfo::ConnectionGroupInfo(){
    id = 0;
    description = "undefined";
    fromNeuronGroupID = 0;
    toNeuronGroupID = 0;
}


/*! Standard constructor */
ConnectionGroupInfo::ConnectionGroupInfo(unsigned int id, const QString& desc, unsigned int fromID, unsigned int toID, QHash<QString, double> paramMap, const SynapseType& synapseType){
	//Check that name and description will fit in the database
    if(desc.size() > MAX_DATABASE_DESCRIPTION_LENGTH)
	throw SpikeStreamException("ConnectionGroup: Description length exceeds maximum possible size in database.");

    this->id = id;
    this->description = desc;
    this->fromNeuronGroupID = fromID;
    this->toNeuronGroupID = toID;
	this->synapseType = synapseType;
    this->parameterMap = paramMap;
}


/*! Copy constructor */
ConnectionGroupInfo::ConnectionGroupInfo(const ConnectionGroupInfo& conGrpInfo){
    this->id = conGrpInfo.id;
    this->description = conGrpInfo.description;
    this->fromNeuronGroupID = conGrpInfo.fromNeuronGroupID;
    this->toNeuronGroupID = conGrpInfo.toNeuronGroupID;
	this->synapseType = conGrpInfo.synapseType;
    this->parameterMap = conGrpInfo.parameterMap;
}


/*! Destructor */
ConnectionGroupInfo::~ConnectionGroupInfo(){
}


/*--------------------------------------------------------- */
/*-----                PUBLIC METHODS                 ----- */
/*--------------------------------------------------------- */

/*! Assignment operator */
ConnectionGroupInfo& ConnectionGroupInfo::operator=(const ConnectionGroupInfo& rhs){
    //Check for self assignment
    if(this == &rhs)
	return *this;

    this->id = rhs.id;
    this->description = rhs.description;
    this->fromNeuronGroupID = rhs.fromNeuronGroupID;
    this->toNeuronGroupID = rhs.toNeuronGroupID;
	this->synapseType = rhs.synapseType;
    this->parameterMap = rhs.parameterMap;

    return *this;
}


/*! Returns the parameter requested or throws an exception if it does not exist */
double ConnectionGroupInfo::getParameter(const QString &parameterName){
	if(parameterMap.contains(parameterName))
		return parameterMap[parameterName];
	throw SpikeStreamException("Parameter '" + parameterName + "' not found in parameter map.");
}


/*! Returns the parameters as an XML string */
QString ConnectionGroupInfo::getParameterXML(){
    QString tmpStr = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
    tmpStr += "<connection_group_parameters>";
    for(QHash<QString, double>::iterator iter = parameterMap.begin(); iter != parameterMap.end(); ++iter){
	tmpStr +="<parameter>";
	tmpStr += "<name>" + iter.key() + "</name>";
	tmpStr += "<value>" + QString::number(iter.value()) + "</value>";
	tmpStr += "</parameter>";
    }
    tmpStr += "</connection_group_parameters>";
    return tmpStr;
}


/*! Returns true if the connection group has this parameter. */
bool ConnectionGroupInfo::hasParameter(const QString &parameterName){
	return parameterMap.contains(parameterName);
}


