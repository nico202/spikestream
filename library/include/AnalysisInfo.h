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


	private:
	    //======================  VARIABLES  ========================
	    unsigned int id;
	    unsigned int networkID;
	    unsigned int archiveID;
	    QDateTime startDateTime;
	    QString description;
	    QHash<QString, double> parameterMap;
	    unsigned int analysisType;
    };

}

#endif//ANALYSISINFO_H
