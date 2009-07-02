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

#ifndef UTILITIES_H
#define UTILITIES_H

//Qt includes
#include <QString>

//Other includes
#include <string>
#include <cstring>
#include <cstdlib>
using namespace std;


//----------------------------------Utilities-------------------------------
/*! Collection of static convenience methods for converting and other 
	tasks. */
//--------------------------------------------------------------------------

class Utilities {
 
	public:
		static int absVal(const int& num);
		static double absVal(const double& num);
		static bool cStringEquals(const char *cString1, const char *cString2, int length);
		static double getDouble(const char *cString);
		static double getDouble(string s);
                static double getDouble(QString s);
		static int getInt(const char *cString);
		static int getInt(string s);
                static int getInt(QString s);
		static unsigned int getNeuronGrpID(string neurGrpStr);
                static unsigned int getNeuronGrpID(QString neurGrpStr);
		static unsigned int getRandomUInt(unsigned int rangeHigh);
		static short getShort(const char *cString);
		static short getShort(string str);
                static short getShort(QString str);
		static unsigned int getUInt(const char *cString);
		static unsigned int getUInt(string s);
                static unsigned int getUInt(QString s);
		static unsigned short getUShort(const char *cString);
		static unsigned short getUShort(string s);
                static unsigned short getUShort(QString s);
		static double round(double number, unsigned int numPlaces);
		static void roundTwoDecimalPlaces(double&);
		static void safeCStringCat(char target[], const char source[], int targetSize);
		static void safeCStringCopy(char target[], const char source[], int targetSize);

};


#endif//UTILITIES_H

