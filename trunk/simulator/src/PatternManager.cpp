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
#include "PatternManager.h"
#include "Debug.h"
#include "Utilities.h"
#include "PatternTypes.h"
#include "SpikeStreamSimulation.h"

//Other includes
#include <iostream>
#include <mysql++.h>
using namespace mysqlpp;
using namespace std;


/*! Constructor. */
PatternManager::PatternManager(DBInterface *netDBInter, DBInterface *pattDBInter, unsigned int neurGrpID, unsigned int pattGrpID){
	//Store references
	networkDBInterface = netDBInter;
	patternDBInterface = pattDBInter;

	//Store id of neuron group that this task is simulating
	neuronGrpID = neurGrpID;

	//Initialise variables
	bufferCounter = 0;
	patternLoaded = false;
	rotatePattern = false;
	patternGrpID = 0;

	//Create a new connection to the pattern dbInterface
	try{
		patternConnection = patternDBInterface->getNewConnection();
		if(!patternConnection){
			SpikeStreamSimulation::systemError("PatternManager: NEW CONNECTION TO DATABASE FAILED");
			return;
		}
		patternQuery = new Query(patternConnection->query());
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown getting pattern query: \""<<er.what()<<"\"";
		SpikeStreamSimulation::systemError(errorStrStream.str());
		return;
	}

	//Load up pattern
	loadPatternGroup(pattGrpID);
}


/*! Destructor. */
PatternManager::~ PatternManager(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING PATTERN MANAGER"<<endl;
	#endif//MEMORY_DEBUG

	if(patternLoaded){
		unloadPatternGroup();
	}

	//Delete the query that we have created on the heap
	delete patternQuery;

	//Close the connection used by this class
        patternConnection->disconnect();
}


//-------------------------------------------------------------------------
//-------------------------- PUBLIC METHODS -------------------------------
//-------------------------------------------------------------------------

/*! Fires neurons in the neuron array according to the pattern. */
void PatternManager::fireNeurons(){
	//Make sure that pattern is loaded
	if(!patternLoaded)
		return;
	
	//Fire all the neurons at the current position in the buffer
	for(vector<unsigned int>::iterator iter = patternBuffer[bufferCounter].begin(); iter != patternBuffer[bufferCounter].end(); ++iter){
		neuronArray[*iter]->fireNeuron();
	}

	//Advance buffer counter if appropriate
	if(patternType == TEMPORAL_PATTERN_VALUE){
		patternBuffer[bufferCounter].clear();//Empty the buffer's vector
		++bufferCounter;
		bufferCounter = bufferCounter % NUMBER_OF_DELAY_VALUES;
	}
}


/*! Loads pattern data from the database into the pattern buffer.
	This data will change the firing pattern when fireNeurons()
	is called. */
// FIXME NEED TO HANDLE ROTATION OF PATTERNS
void PatternManager::loadPatternData(){
	/* Double check that pattern has been successfully loaded
		Not necessarily an error if there is no pattern loaded since it
		may have finished and this method is still being called by neuron simulation */
	if(!patternLoaded){
		return;
	}

	/* Static patterns always insert at zero - delayValue = 0
		so need to empty buffer at this position */
	if(patternType == STATIC_PATTERN_VALUE)
		patternBuffer[0].clear();
		
	//Load pattern
	try{
                Row patternRow;
                if(patternRow = patternResults->fetch_row()){
                    string patternDataString = (std::string) patternRow["Pattern"];
                    fillPatternArray(patternDataString);

                    /* Neuron groups are loaded horizontally from bottonm to top. Patterns are loaded
                            horizontally from top to bottom. So need a bit of fiddling to map one to the other
                            Need to handle rotation of the pattern wrt the neuron group*/
                    if(rotatePattern == false){
                            for(unsigned int i=0; i<neuronGrpLength; ++i){
                                    for(unsigned int j=0; j < neuronGrpWidth; ++j){
                                            int neuronArrayIndex = j + i*neuronGrpWidth;//Location in the neuron array
                                            int patternArrayIndex = j + (neuronGrpLength - i - 1) * neuronGrpWidth;//location in the pattern array

                                            //In a static pattern, the value should be 1 or 0
                                            if(patternType == STATIC_PATTERN_VALUE){
                                                    if(patternArray[patternArrayIndex] == 1){
                                                            patternBuffer[0].push_back(neuronArrayIndex);
                                                    }
                                            }
                                            //Pattern value is the amount of delay
                                            else{
                                                    if(patternArray[patternArrayIndex] > -1){//Neuron fires, so add to patternBuffer
                                                            int insertionPosition = (bufferCounter + patternArray[patternArrayIndex] ) % NUMBER_OF_DELAY_VALUES;
                                                            patternBuffer[insertionPosition].push_back(neuronArrayIndex);
                                                    }
                                            }
                                    }
                            }
                    }
                    else{
                            SpikeStreamSimulation::systemError("PATTERN ROTATION NOT IMPLEMENTED YET.");
                    }

                    #ifdef PATTERN_DEBUG
                            cout<<"Pattern data loaded"<<endl;
                    #endif//PATTERN_DEBUG
                }
                else{
                    //No more rows to fetch, so unload pattern
                    cout<<"PatternManager: End of pattern. Stopping"<<endl;
                    unloadPatternGroup();
                }
	}
	catch (const BadQuery& er) {// Handle any query errors
		ostringstream errorStrStream;
		errorStrStream<<"Bad query when loading pattern data: \""<<er.what()<<"\"";
		SpikeStreamSimulation::systemError(errorStrStream.str());
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown loading pattern data: \""<<er.what()<<"\"";
		SpikeStreamSimulation::systemError(errorStrStream.str());
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown loading pattern data: \""<<er.what()<<"\"";
		SpikeStreamSimulation::systemError(errorStrStream.str());
	}
}


/*! Passes a reference to the array of neurons held in SpikeStreamSimulation. */
void PatternManager::setNeuronArray(Neuron **neurArr, int neurArrLen){
	neuronArray = neurArr;
	numberOfNeurons = neurArrLen;
}


//-------------------------------------------------------------------------
//--------------------------- PRIVATE METHODS -----------------------------
//-------------------------------------------------------------------------

/*! Loads the supplied patternArray with the values from the patternString.
	These are separated with commas. */
void PatternManager::fillPatternArray(string &patternString){
	int patternCounter = 0;
	string tempString = "";
	for(unsigned int stringCounter = 0; stringCounter < patternString.length(); ++stringCounter){
		if((patternString[stringCounter] == '-') || isdigit(patternString[stringCounter])){//Have found a number
			
			//Keep loading number until I reach ',' or ' '
			while(((patternString[stringCounter] == '-') || isdigit(patternString[stringCounter])) && stringCounter != patternString.length()){
				tempString += patternString[stringCounter];
				++stringCounter;
			}
			//Breaks out of while loop when char is not part of a number or at the end of the string
			
			//Convert the number to an integer and add to pattern array
			if(patternCounter >= patternArrayLength){//Will exceed pattern array length
				ostringstream errorStrStream;
				errorStrStream<<"PatternManager: ERROR FILLING PATTERN ARRAY: TOO MANY NUMBERS "<<"PatternCounter = "<<patternCounter<<" length = "<<patternArrayLength<<endl;
				SpikeStreamSimulation::systemError(errorStrStream.str());
				break;
			}

			try{
				patternArray[patternCounter] = Utilities::getInt(tempString);
			}
			catch(std::exception& er){// Catch exceptions thrown converting the string to an integer
				ostringstream errorStrStream;
				errorStrStream<<"Exception thrown extracting integer from pattern array: \""<<er.what()<<"\"";
				SpikeStreamSimulation::systemError(errorStrStream.str());
				break;
			}
			++patternCounter;
			tempString = "";
		}
		//Otherwise character is not part of a number so continue with for loop
	}

	/*Check that correct number of integers have been loaded. 
		Should have exited from the for loop with after filling the array and
		incrementing the pattern counter one more time */
	if(patternCounter != patternArrayLength){//Have not loaded enough into array
		ostringstream errorStrStream;
		errorStrStream<<"PatternManager: ERROR FILLING PATTERN ARRAY: PROBABLY TOO FEW NUMBERS "<<"PatternCounter = "<<patternCounter<<" length = "<<patternArrayLength<<endl;
		SpikeStreamSimulation::systemError(errorStrStream.str());
	}

	#ifdef PATTERN_DEBUG
		cout<<"==================================== START PATTERN ARRAY ==============================="<<endl;
		for(int i=0; i<patternArrayLength; ++i)
			cout<<"Pattern array: "<<i<<"   "<<patternArray[i]<<endl;
		cout<<"===================================== END PATTERN ARRAY ==============================="<<endl;
	#endif//PATTERN_DEBUG
}


/*! Loads up a set of patterns from the database ready to be played back
	as the simulation progresses. */
void PatternManager::loadPatternGroup(unsigned int pattGrpID){
	//Store the current pattern group id
	patternGrpID = pattGrpID;

	//Do any steps necessary to clean up previous archive
	if(patternLoaded){
		unloadPatternGroup();
	}

	/* Get the width and length of the neuron group to see if they match and if
		the pattern needs to be rotated */

	//Get information about neuron group
	try{
		Query networkQuery = networkDBInterface->getQuery();
		networkQuery.reset();
		networkQuery<<"SELECT Width, Length FROM NeuronGroups WHERE NeuronGrpID = "<<neuronGrpID;
                StoreQueryResult networkResults = networkQuery.store();
		Row networkRow(*networkResults.begin());
		neuronGrpWidth = Utilities::getUInt((std::string)networkRow["Width"]);
		neuronGrpLength = Utilities::getUInt((std::string)networkRow["Length"]);
	
		//Get information about pattern 
		Query tempPattQuery = patternDBInterface->getQuery();
		tempPattQuery.reset();
		tempPattQuery<<"SELECT Width, Length, PatternType FROM PatternDescriptions WHERE PatternGrpID = "<<patternGrpID;
                StoreQueryResult tempPattResults = tempPattQuery.store();
		Row tempPattRow(*tempPattResults.begin());
		patternType = Utilities::getUInt((std::string)tempPattRow["PatternType"]);
		unsigned int patternWidth = Utilities::getUInt((std::string)tempPattRow["Width"]);
		unsigned int patternLength = Utilities::getUInt((std::string)tempPattRow["Length"]);
	
		//Sort out the rotation of the pattern
			//FIXME ROTATION IS NOT IMPLEMENTED YET
		if(neuronGrpWidth == patternWidth && neuronGrpLength == patternLength){
			rotatePattern = false;
		} 
		else if(neuronGrpWidth == patternLength && neuronGrpLength == patternWidth){
			rotatePattern = true;
			SpikeStreamSimulation::systemError("PatternManager: ROTATION NOT IMPLEMENTED\nPATTERN DOES NOT MATCH NEURON GROUP WIDTH AND LENGTH.");
			return;
		}
		else{
			SpikeStreamSimulation::systemError("PatternManager: PATTERN DOES NOT MATCH NEURON GROUP WIDTH AND LENGTH");
			return;
		}
	
		//Set up the pattern array that will be used to hold the unpacked pattern data
		patternArrayLength = patternWidth * patternLength;
		patternArray = new int[patternArrayLength];
	
		//Set up the query and the results that will be used to stream from database
		patternQuery->reset();
		(*patternQuery)<<"SELECT PatternID, Pattern FROM PatternData WHERE PatternGrpID = "<<patternGrpID<<" ORDER BY PatternID";
                patternResults = new UseQueryResult(patternQuery->use());//Store pattern results

	}
	catch (const BadQuery& er) {// Handle any query errors
		ostringstream errorStrStream;
		errorStrStream<<"Bad query when loading pattern group: \""<<er.what()<<"\"";
		SpikeStreamSimulation::systemError(errorStrStream.str());
		return;
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown loading pattern group: \""<<er.what()<<"\"";
		SpikeStreamSimulation::systemError(errorStrStream.str());
		return;
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown loading pattern group: \""<<er.what()<<"\"";
		SpikeStreamSimulation::systemError(errorStrStream.str());
		return;
	}


	//Check to see that results are ok
	if(!(*patternResults)){
		SpikeStreamSimulation::systemError("PatternManager: SQL ERROR");
		return;
	}

	#ifdef PATTERN_DEBUG
		cout<<"Pattern description loaded."<<endl;
	#endif//PATTERN_DEBUG

	//Everything ok up to this point
	patternLoaded = true;

	//Load the first pattern
	loadPatternData();
}


/*! Unloads the pattern. */
void PatternManager::unloadPatternGroup(){
	delete patternResults;
	patternGrpID = 0;
	patternLoaded = false;

	//Reset all buffers
	for(int i=0; i<NUMBER_OF_DELAY_VALUES; ++i)
		patternBuffer[i].clear();

	//Free memory from pattern array
	delete [] patternArray;
}


