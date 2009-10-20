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
	    void clearError();
	    QString getErrorMessage() { return errorMessage; }
	    int getTimeStep() { return timeStep; }
	    bool isError() { return error; }
	    void prepareTimeStepAnalysis(int timeStep);
	    void run();
	    void stop();

	signals:
	    void complexFound();
	    void progress(unsigned int timeStep, unsigned int stepsCompleted, unsigned int totalSteps);

	private:
	    //========================  VARIABLES  ========================
	    /*! Records and controls thread running */
	    bool threadRunning;

	    /*! Wrapper around the network database */
	    NetworkDao* networkDao;

	    /*! Wrapper around the archive database */
	    ArchiveDao* archiveDao;

	    /*! Wrapper around the analysis database */
	    StateBasedPhiAnalysisDao* analysisDao;

	    /*! The time step that is being analyzed by this thread */
	    int timeStep;

	    /*! Records if an error has occurred */
	    bool error;

	    /*! Error message associated with an error */
	    QString errorMessage;

    };

}

#endif//ANALYSISTIMESTEPTHREAD_H
