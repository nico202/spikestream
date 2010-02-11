#ifndef LIVELINESSTIMESTEPTHREAD_H
#define LIVELINESSTIMESTEPTHREAD_H

//SpikeStream includes
#include "AbstractAnalysisTimeStepThread.h"

namespace spikestream {

	class LivelinessTimeStepThread : public AbstractAnalysisTimeStepThread {
		Q_OBJECT

		public:
			LivelinessTimeStepThread(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo);
			~LivelinessTimeStepThread();
			void run();

		};

}

#endif//LIVELINESSTIMESTEPTHREAD_H
