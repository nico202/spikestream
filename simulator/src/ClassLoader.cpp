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

//SpikeStream includes
#include "ClassLoader.h"
#include "Debug.h"
#include "Utilities.h"
#include "SpikeStreamSimulation.h"

//Other includes
#include <dlfcn.h>
#include <mysql++.h>
#include <iostream>
using namespace std;
using namespace mysqlpp;


/*! Exception thrown when a neuron class library or its parameter table cannot be found. */
class NeuronTypeNotFoundException : public exception{
	virtual const char* what() const throw(){
		return "Neuron type not found.";
	}
} neuronTypeNotFoundException;


/*! Exception thrown when a synapse class library or its parameter table cannot be found. */
class SynapseTypeNotFoundException : public exception{
	virtual const char* what() const throw(){
		return "Synapse type not found.";
	}
} synapseTypeNotFoundException;


/*! Exception thrown when a synapse class library or its parameter table cannot be found. */
class SpikeStreamRootNotFoundException : public exception{
	virtual const char* what() const throw(){
		return "Environment variable SPIKESTREAM_ROOT is not defined.";
	}
} spikestreamRootNotFoundException;


/*! Constructor.*/
ClassLoader::ClassLoader(DBInterface *netDBInter){
	//Store reference to database interface
	networkDBInterface = netDBInter;

	//Load up the classes from libraries so they can be created easily on request
	loadNeuronClasses();
	loadSynapseClasses();
}


/*! Destructor.*/
ClassLoader::~ ClassLoader(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING CLASS LOADER"<<endl;
	#endif//MEMORY_DEBUG		
}


//-----------------------------------------------------------------------
//------------------------ PUBLIC METHODS -------------------------------
//-----------------------------------------------------------------------

/*! Returns the name of the parameter table for a given connection group.*/
string ClassLoader::getConnGrpParameterTableName(unsigned int connGrpID){
	if(synapseParameterTableMap.count(connGrpID)){
		#ifdef CLASSLOADER_DEBUG
			cout<<"ClassLoader: Found synapse parameter table "<<synapseParameterTableMap[connGrpID]<<endl;
		#endif//CLASSLOADER_DEBUG
		return synapseParameterTableMap[connGrpID];
	}
	SpikeStreamSimulation::systemError("ClassLoader: CANNOT FIND CONNECTION GRP ID IN PARAMETER TABLE MAP", connGrpID);
	throw synapseTypeNotFoundException;
}


/*! Returns the parameter table for a given neuron type. */
string ClassLoader::getNeuronParameterTableName(unsigned short neuronType){
	if(neuronParameterTableMap.count(neuronType)){
		#ifdef CLASSLOADER_DEBUG
			cout<<"ClassLoader: Found parameter table "<<neuronParameterTableMap[neuronType]<<endl;
		#endif//CLASSLOADER_DEBUG
		return neuronParameterTableMap[neuronType];
	}
	SpikeStreamSimulation::systemError("ClassLoader: CANNOT FIND NEURON TYPE IN PARAMETER TABLE MAP", neuronType);
	throw neuronTypeNotFoundException;
}


/*! Creates a new neuron of the specified type. */
Neuron* ClassLoader::getNewNeuron(unsigned short neuronType){
	if(neuronFunctionMap.count(neuronType)){
		Neuron* tempNeuron = neuronFunctionMap[neuronType]();
		#ifdef CLASSLOADER_DEBUG
			cout<<"ClassLoader: Getting new neuron of type "<<(*tempNeuron->getDescription())<<endl;
		#endif//CLASSLOADER_DEBUG
		return tempNeuron;
	}
	else{
		SpikeStreamSimulation::systemError("ClassLoader: CANNOT FIND OR RESOLVE CLASS FOR NEURON TYPE: ", neuronType);
		throw neuronTypeNotFoundException;
	}
}


/*! Creates a new synapse of the specified type. */
Synapse* ClassLoader::getNewSynapse(unsigned short synapseType){
	if(synapseFunctionMap.count(synapseType)){
		Synapse* tempSynapse = synapseFunctionMap[synapseType]();
		#ifdef CLASSLOADER_DEBUG
			cout<<"ClassLoader: Getting new synapse of type "<<(*tempSynapse->getDescription())<<endl;
		#endif//CLASSLOADER_DEBUG
		return tempSynapse;
	}
	else{
		SpikeStreamSimulation::systemError("ClassLoader: CANNOT FIND OR RESOLVE CLASS FOR SYNAPSE TYPE: ", synapseType);
		throw synapseTypeNotFoundException;
	}
}


//-----------------------------------------------------------------------
//------------------------ PRIVATE METHODS ------------------------------
//-----------------------------------------------------------------------

/*! Loads all the neuron classes from libraries and stores function pointers
	to the method that creates them so that they can be created easily
	on demand. */
void ClassLoader::loadNeuronClasses(){
	try{
		Query query= networkDBInterface->getQuery();
		query.reset();
		query<<"SELECT TypeID, ParameterTableName, ClassLibrary FROM NeuronTypes";
                StoreQueryResult neurTypesResult = query.store();
		//Work through result and store a function pointer and the parameter table name for each neuron type
                for(StoreQueryResult::iterator iter = neurTypesResult.begin(); iter != neurTypesResult.end(); ++iter){
			Row neurTypeRow(*iter);
			unsigned int neuronType = Utilities::getUShort((std::string) neurTypeRow["TypeID"]);
	
			//Check for duplicates
			if(neuronFunctionMap.count(neuronType)){
				SpikeStreamSimulation::systemError("ClassLoader: NEURON TYPE HAS ALREADY BEEN LOADED!", neuronType);
				return;
			}
	
			//Get the path to the library
			char* neurLibPath_char = getenv("SPIKESTREAM_ROOT");
			string neuronLibraryPath;
			if(neurLibPath_char != NULL){
				neuronLibraryPath = neurLibPath_char;
			}
			else{
				throw spikestreamRootNotFoundException;
			}
			neuronLibraryPath += "/lib/";
			neuronLibraryPath += (std::string)neurTypeRow["ClassLibrary"];
			#ifdef CLASSLOADER_DEBUG
				cout<<"ClassLoader: Neuron library path = "<<neuronLibraryPath<<endl;
			#endif//CLASSLOADER_DEBUG
	
			//Try to open the library and get the creating neuron function
			void *hndl = dlopen(neuronLibraryPath.data() , RTLD_NOW);
			if(hndl == NULL){
				SpikeStreamSimulation::systemError(dlerror());
				return;
			}
			CreateNeuronFunctionType createNeuronFunction = (CreateNeuronFunctionType) dlsym(hndl, "getClass");
			if ( createNeuronFunction ) {
	
				//Output neuron description in debug mode.
				#ifdef CLASSLOADER_DEBUG
					Neuron* tempNeuron = createNeuronFunction();
					cout<<"ClassLoader: Loading neuron of type "<<(*tempNeuron->getDescription())<<endl;
				#endif//CLASSLOADER_DEBUG
	
				//Store function in map
				neuronFunctionMap[neuronType] = createNeuronFunction;
			}
			else{
				SpikeStreamSimulation::systemError("ClassLoader: CANNOT FIND OR RESOLVE CLASS WHEN LOADING ALL NEURON CLASSES");
				return;
			}
	
			//Store details about parameter table
			neuronParameterTableMap[neuronType] = (std::string) neurTypeRow["ParameterTableName"];
			#ifdef CLASSLOADER_DEBUG
				cout<<"ClassLoader: Neuron parameter table name is "<<neuronParameterTableMap[neuronType]<<endl;
			#endif//CLASSLOADER_DEBUG
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		ostringstream errorStrStream;
		errorStrStream<<"Bad query when loading neuron classes: \""<<er.what()<<"\"";
		SpikeStreamSimulation::systemError(errorStrStream.str());
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown loading neuron classes: \""<<er.what()<<"\"";
		SpikeStreamSimulation::systemError(errorStrStream.str());
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown loading neuron classes: \""<<er.what()<<"\"";
		SpikeStreamSimulation::systemError(errorStrStream.str());
	}
}


/*! Loads all the synapse classes from libraries and stores function pointers
	to the method that creates them so that they can be created easily
	on demand. */
void ClassLoader::loadSynapseClasses(){
	try{
		Query query= networkDBInterface->getQuery();
		query.reset();
		query<<"SELECT TypeID, ParameterTableName, ClassLibrary FROM SynapseTypes";
                StoreQueryResult synTypesResult = query.store();
	
		//Work through result and store a function pointer and the parameter table name for each neuron type
                for(StoreQueryResult::iterator iter = synTypesResult.begin(); iter != synTypesResult.end(); ++iter){
			Row synTypeRow(*iter);
			unsigned int synapseType = Utilities::getUShort((std::string) synTypeRow["TypeID"]);
	
			//Check for duplicates
			if(synapseFunctionMap.count(synapseType)){
				SpikeStreamSimulation::systemError("ClassLoader: SYNAPSE TYPE HAS ALREADY BEEN LOADED!", synapseType);
				return;
			}
	
			//Get the path to the library
			string synapseLibraryPath = getenv("SPIKESTREAM_ROOT");
			synapseLibraryPath += "/lib/";
			synapseLibraryPath += (std::string)synTypeRow["ClassLibrary"];
			#ifdef CLASSLOADER_DEBUG
				cout<<"ClassLoader: Synapse library path = "<<synapseLibraryPath<<endl;
			#endif//CLASSLOADER_DEBUG
	
			//Try to open the library and get the creating neuron function
			void *hndl = dlopen(synapseLibraryPath.data() , RTLD_NOW);
			if(hndl == NULL){
				SpikeStreamSimulation::systemError(dlerror());
				return;
			}
			CreateSynapseFunctionType createSynapseFunction = (CreateSynapseFunctionType) dlsym(hndl, "getClass");
			if ( createSynapseFunction ) {
	
				//Output synapse description in debug mode
				#ifdef CLASSLOADER_DEBUG
					Synapse* tempSynapse = createSynapseFunction();
					cout<<"ClassLoader: Loading synapse of type "<<(*tempSynapse->getDescription())<<endl;
				#endif//CLASSLOADER_DEBUG
	
				//Store function in map
				synapseFunctionMap[synapseType] = createSynapseFunction;
			}
			else{
				SpikeStreamSimulation::systemError("ClassLoader: CANNOT FIND OR RESOLVE CLASS WHEN LOADING ALL SYNAPSE CLASSES");
				return;
			}
	
			/*Store details about parameter table. Need the name of the parameter table for a 
				particular connection group, so query each parameter table for the connection
				groups and store the name of the parameter table for each one */
			query.reset();
			query<<"SELECT ConnGrpID FROM "<<(std::string) synTypeRow["ParameterTableName"];
                        StoreQueryResult connGrpIDRes = query.store();
                        for(StoreQueryResult::iterator cGrpIter = connGrpIDRes.begin(); cGrpIter != connGrpIDRes.end(); ++cGrpIter){
				Row connGrpIDRow(*cGrpIter);
				unsigned int tempConnGrpID = Utilities::getUInt((std::string)connGrpIDRow["ConnGrpID"]);
				synapseParameterTableMap[tempConnGrpID] = (std::string) synTypeRow["ParameterTableName"];
				#ifdef CLASSLOADER_DEBUG
					cout<<"ClassLoader: Synapse parameter table name for ConnGrpID "<<tempConnGrpID<<" = "<<synapseParameterTableMap[tempConnGrpID]<<endl;
				#endif//CLASSLOADER_DEBUG
			}
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		ostringstream errorStrStream;
		errorStrStream<<"Bad query when loading synapse classes: \""<<er.what()<<"\"";
		SpikeStreamSimulation::systemError(errorStrStream.str());
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown loading synapse classes: \""<<er.what()<<"\"";
		SpikeStreamSimulation::systemError(errorStrStream.str());
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown loading synapse classes: \""<<er.what()<<"\"";
		SpikeStreamSimulation::systemError(errorStrStream.str());
	}
}


