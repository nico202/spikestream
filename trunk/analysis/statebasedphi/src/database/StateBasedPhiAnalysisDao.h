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
	    void addComplex(unsigned int analysisID, int timeStep, double phi, QList<unsigned int>& neuronIDList);
	    int getComplexCount(unsigned int analysisID, unsigned int firstTimeStep, unsigned int lastTimeStep);
	    QSqlQueryModel* getStateBasedPhiDataTableModel(unsigned int analysisID);
    };

}

#endif//STATEBASEDPHIANALYSISDAO_H
