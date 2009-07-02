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

#ifndef NEURONTASKHOLDER_H
#define NEURONTASKHOLDER_H

//Other includes
#include <vector>
using namespace std;


//-------------------------- Neuron Task Holder ---------------------------
/*! Stores the current firing pattern of the neuron group and sends it to 
	any tasks that want to monitor the firing neurons rather than the 
	spikes. */
//-------------------------------------------------------------------------

class NeuronTaskHolder {

	public:
		NeuronTaskHolder(int thisTaskID, unsigned int maxNumberOfNeurons);
		~NeuronTaskHolder();
		void addReceivingTask(int);
		void removeReceivingTask(int);
		bool sendFiringNeuronMessages();


		//======================== VARIABLES ==================================
		/*! Keep a record of this task here for error messages.*/
		int thisTaskID;
		
		/*! Vector holding the list of currently firing neurons.*/
		vector<unsigned int> firingNeuronVector;


	private:
		//========================= VARIABLES ================================
		/*! Task IDs of the tasks that will be sent the firing neuron lists held in 
			this task holder. Stored as an integer array to make it simpler to send using 
			pvm_mcast. A vector of task ids would have to be converted.*/
		int  *destinationTaskIDs;

		/*! Size of the destinationTaskIDs array.*/
		int numberOfTasks;


		//=========================== METHODS ================================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		NeuronTaskHolder (const NeuronTaskHolder&);

		/*! Declare assignment private so it cannot be used.*/
		NeuronTaskHolder operator = (const NeuronTaskHolder&);

};


#endif //NEURONTASKHOLDER_H
