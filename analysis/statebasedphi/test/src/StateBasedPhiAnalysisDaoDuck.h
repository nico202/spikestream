#ifndef STATEBASEDPHIANALYSISDAODUCK_H
#define STATEBASEDPHIANALYSISDAODUCK_H

//SpikeStream includes
#include "StateBasedPhiAnalysisDao.h"
using namespace spikestream;

//Qt includes
#include <QList>

/*! Holds information about a complex */
struct Complex {
    QList<unsigned int> neuronIDs;
    double phi;
};


/*! Class injected for testing into SubsetManager */
class StateBasedPhiAnalysisDaoDuck : public StateBasedPhiAnalysisDao {
    public:
	StateBasedPhiAnalysisDaoDuck();
	virtual ~StateBasedPhiAnalysisDaoDuck();
	void addComplex(unsigned int analysisID, int timeStep, QList<unsigned int>& neuronIDList, double phi);
	QList<Complex> getComplexList() { return complexList; }
	void reset();

    private:
	QList<Complex> complexList;
};

#endif//STATEBASEDPHIANALYSISDAODUCK_H

