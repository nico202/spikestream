#ifndef PHIANALYSISTIMESTEPTHREAD_H
#define PHIANALYSISTIMESTEPTHREAD_H

//SpikeStream includes
#include "AbstractAnalysisTimeStepThread.h"


namespace spikestream {

	class PhiAnalysisTimeStepThread : public AbstractAnalysisTimeStepThread {
	Q_OBJECT

	public:
		PhiAnalysisTimeStepThread(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo);
		~PhiAnalysisTimeStepThread();
		void run();

	};

}

#endif//PHIANALYSISTIMESTEPTHREAD_H
