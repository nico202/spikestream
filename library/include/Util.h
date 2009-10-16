#ifndef UTIL_H
#define UTIL_H

//Qt includes
#include <QString>

namespace spikestream {

    /*! Set of static utility methods intended eventually to supersede Utilities.h */
    class Util {
	public:
	    static int getInt(const QString& str);
	    static unsigned int getUInt(const QString& str);
	    static double getDouble(const QString& str);
	    static int getRandom(int min, int max);
	    static void seedRandom(int seed = 0);

    };

}

#endif//UTIL_H

