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

	    unsigned int getID() const { return id; }
	    QString getDescription() const { return description; }
	    unsigned int getNumberOfThreads() { return numberOfThreads; }
	    QHash<QString, double>& getParameterMap() { return parameterMap; }
	    void reset();
	    void setDescription(const QString& description) { this->description = description; }
	    void setNumberOfThreads(unsigned int numThreads) { this->numberOfThreads = numThreads; }


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
