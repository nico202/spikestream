#include "ParameterInfo.h"
using namespace spikestream;

/*! Standard constructor */
ParameterInfo::ParameterInfo(const QString& name, const QString& description){
	this->name = name;
	this->description = description;
}


/*! Copy constructor */
ParameterInfo::ParameterInfo(const ParameterInfo& info){
	this->name = info.name;
	this->description = info.description;
}

/*! Destructor */
ParameterInfo::~ParameterInfo(){
}


/*! Assignment operator */
ParameterInfo& ParameterInfo::operator=(const ParameterInfo& rhs){
	if(this == &rhs)
		return *this;

	this->name = rhs.name;
	this->description = rhs.description;

	return *this;
}
