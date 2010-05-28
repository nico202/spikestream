#include "ParameterInfo.h"
using namespace spikestream;

/*! Standard constructor */
ParameterInfo::ParameterInfo(const QString& name, const QString& description, int type){
	this->name = name;
	this->description = description;
	this->type = type;
}


/*! Copy constructor */
ParameterInfo::ParameterInfo(const ParameterInfo& info){
	this->name = info.name;
	this->description = info.description;
	this->type = info.type;
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
	this->type = rhs.type;

	return *this;
}
