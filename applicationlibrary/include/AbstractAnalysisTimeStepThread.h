#ifndef ABSTRACTANALYSISTIMESTEPTHREAD_H
#define ABSTRACTANALYSISTIMESTEPTHREAD_H

//SpikeStream includes
#include "DBInfo.h"
#include "NetworkDao.h"
#include "ArchiveDao.h"
#include "AnalysisInfo.h"

//Qt includes
#include <QThread>

namespace spikestream {

	/*! Abstract class with functions common to all analysis threads designed to analyze
		a particular time step. */
	class AbstractAnalysisTimeStepThread : public QThread {
		Q_OBJECT

		public:
			AbstractAnalysisTimeStepThread(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo);
			virtual ~AbstractAnalysisTimeStepThread();
			void clearError();
			QString getErrorMessage() { return errorMessage; }
			int getTimeStep() { return timeStep; }
			bool isError() { return error; }
			virtual void prepareTimeStepAnalysis(const AnalysisInfo& anaInfo, int timeStep);
			virtual void run() = 0;
			void stopThread();

		signals:
			void newResultsFound();
			void progress(const QString& msg, unsigned int timeStep, unsigned int stepsCompleted, unsigned int totalSteps);

		protected slots:
			void updateResults();
			void updateProgress(const QString& msg, unsigned int timeStep, unsigned int stepsCompleted, unsigned int totalSteps);

		protected:
			//========================  VARIABLES  ========================
			/*! Information about the network database */
			DBInfo networkDBInfo;

			/*! Information about the archive database */
			DBInfo archiveDBInfo;

			/*! Information about the analysis database */
			DBInfo analysisDBInfo;

			/*! Records and controls thread running */
			bool stop;

			/*! Information about the analysis being run */
			AnalysisInfo analysisInfo;

			/*! The time step that is being analyzed by this thread */
			int timeStep;

			/*! Records if an error has occurred */
			bool error;

			/*! Error message associated with an error */
			QString errorMessage;


			//=======================  METHODS  =======================
			void setError(const QString& message);

	};

}

#endif//ABSTRACTANALYSISTIMESTEPTHREAD_H
