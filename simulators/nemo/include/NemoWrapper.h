#ifndef NEMOWRAPPER_H
#define NEMOWRAPPER_H

//SpikeStream includes
#include "AbstractDeviceManager.h"
#include "AbstractSimulation.h"
#include "ArchiveDao.h"
#include "ArchiveInfo.h"
#include "NetworkDao.h"
#include "ParameterInfo.h"
#include "Pattern.h"
#include "SpikeStreamTypes.h"

//Qt includes
#include <QHash>
#include <QMutex>
#include <QThread>

//Nemo includes
#include "nemo.h"

//Other includes
#include <vector>
using namespace std;


namespace spikestream {

	/*! Wraps the Nemo CUDA simulation library launching separate threads for heavy functions
		such as play and load */
	class NemoWrapper : public QThread, public AbstractSimulation {
		Q_OBJECT

		public:
			NemoWrapper();
			~NemoWrapper();
			void addDeviceManager(AbstractDeviceManager* deviceManager);
			void cancelLoading();
			void cancelResetWeights();
			void cancelSaveWeights();
			void clearWaitForGraphics() { waitForGraphics = false; }
			unsigned getArchiveID() { return archiveInfo.getID(); }
			int getCurrentTask() { return currentTaskID; }
			QString getErrorMessage() { return errorMessage; }
			QList<neurid_t> getFiringNeuronIDs() { return firingNeuronList; }
			nemo_configuration_t getNemoConfig(){ return nemoConfig; }
			unsigned getSTDPFunctionID() { return stdpFunctionID; }
			timestep_t getTimeStep() { return timeStepCounter; }
			unsigned getUpdateInterval_ms() { return this->updateInterval_ms; }
			unsigned getWaitInterval_ms() { return waitInterval_ms; }
			bool isError() { return error; }
			bool isMonitorFiringNeurons() { return monitorFiringNeurons; }
			bool isMonitorWeights() { return monitorWeights; }
			bool isWaitForGraphics()  { return waitForGraphics; }
			bool isWeightsReset() { return weightsReset; }
			bool isWeightsSaved() { return weightsSaved; }
			bool isSimulationLoaded() { return simulationLoaded; }
			bool isSimulationRunning();
			void resetWeights();
			void run();
			void saveWeights();
			void setArchiveMode(bool mode, const QString& archiveDescription = "");
			void setFrameRate(unsigned int frameRate);
			void setInjectCurrent(unsigned neuronGroupID, double percentage, double current, bool sustain);
			void setFiringNeuronIDs(QList<neurid_t>& neurIDList);
			void setInjectCurrentNeuronIDs(QList<neurid_t>& neurIDList, double current);
			void setInjectNoise(unsigned neuronGroupID, double percentage, bool sustain);
			void setFiringInjectionPattern(const Pattern& pattern, unsigned neuronGroupID, bool sustain);
			void setCurrentInjectionPattern(const Pattern& pattern, float current, unsigned neuronGroupID, bool sustain);
			void setMonitor(bool mode);
			void setMonitorNeurons(bool firing, bool membranePotential);
			void setNemoConfig(nemo_configuration_t nemoConfig) { this->nemoConfig = nemoConfig; }
			void setMonitorWeights(bool enable);
			void setNeuronParameters(unsigned neuronGroupID, QHash<QString, double> parameterMap);
			void setSynapseParameters(unsigned connectionGroupID, QHash<QString, double> parameterMap);
			void setSTDPFunctionID(unsigned stdpFunctionID) { this->stdpFunctionID = stdpFunctionID; }
			void setSustainNoise(bool sustain) { this->sustainNoise = sustain; this->sustainCurrent = sustain; }
			void setSustainPattern(bool sustainPattern) { this->sustainPattern = sustainPattern; }
			void setUpdateFiringNeurons(bool updateFiringNeurons){ this->updateFiringNeurons = updateFiringNeurons; }
			void setWaitInterval(unsigned waitInterval_ms) { this->waitInterval_ms = waitInterval_ms; }
			void playSimulation();
			void stepSimulation();
			void stopSimulation();
			void unloadSimulation();


			///====================  VARIABLES  ==========================
			/*! No task defined */
			static const int NO_TASK_DEFINED = 0;

			/*! Task of resetting temporary weight values to stored values. */
			static const int RESET_WEIGHTS_TASK = 1;

			/*! Task of playing simulation */
			static const int RUN_SIMULATION_TASK = 2;

			/*! Task of saving weights from simulation into database. */
			static const int SAVE_WEIGHTS_TASK = 3;

			/*! Task of setting neuron parameters in NeMo */
			static const int SET_NEURON_PARAMETERS_TASK = 4;

			/*! Task of advancing one time step of the simulation. */
			static const int STEP_SIMULATION_TASK = 5;


		signals:
			void progress(int stepsComplete, int totalSteps);
			void simulationStopped();
			void timeStepChanged(unsigned int timeStep);
			void timeStepChanged(unsigned int timeStep, const QList<unsigned>& neuronIDList);
			void timeStepChanged(unsigned int timeStep, const QHash<unsigned, float>& membranePotentialMap);


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

			/*! In monitor membrane potential mode a signal is emitted containing a map
				linking neuron ids with their membrane potential. */
			bool monitorMembranePotential;

			/*! In monitor weights mode the volatile weights are updated at each time step */
			bool monitorWeights;

			/*! Global control to switch monitoring on or off */
			bool monitor;

			/*! Extracts firing neurons at each time step regardless of the monitor state. */
			bool updateFiringNeurons;

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

			/*! Amount of time that wrapper waits before checking for next task to execute. */
			unsigned waitInterval_ms;

			/*! In monitor mode we need to wait for the graphics to update before
				moving on to the next time step */
			bool waitForGraphics;

			/*! Mutex controlling access to variables */
			QMutex mutex;

			/*! Mutex that is locked during run method loop and prevents changes whilst it runs. */
			QMutex runMutex;

			/*! ID of the STDP function */
			unsigned stdpFunctionID;

			/*! List of neurons that are firing at the current time step */
			QList<neurid_t> firingNeuronList;

			/*! Map of neuron groups to inject firing noise into at the next time step.
				The key is the neuron group ID, the value is the number of neurons to fire. */
			QHash<unsigned, unsigned> injectNoiseMap;

			/*! Controls whether noise is sustained for more than one time step */
			bool sustainNoise;

			/*! Map of neuron groups to inject current into at the next time step.
				The key is the neuron group ID, the value is a pair in which
				the first number is the number of neurons to inject current into
				and the second number is the amount of current to inject. */
			QHash<unsigned, QPair<unsigned, double> > injectCurrentMap;

			/*! Whether current is injected into neurons at each time step */
			bool sustainCurrent;

			/*! List of neuron IDs to fire in the next time step */
			QList<neurid_t> neuronIDsToFire;

			/*! Map linking neuron ids to amount of current to inject in next time step */
			QHash<neurid_t, double> neuronIDCurrentMap;

			/*! Map linking neurons with membrane potential */
			QHash<unsigned, float> membranePotentialMap;

			/*! Map of the volatile connection groups.
				The key in the outer map is the volatile connection group ID.
				The key in the inner map is the Nemo ID of the connection.
				The value in the inner map is the SpikeStream ID of the connection. */
			QHash<unsigned, synapse_id*> volatileConGrpMap;

			/*! List of presynaptic neuron ids used for saving weights. */
			QList<unsigned> preSynapticNeuronIDs;

			/*! Flag set to true when weights have been saved. */
			bool weightsSaved;

			/*! Flag set to true when weights are reset */
			bool weightsReset;

			/*! Flag used to cancel weight save */
			bool weightSaveCancelled;

			/*! Flag used to cancel weight reset */
			bool weightResetCancelled;

			/*! Reward used for STDP learning */
			float stdpReward;

			/*! Interval between applications of the learning function. */
			timestep_t applySTDPInterval;

			/*! ID of neuron group within which parameters will be set. */
			unsigned neuronGroupID;

			/*! Map containing neuron parameters. */
			QHash<QString, double> neuronParameterMap;

			/*! Vector of neuron IDs to be fired at next time step */
			vector<unsigned> injectionPatternVector;

			/*! Vector of neuron IDs to have current injected at next time step. */
			vector<unsigned> injectionCurrentNeurIDVector;

			/*! Current associated with the injection IDs. */
			vector<float> injectionCurrentVector;

			/*! Controls whether pattern is injected on every time step. */
			bool sustainPattern;

			/*! Neuron group in which pattern is to be injected. */
			unsigned patternNeuronGroupID;

			/*! List of device managers that interact with devices */
			QList<AbstractDeviceManager*> deviceManagerList;


			//======================  METHODS  ========================
			unsigned addInjectCurrentNeuronIDs();
			unsigned addInjectFiringNeuronIDs();
			void checkNemoOutput(nemo_status_t result, const QString& errorMessage);
			void clearError();
			void fillInjectNoiseArray(unsigned*& array, int* arraySize);
			void getMembranePotential();
			void loadNemo();
			void resetNemoWeights();
			void runNemo();
			void saveNemoWeights();
			void setError(const QString& errorMessage);
			void setExcitatoryNeuronParameters(NeuronGroup* neuronGroup);
			void setInhibitoryNeuronParameters(NeuronGroup* neuronGroup);
			void setNeuronParametersInNemo();
			void stepNemo();
			void unloadNemo();
			void updateNetworkWeights();
	};

}

#endif//NEMOWRAPPER_H
