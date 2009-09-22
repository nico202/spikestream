#include "Util.h"
#include "NumberConversionException.h"
using namespace spikestream;


/*! Converts the QString to an integer.
    Throws a number conversion exception if the conversion fails. */
int Util::getInt(const QString& str){
    bool ok = true;
    int newInt = str.toInt(&ok);
    if(!ok)
	throw NumberConversionException("Error converting " + str + " to integer.");
    return newInt;
}


/*! Converts the QString to an unsigned integer.
    Throws a number conversion exception if the conversion fails. */
unsigned int Util::getUInt(const QString& str){
    bool ok = true;
    unsigned int newInt = str.toUInt(&ok);
    if(!ok)
	throw NumberConversionException("Error converting " + str + " to unsigned integer.");
    return newInt;
}


