#ifndef ANALYSISDAO_H
#define ANALYSISDAO_H

#include "AbstractDao.h"
using namespace spikestream;

namespace spikestream {

    class AnalysisDao : public AbstractDao {
	public:
	    AnalysisDao(const DBInfo& dbInfo);
	    virtual ~AnalysisDao();

	    QSqlQueryModel* getAnalysesTableModel(unsigned int networkID, unsigned int archiveID);

    };

}

#endif//ANALYSISDAO_H
