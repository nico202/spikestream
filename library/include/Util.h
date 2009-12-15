#ifndef UTIL_H
#define UTIL_H

//Qt includes
#include <QString>
#include <QList>

//Other includes
#include <gmpxx.h>

namespace spikestream {

    typedef unsigned char byte;

    /*! Set of static utility methods intended eventually to supersede Utilities.h */
    class Util {
	public:
	    static mpf_class factorial(unsigned int num);
	    static void fillSelectionArray(bool* array, int arraySize, int selectionSize);
	    static int getInt(const QString& str);
	    static unsigned int getUInt(const QString& str);
	    static QList<unsigned int> getUIntList(const QString& str);
	    static double getDouble(const QString& str);
	    static int getRandom(int min, int max);
	    static void printByteArray(byte* byteArr, int arrLen);
	    static void printByteArray(const QByteArray& byteArr);
	    static void printBoolArray(bool arr[], int arrLen);
	    static double rDouble(double num, int numPlaces);
	    static unsigned int rUInt(double num);
	    static void seedRandom(int seed = 0);

    };

}

#endif//UTIL_H

