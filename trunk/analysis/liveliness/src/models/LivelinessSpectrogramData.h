#ifndef LIVELINESSSPECTROGRAMDATA_H
#define LIVELINESSSPECTROGRAMDATA_H

//SpikeStream includes
#include "AbstractSpectrogramData.h"
using namespace spikestream;

namespace spikestream {

	class LivelinessSpectrogramData: public AbstractSpectrogramData {
		public:
			LivelinessSpectrogramData(const AnalysisInfo& analysisInfo);
			~LivelinessSpectrogramData();
			virtual QwtRasterData *copy() const;

		protected:
			//=======================  METHODS  ===========================
			void loadData();

	};

}


#endif//LIVELINESSSPECTROGRAMDATA_H

