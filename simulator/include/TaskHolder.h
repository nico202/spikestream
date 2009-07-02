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

#ifndef TASKHOLDER_H
#define TASKHOLDER_H

//SpikeStream includes
#include "ConnectionHolder.h"
#include "GlobalVariables.h"

//Other includes
#include <vector>
using namespace std;

 
//---------------------------- Task Holder --------------------------------
/*! Used to send a list of spikes to a specific task or number of tasks
	Mainly created to send a list of spikes to a specific task. However it
	can	also send spike lists to several tasks to support monitoring and 
	recording functions. */
//-------------------------------------------------------------------------

class TaskHolder {

	public:
		TaskHolder(int thisTaskID, int primaryDestTaskID);
		~TaskHolder();
		void addReceivingTask(int);
		void printBuffers();
		void removeReceivingTask(int);
		bool sendSpikeMessages();
		void setMaxBufferSize(int maxSize);

		/*! Neuron class needs access to some of the private data members
			so that it can copy its connection holders into this class when
			it fires. */
		friend class Neuron;


	private:
		//=========================== VARIABLES =====================================
		/*! Keep a record of this task here for error messages.*/
		int thisTaskID;
		
		/*! The taskID of the primary destination for spike lists from this task. 
			This should be another task processing neurons.*/
		int primaryDestinationID;

		/*! Array of vectors containing pointers to connection holders that hold
			the spike data that is to be sent.*/
		vector<ConnectionHolder*> spikeMessageBuffer[NUMBER_OF_DELAY_VALUES];
		
		/*! Records how many spikes are in each message buffer. This is different from the message
			buffer length, which holds how many Connection holders are in the message buffer. Each
			connection holder can hold many spikes so the spike count needs to be done separately.*/
		unsigned int messageSpikeCount[NUMBER_OF_DELAY_VALUES];
		
		/*! The maximum size of the arrays held in spikeArrays. 
			This will be the number of neurons in the other task that the neurons in this
			task are connected to.*/
		unsigned int maxBufferSize;
		
		/*! Controls which is the current spike array. This is the spike array that will be sent
			when sendSpikes() is called.*/
		unsigned int bufferCounter;

		/*! Array of task IDs of the tasks that will be sent the spike lists held in this task holder.
			There will be a main task id containing the other neuron groups that need to receive the
			spike list and there may also be tasks that need to receive the spike list for 
			monitoring or archiving. This is stored as an integer array to make it simpler to 
			send using pvm_mcast. Vector would have to be converted.*/
		int  *destinationTaskIDs;

		/*! Size of the destination task id array.*/
		int numberOfTasks;


		//=============================== METHODS ===================================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		TaskHolder (const TaskHolder&);

		/*! Declare assignment private so it cannot be used.*/
		TaskHolder operator = (const TaskHolder&);

};


#endif //TASKHOLDER_H




