#ifndef STATEBASEDPHISPECTROGRAMDATA_H
#define STATEBASEDPHISPECTROGRAMDATA_H

//SpikeStream includes
#include "AbstractSpectrogramData.h"
using namespace spikestream;

namespace spikestream {

	class StateBasedPhiSpectrogramData: public AbstractSpectrogramData {
		public:
			StateBasedPhiSpectrogramData(const AnalysisInfo& analysisInfo);
			~StateBasedPhiSpectrogramData();
			virtual QwtRasterData *copy() const;

		protected:
			//=======================  METHODS  ===========================
			void loadData();

	};

}


#endif//STATEBASEDPHISPECTROGRAMDATA_H

