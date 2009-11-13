#ifndef PHICALCULATOR_H
#define PHICALCULATOR_H


//SpikeStream includes
#include "DBInfo.h"
#include "AnalysisInfo.h"
#include "NetworkDao.h"
#include "ArchiveDao.h"
#include "StateBasedPhiAnalysisDao.h"
using namespace spikestream;

//Qt includes
#include <QObject>

namespace spikestream {

    class PhiCalculator : public QObject {
	Q_OBJECT

	public:
	    PhiCalculator(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo, const AnalysisInfo& anaInfo, unsigned int timeStep);
	    ~PhiCalculator();
	    double getSubsetPhi(QList<unsigned int>& subsetNeurIDs);

	signals:
	    void complexFound();
	    void progress(unsigned int timeStep, unsigned int stepsCompleted, unsigned int totalSteps);


	private:
	    //========================  VARIABLES  ========================
	    /*! Wrapper around the network database */
	    NetworkDao* networkDao;

	    /*! Wrapper around the archive database */
	    ArchiveDao* archiveDao;

	    /*! Wrapper around the analysis database */
	    StateBasedPhiAnalysisDao* stateDao;

	    /*! Information about the analysis being run */
	    AnalysisInfo analysisInfo;

	    /*! The time step that is being analyzed by this thread */
	    int timeStep;

	    /*! Pointer to the stop variable in the controlling thread. */
	    bool* stop;

	    //=========================  METHODS  ==========================
	    void fillSelectionArray(bool* array, int arraySize, int selectionSize);
	    double getPartitionPhi(QList<unsigned int>& aPartition, QList<unsigned int>& bPartition);
    };

}

#endif//PHICALCULATOR_H
