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

#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

//Other includes
#include <iostream>
#include <fstream>
#include <map>
#include <string>
using namespace std;


//--------------------------- Config Loader---------------------------------
/*! Loads the configuration from the specified file. */
//--------------------------------------------------------------------------

class ConfigLoader{

	public:
		ConfigLoader(string);
		~ConfigLoader();
		const char* getCharData(string);
		string getStringData(string);


	private:
		//=========================== VARIABLES =======================================
		/*! Holds the configuration parameters loaded from the file.*/
		map<string, string> configMap;


		//============================ METHODS ========================================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		ConfigLoader(const ConfigLoader&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		ConfigLoader operator = (const ConfigLoader&);

		void printConfig();

};


#endif//CONFIGLOADER_H
