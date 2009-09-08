#include "NeuronGroupInfo.h"
using namespace spikestream;

/*! Empty constructor */
NeuronGroupInfo::NeuronGroupInfo(){
    this->name = "undefined";
    this->description = "undefined";
    this->neuronType = -1;
    this->numberOfNeurons = -1;
}


/*! Normal constructor */
NeuronGroupInfo::NeuronGroupInfo(unsigned int id, const QString& name, const QString& desc, const QHash<QString, double>& paramMap, int neurType, int numNeur){
    this->id = id;
    this->name = name;
    this->description = desc;
    this->parameterMap = paramMap;
    this->neuronType = neurType;
    this->numberOfNeurons = numNeur;
}


/*! Copy constructor */
NeuronGroupInfo::NeuronGroupInfo(const NeuronGroupInfo& neurGrpInfo){
    this->id = neurGrpInfo.id;
    this->name = neurGrpInfo.name;
    this->description = neurGrpInfo.description;
    this->parameterMap = neurGrpInfo.parameterMap;
    this->neuronType = neurGrpInfo.neuronType;
    this->numberOfNeurons = neurGrpInfo.numberOfNeurons;
}


/*! Destructor */
NeuronGroupInfo::~NeuronGroupInfo(){
}


/*--------------------------------------------------------- */
/*-----                PUBLIC METHODS                 ----- */
/*--------------------------------------------------------- */

/*! Assignment operator. */
NeuronGroupInfo& NeuronGroupInfo::operator=(const NeuronGroupInfo& rhs) {
    // Check for self-assignment!
    if (this == &rhs)      // Same object?
      return *this;        // Yes, so skip assignment, and just return *this.

    this->id = rhs.id;
    this->name = rhs.name;
    this->description = rhs.description;
    this->parameterMap = rhs.parameterMap;
    this->neuronType = rhs.neuronType;
    this->numberOfNeurons = rhs.numberOfNeurons;

    return *this;
}


/*! Returns the parameters as an XML string */
QString NeuronGroupInfo::getParameterXML(){
    QString tmpStr = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
    tmpStr += "<neuron_group_parameters>";
    for(QHash<QString, double>::iterator iter = parameterMap.begin(); iter != parameterMap.end(); ++iter){
	tmpStr +="<parameter>";
	tmpStr += "<description>" + iter.key() + "</description>";
	tmpStr += "<value>" + QString::number(iter.value()) + "</value>";
	tmpStr += "</parameter>";
    }
    tmpStr += "</neuron_group_parameters>";
    return tmpStr;
}
