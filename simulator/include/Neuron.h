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

#ifndef NEURON_H
#define NEURON_H

//SpikeStream includes
#include "TaskHolder.h"
#include "ConnectionHolder.h"
#include "NeuronTaskHolder.h"
#include "SimulationClock.h"
#include "GlobalVariables.h"

//Other includes
#include <vector>
#include <map>
#include <string>


//-------------------------------- Neuron ---------------------------------
/*! This class acts as the base class inherited by all neuron classes.
	This base class handles all the message passing aspects of the neuron, 
	and derived classes should add in the modelling aspect to control when 
	the neuron fires.
	
	During a time step each firing synapse changes the membrane potential 
	of the neuron if it decides to pass on the spike. This changing of the 
	membrane potential should call the update method of the neuron if the 
	neuron's state has not already been updated in that time step. Finally 
	when all spikes have been received, the function calculateFinalState()
	is called instructing the neuron to decide whether it should fire or 
	not. This is because excitatory and inhibitory spikes can arrive 
	simultaneously within the same time step, so the final neuron state can 
	only be calculated after all spikes have arrived.

	NOTE Neuron ids from 0-10 are reserved and should never appear in the
	database

	NOTE When the synapse potential is changed as a way of injecting noise
	the preSynapticNeuronID in changeMembranePotential will be set to zero.
*/
	
/* FIXME IT WOULD BE MUCH BETTER TO HOLD THE SIMULATION CLOCK AS A STATIC 
	REFERENCE, BUT WHEN THIS IS SET USING Neuron::setSimulationClock() IT
	DOES NOT CHANGE THE ADDRESS IN CLASSES INHERITING FROM NEURON. WOULD BE 
	GOOD IF THIS COULD BE SORTED OUT. */
//--------------------------------------------------------------------------

class Neuron {
	public:
		Neuron();
		virtual ~Neuron();


		/*--------------------------------------------------------------------------------
		  ------   Methods that subclasses of Neuron may or have to implement       ------
		  --------------------------------------------------------------------------------*/

		/*! Calculates the final state of the neuron after all spikes have been received. The neuron
			should be updated by this method if it is not updated already. */
		virtual void calculateFinalState() = 0;

		/* A synapse may change the membrane potential of the neuron. */
		virtual void changePostSynapticPotential(double amount, unsigned int preSynapticNeuronID) = 0;

		/*! Returns a description of this neuron class for debugging only.
			Destruction of the new string is the responsibility of the invoking method. */
		virtual const string* getDescription() = 0;

		/*! Sets the parameters of the neuron. These should be defined in their own
			database, whose name is listed in the NeuronTypes database. This is called 
			on only one instance of the neuron class with the parameters being set
			and held statically. The parametersChanged() method is called after
			the static setting of the parameters to inform each neuron class
			that the parameters have changed. */
		virtual bool setParameters(map<string, double> paramMap) = 0;

		/*! Called after the parameters have been statically changed to inform each
			neuron class that the parameters have been changed. This enables them 
			to update their learning state, for example, after learning has been 
			switched off. */
		virtual void parametersChanged() = 0;

		/*! This method returns an  string containing an XML description of the variables 
			that are available for monitoring within this class. Overload this method and 
			getMonitoringData() if you want to send monitoring information back to the main 
			application. This will enable you to view a graph of the membrane potential, 
			for example. */
		virtual string getMonitoringInfo();

		/*! Returns a monitor data struct (defined in GlobalVariables.h) containing the
			data that is being monitored. This returned data must match that defined in 
			the string returned by getMonitoringInfo() */
		virtual MonitorData* getMonitoringData(); 


		/*----------------------------------------------------------------------
		  ------       Public methods implemented by Neuron class         ------
		  ----------------------------------------------------------------------*/
		void fireNeuron();
		int getNeuronFireCount();
		unsigned int getNeuronID();


		//Declare SpikeStreamSimulation a friends to enable loading of the neurons
		friend class SpikeStreamSimulation;


	protected:
		//=========================== VARIABLES ===================================
		/*! ID of this neuron. */
		unsigned int neuronID;

		/*! Reference to the main simulation clock. */
		//FIXME THIS WOULD BE BETTER STATIC BUT CREATES PROBLEMS - SEE INTRO
		SimulationClock *simulationClock;

		/*! Time at which neuron last fired. */
		double neuronFireTime;

		/*! Number of times neuron has fired since the beginning of the simulation. */
		int neuronFireCount;

		/*! Pointer to the synapse map, which is used by some neuron models.
			Uses void pointer to avoid include problems. The type of this map is:
			dense_hash_map<unsigned int, dense_hash_map<unsigned int, Synapse*, hash<unsigned int> >*, hash<unsigned int> >
			See SpikeStreamSimulation for more details about this map. */
		void* synapseMapPtr;

		/*! Pointer to all the synpse classes that connect to this neuron
			Stored as void pointer to avoid include problems. */
		vector<void*> preSynapseVector;

		/*! Holds the monitoring data for the neuron. */
		MonitorData monitorData;


	private:
		//========================= VARIABLES =============================
		/*! Array containing references to all the spike task holders that 
			this neuron communicates with. */
		TaskHolder **spikeTaskHolderArray;

		/*! Size of spikeTaskHolderArray. */
		unsigned int numberOfSpikeTaskHolders;

		/*! Reference to the neuron task holder for this neuron group. */
		NeuronTaskHolder *neuronTaskHolder;
		
		/*! The connection map connects task IDs with vectors of connection Holders
			The connectionHolders in each vector are destined for the same task,
			but each has a different delay and is inserted into a different spike
			buffer. */
		/* FIXME THINK ABOUT ALLOCATING CONNECTION HOLDERS ON THE HEAP SINCE THER
			MAY BE A LARGE NUMBER OF THEM. */
		map<int, vector<ConnectionHolder> > connectionMap;

		/*! Holds the complete list of synapse ids that this neuron is connected to
			These are compressed by subtracting the start neuron ID from the neuron ID
			When messages are unpacked the start neuron ID is added back on to get the 
			correct neuron ID. Connection holders point to parts of this array.*/
		unsigned short *connectionArray;

		/*! The number of synapse ids in the connectionArray, which is not its length
			because each compressed neuron ID is an unsigned short and so two fit into
			a single unsigned integer. */
		unsigned int numberOfConnections;
		
		/*! Declare bufferInsertionPoint here to save redeclaring it in fireNeuron
			each time method is called. */
		int bufferInsertionPoint;

		/*! Declare destTID here to save redeclaring it in fireNeuron
			each time method is called. */
		int destTID;


		//================================= METHODS ==================================
		void printConnections();
		void setNeuronID(unsigned int neuronID);
		void setSimulationClock(SimulationClock* simClock);
		void setSynapseMapPtr(void* synMapPtr);

};


#endif //NEURON_H

