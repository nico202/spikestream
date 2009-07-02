/***************************************************************************
 *   SpikeStream Application                                               *
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
#include "Tester.h"
#include "Debug.h"
#include "Utilities.h"

//Other includes
#include <iostream>
using namespace std;


/*! Constructor that calls the test code method. */
Tester::Tester(){
	testCode();
}


/*! Destructor. */
Tester::~Tester(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING TESTER"<<endl;
	#endif//MEMORY_DEBUG
}


//------------------------------------------------------------------------
//------------------------- PUBLIC METHODS -------------------------------
//------------------------------------------------------------------------

/*! Method containing code to be tested.
	Add any code you want to test here and enable the Tester constructor
	in SpikeStreamMainWindow constructor. */
void Tester::testCode(){
/*	string testStr="1e-06";
	double testDoub = Utilities::getDouble(testStr);
	cout<<"TEST DOUBLE RESULT1 = "<<testDoub<<endl;

	testStr="999e-06";
	testDoub = Utilities::getDouble(testStr);
	cout<<"TEST DOUBLE RESULT2 = "<<testDoub<<endl;

	testStr="-5.565E-06";
	testDoub = Utilities::getDouble(testStr);
	cout<<"TEST DOUBLE RESULT3 = "<<testDoub<<endl;

	testStr="1e06";
	testDoub = Utilities::getDouble(testStr);
	cout<<"TEST DOUBLE RESULT4 = "<<testDoub<<endl;

	testStr="999E06";
	testDoub = Utilities::getDouble(testStr);
	cout<<"TEST DOUBLE RESULT5 = "<<testDoub<<endl;

	testStr="-5.565e+06";
	testDoub = Utilities::getDouble(testStr);
	cout<<"TEST DOUBLE RESULT6 = "<<testDoub<<endl;*/
}


