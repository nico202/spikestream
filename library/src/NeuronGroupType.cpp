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
#include "NeuronGroupType.h"
#include <iostream>
using namespace std;


//--------------------------------------------------------------------------
//------------------------ STATIC PUBLIC METHODS ---------------------------
//--------------------------------------------------------------------------

/*! Returns a cstring description of the type. */
char* NeuronGroupType::getDescription(unsigned short type){
	switch(type){
		case RectangularLayer2D: return RECTANGULAR_LAYER_2D_DESC;
		case RectangularLayer3D: return RECTANGULAR_LAYER_3D_DESC;
		case SIMNOSComponentLayer: return SIMNOS_COMPONENT_LAYER_DESC;
		default: return "Neuron Group Type Error: No description found!";
	}
}


/*! Returns the parameters associated with a particular connection type. */
void NeuronGroupType::getParameters(map<string, string>& parameterMap, unsigned short connType){
	switch(connType){
		case RectangularLayer2D: {
			string tempArray [NUM_RECTANGULAR_LAYER_2D_PARAM][2] = RECTANGULAR_LAYER_2D_PARAM;
			for(int i=0; i<NUM_RECTANGULAR_LAYER_2D_PARAM; i++)
				parameterMap[tempArray[i][0]] = tempArray[i][1];
			return;
		}
		case RectangularLayer3D: {
			string tempArray [NUM_RECTANGULAR_LAYER_3D_PARAM][2] = RECTANGULAR_LAYER_3D_PARAM;
			for(int i=0; i<NUM_RECTANGULAR_LAYER_3D_PARAM; i++)
				parameterMap[tempArray[i][0]] = tempArray[i][1];
			return;
		}
		case SIMNOSComponentLayer: {
			string tempArray [NUM_SIMNOS_COMPONENT_LAYER_PARAM][2] = SIMNOS_COMPONENT_LAYER_PARAM;
			for(int i=0; i<NUM_SIMNOS_COMPONENT_LAYER_PARAM; i++)
				parameterMap[tempArray[i][0]] = tempArray[i][1];
			return;
		}
		default: cout<<"Connection Type error: No parameters found!";
	}
}


/*! Returns the type value when given the type description. */
unsigned short NeuronGroupType::getType(string connType){
	if(connType == RECTANGULAR_LAYER_2D_DESC)
		return RectangularLayer2D;
	else if(connType == RECTANGULAR_LAYER_3D_DESC)
		return RectangularLayer3D;
	else if(connType == SIMNOS_COMPONENT_LAYER_DESC)
		return SIMNOSComponentLayer;
	return UNDEFINED_TYPE;

}


/*! Returns the type value when given the type description. */
unsigned short NeuronGroupType::getType(QString connType){
    return getType(connType.toStdString());
}


/*! Returns an array containing all of the type values. */
unsigned short* NeuronGroupType::getTypes(){
	unsigned short *cTypes = new unsigned short[NumberTypes];
	cTypes[0] = RectangularLayer2D;
	cTypes[1] = RectangularLayer3D;
	cTypes[2] = SIMNOSComponentLayer;
	return cTypes;
}


/*! Debugging method to check that parameter definitions are working ok. */
void NeuronGroupType::printParameters(){
	{//Print Rectangular layer 2D parameters. Use brackets so that I can use the same name for the array each time
	cout<<RECTANGULAR_LAYER_2D_DESC<<endl;
	string tempArray[NUM_RECTANGULAR_LAYER_2D_PARAM][2] = RECTANGULAR_LAYER_2D_PARAM;
	for(int i=0; i<NUM_RECTANGULAR_LAYER_2D_PARAM; i++)
		cout<<"Parameter: "<<tempArray[i][0]<<"; default value: "<<tempArray[i][1]<<endl;
	cout<<endl;
	}
	{//Print Rectangular layer 3D parameters. Use brackets so that I can use the same name for the array each time
	cout<<RECTANGULAR_LAYER_3D_DESC<<endl;
	string tempArray[NUM_RECTANGULAR_LAYER_3D_PARAM][2] = RECTANGULAR_LAYER_3D_PARAM;
	for(int i=0; i<NUM_RECTANGULAR_LAYER_3D_PARAM; i++)
		cout<<"Parameter: "<<tempArray[i][0]<<"; default value: "<<tempArray[i][1]<<endl;
	cout<<endl;
	}
	{//Print SIMNOS component parameters. Use brackets so that I can use the same name for the array each time
	cout<<SIMNOS_COMPONENT_LAYER_DESC<<endl;
	string tempArray[NUM_SIMNOS_COMPONENT_LAYER_PARAM][2] = SIMNOS_COMPONENT_LAYER_PARAM;
	for(int i=0; i<NUM_SIMNOS_COMPONENT_LAYER_PARAM; i++)
		cout<<"Parameter: "<<tempArray[i][0]<<"; default value: "<<tempArray[i][1]<<endl;
	cout<<endl;
	}
}
