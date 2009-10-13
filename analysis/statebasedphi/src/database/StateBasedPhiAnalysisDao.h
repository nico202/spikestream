#ifndef STATEBASEDPHIANALYSISDAO_H
#define STATEBASEDPHIANALYSISDAO_H

#include "AnalysisDao.h"
#include "DBInfo.h"
using namespace spikestream;


namespace spikestream {

    class StateBasedPhiAnalysisDao : public AnalysisDao {
	public:
	    StateBasedPhiAnalysisDao(const DBInfo& dbInfo);
	    virtual ~StateBasedPhiAnalysisDao();


	    QSqlQueryModel* getStateBasedPhiDataTableModel(unsigned int analysisID);
    };

}

#endif//STATEBASEDPHIANALYSISDAO_H
