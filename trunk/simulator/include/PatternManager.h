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

#ifndef PATTERNMANAGER_H
#define PATTERNMANAGER_H

//SpikeStream includes
#include "DBInterface.h"
#include "Neuron.h"

//Other includes
#include <string>


//-------------------------- Pattern Manager -------------------------------
/*! Reads in patterns from the database and applies them to the neurons in 
	this task. Works using its own set of buffers to manage the delays of 
	temporally smeared patterns. These buffers can advance independently of
	the time step so that the same pattern can be applied for several steps.

	Temporal patterns are applied once and then a new pattern is applied 
	after a number of timesteps. Fixed patterns are applied every time step.
*/
/* FIXME CHANGE STRINGS FOR STRAIGHT DATA IN THE DATABASE TO SPEED UP 
	LOADING AND SAVE SPACE. */
//--------------------------------------------------------------------------

class PatternManager {

	public:
		PatternManager(DBInterface *netDBInterface, DBInterface *pattDBInterface, unsigned int neurGrpID, unsigned int patternGrpID);
		~PatternManager();
		void fireNeurons();
		void loadPatternData();
		void setNeuronArray(Neuron** neurArr, int neurArrLen);


	private:
		//===================== VARIABLES ==============================
		//References to database handling classes
		DBInterface *networkDBInterface;
		DBInterface *patternDBInterface;

		/*! Store references to query class to enable us to move
			progressively through the pattern in the database.*/
		mysqlpp::Query *patternQuery;

		/*! Need a new connection to the database to be able to use 
			ResUse for the pattern query.*/
		mysqlpp::Connection *patternConnection;

		/*! Store references to result class to enable us to move
			progressively through the pattern in the database.*/
                mysqlpp::UseQueryResult *patternResults;

		/*! The id of the neuron group that is being simulated by this task.*/
		unsigned int neuronGrpID;

		/*! Records when a pattern has been loaded.*/
		bool patternLoaded;

		/*! Current pattern type.*/
		unsigned int patternGrpID;

		/*! Current pattern type.*/
		unsigned int patternType;

		/*! Width of the neuron group.*/
		unsigned int neuronGrpWidth;

		/*! Length of the neuron group.*/
		unsigned int neuronGrpLength;

		/*! Array into which the pattern data is loaded from the database
			by extracting it from a string.*/
		int* patternArray;

		/*! The length of the pattern array.*/
		int patternArrayLength;

		/*! Reference to the neuron array in SpikeStreamSimulation.*/
		Neuron **neuronArray;

		/*! Length of the neuron array in SpikeStreamSimulation.*/		
		int numberOfNeurons;

		/*! Is the pattern rotated wrt the neuron group or not?.*/
		//FIXME NOT IMPLEMENTED
		bool rotatePattern;

		/*! Holds the neurons that need to be fired at each time point.*/
		vector<unsigned int> patternBuffer[NUMBER_OF_DELAY_VALUES];
		
		/*! The current position in the pattern buffer.*/
		unsigned int bufferCounter;


		//===================== METHODS ==============================
		/*! Declare copy constructor private so it cannot be used inadvertently. */
		PatternManager (const PatternManager&);

		/*! Declare assignment private so it cannot be used.*/
		PatternManager operator = (const PatternManager&);

		void fillPatternArray(string &patternString);
		void loadPatternGroup(unsigned int pattGrpID);
		void unloadPatternGroup();

};


#endif //PATTERNMANAGER_H


