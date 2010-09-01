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
			void cancelLoading();
			void cancelSaveWeights();
			void clearWaitForGraphics() { waitForGraphics = false; }
			unsigned getArchiveID() { return archiveInfo.getID(); }
			int getCurrentTask() { return currentTaskID; }
			QString getErrorMessage() { return errorMessage; }
			nemo_configuration_t getNemoConfig(){ return nemoConfig; }
			unsigned getSTDPFunctionID() { return stdpFunctionID; }
			unsigned getUpdateInterval_ms() { return this->updateInterval_ms; }
			bool isError() { return error; }
			bool isMonitorFiringNeurons() { return monitorFiringNeurons; }
			bool isMonitorWeights() { return monitorWeights; }
			bool isWeightsSaved() { return weightsSaved; }
			bool isSimulationLoaded() { return simulationLoaded; }
			bool isSimulationRunning();
			void run();
			void saveWeights();
			void setArchiveMode(bool mode);
			void setFrameRate(unsigned int frameRate);
			void setInjectNoise(unsigned neuronGroupID, double percentage);
			void setMonitorFiringNeurons(bool mode);
			void setNemoConfig(nemo_configuration_t nemoConfig) { this->nemoConfig = nemoConfig; }
			void setMonitorWeights(bool enable);
			void setSTDPFunctionID(unsigned stdpFunctionID) { this->stdpFunctionID = stdpFunctionID; }
			void setUpdateInterval_ms(unsigned int interval) { this->updateInterval_ms = interval; }
			void playSimulation();
			void stepSimulation();
			void stopSimulation();
			void unloadSimulation();

			///====================  VARIABLES  ==========================
			/*! No task defined */
			static const int NO_TASK_DEFINED = 0;

			/*! Task of playing simulation */
			static const int RUN_SIMULATION_TASK = 1;

			/*! Task of advancing one time step of the simulation. */
			static const int STEP_SIMULATION_TASK = 2;

			/*! Task of saving weights from simulation into database. */
			static const int SAVE_WEIGHTS_TASK = 3;


		signals:
			void progress(int stepsComplete, int totalSteps);
			void simulationStopped();
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

			/*! In monitor firing neurons mode a signal is emitted containing a list of the firing neurons */
			bool monitorFiringNeurons;

			/*! In monitor weights mode the volatile weights are updated at each time step */
			bool monitorWeights;

			/*! The time step of the simulation */
			unsigned int timeStepCounter;

			/*! Records if an error has occurred */
			bool error;

			/*! Message associated with an error */
			QString errorMessage;

			/*! Controls whether the thread is running or not */
			bool stopThread;

			/*! Nemo configuration */
			nemo_configuration_t nemoConfig;

			/*! Pointer to the Nemo simulation that has been constructed. */
			nemo_simulation_t nemoSimulation;

			/*! Interval between each time step in milliseconds. */
			unsigned updateInterval_ms;

			/*! In monitor mode we need to wait for the graphics to update before
				moving on to the next time step */
			bool waitForGraphics;

			/*! Mutex controlling access to variables */
			QMutex mutex;

			/*! ID of the STDP function */
			unsigned stdpFunctionID;

			/*! List of neurons that are firing at the current time step */
			QList<unsigned int> firingNeuronList;

			/*! Map of neuron groups to inject noise into at the next time step.
				The key is the neuron group ID, the value is the number of neurons to fire. */
			QHash<unsigned, unsigned> injectNoiseMap;

			/*! List of the IDs of volatile connection groups */
			QList<unsigned> volatileConGrpList;

			/*! List of presynaptic neuron ids used for saving weights. */
			QList<unsigned> preSynapticNeuronIDs;

			/*! Flag set to true when weights have been saved. */
			bool weightsSaved;

			/*! Flag used to cancel weight save */
			bool weightSaveCancelled;

			//======================  METHODS  ========================
			void checkNemoOutput(nemo_status_t result, const QString& errorMessage);
			void clearError();
			void fillInjectNoiseArray(unsigned*& array, int* arraySize);
			void loadNemo();
			void runNemo();
			void setError(const QString& errorMessage);
			void stepNemo();
			void unloadNemo();
			void updateNetworkWeights();
			void saveNemoWeights();
	};

}

#endif//NEMOWRAPPER_H
