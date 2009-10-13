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

#ifndef TESTER_H
#define TESTER_H

//SpikeStream includes
#include "DBInterface.h"


//----------------------------- Tester -------------------------------------
/*! Class used to run quick tests. The constructor can be uncommented in 
	SpikeStreamMainWindow if you want to run any tests.*/
//--------------------------------------------------------------------------

class Tester {

	public:
		Tester();
		~Tester();
		void testCode();


	private:
		//============================== METHODS ==============================
		/*! Declare copy constructor private so it cannot be used inadvertently. */
		Tester (const Tester&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		Tester operator = (const Tester&);

};


#endif //TESTER_H




