//SpikeStream includes
#include "Util.h"
#include "NumberConversionException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QTime>
#include <QStringList>

//Other includes
#include "math.h"
#include <iostream>
using namespace std;


/*! Returns the factorial of a number */
mpf_class Util::factorial (unsigned int num){
	mpz_class result=1;
	for (unsigned int i=1; i<=num; ++i)
		result *= i;
	return result;
}


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


/*! Converts the QString to a bool.
	Throws a number conversion exception if the conversion fails. */
bool Util::getBool(const QString& str){
	if(str.toLower() == "true")
		return true;
	else if (str.toLower() == "false")
		return false;
	throw NumberConversionException("Error converting '" + str + "' to boolean.");
}


/*! Converts the QString to a float.
	Throws a number conversion exception if the conversion fails. */
float Util::getFloat(const QString& str){
	bool ok = true;
	float newFloat = str.toFloat(&ok);
	if(!ok)
		throw NumberConversionException("Error converting '" + str + "' to float.");
	return newFloat;
}


/*! Converts the QString to an integer.
	Throws a number conversion exception if the conversion fails. */
int Util::getInt(const QString& str){
	bool ok = true;
	int newInt = str.toInt(&ok);
	if(!ok)
		throw NumberConversionException("Error converting '" + str + "' to integer.");
	return newInt;
}


/*! Converts the QString to an unsigned integer.
	Throws a number conversion exception if the conversion fails. */
unsigned int Util::getUInt(const QString& str){
	bool ok = true;
	unsigned int newInt = str.toUInt(&ok);
	if(!ok)
		throw NumberConversionException("Error converting '" + str + "' to unsigned integer.");
	return newInt;
}


/*! Converts a string containing a comma separated list of unsigned integers into  a qlist of unsigned ints  */
QList<unsigned int> Util::getUIntList(const QString& str){
	QStringList strList = str.split(",", QString::SkipEmptyParts);
	QList<unsigned int> uIntList;
	foreach(QString tmpStr, strList)
		uIntList.append(Util::getUInt(tmpStr.trimmed()));
	return uIntList;
}


/*! Converts the QString to a double
	Throws a number conversion exception if the conversion fails. */
double Util::getDouble(const QString& str){
	bool ok = true;
	double newDouble = str.toDouble(&ok);
	if(!ok)
		throw NumberConversionException("Error converting '" + str + "' to double.");
	return newDouble;
}


/*! Prints out the names and values of the parameters stored in the map */
void Util::printParameterMap(const QHash<QString, double>& paramMap){
	cout<<"---------------  Parameter map  -------------"<<endl;
	for(QHash<QString, double>::const_iterator iter = paramMap.begin(); iter != paramMap.end(); ++iter){
		cout<<"\tKey: "<<iter.key().toStdString().data()<<" value: "<<iter.value()<<endl;
	}
	cout<<endl;
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


/*! Carries out a safe copy of source cstring to target cstring that is no larger than targetSize. */
void Util::safeCStringCopy(char target[], const char source[], int targetSize){
	int newLength = strlen(source);
	if(newLength > targetSize){
		throw SpikeStreamException("Source cstring too large for destination cstring.");
	}
	int i;
	for(i=0; i< newLength; ++i) //Copy everything up to newLength-1
		target[i] = source[i];
	target[i] = '\0';//Finish string with null character
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


/*! Adds the trainign pattern to the weightless neuron */
void Util::addTraining(WeightlessNeuron& neuron, QString trainingPattern, int output){
	unsigned char* byteArr;
	int arrLen;
	fillByteArray(byteArr, arrLen, trainingPattern);
	QByteArray qByteArr = QByteArray::fromRawData((const char*)byteArr, arrLen);
	neuron.addTraining(qByteArr, output);
	delete [] byteArr;
}


/*! Returns true if the string of 1s and 0s matches the 1s and 0s in the byte array and the output */
bool Util::bitsEqual(unsigned char* byteArr, QString bitPattStr, int output){
	if(byteArr[0] != output)
		return false;

	for(int i=0; i<bitPattStr.length(); ++i){
		if(bitPattStr[i] == '1' && (byteArr[1 + i/8] & ( 1<<(i % 8) )))//1 is equal
			;//do nothing
		else if(bitPattStr[i] == '0' && !(byteArr[1 + i/8] & ( 1<<(i % 8) )))//0 is equal
			;//Do nothing
		else
			return false;//String and byte array do not match
	}
	return true;
}


/*! Fills the suplied array with the 1s and 0s specified in the string. */
void Util::fillByteArray(unsigned char*& byteArr, int& arrLen, QString byteStr){
	if(byteStr.length() % 8 == 0)
		arrLen = byteStr.length() / 8;
	else
		arrLen = byteStr.length() / 8 + 1;
	byteArr = new unsigned char[arrLen];

	//Initialize array
	for(int i=0; i<arrLen; ++i)
		byteArr[i] = 0;

	//Set bits corresponding to 1's in byte string
	for(int i=0; i<byteStr.length(); ++i){
		if(byteStr[i] == '1')
			byteArr[i/8] |= 1<<(i % 8);
	}
}


/*! Sets the generalization in a map of weightless neurons. */
void Util::setGeneralization(QHash<unsigned int, WeightlessNeuron*>& weiNeurMap, double gen){
	for(QHash<unsigned int, WeightlessNeuron*>::iterator iter = weiNeurMap.begin(); iter != weiNeurMap.end(); ++iter)
		iter.value()->setGeneralization(gen);
}





