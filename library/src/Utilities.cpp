/***************************************************************************
 *   SpikeStream Library                                                   *
 *   Copyright (C) 2007 by David Gamez                                     *
 *   david@davidgamez.eu                                                   *
 *   Version 0.1                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

//SpikeStream includes
#include "Utilities.h"

//Other includes
#include <math.h>
#include <iostream>
using namespace std;

//Testing debug variables
//#define TEST_ROUND


/*! The maximum value of a short. */
#define MAX_SHORT_VALUE 32768

/*! The minimum value of a short. */
#define MIN_SHORT_VALUE -32768

/*! The maximum value of an unsigned short. */
#define MAX_USHORT_VALUE 65536


/*! Exception thrown when there is a number conversion error. */
class NumConversionException : public exception{

  virtual const char* what() const throw(){
    return "Number conversion error";
  }
} numConversionException;



/*! Exception thrown when there is a different kind of utilities error. */
class UtilitiesException : public exception{

  virtual const char* what() const throw(){
    return "Utilities error";
  }
} genUtilitiesException;


//-------------------------------------------------------------------------
//---------------------- PUBLIC STATIC METHODS ----------------------------
//-------------------------------------------------------------------------

/*! Returns the absolute value of the number.*/
int Utilities::absVal(const int& num){
	if(num < 0)
		return num * -1;
	return num;
}


/*! Returns the absolute value of the number.*/
double Utilities::absVal(const double& num){
	if(num < 0.0)
		return num * -1.0;
	return num;
}


/*! Returns true if string1 = string2 without the possibility of a 
	buffer overflow. */
bool Utilities::cStringEquals(const char *cString1, const char *cString2, int length){
	for(int i=0; i<length; ++i){
		if(cString1[i] != cString2[i]){
			return false;
		}
		//Both strings are shorter than length, but equal so return true
		else if(cString1[i] == '\0')
			return true;
	}
	//Have not returned so far, so strings must be equal over the specified length
	return true;
}


/*! Converts cstring to double. */
double Utilities::getDouble(const char *cString){
	int pointCount = 0;//Records the number of points in the double
	int eCount = 0;//Records the number of 'e's in the double for floating point stuff
	int dashCount =0;//Records the number of '-'s in the double excluding the first one
	int plusCount = 0;//Records the number of '+'s in the double

	//First check to see if first character is a negative sign or a digit
	if((cString[0] == '-') || isdigit(cString[0])){
		//Work through rest of characters to check they are digits or the letter E or e and there is not more than 1 point
		for(int i=1; i<strlen(cString); i++){
			if(cString[i] == '.'){
				++pointCount;
				if(pointCount > 1){
					cerr<<"Utilities: ERROR IN CSTRING TO DOUBLE CONVERSION. TOO MANY POINTS IN CSTRING: \""<<cString<<"\""<<endl;
					throw numConversionException;
				}
			}
			else if(cString[i] == 'E' || cString[i] == 'e'){
				++eCount;
				if(eCount > 1){
					cerr<<"Utilities: ERROR IN CSTRING TO DOUBLE CONVERSION. TOO MANY 'e's IN CSTRING: \""<<cString<<"\""<<endl;
					throw numConversionException;
				}
			}
			else if(cString[i] == '-'){
				++dashCount;
				if(dashCount > 1){
					cerr<<"Utilities: ERROR IN CSTRING TO DOUBLE CONVERSION. TOO MANY '-'s IN CSTRING: \""<<cString<<"\""<<endl;
					throw numConversionException;
				}
			}
			else if(cString[i] == '+'){
				++plusCount;
				if(plusCount > 1){
					cerr<<"Utilities: ERROR IN CSTRING TO DOUBLE CONVERSION. TOO MANY '+'s IN CSTRING: \""<<cString<<"\""<<endl;
					throw numConversionException;
				}
			}
			else if(!isdigit(cString[i])){
				cerr<<"Utilities: ERROR IN CSTRING TO DOUBLE CONVERSION. CSTRING CONTAINS NON DIGITS: \""<<cString<<"\""<<endl;
				throw numConversionException;
			}
		}
		//Convert and return the new double.
		return atof(cString);
	}
	else {
		cerr<<"Utilities: ERROR IN CSTRING TO DOUBLE CONVERSION. FIRST CHARACTER IN CSTRING CONTAINS NON DIGITS: \""<<cString<<"\""<<endl;
		throw numConversionException;
	}
}


/*! Converts string to double. */
double Utilities::getDouble(string s){
	return Utilities::getDouble(s.data());
}


/*! Converts QString to double. */
double Utilities::getDouble(QString s){
        return Utilities::getDouble(s.toStdString().data());
}

/*! Converts cstring to signed integer. */
int Utilities::getInt(const char *cString){
	//First check to see if first character is a negative sign or a digit
	if((cString[0] == '-') || isdigit(cString[0])){
		for(int i=1; i<strlen(cString); i++){//Work through rest of characters to check they are digits
			if(!isdigit(cString[i])){
				cerr<<"Utilities: ERROR IN CSTRING TO INT CONVERSION. CSTRING CONTAINS NON DIGITS: \""<<cString<<"\""<<endl;
				throw numConversionException;
			}
		}
		//Create and return the new integer.
		int newInt = atoi(cString);
		return newInt;
	}
	else {
		cerr<<"Utilities: ERROR IN CSTRING TO INT CONVERSION. CSTRING CONTAINS NON DIGITS: \""<<cString<<"\""<<endl;
		throw numConversionException;
	}
}


/*! Converts string to integer. */
int Utilities::getInt(string s){
	return Utilities::getInt(s.data());
}


/*! Converts QString to integer. */
int Utilities::getInt(QString s){
        return Utilities::getInt(s.toStdString().data());
}


/*! Extracts NeuronGrpID from string of the form "layerName[NeuronGrpID]". */
unsigned int Utilities::getNeuronGrpID(string neurGrpStr){
	 //Chop off the first part of the string up to the bracket: string should equal "NeuronGrpID]"
    neurGrpStr = neurGrpStr.substr( neurGrpStr.find('[') + 1);
	string uIntStr  = "";
	int counter = 0;
	while(neurGrpStr[counter] != ']'){
		uIntStr += neurGrpStr[counter];
		++counter;
		if(counter == 100){
			cerr<<"Utilities: ERROR EXTRACTING NEURON GROUP ID FROM STRING: "<<neurGrpStr<<endl;
			throw genUtilitiesException;
		}
	}
	return getUInt(uIntStr);
}


/*! Extracts NeuronGrpID from QString of the form "layerName[NeuronGrpID]". */
unsigned int Utilities::getNeuronGrpID(QString str){
    return Utilities::getNeuronGrpID(str.toStdString());
}


/*! Returns a random unsigned integer betwen 0 and rangeHigh.
	rand() returns a number between 0 and RANGE_MAX, so dividing
	this by RANGE MAX gives a number between 0 and 1.0 */
unsigned int Utilities::getRandomUInt(unsigned int rangeHigh){
	return (unsigned int)rint( (double)rangeHigh * ( (double)rand() / (double)RAND_MAX ));
}


/*! Converts cstring to short. */
short Utilities::getShort(const char *cString){
	int tempInt = getInt(cString);
	if((tempInt > MAX_SHORT_VALUE) || (tempInt < MIN_SHORT_VALUE)){//Check it is in range for short
		cerr<<"Utilities: UNSIGNED SHORT IS OUTSIDE OF SPECIFIED RANGE: "<<tempInt<<endl;
		throw numConversionException;
	}
	return (short)tempInt;
}


/*! Converts string to short. */
short Utilities::getShort(string str){
	return getShort(str.data());
}


/*! Converts QString to short. */
short Utilities::getShort(QString str){
        return getShort(str.toStdString().data());
}


/*! Converts cstring to unsigned integer. */
unsigned int Utilities::getUInt(const char *cString){
	//First check that all characters are digits. Unsigned int so no negative sign allowed
	for(int i=0; i<strlen(cString); i++){
		if(!isdigit(cString[i])){
			cerr<<"Utilities: ERROR IN CSTRING TO UINT CONVERSION. CSTRING CONTAINS NON DIGITS: \""<<cString<<"\""<<endl;
			throw numConversionException;
		}
	}
	int newInt = atoi(cString);
	return newInt;
}


/*! Converts string to unsigned integer. */
unsigned int Utilities::getUInt(string s){
	return Utilities::getUInt(s.data());
}


/*! Converts QString to unsigned integer. */
unsigned int Utilities::getUInt(QString s){
        return Utilities::getUInt(s.toStdString().data());
}


/*! Converts cstring to unsigned short. */
unsigned short Utilities::getUShort(const char *cString){
	//First check that all characters are digits. Unsigned int so no negative sign allowed
	for(int i=0; i<strlen(cString); i++){
		if(!isdigit(cString[i])){
			cerr<<"Utilities: ERROR IN CSTRING TO UINT CONVERSION. CSTRING CONTAINS NON DIGITS: \""<<cString<<"\""<<endl;
			throw numConversionException;
		}
	}
	int newInt = atoi(cString);
	if(newInt >= 0 && newInt < MAX_USHORT_VALUE)
		return newInt;
	cerr<<"Utilities: UNSIGNED SHORT IS OUTSIDE OF SPECIFIED RANGE: "<<newInt<<endl;
	throw numConversionException;
}


/*! Converts string to unsigned short. */
unsigned short Utilities::getUShort(string s){
	return Utilities::getUShort(s.data());
}


/*! Converts QString to unsigned short. */
unsigned short Utilities::getUShort(QString s){
        return Utilities::getUShort(s.toStdString().data());
}


/*! Rounds the given double to the specified number of decimal places. */
double Utilities::round(double tempDoub, unsigned int numPlaces){
	#ifdef TEST_ROUND
		cout<<"Rounding "<<tempDoub<<" to "<<numPlaces<<" decimal places. ";
	#endif//TEST_ROUND

	tempDoub = tempDoub * ( pow(10.0, (double)numPlaces) );
	tempDoub = rint(tempDoub);
	tempDoub = tempDoub / ( pow(10.0, (double)numPlaces) );

	#ifdef TEST_ROUND
		cout<<"Result =  "<<tempDoub<<endl;
	#endif//TEST_ROUND

	return tempDoub;
}


/*! Rounds the given double to two decimal places. */
void Utilities::roundTwoDecimalPlaces(double &tempDoub){
	tempDoub = tempDoub * 100.0;
	tempDoub = rint(tempDoub);
	tempDoub = tempDoub * 0.01;
}


/*! Adds source onto the end of target without potential buffer overflow. */
void Utilities::safeCStringCat(char target[], const char source[], int targetSize){
	//Do intitial checks on target size
	if(targetSize < 0){
		cerr<<"Utilities: String target size less than 0"<<endl;
		throw genUtilitiesException;
	}
	else if (targetSize != (int)strlen(target)){
		cerr<<"Utilities: TARGET LENGTH "<<strlen(target)<<" DOES NOT MATCH targetSize "<<targetSize<<endl;
		throw genUtilitiesException;
	}
	
	// Find the position of the null character in target
	int endLocation = 0;
	for(int i=0; i<targetSize; i++){
		if(target[i] == '\0'){
			endLocation = i;
			break;
		}
	}
	
	//Check that new size of string will fit within target
	int newLength = endLocation + strlen(source);
	if(newLength > targetSize - 1){
		cerr<<"Utilities: SOURCE CSTRING TOO LARGE FOR DESTINATION CSTRING"<<endl;
		throw genUtilitiesException;
	}
	
	//Add source onto the end of target
	int i;
	for(i=endLocation; i< newLength; i++) //Copy everything up to newLength-1
		target[i] = source[i];
	target[i] = '\0';//Finish string with null character
}


/*! Carries out a safe copy of source cstring to target cstring that is no larger than targetSize. */
void Utilities::safeCStringCopy(char target[], const char source[], int targetSize){
	int newLength = strlen(source);
	if(newLength > targetSize){
		cerr<<"Utilities: SOURCE CSTRING TOO LARGE FOR DESTINATION CSTRING"<<endl;
		throw genUtilitiesException;
	}
	int i;
	for(i=0; i< newLength; ++i) //Copy everything up to newLength-1
		target[i] = source[i];
	target[i] = '\0';//Finish string with null character
}

