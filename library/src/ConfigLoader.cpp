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
#include "ConfigLoader.h"


/*! Constructor loads up the configuration data from the given file path. */
ConfigLoader::ConfigLoader(string configPath){

	//Open stream to read file
	ifstream configReader;
	configReader.open(configPath.data());

	//If it is not there, throw exception and exit.
	if(!configReader){
		cout<<"Configuration file not found!"<<endl;
		throw 0;
	}

	//Next step is to read file, extract configuration parameters and put them in a map
	char row[200];
	while(!configReader.eof()){
		configReader.getline(row, 200);
		
		//Extracts key and value pairs
		if (!(row[0] == '#' | row[0] == ' ' | row[0] == 13 | row[0] == 0)){
			string key = "";
			int j=0;
			while((row[j] != ' ') && (row[j]!= '=') && j<200){
				key += row[j];
				j++;
			}
			while(row[j] == ' ' | row [j] == '=')
				j++;
			string value = "";
			while(row[j] != 0 && row [j] != ' ' && row[j] != 13 && row[j] != 0){
				value += row[j];
				j++;
			}

			//Add to map
			configMap[key] = value;
		}
	}
	configReader.close();
}


/*! Destructor. */
ConfigLoader::~ConfigLoader(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING CONFIG LOADER"<<endl;
	#endif//MEMORY_DEBUG
}


//-----------------------------------------------------------------------
//------------------------ PUBLIC METHODS -------------------------------
//-----------------------------------------------------------------------

/*! Extracts the configuration parameter as a cstring. */
const char* ConfigLoader::getCharData(string key){
	string s = configMap[key];
	if(s.empty()){
		cerr<<"Key \""<<key<<"\" not present in config file"<<endl;
		throw 0;
	}
	return s.data();
}


/*! Extracts the configuration parameter as a string. */
string ConfigLoader::getStringData(string key){
	string s = configMap[key];
	if(s.empty()){
		cerr<<"Key \""<<key<<"\" not present in config file"<<endl;
		throw 0;
	}
	return s;
}


//-----------------------------------------------------------------------
//--------------------------- PRIVATE METHODS ---------------------------
//-----------------------------------------------------------------------

/*! Debug method for printing out the loaded configuration parameters. */
void ConfigLoader::printConfig(){
	cout<<"Configuration for Neuron Application"<<endl;
	cout<<"---------------------------------------"<<endl;
	map<string, string>::iterator iter;
    for (iter = configMap.begin(); iter != configMap.end(); ++iter) {
        cout << "Key: " << iter->first << "; "
             << "Value: " << iter->second << endl;
    }
}


