/***************************************************************************
 *   SpikeStream Simulation                                                *
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

#ifndef CLASSLOADER_H
#define CLASSLOADER_H

//SpikeStream includes
#include "DBInterface.h"
#include "Neuron.h"
#include "Synapse.h"

//Other includes
#include <map>
#include <string>
using namespace std;


/*! The type of the function used to create neurons. */
typedef Neuron* (*CreateNeuronFunctionType)();

/*! The type of the function used to create synapses. */
typedef Synapse* (*CreateSynapseFunctionType)();


//---------------------------- Class Loader -------------------------------
/*! Dynamically loads neuron and synapse classes from libraries.
	The location of these libraries is defined in the NeuronTypes and
	SynapseTypes tables. */
//-------------------------------------------------------------------------

class ClassLoader {

	public:
		ClassLoader(DBInterface *netDBInter);
		~ClassLoader();
		string getConnGrpParameterTableName(unsigned int connGrpID);
		string getNeuronParameterTableName(unsigned short neuronType);
		Neuron* getNewNeuron(unsigned short neuronType);
		Synapse* getNewSynapse(unsigned short synapseType);


	private:
		//========================== VARIABLES ============================
		/*! Hold reference to dbinterface.*/
		DBInterface *networkDBInterface;

		/*! Map holding function pointers that create neurons of each type.*/
		map<unsigned short, CreateNeuronFunctionType> neuronFunctionMap;

		/*! Map holding function pointers that create synapses of each type.*/
		map<unsigned short, CreateSynapseFunctionType> synapseFunctionMap;

		/*! Map holding details about neuron parameter tables.*/
		map<unsigned short, string> neuronParameterTableMap;//Key is neuron type

		/*! Map holding details about synapse parameter tables.*/
		map<unsigned int, string> synapseParameterTableMap;//Key is connection group id


		//========================== METHODS ============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		ClassLoader (const ClassLoader&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		ClassLoader operator = (const ClassLoader&);

		void loadNeuronClasses();
		void loadSynapseClasses();

};


#endif //CLASSLOADER_H
