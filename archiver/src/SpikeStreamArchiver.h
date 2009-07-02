/***************************************************************************
 *   SpikeStream Archiver                                                  *
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

#ifndef SPIKESTREAMARCHIVER_H
#define SPIKESTREAMARCHIVER_H

//SpikeStream includes
#include "DBInterface.h"
#include "GlobalVariables.h"

//Other includes
#include "mysql++.h"
#include <string>


/*! Holds data from a time step when messages have not yet been received
	from all neuron groups.*/
struct TimeStepHolder {
	unsigned int spikeMessageCount;
	string xmlString;
};


//------------------------- SpikeStream Archiver ---------------------------
/*! Main class for the archiver that records firing neuron or spike patterns 
	in the database. Patterns are stored as a model XML file, which records 
	the structure of the neural network, and data XML files that record the 
	firing or spike pattern of the network at each time step.
*/
//	FIXME CONVERT TO DENSE_HASH_MAP FOR SPEED 
//--------------------------------------------------------------------------*/

class SpikeStreamArchiver {

	public:
		SpikeStreamArchiver(int argc, char **argv);
		~SpikeStreamArchiver();
		static void systemError(const string &message);
		static void systemError(const char *message, int messageData1);
		static void systemError(const char *message);


	private: 
		//======================== VARIABLES ==============================
		/*! PVM task id of this process */
		int myTaskID;

		/*! PVM task id of the process that spawned this process. Static so
			that it can be used in the static systemError methods. */
		static int parentTaskID;

		/*! Reference to archive database handling class. */
		DBInterface *archiveDBInterface;

		/*! Reference to network database handling class. */
		DBInterface *neuralNetworkDBInterface;

		/*! Controls whether the main run method is running. */
		bool stop;
		
		/*! A network model is stored at the beginning of each simulation run.
			This variable keeps track of whether any network data has been stored
			for this archive. If not, then the network model is deleted
			when this task exits to avoid the build up of empty archives.*/
		bool networkDataStored;
		
		/*! Controls whether the archive records firing neurons or spikes. */
		unsigned int archiveType;	

		/*! The sum of the task ids from the neuron groups that send data
			to this class. Used to keeps track of whether all messages 
			have been received. */
		unsigned int spikeMessageTotal;

		/*! Records the link between taskIDs and neuron groups IDs
			The key is the taskID, the data is the neuronGrpID. */
		map<int, unsigned int> taskToNeuronGrpMap;

		/*! Holds the time step holders for each time step. These are
			removed when all messages have been received for the time
			step. */
		map<unsigned int, TimeStepHolder> timeStepMap;

		/*! Holds all the currently firing neurons.
			Use a map for this to eliminate duplicates */
		map<unsigned int, bool> firingNeuronMap;

		/*! Used to unpack the number of spikes from a message.
			Declare it here to save declaring it each
			time a message is received. */
		unsigned int numberOfSpikes;

		/*! Array to unpack the spikes or neuron ids into. */
		unsigned int* unpackArray;

		/*! Define integer here to extract from neuron id. */
		unsigned int unpkFromNeurID;

		/*! Keeps track of the time step in the current and previous messages. */
		unsigned int messageTimeStep;

		/*! Records when the simulation is started. This is used to link the model
			and data files for a simulation run. */
		unsigned int simulationStartTime;


		/*! Name of the archive. */
		char archiveName[MAX_DATABASE_NAME_LENGTH];


		/*! Holds the start neuron ID for each task processing neurons 
			The key is the task id, the data is the start neuron id of that neuron group. */
		map<int, unsigned int> startNeurIDTaskMap;

		/*! When the archiver has an error it enters error state, waits for an exit 
			message and then exits. This is to enable the Simulation Manager to clean up
			properly. */
		static bool errorState;


		//========================= METHODS ==================================
		/*! Declare copy constructor private so that it cannot be used inadvertently.*/
		SpikeStreamArchiver(const SpikeStreamArchiver&);

		/*! Declare assignment private so that it cannot be used inadvertently.*/
		SpikeStreamArchiver operator = (const SpikeStreamArchiver&);

		bool archiveNeuralNetwork();
		void cleanUpArchiver();
		string getString(unsigned int);
		bool loadTaskIDs();
		void processFiringNeuronList(int senderTID);
		void processSpikeList(int senderTID);
		void run();
		static bool sendMessage(int taskID, int msgtag);
		static bool sendMessage(int taskId, int msgtag, unsigned int msgInteger);
		static bool sendMessage(int taskID, int msgtag, const char* charArray);
		void startArchiving();
		void stopArchiving();
		bool storeNetworkData(unsigned int unixTime, string networkDataString);

};


#endif //SPIKESTREAMARCHIVER_H


