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
			static void load(const QString& filePath, Pattern& pattern);

		private:
			static Box getBox(const QString& str);
			static Point3D getPoint(const QString& str);
	};

}

#endif//PATTERNMANAGER_H
