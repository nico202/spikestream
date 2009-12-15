#ifndef ANALYSISDAO_H
#define ANALYSISDAO_H

#include "AbstractDao.h"
#include "AnalysisInfo.h"
using namespace spikestream;

namespace spikestream {

    class AnalysisDao : public AbstractDao {
	public:
	    AnalysisDao(const DBInfo& dbInfo);
	    AnalysisDao();
	    virtual ~AnalysisDao();

	    void addAnalysis(AnalysisInfo& analysisInfo);
	    void deleteAnalysis(unsigned int analysisID);
	    QList<AnalysisInfo> getAnalysesInfo(unsigned int networkID, unsigned int archiveID);
	    void updateDescription(unsigned int analysisID, const QString& description);

    };

}

#endif//ANALYSISDAO_H
