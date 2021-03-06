#ifndef IZHIACCURACYMANAGER_H
#define IZHIACCURACYMANAGER_H

//SpikeStream includes
#include "NemoWrapper.h"
#include "SpikeStreamThread.h"
#include "SpikeStreamTypes.h"

//Qt includes
#include <QString>
#include <QTextStream>


namespace spikestream {

	/*! Runs selected experiments on network exploring how temporal code can be used
		as part of pattern learning in a network. */
	class IzhiAccuracyManager : public SpikeStreamThread {
		Q_OBJECT

		public:
			IzhiAccuracyManager();
			~IzhiAccuracyManager();
			void run();
			void startExperiment(NemoWrapper* nemoWrapper, QHash<QString, double>& parameterMap);

		signals:
			void statusUpdate(QString statusMsg);

		private:
			//=====================  VARIABLES  =====================
			/*! Wrapper of simulation */
			NemoWrapper* nemoWrapper;

			/*! Seed for random number generator */
			int randomSeed;

			/*! ID of experiment that is carried out. */
			int experimentNumber;

			/*! Excitatory neuron group */
			NeuronGroup* excitatoryNeuronGroup;

			/*! Inhibitory neuron group */
			NeuronGroup* inhibitoryNeuronGroup;

			static const int NO_EXPERIMENT = -1;
			static const int EXPERIMENT1 = 0;

			/*! Time to pause between sections of the experiment */
			int pauseInterval_ms;

			/*! Keeps track of the time step */
			timestep_t timeStep;

			/*! Neuron IDs ordered in same way as Izhikevich experiment. */
			QList<neurid_t> orderedNeuronIDList;


			//======================  METHODS  ======================
			void runExperiment1();
			void runExperiment2();
			void stepNemo(unsigned numTimeSteps, bool injectCurrent, QTextStream* textStream = NULL);
			void storeNeuronGroups();
			void storeParameters(QHash<QString, double>& parameterMap);

	};

}

#endif//IZHIACCURACYMANAGER_H
