#ifndef STATEBASEDPHIANALYSISDAO_H
#define STATEBASEDPHIANALYSISDAO_H

#include "AnalysisDao.h"
#include "Complex.h"
#include "DBInfo.h"
using namespace spikestream;


namespace spikestream {

	class StateBasedPhiAnalysisDao : public AnalysisDao {
	public:
		StateBasedPhiAnalysisDao(const DBInfo& dbInfo);
		StateBasedPhiAnalysisDao();
		virtual ~StateBasedPhiAnalysisDao();
		virtual void addComplex(unsigned int analysisID, int timeStep, QList<unsigned int>& neuronIDList, double phi);
		void deleteTimeSteps(unsigned int analysisID, unsigned int firstTimeStep, unsigned int lastTimeStep);
		unsigned int getComplexCount(unsigned int analysisID, unsigned int firstTimeStep, unsigned int lastTimeStep);
		QList<Complex> getComplexes(unsigned int analysisID);
	};

}

#endif//STATEBASEDPHIANALYSISDAO_H
