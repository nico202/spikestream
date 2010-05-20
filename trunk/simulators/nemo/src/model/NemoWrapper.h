#ifndef NEMOWRAPPER_H
#define NEMOWRAPPER_H

//Qt includes
#include <QThread>


namespace spikestream {

	/*! Wraps the Nemo CUDA simulation library launching separate threads for heavy functions
		such as play and load */
	class NemoWrapper : public QThread {
		Q_OBJECT

		public:
			NemoWrapper();
			~NemoWrapper();
			int getCurrentTask() { return currentTaskID; }
			QString getErrorMessage() { return errorMessage; }
			bool isError() { return error; }
			bool isSimulationLoaded() { return simulationLoaded; }
			void prepareLoadSimulation();
			void prepareRunSimulation();
			void run();
			void stop();
			void unload();
			void updatePprogress(int stepsComplete, int totalSteps);


			///====================  VARIABLES  ==========================
			/*! No task defined */
			static const int NO_TASK_DEFINED = 0;

			/*! Task of loading simulation */
			static const int LOAD_SIMULATION_TASK = 1;

			/*! Task of playing simulation */
			static const int RUN_SIMULATION_TASK = 2;


		signals:
			void progress(int stepsComplete, int totalSteps);


		private slots:
			void updateProgress(int stepsComplete, int totalSteps);


		private:
			//======================  VARIABLES  ========================
			/*! ID of the current task that is running */
			int currentTaskID;

			/*! Records that simulation is currently loaded and ready to play */
			bool simulationLoaded;

			/*! Records if an error has occurred */
			bool error;

			/*! Message associated with an error */
			QString errorMessage;

			/*! Controls whether the thread is running or not */
			bool stopThread;

			//======================  METHODS  ========================
			void clearError();
			void loadSimulation();
			void runSimulation();
			void setError(const QString& errorMessage);

	};

}

#endif//NEMOWRAPPER_H
