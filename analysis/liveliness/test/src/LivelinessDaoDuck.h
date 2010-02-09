#ifndef LIVELINESSDAODUCK_H
#define LIVELINESSDAODUCK_H

//SpikeStream includes
#include "Cluster.h"
#include "LivelinessDao.h"
using namespace spikestream;

//Qt includes
#include <QList>


/*! Class injected for testing into WeightlessLivelinessAnalyzer */
class LivelinessDaoDuck : public LivelinessDao {
	public:
		LivelinessDaoDuck();
		virtual ~LivelinessDaoDuck();
		void addCluster(unsigned int analysisID, int timeStep, QList<unsigned int>& neuronIDList, double liveliness);
		QList<Cluster>& getClusterList() { return clusterList; }
		void reset();

	private:
		QList<Cluster> clusterList;
};

#endif//LIVELINESSDAODUCK_H

