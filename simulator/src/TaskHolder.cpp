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
#include "TaskHolder.h"
#include "Debug.h"
#include "PVMMessages.h"
#include "SimulationClock.h"
#include "SpikeStreamSimulation.h"

//Other includes
#include "pvm3.h"
#include <iostream>
using namespace std;


/*! Constructor. */
TaskHolder::TaskHolder(int thisTID, int destTID){
	//Store this and destination task ids
	thisTaskID = thisTID;
	primaryDestinationID = destTID;

	//Currently there is only 1 destination task id
	numberOfTasks = 1;
	
	/* Set up array to hold destination tasks. 
		This may increase if monitoring or archiving is needed. */
	destinationTaskIDs = new int[1];
	destinationTaskIDs[0] = primaryDestinationID;

	// Initialise the current timeCounter
	bufferCounter = 0;

	//Initialise the message spike count array
	for(int i=0; i<NUMBER_OF_DELAY_VALUES; ++i)
		messageSpikeCount[i] = 0;
}


/*! Destructor. */
TaskHolder::~TaskHolder(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING TASK HOLDER. TID: "<<thisTaskID<<endl;
	#endif//MEMORY_DEBUG	

	//Delete dynamic arrays
	if(numberOfTasks > 0)
		delete [] destinationTaskIDs;
}


//-----------------------------------------------------------------------------
//----------------------------- PUBLIC METHODS --------------------------------
//-----------------------------------------------------------------------------

/*! Adds a receiving task to the task array.
	Done in this clunky way because the broadcast message function in pvm needs an
	integer array, not a vector, as one of its arguments. */
void TaskHolder::addReceivingTask(int newTaskID){
	//First filter out accidental duplicates
	for	(int i=0; i<numberOfTasks; ++i){
		if (destinationTaskIDs[i] == newTaskID)
			return;
	}

	// Store a reference to the old array
	int *oldArray = destinationTaskIDs;
	
	//Create a new array to hold taskIDs
	destinationTaskIDs = new int[numberOfTasks + 1];
	
	//Copy old array into new array
	for(int i=0; i<numberOfTasks; i++)
		destinationTaskIDs[i] = oldArray[i];
		
	//Add new task to array
	destinationTaskIDs[numberOfTasks] = newTaskID;
	
	//Remove the old array 
	if(numberOfTasks > 0)
		delete [] oldArray;

	//Increase the number of tasks.
	++numberOfTasks;
}


/*! Debug method that prints out the contents of the spike message buffer. */
void TaskHolder::printBuffers(){
	cout<<"TaskHolder for task "<<thisTaskID<<"; Buffer counter = "<<bufferCounter<<"; Printing buffers.............."<<endl;
	for(int i=0; i<NUMBER_OF_DELAY_VALUES; ++i){
		if(messageSpikeCount[i] > 0){// Only print buffers that have data in them
			cout<<"Buffer "<<i<<": Size: "<<messageSpikeCount[i]<<"; "<<endl;
			for(vector<ConnectionHolder*>::iterator iter = spikeMessageBuffer[i].begin(); iter != spikeMessageBuffer[i].end(); ++iter){
			cout<<"\tConnection Holder. Delay="<<(*iter)->delay<<"; Size="<<(*iter)->numConnIDs;
				/* Now print contents of each connection id. 
					Remember that each connection id spans two integers */
				for(int j=0; j<(*iter)->numConnIDs * 2; j += 2)
					cout<<" from "<<(*iter)->connIDArray[j]<<" to "<<(*iter)->connIDArray[j+1]<<";";
				cout<<endl;
			}
		}
	}
}


/*! Removes a receiving task from the task holder. */
void TaskHolder::removeReceivingTask(int taskID){
	/* This method should only be called to remove additional tasks. 
		Should be at least one task left */
	if(taskID == primaryDestinationID){
		cerr<<"TaskHolder.cpp: CANNOT REMOVE PRIMARY TASK. TRYING TO REMOVE: "<<taskID<<endl;
		return;
	}
	
	//Check that task ID is in receivingTaskIDs
	bool taskIDFound = false;
	for(int i=0; i<numberOfTasks; ++i)
		if(destinationTaskIDs[i] == taskID)
			taskIDFound = true;
	
	if(!taskIDFound){
		cerr<<"TaskHolder: TRYING TO REMOVE RECEIVING TASK. TASK ID CANNOT BE FOUND. TID: "<<thisTaskID<<endl;
		return;
	}

	// Store a reference to the old array
	int *oldArray = destinationTaskIDs;
	
	//Create a new array to hold taskIDs
	destinationTaskIDs = new int[numberOfTasks - 1];
	
	//Copy old array into new array
	int counter = 0;
	for(int i=0; i<numberOfTasks; ++i)
		if(destinationTaskIDs[i] != taskID){
			destinationTaskIDs[counter] = oldArray[i];
			++counter;
		}
	
	//Remove the old array and decrease the number of tasks.
	delete [] oldArray;
	--numberOfTasks;
}


/*! Sends spike messages to other tasks and this task. */
bool TaskHolder::sendSpikeMessages(){
	//Initialise the send buffer
	int info = pvm_initsend( PvmDataDefault );
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"TaskHolder: ERROR INITIALISING SEND BUFFER; TASK ID = "<<thisTaskID<<endl;
			return false;
		}
	#endif//PVM_DEBUG


	 /* Work through spikeVectors 
	 	for each connection holder pack the array using the size.
		In non-debug mode run as quickly as possible without checking for error messages.*/

	//Add timestep to message
	unsigned int messageTimeStep = SpikeStreamSimulation::simulationClock->getTimeStep(); 
	info = pvm_pkuint(&messageTimeStep, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"TaskHolder: ERROR PACKING TIME STEP INTO MESSAGE; TASK ID = "<<thisTaskID<<endl;
			return false;
		}
	#endif//PVM_DEBUG


	//Add number specifying number of spikes that will be in message
	//if(messageTimeStep > 200)
	//	cout<<"TIME: "<<messageTimeStep<<" MESSAGE SPIKE COUNT: "<<messageSpikeCount[bufferCounter]<<endl;
	info = pvm_pkuint(&messageSpikeCount[bufferCounter], 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"TaskHolder: ERROR PACKING SPIKE COUNT INTO MESSAGE; TASK ID = "<<thisTaskID<<endl;
			return false;
		}
	#endif//PVM_DEBUG

	//Add up the number of spikes being sent
	#ifdef RECORD_STATISTICS
		SpikeStreamSimulation::statistics.spikeTotal += messageSpikeCount[bufferCounter];
	#endif//RECORD_STATISTICS

	//Pack the spikes into the message
	for(vector<ConnectionHolder*>::iterator iter = spikeMessageBuffer[bufferCounter].begin(); iter != spikeMessageBuffer[bufferCounter].end(); ++iter){
		info = pvm_pkuint((unsigned int*)(*iter)->connIDArray, (*iter)->numConnIDs, 1);//Pack both shorts as an int otherwise pvm treats shorts as ints
		#ifdef PVM_DEBUG
			if(info < 0){
				cerr<<"TaskHolder: ERROR PACKING SPIKES INTO MESSAGE; TASK ID = "<<thisTaskID<<endl;
				return false;
			}
		#endif//PVM_DEBUG
	}


	/* Send message to other tasks. 
		Might be more than 1 task ID Use multicast to send the message to all of the other task IDs */
	//Output debugging information if required
	#ifdef TASK_DEBUG
		cout<<"TaskHolder: Sending message to other tasks Destination task IDS (number of tasks = "<<numberOfTasks<<"): "<<endl;
		for(int i=0; i<numberOfTasks; i++)
			cout<<"\t"<<destinationTaskIDs[i]<<endl;
	#endif//TASK_DEBUG

	#ifdef SPIKE_DEBUG
		cout<<"TaskHolder: Sending spike message to "<<primaryDestinationID<<"; Message time step = "<<messageTimeStep<<"; Number of spikes = "<<messageSpikeCount[bufferCounter]<<endl;
	#endif//SPIKE_DEBUG

	//Multicast the message to all tasks except this task
	info = pvm_mcast( destinationTaskIDs, numberOfTasks, SPIKE_LIST_MSG);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"TaskHolder: ERROR MULTICASTING MESSAGE; TASK ID = "<<thisTaskID<<endl;
			return false;
		}
	#endif//PVM_DEBUG


	/* Update neurons in this task
		mcast never sends messages to this task, so need to send a separate message to update the 
		neurons in this task. */
	if(primaryDestinationID == thisTaskID){
		info = pvm_send(primaryDestinationID, SPIKE_LIST_MSG);
		#ifdef PVM_DEBUG
			if(info < 0){
				cerr<<"TaskHolder: ERROR SENDING MESSAGE TO SELF; FROM TASK ID = "<<thisTaskID<<" TO TASK ID = "<<primaryDestinationID<<endl;
				return false;
			}
		#endif//PVM_DEBUG
	}

	/* Reset anything that needs to be reset about buffer */
	messageSpikeCount[bufferCounter] = 0; //Reset count of spikes in buffer
	spikeMessageBuffer[bufferCounter].clear(); //Empty vector of ConnectionHolder*s
	
	//Increase the buffer counter and make it circular
	++bufferCounter;
	bufferCounter %= NUMBER_OF_DELAY_VALUES;
	
	return true;
}


/*! Allocates memory for all the spike vectors to save incrementally increasing their size
	This maximum number is likely to be close to the number of neurons in the task since each
	of these could add a connection holder for any point in time. */
void TaskHolder::setMaxBufferSize(int maxBSize){
	maxBufferSize = maxBSize;
	for(int i=0; i<NUMBER_OF_DELAY_VALUES; i++){
		//Reserve space in the vector
		spikeMessageBuffer[i].reserve(maxBufferSize);
	}
}


