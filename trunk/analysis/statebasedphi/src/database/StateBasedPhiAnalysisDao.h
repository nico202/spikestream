#ifndef STATEBASEDPHIANALYSISDAO_H
#define STATEBASEDPHIANALYSISDAO_H

#include "AnalysisDao.h"
#include "DBInfo.h"
using namespace spikestream;


namespace spikestream {

    class StateBasedPhiAnalysisDao : public AnalysisDao {
	public:
	    StateBasedPhiAnalysisDao(const DBInfo& dbInfo);
	    StateBasedPhiAnalysisDao();
	    virtual ~StateBasedPhiAnalysisDao();
	    virtual void addComplex(unsigned int analysisID, int timeStep, QList<unsigned int>& neuronIDList, double phi);
	    void deleteTimeSteps(unsigned int firstTimeStep, unsigned int lastTimeStep);
	    int getComplexCount(unsigned int analysisID, unsigned int firstTimeStep, unsigned int lastTimeStep);
	    QSqlQueryModel* getStateBasedPhiDataTableModel(unsigned int analysisID);
    };

}

#endif//STATEBASEDPHIANALYSISDAO_H
