#include "AnalysisInfo.h"
using namespace spikestream;

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
}


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

    return *this;
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
    this->numberOfThreads = 5;
}
