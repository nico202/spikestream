#ifndef ANALYSISRUNNER_H
#define ANALYSISRUNNER_H

//SpikeStream includes
#include "AnalysisTimeStepThread.h"
#include "DBInfo.h"

//Qt includes
#include <QHash>
#include <QString>
#include <QThread>

namespace spikestream {

    struct AnalysisProgress {
	unsigned int timeStep;
	unsigned int stepsCompleted;
	unsigned int totalSteps;
    };


    class AnalysisRunner : public QThread {
	Q_OBJECT

	public:
	    AnalysisRunner(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo);
	    ~AnalysisRunner();
	    QString getErrorMessage() { return errorMessage; }
	    bool isError() { return error; }
	    void prepareAnalysisTask(const AnalysisInfo& anaInfo, int firstTimeStep, int lastTimeStep);
	    void run();
	    void stop();

	signals:
	    void progress(AnalysisProgress& progress);
	    void complexFoundSignal();

	private slots:
	    void complexFound();
	    void progress(AnalysisProgress& progress);
	    void threadFinished();

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
	    QHash<unsigned int, AnalysisTimeStepThread*> subThreadMap;

    };

}

#endif//ANALYSISRUNNER_H

