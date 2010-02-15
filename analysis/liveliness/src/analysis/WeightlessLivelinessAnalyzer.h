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

	typedef unsigned char byte;

	/*! Analyzes weightless neurons for state-based liveliness.
		Identifies the liveliness of connections for a particular state as well as the liveliness
		of neurons and clusters of connected lively neurons. */
	class WeightlessLivelinessAnalyzer : public QObject {
		Q_OBJECT

		public:
			WeightlessLivelinessAnalyzer(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo, const AnalysisInfo& anaInfo, unsigned int timeStep);
			WeightlessLivelinessAnalyzer();
			~WeightlessLivelinessAnalyzer();
			void calculateConnectionLiveliness();
			void fillInputArray(WeightlessNeuron* weiNeuron, byte*& inPatArr, int& inPatArrLen);
			void flipBits(byte inPatArr[], int inPatArrLen, QList<unsigned int>& indexList);
			QHash<unsigned int, QHash<unsigned int, double> > getFromConnectionLivelinessMap() { return fromConnectionLivelinessMap; }
			QHash<unsigned int, double> getNeuronLivelinessMap() { return neuronLivelinessMap; }
			void identifyClusters();
			void runCalculation(const bool * const stop);
			void setAnalysisInfo(const AnalysisInfo& anaInfo) { this->analysisInfo = anaInfo; }
			void setFromConnectionLivelinessMap(QHash<unsigned int, QHash<unsigned int, double> > conLivMap) { this->fromConnectionLivelinessMap = conLivMap; }
			void setToConnectionLivelinessMap(QHash<unsigned int, QHash<unsigned int, double> > conLivMap) { this->toConnectionLivelinessMap = conLivMap; }
			void setFiringNeuronMap(QHash<unsigned int, bool> firingNeuronMap) { this->firingNeuronMap = firingNeuronMap; }
			void setLivelinessDao(LivelinessDao* livelinessDao) { this->livelinessDao = livelinessDao; }
			void setNeuronLivelinessMap(QHash<unsigned int, double>& neurLivMap) { this->neuronLivelinessMap = neurLivMap; }
			void setWeightlessNeuronMap(QHash<unsigned int, WeightlessNeuron*> weiNeurMap) { this->weightlessNeuronMap = weiNeurMap; }

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

			/*! Map containing all of the weightless neurons in the network */
			QHash<unsigned int, WeightlessNeuron*> weightlessNeuronMap;

			/*! Map containing the liveliness of each connection FROM the neuron which is the key. */
			QHash<unsigned int, QHash<unsigned int, double> > fromConnectionLivelinessMap;

			/*! Map containing the liveliness of each connection TO the neuron which is the key. */
			QHash<unsigned int, QHash<unsigned int, double> > toConnectionLivelinessMap;

			/*! Map containing the liveliness of each neuron */
			QHash<unsigned int, double>  neuronLivelinessMap;

			/*! Map of the neurons firing at this time step */
			QHash<unsigned int, bool> firingNeuronMap;

			/*! Number of steps in the calculation */
			unsigned int numberOfProgressSteps;

			/*! Step we are at in the calculation */
			unsigned int progressCounter;


			//=======================  METHODS  ==========================
			void deleteWeightlessNeurons();
			double getClusterLiveliness(QList<unsigned int>& neuronIDs);
			void loadFiringNeurons();
			void loadWeightlessNeurons();
			void setConnectionLiveliness(unsigned int fromNeurID, unsigned int toNeurID, double liveliness);
			void saveNeuronLiveliness();
			void updateProgress(const QString& msg);
	};

}

#endif//WEIGHTLESSLIVELINESSANALYZER_H

