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

#ifndef NEURONGROUPTYPE_H
#define NEURONGROUPTYPE_H

//Qt includes
#include <QString>

//Other includes
#include <map>
#include <string>
using namespace std;


/*! Number of types of neuron group
	NOTE THIS NEEDS TO BE UPDATED WHENEVER A TYPE IS ADDED */
#define NUM_NEUR_GRP_TYPES 3



/*! 2D RECTANGULAR LAYER.
	Neurons are distributed within a single layer.*/
#define RECTANGULAR_LAYER_2D_VALUE 1

/*! Description of 2D rectangular neuron group.*/
#define RECTANGULAR_LAYER_2D_DESC "2D Rectangular Layer"

/*! Number of parameters for 2D rectangular neuron group.*/
//FIXME NOT IMPLEMENTED YET
#define NUM_RECTANGULAR_LAYER_2D_PARAM 2

/*! Parameters for 2D rectangular neuron group.*/
#define RECTANGULAR_LAYER_2D_PARAM {{"Temp parameter 1", "10"}, {"Temp parameter 2", "5"}}



/*! 3D RECTANGULAR LAYER.
	Box within which the neurons are distributed probabilistically. */
#define RECTANGULAR_LAYER_3D_VALUE 2

/*! Description of 3D rectangular neuron group.*/
#define RECTANGULAR_LAYER_3D_DESC "3D Rectangular Layer"

/*! Number of parameters for 3D rectangular neuron group.*/
#define NUM_RECTANGULAR_LAYER_3D_PARAM 2

/*! Parameters for 3D rectangular neuron group.*/
#define RECTANGULAR_LAYER_3D_PARAM {{"Temp parameter 3", "88"}, {"Temp parameter 4", "99"}}



/*! SIMNOS Component.
	This is a neuron group that is created from the SIMNOSComponent table in the 
	Devices database. This type of neuron group represents some sub component of
	SIMNOS, such as an arm or the head, and is topographically connected to the
	sensors listed in the SIMNOSComponent table. */
#define SIMNOS_COMPONENT_LAYER_VALUE 3

/*! Description of SIMNOS component neuron group.*/
#define SIMNOS_COMPONENT_LAYER_DESC "SIMNOS Component"

/*! Number of parameters for SIMNOS component neuron group.*/
#define NUM_SIMNOS_COMPONENT_LAYER_PARAM 2

/*! Parameters for SIMNOS component neuron group.*/
#define SIMNOS_COMPONENT_LAYER_PARAM {{"Temp parameter 3", "88"}, {"Temp parameter 4", "99"}}



/*! Default type when a type cannot be found.*/
#define UNDEFINED_TYPE 0


//---------------------------- Neuron Group Type ---------------------------
/*! Defines different neuron group types supported by the simulator. This 
	includes simple layers and 3D arrangements of neurons. */
//--------------------------------------------------------------------------

class NeuronGroupType {

	public:
		//=========================== VARIABLES =======================================
		/* Variables holding types to enable other classes to easily access them.*/
		/*! Number of types of neuron group.*/
		static const int NumberTypes= NUM_NEUR_GRP_TYPES;
	
		/*! 2D rectangular neuron group.*/
		static const unsigned short RectangularLayer2D = RECTANGULAR_LAYER_2D_VALUE;

		/*! 3D rectangular neuron group.*/
		static const unsigned short RectangularLayer3D = RECTANGULAR_LAYER_3D_VALUE;

		/*! SIMNOS component neuron group.*/
		static const unsigned short SIMNOSComponentLayer = SIMNOS_COMPONENT_LAYER_VALUE;


		//============================ METHODS ========================================
		//Collection of methods to access types in different ways
		static char* getDescription(unsigned short);
		static void getParameters(map<string, string>&, unsigned short);
		static unsigned short getType(string);
                static unsigned short getType(QString);
		static unsigned short* getTypes();
		static void printParameters();

};


#endif//NEURONGROUPTYPE_H
