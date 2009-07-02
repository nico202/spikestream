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
#include "NeuronTaskHolder.h"
#include "Debug.h"
#include "GlobalVariables.h"
#include "PVMMessages.h"
#include "SpikeStreamSimulation.h"

//Other includes
#include "pvm3.h"
#include <iostream>
using namespace std;


/*! Constructor. */
NeuronTaskHolder::NeuronTaskHolder(int myTID, unsigned int maxNumberOfNeurons){
	//Store this task id for error messages
	thisTaskID = myTID;

	//Allocate memory for the vector to improve performance
	firingNeuronVector.reserve(maxNumberOfNeurons);

	//Initialise variables
	numberOfTasks = 0;
}


/*! Destructor. */
NeuronTaskHolder::~NeuronTaskHolder(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING NEURONTASKHOLDER"<<endl;
	#endif//MEMORY_DEBUG

	if(numberOfTasks > 0)
		delete [] destinationTaskIDs;
}


//----------------------------------------------------------------------------
//--------------------------- PUBLIC METHODS ---------------------------------
//----------------------------------------------------------------------------

/*! Adds a receiving task to the task array.
	Done in this clunky way because the broadcast message function in pvm 
	needs an integer array, not a vector, as one of its arguments. */
void NeuronTaskHolder::addReceivingTask(int newTaskID){
	// Store a reference to the old array
	int *oldArray = destinationTaskIDs;
	
	//Create a new array to hold taskIDs
	destinationTaskIDs = new int[numberOfTasks + 1];
	
	//Copy old array into new array
	for(int i=0; i<numberOfTasks; i++)
		destinationTaskIDs[i] = oldArray[i];
		
	//Add new task to array
	destinationTaskIDs[numberOfTasks] = newTaskID;
	
	//Remove the old array and increase the number of tasks.
	if(numberOfTasks > 0)
		delete [] oldArray;
	++numberOfTasks;
}


/*! Removes a receiving task from the task holder. */
void NeuronTaskHolder::removeReceivingTask(int taskID){
	//Check that task ID is in receivingTaskIDs
	bool taskIDFound = false;
	for(int i=0; i<numberOfTasks; ++i)
		if(destinationTaskIDs[i] == taskID)
			taskIDFound = true;
	
	if(!taskIDFound){
		cerr<<"NeuronTaskHolder: TRYING TO REMOVE RECEIVING TASK. TASK ID CANNOT BE FOUND. TID: "<<taskID<<endl;
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


/*! Sends the list of firing neurons to the receiving tasks. */
bool NeuronTaskHolder::sendFiringNeuronMessages(){
	unsigned int numberOfNeurons = firingNeuronVector.size();

	//Keep track of the total number of neurons in statistics mode
	#ifdef RECORD_STATISTICS
		SpikeStreamSimulation::statistics.neuronFireTotal += numberOfNeurons;
	#endif//RECORD_STATISTICS

	//Reset vector and return if there are no recipients of messages
	if(numberOfTasks == 0){
		firingNeuronVector.clear();
		return true;
	}

	//Initialise the send buffer
	int info = pvm_initsend( PvmDataDefault);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"NeuronTaskHolder: ERROR INITIALISING BUFFER; TASK ID = "<<thisTaskID<<endl;
			return false;
		}
	#endif//PVM_DEBUG
	 
	//Add timestep to message
	unsigned int messageTimeStep = SpikeStreamSimulation::simulationClock->getTimeStep(); 
	info = pvm_pkuint(&messageTimeStep, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"NeuronTaskHolder: ERROR PACKING TIME STEP INTO MESSAGE; TASK ID = "<<thisTaskID<<endl;
			return false;
		}
	#endif//PVM_DEBUG

	 //Add number specifying number of neurons that will be in message
	info = pvm_pkuint(&numberOfNeurons, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"NeuronTaskHolder: ERROR PACKING NEURON COUNT INTO MESSAGE; TASK ID = "<<thisTaskID<<endl;
			return false;
		}
	#endif//PVM_DEBUG

	//Add the firing neuron ids to the message
	for(vector<unsigned int>::iterator iter = firingNeuronVector.begin(); iter != firingNeuronVector.end(); ++iter){
		info = pvm_pkuint(&(*iter), 1, 1);
		#ifdef PVM_DEBUG
			if(info < 0){
				cerr<<"NeuronTaskHolder: ERROR PACKING NEURON IDS INTO MESSAGE; TASK ID = "<<thisTaskID<<endl;
				return false;
			}
		#endif//PVM_DEBUG
	}
	
	//Output debugging information if required
	#ifdef NEURON_TASK_DEBUG
		//Print destinations for the messages
		cout<<"NeuronTaskHolder: Destination task IDS (number of tasks = "<<numberOfTasks<<"): ";
		for(int i=0; i<numberOfTasks; i++)
			cout<<destinationTaskIDs[i]<<", ";
		cout<<endl;

		//Print contents of the messages.
		cout<<"Timestep = "<<SpikeStreamSimulation::simulationClock->getTimeStep()<<"; Number of neurons = "<<numberOfNeurons<<endl;
		for(vector<unsigned int>::iterator iter = firingNeuronVector.begin(); iter != firingNeuronVector.end(); ++iter)
			cout<<"NEURON ID = "<<(*iter)<<", ";
		cout<<endl;
	#endif//NEURON_TASK_DEBUG


	/* Send message to other tasks
		Might be more than 1 task ID Use multicast to send the message to all of the task IDs */
	info = pvm_mcast( destinationTaskIDs, numberOfTasks, FIRING_NEURON_LIST_MSG);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"NeuronTaskHolder: ERROR SENDING MESSAGE; TASK ID = "<<thisTaskID<<endl;
			return false;
		}
	#endif//PVM_DEBUG
	
	// Reset firingNeuronVector
	firingNeuronVector.clear();
	
	//If we have reached this point everything should be ok.
	return true;
}




