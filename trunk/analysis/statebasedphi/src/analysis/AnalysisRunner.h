#ifndef ANALYSISRUNNER_H
#define ANALYSISRUNNER_H

//SpikeStream includes
#include "AnalysisTimeStepThread.h"
#include "AnalysisProgress.h"
#include "DBInfo.h"

//Qt includes
#include <QHash>
#include <QString>
#include <QThread>
#include <QMutex>

namespace spikestream {

    class AnalysisRunner : public QThread {
	Q_OBJECT

	public:
	    AnalysisRunner(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo);
	    ~AnalysisRunner();
	    QString getErrorMessage() { return errorMessage; }
	    bool isError() { return error; }
	    void prepareAnalysisTask(const AnalysisInfo& anaInfo, int firstTimeStep, int lastTimeStep);
	    void reset();
	    void run();
	    void setError(const QString& message);
	    void stop();

	signals:
	    void progress(AnalysisProgress& progress);
	    void complexFound();

	private slots:
	    void threadFinished();
	    void updateComplexes();
	    void updateProgress(unsigned int timeStep, unsigned int stepsCompleted, unsigned int totalSteps);

	private:
	    //========================  VARAIBLES  =========================
	    /*! Information about the analysis */
	    AnalysisInfo analysisInfo;

	    /*! Information about the network database */
	    DBInfo networkDBInfo;

	    /*! Information about the archive database */
	    DBInfo archiveDBInfo;

	    /*! Information about the analysis database */
	    DBInfo analysisDBInfo;

	    /*! Records when an error has occurred */
	    bool error;

	    /*! Error message */
	    QString errorMessage;

	    /*! First time step to be analyzed */
	    int firstTimeStep;

	    /*! Last time step to be analyzed */
	    int lastTimeStep;

	    /*! The next time step to be analyzed */
	    int nextTimeStep;

	    /*! Map of the currently running threads */
	    QHash<int, AnalysisTimeStepThread*> subThreadMap;

	    /*! Mutex to make sure that we only process one thread finished
		method at a time. */
	    QMutex mutex;

	    /*! Controls whether the thread is running or not */
	    bool stopThread;

	    //=========================  METHODS  ===========================
	    void clearError();
	    int getNextTimeStep();
	    void startAnalysisTimeStepThread(int timeStep);
	    bool subThreadsRunning();
    };

}

#endif//ANALYSISRUNNER_H

