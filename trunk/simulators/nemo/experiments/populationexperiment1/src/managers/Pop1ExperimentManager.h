#ifndef POP1EXPERIMENTMANAGER_H
#define POP1EXPERIMENTMANAGER_H

//SpikeStream includes
#include "NemoWrapper.h"
#include "SpikeStreamThread.h"

//Qt includes
#include <QString>


namespace spikestream {

	class Pop1ExperimentManager : public SpikeStreamThread {
		Q_OBJECT

		public:
			Pop1ExperimentManager();
			~Pop1ExperimentManager();
			void run();
			void startExperiment();

		private:

	};

}

#endif//PATTERNMANAGER_H
