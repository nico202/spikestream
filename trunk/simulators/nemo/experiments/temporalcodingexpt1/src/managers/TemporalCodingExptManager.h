#ifndef TEMPORALCODINGEXPTMANAGER_H
#define TEMPORALCODINGEXPTMANAGER_H

//SpikeStream includes
#include "NemoWrapper.h"
#include "SpikeStreamThread.h"
#include "SpikeStreamTypes.h"

//Qt includes
#include <QString>


namespace spikestream {

	/*! Runs selected experiments on network exploring how temporal code can be used
		as part of pattern learning in a network. */
	class TemporalCodingExptManager : public SpikeStreamThread {
		Q_OBJECT

		public:
			TemporalCodingExptManager();
			~TemporalCodingExptManager();
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

			static const int NO_EXPERIMENT = -1;
			static const int EXPERIMENT1 = 0;
			static const int EXPERIMENT2 = 1;
			static const int EXPERIMENT3 = 2;
			static const int EXPERIMENT4 = 3;

			/*! Neuron group to inject patterns into */
			NeuronGroup* inputNeuronGroup;

			/*! Neuron group to read results from */
			NeuronGroup* featureNeuronGroup;

			/*! Time to pause between sections of the experiment */
			int pauseInterval_ms;

			/*! Number of input neurons */
			int numInputNeurons;

			/*! Starting position of input layer on x axis */
			int inputXStart;

			/*! Starting position of input layer on y axis */
			int inputYStart;

			/*! Starting position of input layer on z axis */
			int inputZStart;

			/*! Number of steps to run network after pattern has been injected. */
			int numInterExptSteps;

			/*! Number of steps to run network before result is read. */
			int numResultSteps;

			/*! Number of time steps that pattern extends over */
			int numTimeSteps;


			//======================  METHODS  ======================
			void applyInputSequence(QList<Pattern>& patternList);
			QList<unsigned> getDownTrend();
			QList<unsigned> getDownUpTrend();
			void getResults();
			QList<Pattern> getTemporalPatternSequence(QList<unsigned>& numberSequence);
			QList<unsigned> getUpTrend();
			QList<unsigned> getUpDownTrend();
			void runExperiment1();
			void runExperiment2();
			void runExperiment3();
			void runExperiment4();
			void stepNemo(unsigned numTimeSteps);
			void storeParameters(QHash<QString, double>& parameterMap);

	};

}

#endif//TEMPORALCODINGEXPTMANAGER_H
