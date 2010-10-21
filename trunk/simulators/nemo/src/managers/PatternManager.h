#ifndef PATTERNMANAGER_H
#define PATTERNMANAGER_H

//SpikeStream includes
#include "Pattern.h"

//Qt includes
#include <QString>


namespace spikestream {

	class PatternManager {
		public:
			PatternManager();
			~PatternManager();
			static Pattern* load(const QString& filePath);

		private:
	};

}

#endif//PATTERNMANAGER_H
