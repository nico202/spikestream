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

#ifndef CONNECTIONTYPE_H
#define CONNECTIONTYPE_H

//Qt includes
#include <QString>

//Other includes
#include <map>
#include <string>
using namespace std;


/*! Number of types and number of intra / inter types.
	NOTE THESE NEED TO BE UPDATED WHENEVER A TYPE IS ADDED. */
#define NUM_CONNECTION_TYPES 10

/*! Number of types of connection within a single layer. */
#define NUMBER_INTRA_TYPES 2

/*! Number of types of connection between two layers. */
#define NUMBER_INTER_TYPES 8


//------------------------------------------------------------------------------
//----------------------------- INTRA TYPES ------------------------------------
//------------------------------------------------------------------------------

/*! SIMPLE CORTEX TYPE. 
	Used for short range excitation and long range inhibition within a single layer. */
#define SIMPLE_CORTEX_VALUE 1

/*! Description used for simple cortex connections. */
#define SIMPLE_CORTEX_DESC "Simple Cortex"

/*! Number of simple cortex connection parameters. */
#define NUM_SIMPLE_CORTEX_PARAM 9

/*! Parmeters for simple cortex connection. */
#define SIMPLE_CORTEX_PARAM {{"Inhibition radius", "10"}, {"Excitation radius", "5"}, {"Excitation connection density", "1.0"}, {"Inhibition connection density", "1.0"}, {"Overlap", "0"}, {"Normal weight distribution?", "1"}, {"Weight range", "0.2"}, {"Excitation weight", "0.8"}, {"Inhibition weight", "-0.8"}}


/*! UNSTRUCTURED EXCITATORY/ INHIBITORY INTRA
	Unstructured connections within a single layer in which each neuron either makes excitatory
	or inhibitory connections. */
#define UNSTRUCTURED_EX_INHIB_INTRA_VALUE 2

/*! Description used for simple cortex connections. */
#define UNSTRUCTURED_EX_INHIB_INTRA_DESC "Unstructured excitatory/ inhibitory (intra)"

/*! Number of simple cortex connection parameters. */
#define NUM_UNSTRUCTURED_EX_INHIB_INTRA_PARAM 7

/*! Parmeters for simple cortex connection. */
#define UNSTRUCTURED_EX_INHIB_INTRA_PARAM {{"Excitatory percentage", "80.0"}, {"Excitation weight", "0.111"}, {"Excitation weight range", "0.0"}, {"Inhibition weight", "-1.0"}, {"Inhibition weight range", "0.0"}, {"Excitation connection prob", "0.02"}, {"Inhibition connection prob", "0.02"}}


//------------------------------------------------------------------------------
//----------------------------- INTER TYPES ------------------------------------
//------------------------------------------------------------------------------

/*! OFF CENTRE ON SURROUND TYPE.
	Inhibitory centre; excitatory surround.
	Used for connections between layers in visual system and elsewhere.*/
//FIXME THIS TYPE IS NOT IMPLMENTED YET.
#define OFF_CENTRE_ON_SURROUND_VALUE 3

/*! Description used for off centre on surround connections. */
#define OFF_CENTRE_ON_SURROUND_DESC "Off Centre On Surround"

/*! Number of off centre on surround connection parameters. */
#define NUM_OFF_CENTRE_ON_SURROUND_PARAM 10

/*! Parmeters for off centre on surround connection. */
#define OFF_CENTRE_ON_SURROUND_PARAM {{"Outer width", "30"}, {"Outer length", "30"}, {"Inner width", "10"}, {"Inner length", "10"}, {"Overlap", "0"}, {"Rotate?", "0"}, {"Normal weight distribution?", "1"}, {"Weight range", "0.2"}, {"Excitation weight", "0.9"}, {"Inhibition weight", "-0.9"}}



/*! ON CENTRE OFF SURROUND TYPE.
	Excitatory centre; inhibitory surround.
	Used for connections between layers in visual system and elsewhere. */
#define ON_CENTRE_OFF_SURROUND_VALUE 4

/*! Description used for on centre off surround connections. */
#define ON_CENTRE_OFF_SURROUND_DESC "On Centre Off Surround"

/*! Number of on centre off surround connection parameters. */
#define NUM_ON_CENTRE_OFF_SURROUND_PARAM 10

/*! Parmeters for on centre off surround connection. */
#define ON_CENTRE_OFF_SURROUND_PARAM {{"Outer width", "30"}, {"Outer length", "30"}, {"Inner width", "10"}, {"Inner length", "10"}, {"Overlap", "0"}, {"Rotate?", "0"}, {"Normal weight distribution?", "1"}, {"Weight range", "0.2"}, {"Excitation weight", "0.8"}, {"Inhibition weight", "-0.8"}}



/*! SIMNOS COMPONENT TYPE.
	A connection type that abstracts sensor data into a component from 
	SIMNOS/CRONOS, such as an arm or leg. This type of connection is 
	topographic, but only connects to a selection of the rows of neurons 
	in the device layer. */
#define SIMNOS_COMPONENT_VALUE 5

/*! Description used for SIMNOS component connections. */
#define SIMNOS_COMPONENT_DESC "SIMNOS Component Connection"

/*! Number of SIMNOS component connection parameters. */
#define NUM_SIMNOS_COMPONENT_PARAM 2

/*! Parmeters for SIMNOS component connection. */
#define SIMNOS_COMPONENT_PARAM {{"Average weight", "0.5"}, {"Weight range", "0.5"}}



/*! TEMPORARY VIRTUAL TYPE.
	A connection between two neuron groups that is used for simulation purposes only
	to ensure that every neuron group has at least one input to drive it. No connections
	are added to the connection database for this type of connection. temporary virtual connections
	are created when the simulation manager is initialised. Their purpose is to ensure
	synchronization. */
#define TEMP_VIRTUAL_VALUE 6

/*! Description used for temporary virtual connections. */
#define TEMP_VIRTUAL_DESC "Temporary Virtual"

/*! Number of temporary virtual connection parameters. */
#define NUM_TEMP_VIRTUAL_PARAM 0

/*! Parmeters for temporary virtual connection. */
#define TEMP_VIRTUAL_PARAM



/*! TOPOGRAPHIC TYPE.
	Creates connections that are topographically mapped between 
	two layers of the same size.*/
#define TOPOGRAPHIC_VALUE 7

/*! Description used for topographic connections. */
#define TOPOGRAPHIC_DESC "Topographic"

/*! Number of topographic connection parameters. */
#define NUM_TOPOGRAPHIC_PARAM 4

/*! Parmeters for topographic connection. */
#define TOPOGRAPHIC_PARAM {{"Average weight", "0.5"}, {"Weight range", "0.5"}, {"Overlap", "0.0"}, {"Rotate", "0.0"}}



/*! UNSTRUCTURED TYPE.
	Creates a set of random unstructured connections between two layers.
	Parameters are used to set how dense the connections are. */
#define UNSTRUCTURED_VALUE 8

/*! Description used for unstructured connections. */
#define UNSTRUCTURED_DESC "Unstructured"

/*! Number of unstructured connection parameters. */
#define NUM_UNSTRUCTURED_PARAM 3

/*! Parmeters for unstructured connection. */
#define UNSTRUCTURED_PARAM {{"Connection density", "0.5"}, {"Average weight", "0.5"}, {"Weight range", "0.5"}}


/*! UNSTRUCTURED EXCITATORY/ INHIBITORY INTER
	Unstructured connections between two layers in which each neuron either makes excitatory
	or inhibitory connections. */
#define UNSTRUCTURED_EX_INHIB_INTER_VALUE 9

/*! Description used for simple cortex connections. */
#define UNSTRUCTURED_EX_INHIB_INTER_DESC "Unstructured excitatory/ inhibitory (inter)"

/*! Number of simple cortex connection parameters. */
#define NUM_UNSTRUCTURED_EX_INHIB_INTER_PARAM 7

/*! Parmeters for simple cortex connection. */
#define UNSTRUCTURED_EX_INHIB_INTER_PARAM {{"Excitatory percentage", "80.0"}, {"Excitation weight", "0.111"}, {"Excitation weight range", "0.0"}, {"Inhibition weight", "-1.0"}, {"Inhibition weight range", "0.0"}, {"Excitation connection prob", "0.02"}, {"Inhibition connection prob", "0.02"}}


/*! VIRTUAL TYPE.
	A connection between two neuron groups that is used for simulation purposes only
	to ensure that every neuron group has at least one input to drive it. No connections
	are added to the connection database for this type of connection. This virtual connection
	is created or deleted by the user. Temporary virtual connections (see next) are automatically
	created to make the simulation run.*/
#define VIRTUAL_VALUE 10

/*! Description used for virtual connections. */
#define VIRTUAL_DESC "Virtual"

/*! Number of virtual connection parameters. */
#define NUM_VIRTUAL_PARAM 0

/*! Parmeters for virtual connection. */
#define VIRTUAL_PARAM 



/*! UNDEFINED TYPE.
	Default type when a type cannot be found. */
#define UNDEFINED_TYPE 0



//---------------------------- Connection Type ------------------------------
/*! Class that holds different connection group types, covering the different
	connection topologies that are possible. The topology defines how the 
	connections will be arranged. 
 
	Two types of connection types are defined within this file:
	
		>> Intra layer connection types apply to neurons that are connecting 
			to other neurons in the same layer.
		>> Inter layer connection types apply to neurons that are connecting 
			to other neurons in different layers.

	The specification of the type includes an identifier, description and 
	list of parameters with their default values. The methods implmenting 
	these connections can be found in "ConnectionManager.cpp"
	There are also dependent methods in "ConnectionPropertiesDialog.cpp"
	which use the type to check the size of the layers to see if they match 
	before creating the connection. Any changes to this file should be 
	reflected there as well.

	The parameters should all be doubles between -100000 and 100000. 
	This makes error checkeing easier, but can be changed if necessary. 
	FIXME SHOULD REALLY ADD A THIRD TYPE CALLED 'OTHER' THAT CAN BE APPLIED
	WITHIN AND BETWEEN LAYERS OR BOTH.*/
//----------------------------------------------------------------------------

class ConnectionType {

	public:
		//=========================== VARIABLES =======================================
		/* Variables holding the number of types and number of inter/intra types.
			It seems cleaner for other classes to access these as variables than as 
			definitions.*/
		/*! Number of types of connection.*/
		static const int NumberTypes= NUM_CONNECTION_TYPES;

		/*! Number of types of connection within a single layer.*/
		static const int NumberIntraTypes = NUMBER_INTRA_TYPES;

		/*! Number of types of connection betwen two layers.*/
		static const int NumberInterTypes = NUMBER_INTER_TYPES;


		//Intra layer types
		/*! Simple cortex connection. */
		static const unsigned short SimpleCortex = SIMPLE_CORTEX_VALUE;

		/*! Unstructured excitatory/ inhibitory connection. */
		static const unsigned short UnstructuredExInhibIntra = UNSTRUCTURED_EX_INHIB_INTRA_VALUE;

		//Inter layer types
		/*! On centre off surround connection.*/
		static const unsigned short OnCentreOffSurround = ON_CENTRE_OFF_SURROUND_VALUE;

		/*! Off centre on surround connection.*/
		//FIXME NOT IMPLEMENTED
		static const unsigned short OffCentreOnSurround = OFF_CENTRE_ON_SURROUND_VALUE;

		/*! Unstructured connection.*/
		static const unsigned short Unstructured = UNSTRUCTURED_VALUE;

		/*! Unstructured excitatory/ inhibitory connection.*/
		static const unsigned short UnstructuredExInhibInter = UNSTRUCTURED_EX_INHIB_INTER_VALUE;

		/*! Topographic connection.*/
		static const unsigned short Topographic = TOPOGRAPHIC_VALUE;

		/*! SIMNOS component connection.*/
		static const unsigned short SIMNOSComponent = SIMNOS_COMPONENT_VALUE;

		/*! Virtual connection.*/
		static const unsigned short Virtual = VIRTUAL_VALUE;

		/*! Temporary virtual connection.*/
		static const unsigned short TempVirtual = TEMP_VIRTUAL_VALUE;
		
		
		//============================ METHODS ========================================
		static unsigned short* getAllTypes();
		static char* getDescription(unsigned short);
		static unsigned short* getInterLayerTypes();
		static unsigned short* getIntraLayerTypes();
		static void getParameters(map<string, string>&, unsigned short);
		static unsigned short getType(string);
                static unsigned short getType(QString);
		static void printParameters();

};


#endif//CONNECTIONTYPE_H
