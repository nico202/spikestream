#ifndef NEMOWRAPPER_H
#define NEMOWRAPPER_H

//SpikeStream includes
#include "ArchiveDao.h"
#include "ArchiveInfo.h"
#include "NetworkDao.h"
#include "ParameterInfo.h"

//Qt includes
#include <QHash>
#include <QMutex>
#include <QThread>

//Nemo includes
#include "nemo.h"


namespace spikestream {

	/*! Wraps the Nemo CUDA simulation library launching separate threads for heavy functions
		such as play and load */
	class NemoWrapper : public QThread {
		Q_OBJECT

		public:
			NemoWrapper();
			~NemoWrapper();
			void clearWaitForGraphics() { waitForGraphics = false; }
			unsigned getArchiveID() { return archiveInfo.getID(); }
			int getCurrentTask() { return currentTaskID; }
			QHash<QString, double> getDefaultParameterValues(){ return defaultParameterMap; }
			QString getErrorMessage() { return errorMessage; }
			QList<ParameterInfo> getParameterInfoList(){ return parameterInfoList; }
			QHash<QString, double> getParameterValues(){ return parameterMap; }
			unsigned getUpdateInterval_ms() { return this->updateInterval_ms; }
			bool isError() { return error; }
			bool isMonitorMode() { return monitorMode; }
			bool isSimulationLoaded() { return simulationLoaded; }
			void prepareLoadSimulation();
			void prepareRunSimulation();
			void prepareStepSimulation();
			void run();
			void setArchiveMode(bool mode);
			void setFrameRate(unsigned int frameRate);
			void setMonitorMode(bool mode);
			void setParameters(const QHash<QString, double>& parameterMap);
			void setUpdateInterval_ms(unsigned int interval) { this->updateInterval_ms = interval; }
			void stop();
			QString testConfiguration();
			void unloadSimulation();


			///====================  VARIABLES  ==========================
			/*! No task defined */
			static const int NO_TASK_DEFINED = 0;

			/*! Task of loading simulation */
			static const int LOAD_SIMULATION_TASK = 1;

			/*! Task of playing simulation */
			static const int RUN_SIMULATION_TASK = 2;

			/*! Task of advancing one time step of the simulation. */
			static const int STEP_SIMULATION_TASK = 3;


		signals:
			void progress(int stepsComplete, int totalSteps);
			void timeStepChanged(unsigned int timeStep, const QList<unsigned>& neuronIDList);


		private slots:
			void updateProgress(int stepsComplete, int totalSteps);


		private:
			//======================  VARIABLES  ========================
			/*! Thread specific version of the network dao */
			NetworkDao* networkDao;

			/*! Thread specific version of the archive dao */
			ArchiveDao* archiveDao;

			/*! Information about the archive */
			ArchiveInfo archiveInfo;

			/*! ID of the current task that is running */
			int currentTaskID;

			/*! Records that simulation is currently loaded and ready to play */
			bool simulationLoaded;

			/*! In archive mode the current firing neurons are written to disk */
			bool archiveMode;

			/*! In monitor mode a signal is emitted containing a list of the firing neurons */
			bool monitorMode;

			/*! The time step of the simulation */
			unsigned int timeStepCounter;

			/*! Records if an error has occurred */
			bool error;

			/*! Message associated with an error */
			QString errorMessage;

			/*! Controls whether the thread is running or not */
			bool stopThread;

			/*! List of information about the parameters associated with Nemo */
			QList<ParameterInfo> parameterInfoList;

			/*! Values of the parameters associated with Nemo.
				The key is the parameter name; the value is the value of the parameter. */
			QHash<QString, double> parameterMap;

			/*! Default values of the parameters associated with Nemo.
				The key is the parameter name; the value is the value of the parameter. */
			QHash<QString, double> defaultParameterMap;

			/*! Pointer to the Nemo simulation that has been constructed. */
			nemo_simulation_t nemoSimulation;

			/*! Interval between each time step in milliseconds. */
			unsigned updateInterval_ms;

			/*! In monitor mode we need to wait for the graphics to update before
				moving on to the next time step */
			bool waitForGraphics;

			/*! Mutex controlling access to variables */
			QMutex mutex;

			QList<unsigned int> firingNeuronList;


			//======================  METHODS  ========================
			void buildParameters();
			void checkNemoOutput(nemo_status_t result, const QString& message);
			void clearError();
			void loadSimulation();
			void runSimulation();
			void setError(const QString& errorMessage);
			void stepSimulation();

	};

}

#endif//NEMOWRAPPER_H
