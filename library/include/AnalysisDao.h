#ifndef ANALYSISDAO_H
#define ANALYSISDAO_H

//SpikeStream includes
#include "AbstractDao.h"
#include "AnalysisInfo.h"
using namespace spikestream;

namespace spikestream {

	/*! Data access object for the analysis database */
    class AnalysisDao : public AbstractDao {
		public:
			AnalysisDao(const DBInfo& dbInfo);
			AnalysisDao();
			virtual ~AnalysisDao();

			void addAnalysis(AnalysisInfo& analysisInfo);
			void deleteAnalysis(unsigned int analysisID);
			void deleteAllAnalyses();
			QList<AnalysisInfo> getAnalysesInfo(unsigned int networkID, unsigned int archiveID, unsigned int analysisType);
			bool networkHasAnalyses(unsigned networkID);//UNTESTED
			void updateDescription(unsigned int analysisID, const QString& description);

    };

}

#endif//ANALYSISDAO_H
