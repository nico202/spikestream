#include "ConnectionGroupInfo.h"
using namespace spikestream;

/*! Empty constructor */
ConnectionGroupInfo::ConnectionGroupInfo(){
    id = 0;
    description = "undefined";
    fromNeuronGroupID = 0;
    toNeuronGroupID = 0;
    synapseType = 0;
}

/*! Standard constructor */
ConnectionGroupInfo::ConnectionGroupInfo(unsigned int id, const QString& desc, unsigned int fromID, unsigned int toID, unsigned int synType, QHash<QString, double> paramMap){
    this->id = id;
    this->description = desc;
    this->fromNeuronGroupID = fromID;
    this->toNeuronGroupID = toID;
    this->synapseType = synType;
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
