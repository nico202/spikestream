#ifndef ANALYSISINFO_H
#define ANALYSISINFO_H

//Qt includes
#include <QString>
#include <QHash>
#include <QDateTime>

namespace spikestream {

    class AnalysisInfo {
	public:
	    AnalysisInfo();
	    AnalysisInfo(unsigned int analysisID, unsigned int networkID, unsigned int archiveID, const QDateTime& startDateTime, const QString& description, const QHash<QString, double>& parameterMap, unsigned int analysisType);
	    AnalysisInfo(const AnalysisInfo& analysisInfo);
	    AnalysisInfo& operator=(const AnalysisInfo& rhs);

	    unsigned int getAnalyisType() const { return analysisType; }
	    unsigned int getID() const { return id; }
	    QString getDescription() const { return description; }
	    unsigned int getNetworkID() const { return networkID; }
	    unsigned int getArchiveID() const { return archiveID; }
	    QDateTime getStartDateTime() const { return startDateTime; }
	    unsigned int getNumberOfThreads() { return numberOfThreads; }
	    QHash<QString, double>& getParameterMap() { return parameterMap; }
	    QString getParameterXML() const;
	    void reset();
	    void setDescription(const QString& description) { this->description = description; }
	    void setNumberOfThreads(unsigned int numThreads) { this->numberOfThreads = numThreads; }
	    void setID(unsigned int id) { this->id = id; }
	    void setAnalysisType(unsigned int typeID) { this->analysisType = typeID; }
	    void setArchiveID(unsigned int archiveID) { this->archiveID = archiveID; }
	    void setNetworkID(unsigned int networkID) { this->networkID = networkID; }


	private:
	    //======================  VARIABLES  ========================
	    unsigned int id;
	    unsigned int networkID;
	    unsigned int archiveID;
	    QDateTime startDateTime;
	    QString description;
	    QHash<QString, double> parameterMap;
	    unsigned int analysisType;
	    unsigned int numberOfThreads;
    };

}

#endif//ANALYSISINFO_H
