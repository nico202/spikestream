//SpikeStream includes
#include "Util.h"
#include "NumberConversionException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QTime>

//Other includes
#include "math.h"
#include <iostream>
using namespace std;


/*! Fills an array to select k neurons out of n. */
void Util::fillSelectionArray(bool* selectionArray, int arraySize, int selectionSize){
    int nonSelectionSize = arraySize - selectionSize;

    //Add zeros at start of array up to the non-selection size
    for(int i=0; i<nonSelectionSize; ++i)
	selectionArray[i] = false;

    //Add 1s to the rest of the array
    for(int i=nonSelectionSize; i<arraySize; ++i)
	selectionArray[i] = true;
}


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


/*! Prints out a bool array  */
void Util::printBoolArray(bool arr[], int arrLen){
    cout<<"Bool Array: ";
    for(int i=0; i< arrLen; ++i){
	if(arr[i])
	    cout<<"1";
	else
	    cout<<"0";
    }
    cout<<endl;
}


/*! Prints out the byte array */
void Util::printByteArray(byte* byteArr, int arrLen){
    cout<<"Byte array: ";
    for(int i=0; i< arrLen*8; ++i){
	if(byteArr[i/8] & 1<<(i%8))
	    cout<<"1";
	else
	    cout<<"0";
    }
    cout<<endl;
}


/*! Prints out the bits in a QByteArray */
void Util::printByteArray(const QByteArray& byteArr){
    cout<<"QByteArray: ";
    for(int i=0; i< byteArr.size(); ++i){
	byte tmpByte = (unsigned char) byteArr.at(i);
	for(int j=0; j<8; ++j){
	    if(tmpByte & 1<<j)
		cout<<"1";
	    else
		cout<<"0";
	}
    }
    cout<<endl;
}


/*! Rounds the given double to the specified number of decimal places. */
double Util::rDouble(double num, int numPlaces){
    double tempDoub = num * ( pow(10.0, (double)numPlaces) );
    tempDoub = rint(tempDoub);
    tempDoub /= ( pow(10.0, (double)numPlaces) );
    return tempDoub;
}


/*! Rounds the double and returns it as an unsigned integer */
unsigned int Util::rUInt(double num){
    num = rint(num);
    return (unsigned int) num;
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




