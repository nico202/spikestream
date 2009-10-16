//SpikeStream includes
#include "Util.h"
#include "NumberConversionException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

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


/*! Converts the QString to a double
    Throws a number conversion exception if the conversion fails. */
double Util::getDouble(const QString& str){
    bool ok = true;
    qDebug()<<"STRING IS: "<<str;
    double newDouble = str.toDouble(&ok);
    if(!ok)
	throw NumberConversionException("Error converting " + str + " to double.");
    return newDouble;
}


/*! Returns a random number in the range specified */
int Util::getRandom(int min, int max){
    if(max <= min)
	throw SpikeStreamException("Incorrect range for random number: maximum <= minimum.");

    int randomNum = min;
    randomNum += qrand() % (max - min);
    return randomNum;
}


/*! Seeds the random number generator.
    If the seed is not specified or zero, number of seconds to midnight is used as the seed.*/
void Util::seedRandom(int seed){
    if(seed == 0){
	QTime midnight(0, 0, 0);
	qsrand(midnight.secsTo(QTime::currentTime()));
    }
    else
	qsrand(seed);
}



To get an integer in the range [0, x)


