//SpikeStream includes
#include "NeuronType.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

/*! Standard constructor */
NeuronType::NeuronType(unsigned int neuronTypeID, const QString& description, const QString& paramTableName, const QString& classLibraryName){
	this->id = neuronTypeID;
	this->description = description;
	this->parameterTableName = paramTableName;
	this->classLibraryName = classLibraryName;
}


/*! Copy constructor */
NeuronType::NeuronType(const NeuronType& neurType){
	this->id = neurType.id;
	this->description = neurType.description;
	this->parameterTableName = neurType.parameterTableName;
	this->classLibraryName = neurType.classLibraryName;
	this->parameterInfoList = neurType.parameterInfoList;
}


/*! Assignment operator */
NeuronType& NeuronType::operator=(const NeuronType& rhs){
	//Check for self assignment
	if(this == &rhs)
		return *this;

	this->id = rhs.id;
	this->description = rhs.description;
	this->parameterTableName = rhs.parameterTableName;
	this->classLibraryName = rhs.classLibraryName;
	this->parameterInfoList = rhs.parameterInfoList;

	return *this;
}


/*! Destructor */
NeuronType::~NeuronType(){
}


/*----------------------------------------------------------*/
/*-----                PUBLIC METHODS                  -----*/
/*----------------------------------------------------------*/

/*! Returns the ID of this neuron type */
unsigned int NeuronType::getID() const {
	return id;
}


/*! Returns a description of this neuron type */
QString NeuronType::getDescription() const {
	return description;
}


/*! Returns the name of the database table holding parameters associated with this neuron type */
QString NeuronType::getParameterTableName() const {
	return parameterTableName;
}


/*! Returns the list of parameter types */
QList<ParameterInfo> NeuronType::getParameterInfoList() const{
	return parameterInfoList;
}


/*! Returns, where applicable, the location of a file holding code for this synapse type. */
QString NeuronType::getClassLibaryName() const{
	return classLibraryName;
}


/*! Sets the information about the parameters */
void NeuronType::setParameterInfoList(QList<ParameterInfo>& parameterInfoList){
	this->parameterInfoList = parameterInfoList;
}


