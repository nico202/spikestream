#ifndef TRANSFERENTROPYTIMESTEPTHREAD_H
#define TRANSFERENTROPYTIMESTEPTHREAD_H

//SpikeStream includes
#include "AbstractAnalysisTimeStepThread.h"

namespace spikestream {

	class TransferEntropyTimeStepThread : public AbstractAnalysisTimeStepThread {
		Q_OBJECT

		public:
			TransferEntropyTimeStepThread(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo);
			~TransferEntropyTimeStepThread();
			void run();

		};

}

#endif//TRANSFERENTROPYTIMESTEPTHREAD_H
