#ifndef ANALYSISDAO_H
#define ANALYSISDAO_H

#include "AbstractDao.h"
#include "AnalysisInfo.h"
using namespace spikestream;

namespace spikestream {

    class AnalysisDao : public AbstractDao {
	public:
	    AnalysisDao(const DBInfo& dbInfo);
	    virtual ~AnalysisDao();

	    void addAnalysis(AnalysisInfo& analysisInfo);
	    QSqlQueryModel* getAnalysesTableModel(unsigned int networkID, unsigned int archiveID);

    };

}

#endif//ANALYSISDAO_H
