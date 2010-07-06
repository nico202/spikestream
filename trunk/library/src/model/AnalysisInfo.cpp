#include "AnalysisInfo.h"
#include "SpikeStreamAnalysisException.h"
using namespace spikestream;

/*! Default number of threads */
#define DEFAULT_NUMBER_OF_THREADS 5


/*! Empty constructor */
AnalysisInfo::AnalysisInfo(){
	reset();
}


/*! Standard constructor */
AnalysisInfo::AnalysisInfo(unsigned int analysisID, unsigned int networkID, unsigned int archiveID, const QDateTime& startDateTime, const QString& description, const QHash<QString, double>& parameterMap, unsigned int analysisType){
	this->id = analysisID;
	this->networkID = networkID;
	this->archiveID = archiveID;
	this->startDateTime = startDateTime;
	this->parameterMap = parameterMap;
	this->description = description;
	this->analysisType = analysisType;
	this->numberOfThreads = DEFAULT_NUMBER_OF_THREADS;
}


/*! Copy constructor */
AnalysisInfo::AnalysisInfo(const AnalysisInfo& analysisInfo){
	this->id = analysisInfo.id;
	this->networkID = analysisInfo.networkID;
	this->archiveID = analysisInfo.archiveID;
	this->startDateTime = analysisInfo.startDateTime;
	this->parameterMap = analysisInfo.parameterMap;
	this->description = analysisInfo.description;
	this->analysisType = analysisInfo.analysisType;
	this->numberOfThreads = analysisInfo.numberOfThreads;
}


/*----------------------------------------------------------*/
/*-----                PUBLIC METHODS                  -----*/
/*----------------------------------------------------------*/

/*! Assignment operator */
AnalysisInfo& AnalysisInfo::operator=(const AnalysisInfo& rhs){
	//Check for self assignment
	if(this == &rhs)
		return *this;

	this->id = rhs.id;
	this->networkID = rhs.networkID;
	this->archiveID = rhs.archiveID;
	this->startDateTime = rhs.startDateTime;
	this->parameterMap = rhs.parameterMap;
	this->description = rhs.description;
	this->analysisType = rhs.analysisType;
	this->numberOfThreads = rhs.numberOfThreads;

	return *this;
}


/*! Returns the requested parameter or throws an exception if it does not exist. */
double AnalysisInfo::getParameter(const QString& paramKey){
	if(!parameterMap.contains(paramKey))
		throw SpikeStreamAnalysisException("Parameter key '" + paramKey + "' is not present in the parameter map.");
	return parameterMap[paramKey];
}


/*! Returns the parameter map in XML format */
QString AnalysisInfo::getParameterXML() const{
	QString tmpStr = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
	tmpStr += "<analysis_parameters>";
	for(QHash<QString, double>::const_iterator iter = parameterMap.begin(); iter != parameterMap.end(); ++iter){
		tmpStr +="<parameter>";
		tmpStr += "<name>" + iter.key() + "</name>";
		tmpStr += "<value>" + QString::number(iter.value()) + "</value>";
		tmpStr += "</parameter>";
	}
	tmpStr += "</analysis_parameters>";
	return tmpStr;
}


/*! Resets the information to default empty values */
void AnalysisInfo::reset(){
	this->id = 0;
	this->networkID = 0;
	this->archiveID = 0;
	this->startDateTime = QDateTime::currentDateTime();
	this->description = "Untitled";
	this->analysisType = 0;
	this->parameterMap.clear();
	this->numberOfThreads = DEFAULT_NUMBER_OF_THREADS;
}


/*! Sets a parameter value */
void AnalysisInfo::setParameter(const QString& paramKey, double paramValue){
	parameterMap[paramKey] = paramValue;
}

