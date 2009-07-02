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

#ifndef SYNAPSE_H
#define SYNAPSE_H

//SpikeStream includes
#include "Neuron.h"
#include "SimulationClock.h"
#include "GlobalVariables.h"


/*! The minimum double weight for a synapse. */
#define MIN_DOUBLE_WEIGHT -1.0

/*! The maximum double weight for a synapse. */
#define MAX_DOUBLE_WEIGHT 1.0

/*! The minimum short weight for a synapse. */
#define MIN_SHORT_WEIGHT -127

/*! The maximum short weight for a synapse. */
#define MAX_SHORT_WEIGHT 127


//---------------------------- Synapse --------------------------------------
/*! Base class inherited by all synapse classes This base class handles all 
	the simulation related stuff. The modelling aspects, such as STDP learning
	etc. should be implemented by classes inheriting from this class.

	NOTE getWeight() and getShortWeight() are virtual because there may need 
	to be some retrospective calculations done on the weight to get its 
	current value. */

/* FIXME IT WOULD BE MUCH BETTER TO HOLD THE SIMULATION CLOCK AS A STATIC 
	REFERENCE, BUT WHEN THIS IS SET USING Synapse::setSimulationClock() IT
	DOES NOT CHANGE THE ADDRESS IN CLASSES INHERITING FROM SYNAPSE. IT WOULD 
	BE 	GOOD IF THIS COULD BE SORTED OUT. */
//---------------------------------------------------------------------------

class Synapse {

	public:
		Synapse();
		virtual ~Synapse();


		/*--------------------------------------------------------------------------------
		  ------   Methods that subclasses of Synapse may or have to implement      ------
		  --------------------------------------------------------------------------------*/

		/*! Should return a descriptive name for the synapse. This is sometimes
			useful for debugging class loading. */
		virtual const string* getDescription() = 0;

		/* Should return the weight as a short between MIN_SHORT_WEIGHT and MAX_SHORT_WEIGHT.
			This is a virtual method because some implementations may need the state of 
			the weight to be calculated retrospectively. */
		virtual short getShortWeight() = 0;

		/*! Should return the weight as a double between MIN_DOUBLE_WEIGHT and 
			MAX_DOUBLE_WEIGHT. This is a virtual method because some implementations 
			may need the state of the weight to be calculated retrospectively. */
		virtual double getWeight() = 0;

		/*! Called when the parameters of the synapse have changed.
			The parameters of the synapses are held as references to parameter
			maps and when these are reloaded this method is called. */
		virtual bool parametersChanged() = 0;

		/*! Called when a spike is routed to this synapse. In event based simulation
			the synapse should be updated by this method. */
		virtual void processSpike() = 0;

		/*! Called to update synapse class when all synapses are being updated at each
			time step. This method is never called during event based simulation, when 
			the synapse class should be updated whenever it processes a spike. */
		virtual void calculateFinalState() = 0;

		/*! This method returns an  string containing an XML description of the variables 
			that are available for monitoring within this class. Overload this method and 
			getMonitoringData() if you want to send monitoring information back to the main 
			application. This will enable you to view a graph of the weight,
			for example. */
		virtual string getMonitoringInfo();

		/*! Returns a monitor data struct (defined in GlobalVariables.h) containing the
			data that is being monitored. This returned data must match that defined in 
			the string returned by getMonitoringInfo(). */
		virtual MonitorData* getMonitoringData(); 


		/*----------------------------------------------------------------------
		  ------       Public methods implemented by Synapse class        ------
		  ----------------------------------------------------------------------*/
		unsigned int getPresynapticNeuronID();
		void print();
		void setWeight(double weight);

		//Make SpikeStreamSimulation a friend to enable the loading of data
		friend class SpikeStreamSimulation;


	protected:
		//============================= VARIABLES =============================
		/*! Holds a reference to the neuron that this synapse is connected to. 
			This will be a class that inherits from the abstract Neuron class.*/
		Neuron *postSynapticNeuron;

		/*! Holds the preSynapticNeuronID to enable the neuron to change its 
			weight by accessing the synapse map.*/
		unsigned int preSynapticNeuronID;

		/*! The weight of the synapse. Stored as a double so that it can be used 
			without casting in 	calculations.*/
		double weight;

		/*! Reference to the simulation clock.*/
		//FIXME THIS WOULD BE BETTER STATIC, BUT HAD DYNAMIC LOADING PROBLEMS 
		SimulationClock* simulationClock;

		/*! Reference to the map containing the parameters for each connection
			group.*/
		map<string, double>* parameterMap;

		/*! Holds the monitoring data for the neuron.*/
		MonitorData monitorData;


	private:
		//============================ METHODS ================================
		void setParameterMapReference(map<string, double>* paramMap);
		void setPostSynapticNeuron(Neuron *neuron);
		void setPreSynapticNeuronID(unsigned int preSynNeurID);
		void setSimulationClock(SimulationClock* simClock);

};


#endif //SYNAPSE_H

