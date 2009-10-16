#ifndef ANALYSISTIMESTEPTHREAD_H
#define ANALYSISTIMESTEPTHREAD_H

//SpikeStream includes
#include "DBInfo.h"
#include "NetworkDao.h"
#include "ArchiveDao.h"
#include "StateBasedPhiAnalysisDao.h"

//Qt includes
#include <QThread>

namespace spikestream {

    class AnalysisTimeStepThread : public QThread {
	Q_OBJECT

	public:
	    AnalysisTimeStepThread(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo);
	    ~AnalysisTimeStepThread();
	    int getTimeStep() { return timeStep; }
	    void prepareTimeStepAnalysis(int timeStep);
	    void run();
	    void stop();

	signals:
	    void complexFound();
	    void progress(unsigned int stepsCompleted, unsigned int totalSteps);

	private:
	    //========================  VARIABLES  ========================
	    /*! Records and controls thread running */
	    bool threadRunning;

	    /*! Wrapper around the network database */
	    NetworkDao networkDao;

	    /*! Wrapper around the archive database */
	    ArchiveDao archiveDao;

	    /*! Wrapper around the analysis database */
	    StateBasedPhiDao analysisDao;

	    /*! The time step that is being analyzed by this thread */
	    int timeStep;

    };

}

#endif//ANALYSISTIMESTEPTHREAD_H
