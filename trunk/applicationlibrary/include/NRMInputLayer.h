#ifndef NRMINPUTLAYER_H
#define NRMINPUTLAYER_H

#include "NRMLayer.h"

namespace spikestream {

	/*! Holds information about NRM input layers */
	class NRMInputLayer : public NRMLayer {
		public:
			//========================  METHODS  =========================
					NRMInputLayer();
					virtual ~NRMInputLayer();
			void print();


			//=======================  VARIABLES  ========================
			int winType;
			int width2;
			int height2;
			int r;
			int b;
			char* bitmapName;
			bool noDisp;
			int colThresh;
			int invHor;
			int invVer;
			int segBorder;
	};

}

#endif // NRMINPUTLAYER_H
