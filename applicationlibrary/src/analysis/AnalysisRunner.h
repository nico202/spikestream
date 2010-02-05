#ifndef ANALYSISRUNNER_H
#define ANALYSISRUNNER_H

//SpikeStream includes
#include "AbstractAnalysisTimeStepThread.h"
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
			void setTimeStepThreadCreationFunction(AbstractAnalysisTimeStepThread* (*createAnalysisTimeStepThread)(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo));
			void stop();

		signals:
			void newResultsFound();
			void progress(const QString& msg, unsigned int timeStep, unsigned int stepsCompleted, unsigned int totalSteps);
			void timeStepComplete(unsigned int timeStep);

		private slots:
			void threadFinished();
			void updateResults();
			void updateProgress(const QString& msg, unsigned int timeStep, unsigned int stepsCompleted, unsigned int totalSteps);

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
			QHash<int, AbstractAnalysisTimeStepThread*> subThreadMap;

			/*! Mutex to make sure that we only process one thread finished
			method at a time. */
			QMutex mutex;

			/*! Controls whether the thread is running or not */
			bool stopThread;

			/*! Pointer to a function that creates a new time step thread of the appropriate class */
			AbstractAnalysisTimeStepThread* (*createAnalysisTimeStepThread)(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo);

			//=========================  METHODS  ===========================
			void clearError();
			int getNextTimeStep();
			void startAnalysisTimeStepThread(int timeStep);
			bool subThreadsRunning();
	};

}

#endif//ANALYSISRUNNER_H

