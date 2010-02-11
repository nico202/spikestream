#ifndef LIVELINESSDAO_H
#define LIVELINESSDAO_H

#include "AnalysisDao.h"
#include "Cluster.h"
#include "DBInfo.h"
using namespace spikestream;


namespace spikestream {

	class LivelinessDao : public AnalysisDao {
		public:
			LivelinessDao(const DBInfo& dbInfo);
			LivelinessDao();
			virtual ~LivelinessDao();
			virtual void addCluster(unsigned int analysisID, int timeStep, QList<unsigned int>& neuronIDList, double liveliness);
			bool containsAnalysisData(unsigned int analysisID, unsigned int firstTimeStep, unsigned int lastTimeStep);
			void deleteTimeSteps(unsigned int analysisID, unsigned int firstTimeStep, unsigned int lastTimeStep);
			QList<Cluster> getClusters(unsigned int analysisID);
			double getNeuronLiveliness(unsigned int analysisID, unsigned int timeStep, unsigned int neuronID);
			double getMaxNeuronLiveliness(unsigned int analysisID);
			void setNeuronLiveliness(unsigned int analysisID, int timeStep,unsigned int neuronID, double liveliness);
	};

}

#endif//LIVELINESSDAO_H
