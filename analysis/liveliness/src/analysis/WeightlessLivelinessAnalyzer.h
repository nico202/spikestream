#ifndef WEIGHTLESSLIVELINESSANALYZER_H
#define WEIGHTLESSLIVELINESSANALYZER_H

//SpikeStream includes
#include "DBInfo.h"
#include "AnalysisInfo.h"
#include "NetworkDao.h"
#include "ArchiveDao.h"
#include "LivelinessDao.h"

//Qt includes
#include <QObject>

namespace spikestream {

	/*! Analyzes weightless neurons for state-based liveliness.
		Identifies the liveliness of connections for a particular state as well as the liveliness
		of neurons and clusters of connected lively neurons. */
	class WeightlessLivelinessAnalyzer {
		Q_OBJECT

		public:
			WeightlessLivelinessAnalyzer(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo, const AnalysisInfo& anaInfo, unsigned int timeStep);
			~WeightlessLivelinessAnalyzer();
			void runCalculation(const bool * const stop);

		signals:
			 void newResultsFound();
			 void progress(const QString& msg, unsigned int timeStep, unsigned int stepsCompleted, unsigned int totalSteps);

		private:
			//========================  VARIABLES  =============================
			/*! Wrapper around the network database */
			NetworkDao* networkDao;

			/*! Wrapper around the archive database */
			ArchiveDao* archiveDao;

			/*! Wrapper around the analysis database */
			LivelinessDao* livelinessDao;

			/*! Information about the analysis being run */
			AnalysisInfo analysisInfo;

			/*! The time step that is being analyzed by this thread */
			int timeStep;

			/*! Pointer to the stop variable in the controlling thread. */
			const bool* stop;

			/*! Nested  maps containing each neuron's to connections.*/
			QHash<unsigned int, QHash<unsigned int, bool> > toConnectionMap;

			/*! Map containing all of the weightless neurons in the network */
			QHash<unsigned int, WeightlessNeuron*> weightlessNeuronMap;

			/*! Map of the neurons firing at this time step */
			QHash<unsigned int, bool> firingNeuronMap;


			//=======================  METHODS  ==========================
			void calculateConnectionLiveliness();
			void calculateNeuronLiveliness();
			void identifyClusters();
			void deleteWeightlessNeurons();
			void loadFiringNeurons();
			void loadWeightlessNeurons();

	};

}

#endif//WEIGHTLESSLIVELINESSANALYZER_H

