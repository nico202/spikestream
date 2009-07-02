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
#include "Neuron.h"
#include "Debug.h"

//Other includes
#include <iostream>
using namespace std;


/*! Constructor. */
Neuron::Neuron(){
	/* Make neuron fire time large and negative so that at start up it appears as if the
		neuron fired in the distant past */
	neuronFireTime = -100.0;
	neuronFireCount = 0;

	//Initialise monitoring data
	monitorData.dataArray = NULL;
	monitorData.length = 0;
}


/*! Destructor. */
Neuron::~Neuron(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING NEURON."<<endl;
	#endif//MEMORY_DEBUG	

	delete [] connectionArray;
	delete [] spikeTaskHolderArray;

	//Connection holders do not need deleting since they are currently on the stack
}


//-----------------------------------------------------------------------------------
//------------------------------ PUBLIC METHODS -------------------------------------
//-----------------------------------------------------------------------------------

/*! This function fires the neuron and adds a reference to the list of synapse ids 
	that it connects to to the appropriate task holder. These task ids are then copied 
	directly into the message buffer when the spike lists are sent. */
void Neuron::fireNeuron(){
	neuronFireTime = simulationClock->getSimulationTime();
	++neuronFireCount;

	#ifdef FIRE_NEURON_DEBUG
		cout<<"Firing neuron "<<neuronID<<" at simulationTime "<<neuronFireTime<<" with neuronFireCount = "<<neuronFireCount<<endl;
	#endif //FIRE_NEURON_DEBUG

	/* Work through the list of connections
		Need to store the reference to the array and the array size in the appropriate
		place in the spike task holder */
	for(unsigned int i=0; i<numberOfSpikeTaskHolders; ++i){

		//Get pointer to the array containing all the connections for this task holder
		destTID = spikeTaskHolderArray[i]->primaryDestinationID;
		
		/* Work through the vector of connection holders for the task */
		for(vector<ConnectionHolder>::iterator iter = connectionMap[destTID].begin(); iter != connectionMap[destTID].end(); ++iter){

			/* Get the point in time at which I want to insert the connection holder. 
				The delay of the connection holder has to be added to the time counter, which
				cycles through 0 - NUMBER_OF_DELAY_VALUES */
			bufferInsertionPoint = (spikeTaskHolderArray[i]->bufferCounter + iter->delay) % NUMBER_OF_DELAY_VALUES;

			/* NOTE noise in the delay could be added here. It is essential that this does not
				lead to spikes overtaking each other or arriving at the same time, which would
				not be biologically realistic. So only want to alter delay by a few percent at most */
			
			/* This line performs the following:
				Get the task holder at taskHolderArray[i]
				In the array of buffers held as vectors, 1 for each time point,
				Add a reference to a connectionHolder, containing a list of connectionIDs for this point */
			spikeTaskHolderArray[i]->spikeMessageBuffer[bufferInsertionPoint].push_back(&(*iter));
			
			//Increase the count of the number of spikes in the message
			spikeTaskHolderArray[i]->messageSpikeCount[bufferInsertionPoint] += iter->numConnIDs;
		}
	}

	//Add the neuron ID to the neuronTaskHolder
	neuronTaskHolder->firingNeuronVector.push_back(neuronID);	
}


/* Returns a MonitorData structure containing a pointer to an array containing the current values 
	of the monitored data items in the same order as they were listed in the XML file. This data
	structure also contains the length of the array. */
MonitorData* Neuron::getMonitoringData(){
	return &monitorData;
}

/* Returns a string containing the data that is output by this neuron in monitoring mode in XML format.
	The implementation in Neuron does not contain any monitored parameters. Overload this method if you 
	want to send actual information back. */ 
string Neuron::getMonitoringInfo(){
	string xmlString = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
	xmlString += "<monitor_info>";
	xmlString += "</monitor_info>";
	return xmlString;
}


/*! Returns the number of times the neuron has fired since the beginning
	of the simulation. */
int Neuron::getNeuronFireCount(){
	return neuronFireCount;
}


/*! Accesses the neuronID of this neuron. */
unsigned int Neuron::getNeuronID(){
	return neuronID;
}


//------------------------------------------------------------------------------
//---------------------------- PRIVATE METHODS ---------------------------------
//------------------------------------------------------------------------------

/*! Prints all of the connections. */
void Neuron::printConnections(){
 	cout<<"===================================== START NEURON ============================================="<<endl;
	cout<<"Neuron ID: "<<neuronID<<"; Number of task holders: "<<numberOfSpikeTaskHolders<<endl;

	//Work through each task holder
	for(map<int, vector<ConnectionHolder> >::iterator taskIter = connectionMap.begin(); taskIter != connectionMap.end(); ++taskIter){
		cout<<"     TaskID: "<<taskIter->first<<endl;

		//Work through each of the connectionHolders held within the task holder
		for(vector<ConnectionHolder>::iterator connHoldIter = taskIter->second.begin(); connHoldIter != taskIter->second.end(); ++connHoldIter){
			cout<<"          Delay: "<<connHoldIter->delay<<"; Size: "<<connHoldIter->numConnIDs<<endl;

			/* Output the from and to neuronIDs in the connectionHolder. 
				There are two numbers for each ID so need to advance i by 2 each time */
			for(int i=0; i<connHoldIter->numConnIDs; i += 2)
				cout<<"               FromNeuron: "<<connHoldIter->connIDArray[i]<<"; ToNeuron: "<<connHoldIter->connIDArray[i+1]<<endl;
		}
	}
	cout<<"ConnectionArray ( Number of connections = "<<numberOfConnections<<" ): ";
	for(unsigned int i=0; i< numberOfConnections; ++i)
		cout<<" "<<connectionArray[i];
	cout<<endl;
	cout<<"======================================== END NEURON ============================================"<<endl;
}


/*! Sets the neuron ID. This cannot be done in the constructor since an 
	array of neurons is created all at once using empty constructors when 
	the neuron array is declared. */
void Neuron::setNeuronID(unsigned int neurID){
	neuronID = neurID;
}


/*! Store reference to simulation clock. */
/* FIXME THIS WOULD BE BETTER STATIC, BUT HAVE HAD PROBLEMS WITH THIS CONNECTED
	EITHER WITH INHERITANCE OF STATIC VARIABLES, LOADING OF DYNAMIC LIBRARIES OR 
	PERHAPS BOTH! */
void Neuron::setSimulationClock(SimulationClock* simClock){
	simulationClock = simClock;
}


/*! Passes a pointer to the synapse map
	Note that this is of type: dense_hash_map<unsigned int, dense_hash_map<unsigned int, Synapse*, hash<unsigned int> >*, hash<unsigned int> >
	I.E. it is a dense_hash_map containing pointers to further dense hash maps
	See the SpikeStreamSimulation code for examples of iterating through and accessing this map. */
void Neuron::setSynapseMapPtr(void* synMapPtr){
	synapseMapPtr = synMapPtr;
}


