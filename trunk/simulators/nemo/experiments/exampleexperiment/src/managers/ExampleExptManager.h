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

			/*! Time to pause between sections of the experiment */
			int pauseInterval_ms;


			//======================  METHODS  ======================
			void runExperiment();
			void stepNemo(unsigned numTimeSteps);
			void storeParameters(QHash<QString, double>& parameterMap);

	};

}

#endif//TEMPORALCODINGEXPTMANAGER_H
