#ifndef POP1EXPERIMENTMANAGER_H
#define POP1EXPERIMENTMANAGER_H

//SpikeStream includes
#include "Pattern.h"
#include "NemoWrapper.h"
#include "SpikeStreamThread.h"
#include "SpikeStreamTypes.h"

//Qt includes
#include <QString>


namespace spikestream {

	/*! Runs selected experiments on network exploring how population code can be used
		in a recurrent network. */
	class Pop1ExperimentManager : public SpikeStreamThread {
		Q_OBJECT

		public:
			Pop1ExperimentManager();
			~Pop1ExperimentManager();
			void run();
			void startExperiment(NemoWrapper* nemoWrapper, NeuronGroup* neuronGroup, QHash<QString, double>& parameterMap);

		signals:
			void statusUpdate(QString statusMsg);

		private:
			//=====================  VARIABLES  =====================
			/*! Wrapper of simulation */
			NemoWrapper* nemoWrapper;

			/*! Neuron group being experimented on. */
			NeuronGroup* neuronGroup;

			/*! Seed for random number generator */
			int randomSeed;

			/*! ID of experiment that is carried out. */
			int experimentNumber;

			static const int NO_EXPERIMENT = -1;
			static const int EXPERIMENT1 = 0;
			static const int EXPERIMENT2 = 1;
			static const int EXPERIMENT3 = 2;

			/*! Number of neurons along the X axis */
			int numXNeurons;

			/*! Number of neurons along the Y axis */
			int numYNeurons;

			/*! Start position of layer on X axis */
			float xStart;

			/*! Start position of layer on Y axis */
			float yStart;

			/*! Start position of layer on Z axis */
			float zStart;

			/*! Number of patterns in the test */
			int numPatterns;

			/*! Number of columns to shift by a random amount. */
			int numNoiseCols;

			/*! Amount to shift each column by */
			int randomShiftAmount;

			/*! Number of time steps between injecting pattern and reading back the resulting activity. */
			int stepsToPatternRead;

			/*! Time to pause between sections of the experiment */
			int pauseInterval_ms;


			//======================  METHODS  ======================
			void addHammingNoise(QList<Pattern>& patternList);
			int getAverageHammingDistanceNoise();
			int getHammingDistance(const QList<neurid_t>& expectedList, QList<neurid_t> actualList);
			Pattern getPattern();
			QList<int> getRandomSelectionIndexes(int numSelections, int max);
			int getXValue(neurid_t neuronID);
			int getYValue(neurid_t neuronID);
			void runExperiment();
			void stepNemo(unsigned numTimeSteps);
			void storeParameters(QHash<QString, double>& parameterMap);
			void subtractRandomPoints(QList<Pattern>& patternList);

	};

}

#endif//PATTERNMANAGER_H
