/***************************************************************************
 *   SpikeStream STDP1 Neuron                                              *
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

#ifndef STDP1NEURON_H
#define STDP1NEURON_H

//SpikeStream includes
#include "Neuron.h"

//Other includes
#include <string>
using namespace std;


//------------------------------ STDP1 Neuron -----------------------------
/*! Neuron based on J.M. Brader, W. Senn and S. Fusi (2006). Learning real
	world stimuli in a neural network with spike driven synaptic dynamics. 
	Submitted to Neural Computation (2006).
	Available at: http://www.ini.unizh.ch/~fusi/papers/bsf05.pdf. */
//-------------------------------------------------------------------------

class STDP1Neuron : public Neuron {

	public:
		STDP1Neuron();
		~STDP1Neuron();

		//Public methods inherited from Neuron
		inline void calculateFinalState();
		inline void changePostSynapticPotential(double amount, unsigned int preSynapticNeuronID);
		inline const string* getDescription();
		string getMonitoringInfo();
		MonitorData* getMonitoringData();
		inline void parametersChanged();
		bool setParameters(map<string, double> paramMap);


	private:
		//============================== VARIABLES ===================================
		/* --------------------------------------------------------------------------
		      Variables controlling the neuron's behaviour are static so that they
				can be set easily for the whole neuron group.
		   --------------------------------------------------------------------------*/

		/*! Neuron fires when its post synaptic potential crosses this threshold.*/
		static double threshold;

		/*! Controls the decay rate of the post synaptic potential.*/
		static double membraneTimeConstant_millisec;

		/*! Neuron cannot fire when it is in its refractory period.*/
		static double refractoryPeriod_millisec;

		/*! Parameter controlling the relative refractory period.*/
		static double refractoryParam_m;

		/*! Parameter controlling the relative refractory period.*/
		static double refractoryParam_n;

		/*! Whether the neuron is in learning mode.*/
		static bool learningMode;

		/*! Controls the behaviour of the calcuim concentration.
			This is used during learning as a measure of the firing rate of the neuron.*/
		static double calciumIncreaseAmnt;

		/*! Controls the behaviour of the calcuim concentration.
			This is used during learning as a measure of the firing rate of the neuron.*/
		static double calciumDecayRate;	

		/*! Limits the degree to which the membrane potential can be hyperpolarized. Otherwise
			an inhibitory layer can increase the polarization without limit, which is not
			biologically realistic and affects the time that the neuron takes to recover
			from inhibition.*/
		static double minPostsynapticPotential;


		/* --------------------------------------------------------------------------- 
		   ----------------- Variables held within each neuron -----------------------
		   ---------------------------------------------------------------------------*/

		/*! The total post synaptic potential caused by all previous spikes.*/
		double pspTotal;

		/*! Used for the final calculation, taking account of refractory period etc.*/
		double membranePotential;

		/*! The current time.*/
		double currentTime;

		/*! The time since the neuron was last updated.*/
		double lastUpdateTime;
	
		/*! The time since the neuron last fired.*/
		double timeSinceLastFire;

		/*! The previous learning mode. Used to check for learning mode changes.*/
		bool oldLearningMode;

		/*! Calcium level in the cell
			Used during learning to measure the activity level of the neuron.*/
		double calciumConc;

		/*! Record when the final state is calculated so that this method can be invoked
			prior to returning monitoring data without calling it twice.*/
		double finalStateUpdateTime;


		//============================= METHODS ======================================
		void calculateMembranePotential();
		void printParameters();
		void update();

};


#endif//STDP1NEURON_H


