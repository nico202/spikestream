/***************************************************************************
 *   SpikeStream STDP1 Synapse                                             *
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

#ifndef STDP1SYNAPSE_H
#define STDP1SYNAPSE_H

//SpikeStream includes
//#include "Debug.h"
#include "SimulationClock.h"
#include "Synapse.h"

//Other includes
#include <cmath>
#include <iostream>
using namespace std;


//----------------------------- STDP1 Synapse ------------------------------
/*! Synapse based on J.M. Brader, W. Senn and S. Fusi (2006). Learning real 
	world stimuli in a neural network with spike driven synaptic dynamics. 
	Submitted to Neural Computation (2006). 
	Available at: http://www.ini.unizh.ch/~fusi/papers/bsf05.pdf. */
//--------------------------------------------------------------------------

class STDP1Synapse : public Synapse {

	public:
		STDP1Synapse();
		~STDP1Synapse();

		//Public methods inherited from synapse
		void calculateFinalState();
		const string* getDescription();
		string getMonitoringInfo();
		MonitorData* getMonitoringData();
		inline short getShortWeight();
		inline double getWeight();
		bool parametersChanged();
		inline void processSpike();

		//Public methods unique to this class
		void updateWeight(double membranePotential, double calciumConc);
		bool testFunction();


	private:
		//=============================== VARIABLES ===================================
		/* Parameters are held in the synapse map and accessed using these static
			string keys. */

		/*! Threshold of calcium concentration controlling weight change during learning.*/
		static const string calciumThreshUpLow;

		/*! Threshold of calcium concentration controlling weight change during learning.*/
		static const string calciumThreshUpHigh;

		/*! Threshold of calcium concentration controlling weight change during learning.*/
		static const string calciumThreshDownLow;

		/*! Threshold of calcium concentration controlling weight change during learning.*/
		static const string calciumThreshDownHigh;

		/*! Weight change is voltage dependent and only takes place once the post synaptic
			neuron's membrane potential is over a given threshold.*/
		static const string weightChangeThreshold;

		/*! Weight increase takes place in small jumps.*/
		static const string weightIncreaseAmnt;

		/*! Weight increase takes place in small jumps.*/
		static const string weightDecreaseAmnt;

		/*! In the absence of stimulation or weight change, the weight drifts towards the
			maximumDrift if it is above this threshold and towards the minimumDrift if it
			is below this threshold.*/
		static const string driftThreshold;

		/*! The positive drift amount.*/
		static const string positiveDrift;

		/*! The negative drift amount.*/
		static const string negativeDrift;

		/*! The maximum value that the weight can drift to. This can be used to prevent an
			inhibitory connection drifting into an excitatory connection when it would be
			better for it to drift towards zero.*/
		static const string maximumDrift;

		/*! The minimum value that the weight can drift to. This can be used to prevent an
			excitatory connection drifting into an inhibitory connection when it would be
			better for it to drift towards zero.*/
		static const string minimumDrift;

		/*! Whether synapse is in learning mode or not.*/
		static const string learning;

		/*! Enables the synapse to be disabled so that it does not transmit spikes.*/
		static const string disable;

		/*! Records the timestep in which the last spike was received. This is so that 
			the learning method only applies to synapses that have received a spike in 
			the last time step.*/
		int spikeTimeStep;

		/*! The current time.*/
		double currentTime;

		/*! The time since the synapse was last updated.*/
		double lastUpdateTime;

		/*! Need the previous learning mode to determine whether this has been switched on or off.
			Cannot make this static because there could be different learning modes present in the
			same neuron group.*/
		bool oldLearningMode;


		//=============================== METHODS ===================================
		void calculateWeight(bool spikeReceived);
		bool checkParameters();
		void normaliseWeight();
		void printParameters();

};


#endif//STDP1SYNAPSE_H


