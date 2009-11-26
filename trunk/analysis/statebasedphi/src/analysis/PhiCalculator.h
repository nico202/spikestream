#ifndef PHICALCULATOR_H
#define PHICALCULATOR_H


//SpikeStream includes
#include "DBInfo.h"
#include "AnalysisInfo.h"
#include "NetworkDao.h"
#include "ArchiveDao.h"
#include "StateBasedPhiAnalysisDao.h"
#include "ProbabilityTable.h"
#include "WeightlessNeuron.h"
using namespace spikestream;

//Qt includes
#include <QObject>

namespace spikestream {

    class PhiCalculator : public QObject {
	Q_OBJECT

	public:
	    PhiCalculator(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo, const AnalysisInfo& anaInfo, unsigned int timeStep, const bool* stop);
	    PhiCalculator();
	    ~PhiCalculator();
	    void calculateReverseProbability(ProbabilityTable& causalTable, ProbabilityTable& reverseTable);
	    void fillPartitionLists(QList<unsigned int>& aPartition, QList<unsigned int>& bPartition, bool* partitionArray, int arrayLength, QList<unsigned int>& subsetNeurIDs);
	    void fillProbabilityTable(ProbabilityTable& table, QList<unsigned int> neurIDList);
	    double getCausalProbability(QList<unsigned int>& neurIDList, const QString& x0Pattern);
	    double getSubsetPhi(QList<unsigned int>& subsetNeurIDs);
	    double getPartitionPhi(QList<unsigned int>& aPartition, QList<unsigned int>& bPartition);
	    void loadWeightlessNeurons();
	    void setWeightlessNeuronMap(QHash<unsigned int, WeightlessNeuron*> newMap);
	    void setFiringNeuronMap(QHash<unsigned int, bool> newMap);

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
	    const bool* stop;

	    /*! Map containing all of the weightless neurons in the network */
	    QHash<unsigned int, WeightlessNeuron*> weightlessNeuronMap;

	    /*! Map of the neurons firing at this time step */
	    QHash<unsigned int, bool> firingNeuronMap;

	    //=========================  METHODS  ==========================
	    void deleteWeightlessNeurons();
	    int getFiringState(unsigned int neurID);
	    void loadFiringNeurons();
    };

}

#endif//PHICALCULATOR_H
