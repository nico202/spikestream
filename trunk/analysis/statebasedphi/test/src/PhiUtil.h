#ifndef PHIUTIL_H
#define PHIUTIL_H

//SpikeStream includes
#include "PhiCalculator.h"

//Qt includes
#include <QString>

class PhiUtil {
	public:
		static PhiCalculator* buildPhiTestNetwork1();
		static PhiCalculator* buildPhiTestNetwork2();

};

#endif//PHIUTIL_H
