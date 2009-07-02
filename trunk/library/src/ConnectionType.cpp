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
#include "ConnectionType.h"

//Other includes
#include <iostream>
using namespace std;


//--------------------------------------------------------------------------
//------------------------ STATIC PUBLIC METHODS ---------------------------
//--------------------------------------------------------------------------

/*! Returns an array containing all of the type values. 
	NOTE Potential memory leaks here since a reference is being returned to a 
	dynamically allocated array. */
unsigned short* ConnectionType::getAllTypes(){
	unsigned short *cTypes = new unsigned short[NumberTypes];
	cTypes[0] = SimpleCortex;
	cTypes[1] = UnstructuredExInhibIntra;
	cTypes[2] = OnCentreOffSurround;
	cTypes[3] = OffCentreOnSurround;
	cTypes[4] = Unstructured;
	cTypes[5] = UnstructuredExInhibInter;
	cTypes[6] = Virtual;
	cTypes[7] = TempVirtual;
	cTypes[8] = Topographic;
	cTypes[9] = SIMNOSComponent;
	return cTypes;
}


/*! Returns a cstring description of the type. */
char* ConnectionType::getDescription(unsigned short type){
	switch(type){
		case SimpleCortex: return SIMPLE_CORTEX_DESC;
		case UnstructuredExInhibIntra: return UNSTRUCTURED_EX_INHIB_INTRA_DESC;
		case OnCentreOffSurround: return ON_CENTRE_OFF_SURROUND_DESC;
		case OffCentreOnSurround: return OFF_CENTRE_ON_SURROUND_DESC;
		case Unstructured: return UNSTRUCTURED_DESC;
		case UnstructuredExInhibInter: return UNSTRUCTURED_EX_INHIB_INTER_DESC;
		case Virtual: return VIRTUAL_DESC;
		case TempVirtual: return TEMP_VIRTUAL_DESC;
		case Topographic: return TOPOGRAPHIC_DESC;
		case SIMNOSComponent: return SIMNOS_COMPONENT_DESC;
		default: return "Connection Type error: No description found!";
	}
}


/*! Returns all of the inter layer type values.
	NOTE the calling method should delete the dynamically allocated array 
	that is returned here to avoid a memory leak. */
unsigned short* ConnectionType::getInterLayerTypes(){
	unsigned short *cTypes = new unsigned short[NumberInterTypes];
	cTypes[0] = OnCentreOffSurround;
	cTypes[1] = OffCentreOnSurround;
	cTypes[2] = Unstructured;
	cTypes[3] = UnstructuredExInhibInter;
	cTypes[4] = Virtual;
	cTypes[5] = TempVirtual;
	cTypes[6] = Topographic;
	cTypes[7] = SIMNOSComponent;
	return cTypes;
}


/*! Returns all the intra layer type values.
	NOTE Potential memory leaks here since a reference is being returned 
	to a dynamically allocated array. */
unsigned short* ConnectionType::getIntraLayerTypes(){
	unsigned short *cTypes = new unsigned short[NumberIntraTypes];
	cTypes[0] = SimpleCortex;
	cTypes[1] = UnstructuredExInhibIntra;
	return cTypes;
}


/*! Returns the parameters associated with a particular connection type. */
void ConnectionType::getParameters(map<string, string>& parameterMap, unsigned short connType){
	switch(connType){
		case SimpleCortex: {
			string tempArray [NUM_SIMPLE_CORTEX_PARAM][2] = SIMPLE_CORTEX_PARAM;
			for(int i=0; i<NUM_SIMPLE_CORTEX_PARAM; i++)
				parameterMap[tempArray[i][0]] = tempArray[i][1];
			return;
			}
		case UnstructuredExInhibIntra: {
			string tempArray [NUM_UNSTRUCTURED_EX_INHIB_INTRA_PARAM][2] = UNSTRUCTURED_EX_INHIB_INTRA_PARAM;
			for(int i=0; i<NUM_UNSTRUCTURED_EX_INHIB_INTRA_PARAM; i++)
				parameterMap[tempArray[i][0]] = tempArray[i][1];
			return;
			}
		case OnCentreOffSurround: {
			string tempArray [NUM_ON_CENTRE_OFF_SURROUND_PARAM][2] = ON_CENTRE_OFF_SURROUND_PARAM;
			for(int i=0; i<NUM_ON_CENTRE_OFF_SURROUND_PARAM; i++)
				parameterMap[tempArray[i][0]] = tempArray[i][1];
			return;
			}
		case OffCentreOnSurround: {
			string tempArray [NUM_OFF_CENTRE_ON_SURROUND_PARAM][2] = OFF_CENTRE_ON_SURROUND_PARAM;
			for(int i=0; i<NUM_OFF_CENTRE_ON_SURROUND_PARAM; i++)
				parameterMap[tempArray[i][0]] = tempArray[i][1];
			return;
			}
		case Unstructured: {
			string tempArray [NUM_UNSTRUCTURED_PARAM][2] = UNSTRUCTURED_PARAM;
			for(int i=0; i<NUM_UNSTRUCTURED_PARAM; i++)
				parameterMap[tempArray[i][0]] = tempArray[i][1];
			return;
			}
		case UnstructuredExInhibInter: {
			string tempArray [NUM_UNSTRUCTURED_EX_INHIB_INTER_PARAM][2] = UNSTRUCTURED_EX_INHIB_INTER_PARAM;
			for(int i=0; i<NUM_UNSTRUCTURED_EX_INHIB_INTER_PARAM; i++)
				parameterMap[tempArray[i][0]] = tempArray[i][1];
			return;
			}
		case Virtual: {
			return; //No virtual parameters
		}
		case TempVirtual: {
			return;//No temp virtual parameters
		}
		case Topographic: {
			string tempArray [NUM_TOPOGRAPHIC_PARAM][2] = TOPOGRAPHIC_PARAM;
			for(int i=0; i<NUM_TOPOGRAPHIC_PARAM; i++)
				parameterMap[tempArray[i][0]] = tempArray[i][1];
			return;
		}
		case SIMNOSComponent: {
			string tempArray [NUM_SIMNOS_COMPONENT_PARAM][2] = SIMNOS_COMPONENT_PARAM;
			for(int i=0; i<NUM_SIMNOS_COMPONENT_PARAM; i++)
				parameterMap[tempArray[i][0]] = tempArray[i][1];
			return;
		}
		default: cout<<"Connection Type error: No parameters found!";
	}
}


/*! Returns the type value when given the type description. */
unsigned short ConnectionType::getType(string connType){
	if(connType == SIMPLE_CORTEX_DESC)
		return SimpleCortex;
	else if(connType == UNSTRUCTURED_EX_INHIB_INTRA_DESC)
		return UnstructuredExInhibIntra;
	else if(connType == ON_CENTRE_OFF_SURROUND_DESC)
		return OnCentreOffSurround;
	else if(connType == OFF_CENTRE_ON_SURROUND_DESC)
		return OffCentreOnSurround;
	else if(connType == UNSTRUCTURED_DESC)
		return Unstructured;
	else if(connType == UNSTRUCTURED_EX_INHIB_INTER_DESC)
		return UnstructuredExInhibInter;
	else if(connType == VIRTUAL_DESC)
		return Virtual;
	else if(connType == TEMP_VIRTUAL_DESC)
		return TempVirtual;
	else if(connType == TOPOGRAPHIC_DESC)
		return Topographic;
	else if(connType == SIMNOS_COMPONENT_DESC)
		return SIMNOSComponent;
	return UNDEFINED_TYPE;
}


/*! Returns the type value when given the type description. */
unsigned short ConnectionType::getType(QString connType){
    return getType(connType.toStdString());
}


/*! Debugging method to check that parameter definitions are working ok. */
void ConnectionType::printParameters(){
	{//Print simple cortex parameters. Use brackets so that I can use the same name for the array each time
	cout<<SIMPLE_CORTEX_DESC<<endl;
	string tempArray[NUM_SIMPLE_CORTEX_PARAM][2] = SIMPLE_CORTEX_PARAM;
	for(int i=0; i<NUM_SIMPLE_CORTEX_PARAM; i++)
		cout<<"Parameter: "<<tempArray[i][0]<<"; default value: "<<tempArray[i][1]<<endl;
	cout<<endl;
	}
	{//Print unstructured excitatory inhibitory parameters. Use brackets so that I can use the same name for the array each time
	cout<<UNSTRUCTURED_EX_INHIB_INTRA_DESC<<endl;
	string tempArray[NUM_UNSTRUCTURED_EX_INHIB_INTRA_PARAM][2] = UNSTRUCTURED_EX_INHIB_INTRA_PARAM;
	for(int i=0; i<NUM_UNSTRUCTURED_EX_INHIB_INTRA_PARAM; i++)
		cout<<"Parameter: "<<tempArray[i][0]<<"; default value: "<<tempArray[i][1]<<endl;
	cout<<endl;
	}
	{//Print on centre off surround parameters
	cout<<ON_CENTRE_OFF_SURROUND_DESC<<endl;
	string tempArray [NUM_ON_CENTRE_OFF_SURROUND_PARAM][2] = ON_CENTRE_OFF_SURROUND_PARAM;
	for(int i=0; i<NUM_ON_CENTRE_OFF_SURROUND_PARAM; i++)
		cout<<"Parameter: "<<tempArray[i][0]<<"; default value: "<<tempArray[i][1]<<endl;
	cout<<endl;
	}
	{//Print off centre on surround parameters
	cout<<OFF_CENTRE_ON_SURROUND_DESC<<endl;
	string tempArray [NUM_OFF_CENTRE_ON_SURROUND_PARAM][2] = OFF_CENTRE_ON_SURROUND_PARAM;
	for(int i=0; i<NUM_OFF_CENTRE_ON_SURROUND_PARAM; i++)
		cout<<"Parameter: "<<tempArray[i][0]<<"; default value: "<<tempArray[i][1]<<endl;
	cout<<endl;
	}
	{//Print unstructured parameters
	cout<<UNSTRUCTURED_DESC<<endl;
	string tempArray [NUM_UNSTRUCTURED_PARAM][2] = UNSTRUCTURED_PARAM;
	for(int i=0; i<NUM_UNSTRUCTURED_PARAM; i++)
		cout<<"Parameter: "<<tempArray[i][0]<<"; default value: "<<tempArray[i][1]<<endl;
	cout<<endl;
	}
	{//Print unstructured excitatory inhibitory parameters. Use brackets so that I can use the same name for the array each time
	cout<<UNSTRUCTURED_EX_INHIB_INTER_DESC<<endl;
	string tempArray[NUM_UNSTRUCTURED_EX_INHIB_INTER_PARAM][2] = UNSTRUCTURED_EX_INHIB_INTER_PARAM;
	for(int i=0; i<NUM_UNSTRUCTURED_EX_INHIB_INTER_PARAM; i++)
		cout<<"Parameter: "<<tempArray[i][0]<<"; default value: "<<tempArray[i][1]<<endl;
	cout<<endl;
	}
	{//Print unstructured parameters
	cout<<TOPOGRAPHIC_DESC<<endl;
	string tempArray [NUM_TOPOGRAPHIC_PARAM][2] = TOPOGRAPHIC_PARAM;
	for(int i=0; i<NUM_TOPOGRAPHIC_PARAM; i++)
		cout<<"Parameter: "<<tempArray[i][0]<<"; default value: "<<tempArray[i][1]<<endl;
	cout<<endl;
	}
	{//Print unstructured parameters
	cout<<SIMNOS_COMPONENT_DESC<<endl;
	string tempArray [NUM_SIMNOS_COMPONENT_PARAM][2] = SIMNOS_COMPONENT_PARAM;
	for(int i=0; i<NUM_SIMNOS_COMPONENT_PARAM; i++)
		cout<<"Parameter: "<<tempArray[i][0]<<"; default value: "<<tempArray[i][1]<<endl;
	cout<<endl;
	}
	cout<<VIRTUAL_DESC<<endl;
	cout<<"No parameters"<<endl;
	cout<<TEMP_VIRTUAL_DESC<<endl;
	cout<<"No parameters"<<endl;
}

