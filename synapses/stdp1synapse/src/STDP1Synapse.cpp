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

//SpikeStream includes
#include "STDP1Synapse.h"
#include "Debug.h"
#include "SimulationClock.h"

//Other includes
#include <cmath>
#include <iostream>
using namespace std;


//Defines used for debuggging
//#define SYNAPSE_PARAMETERS_DEBUG
//#define LEARNING_DEBUG


/*! When this is defined the synapse's weight can be sent to the SpikeStream
	Application to draw a live graph. */
#define MONITOR_WEIGHT


// Initialise and declare static variables
const string STDP1Synapse::calciumThreshUpLow("CalciumThreshUpLow");
const string STDP1Synapse::calciumThreshUpHigh("CalciumThreshUpHigh");
const string STDP1Synapse::calciumThreshDownLow("CalciumThreshDownLow");
const string STDP1Synapse::calciumThreshDownHigh("CalciumThreshDownHigh");
const string STDP1Synapse::weightChangeThreshold("WeightChangeThreshold");
const string STDP1Synapse::weightIncreaseAmnt("WeightIncreaseAmnt");
const string STDP1Synapse::weightDecreaseAmnt("WeightDecreaseAmnt");
const string STDP1Synapse::driftThreshold("DriftThreshold");
const string STDP1Synapse::positiveDrift("PositiveDrift");
const string STDP1Synapse::negativeDrift("NegativeDrift");
const string STDP1Synapse::maximumDrift("MaximumDrift");
const string STDP1Synapse::minimumDrift("MinimumDrift");
const string STDP1Synapse::learning("Learning");
const string STDP1Synapse::disable("Disable");


/*! Function used to create a STDP1Synapse class when library is dynamically loaded. */
extern "C" {
	Synapse* getClass(){
		return new STDP1Synapse;
	}
}


/*! Constructor. */
STDP1Synapse::STDP1Synapse() : Synapse() {
	//Initialise variables
	spikeTimeStep = -1;
	lastUpdateTime = 0.0;
	oldLearningMode = false;

	//Initialise MonitorData structure depending on what we are monitoring
	int count = 0;
	#ifdef MONITOR_WEIGHT
		count++;
	#endif//MONITOR_WEIGHT

	monitorData.dataArray = new double[count];
	monitorData.length = count;
}


/*! Destructor. */
STDP1Synapse::~STDP1Synapse(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING STDPSynapse."<<endl;
	#endif//MEMORY_DEBUG

	delete [] monitorData.dataArray;
}


//---------------------------------------------------------------------------------------
//-------------------------------- PUBLIC METHODS ---------------------------------------
//---------------------------------------------------------------------------------------

/*! Needs to be implemented by all classes inheriting from Synapse.
	Calls calculate weight, which updates the weight in learning mode. 
	The weight will already have been updated if a spike has been received during
	this time step.*/
void STDP1Synapse::calculateFinalState(){
	if((*parameterMap)[learning] == 1.0)
		calculateWeight(false);
}


/*! Returns a description of this class for debugging purposes.
	Invoking method has the responsibility of deleting the description string. */
const string* STDP1Synapse::getDescription(){
	string* tempstr = new string("STDP1 Synapse");
	return tempstr;
}


/*! Returns a NeuronData structure containing a pointer to an array containing the current values 
	of the monitored data items in the same order as they were listed in the XML file. */
MonitorData* STDP1Synapse::getMonitoringData(){
	/* Update any variables that change continuously over time. All of the spike processing and
		updating should have been completed by this point, but in event mode the synapse will 
		not have been updated if it has not received a spike*/
	if((*parameterMap)[learning] == 1.0)
		calculateWeight(false);

	#ifdef MONITOR_WEIGHT
		monitorData.dataArray[0] = weight;
	#endif//MONITOR_WEIGHT

	return &monitorData;
}


/*! Returns a string containing the data that is output by this neuron in monitoring mode in XML format. */ 
string STDP1Synapse::getMonitoringInfo(){
	string xmlString = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
	xmlString += "<monitor_info>";

	#ifdef MONITOR_WEIGHT
		xmlString += "<data><description>Weight</description><range_high>1.0</range_high><range_low>-1.0</range_low></data>";
	#endif//MONITOR_WEIGHT

	xmlString += "</monitor_info>";
	return xmlString;
}


/*! Returns the weight as a short between -127 and 127. */
short STDP1Synapse::getShortWeight(){
	/* Update any variables that change continuously over time. */
	if((*parameterMap)[learning] == 1.0)
		calculateWeight(false);

	//Convert weight to a number between -127 and 127
	short shortWeight = (short)rint(weight * 127.0);

	//Run an extra check in debug mode
	#ifdef SAVE_WEIGHTS_DEBUG
		cout<<"STDP1Synapse ["<<preSynapticNeuronID<<", "<<postSynapticNeuron->getNeuronID()<<"]: Saving short weight. Weight = "<<weight<<"; short weight = "<<shortWeight<<endl;
		if(shortWeight > MAX_SHORT_WEIGHT || shortWeight < MIN_SHORT_WEIGHT){
			cerr<<"STDP1Synapse ["<<preSynapticNeuronID<<", "<<postSynapticNeuron->getNeuronID()<<"]: BYTE WEIGHT OUT OF RANGE: "<<shortWeight<<endl;
			return 0;
		}
	#endif//SAVE_WEIGHTS_DEBUG

	return shortWeight;
}


/*! Returns the weight as a double. */
double STDP1Synapse::getWeight(){
	/* Update any variables that change continuously over time. */
	if((*parameterMap)[learning] == 1.0)
		calculateWeight(false);//Take account of the current time step because no spike has been received

	return weight;
}


/*! Called when the parameters have been changed.
	This method checks that the new parameters in the parameter map make sense. 
	NOTE May need to check for learning mode being switched on and off. */
bool STDP1Synapse::parametersChanged(){
	if(checkParameters()){
		if(!oldLearningMode && (*parameterMap)[learning] == 1.0){//Learning has been switched on
			lastUpdateTime = simulationClock->getSimulationTime();//Bring last update time up to date so that the weight drift is not calculated for the period when learning was switched off
			oldLearningMode = true;
		}
		else if (oldLearningMode && (*parameterMap)[learning] == 0.0){//Learning has been switched off
			calculateWeight(false);//Bring weight up to date before learning is switched off
			oldLearningMode = false;
		}
		return true;//Don't care if learning mode has stayed the same
	}
	else{
		return false;
	}
}


/*! Processes an incoming spike from the presynaptic neuron. */
void STDP1Synapse::processSpike(){
	//First check to see if synapse is disabled.
	if((*parameterMap)[disable] == 1.0)
		return;

	/* In learning mode the spike may have parameters that change over time and need to be updated.
		Will also need to record the time step at which the spike was received to save updating 
		every synapse every time updateWeight is called by a neuron that has received a spike. */
	if((*parameterMap)[learning] == 1.0){
		/* Update any variables that change continuously over time. */
		calculateWeight(true);//Update the weight, but exclude the current time step because a spike has been received
	
		//Record the time step of the spike
		spikeTimeStep = simulationClock->getTimeStep();
	}

	//Alter the postsynaptic potential
	postSynapticNeuron->changePostSynapticPotential(weight, preSynapticNeuronID);
}


/*! Debugging method called by neuron to check that all the linking is working ok. */
bool STDP1Synapse::testFunction(){
	cout<<"STDP1Synapse ["<<preSynapticNeuronID<<", "<<postSynapticNeuron->getNeuronID()<<"]: Test function called. "<<endl;
	return true;
}


/*! Called by the neuron during learning mode when all spikes have been received.
	Alters the weight depending on the membrane potential and the calcium concentration. */
void STDP1Synapse::updateWeight(double membranePotential, double calciumConc){
	#ifdef LEARNING_DEBUG
		cout<<"STDP1Synapse ["<<preSynapticNeuronID<<", "<<postSynapticNeuron->getNeuronID()<<"]: Updating weight."<<endl;
	#endif//LEARNING_DEBUG

	/* NOTE May need to update here if this contains any
		variables that change continuously over time. */

	if((*parameterMap)[learning] == 1.0){//Check that synapse is in learning mode

		//Only want to update synapses that have received a spike in this timestep
		if(spikeTimeStep != simulationClock->getTimeStep()){
			#ifdef LEARNING_DEBUG
				cout<<"STDP1Synapse ["<<preSynapticNeuronID<<", "<<postSynapticNeuron->getNeuronID()<<"]: Spike has not received a spike. Applying positive or negative drift and returning."<<endl;
			#endif//LEARNING_DEBUG
			return;
		}

		/* The weight should be up to date at this point since we are only running this calculation on synapses that have received
			a spike and the process spike method updates the weight. */

		//Change weight depending on the value of the membrane potential and calcium concentration
		if(membranePotential > (*parameterMap)[weightChangeThreshold]){//Increase weight
			if(calciumConc > (*parameterMap)[calciumThreshUpLow] && calciumConc < (*parameterMap)[calciumThreshUpHigh]){
				//Increase weight
				weight += (*parameterMap)[weightIncreaseAmnt];
				normaliseWeight();
				return;
			}
		}
		else{//Decrease weight if calcium is within range
			if(calciumConc > (*parameterMap)[calciumThreshDownLow] && calciumConc < (*parameterMap)[calciumThreshDownHigh]){
				//Decrease weight
				weight -= (*parameterMap)[weightDecreaseAmnt];
				normaliseWeight();
				return;
			}
		}

		//If we have reached this point, none of the conditions for changing the weight will have applied, so drift positive or negative
		if(weight > (*parameterMap)[driftThreshold] ){
			weight += (*parameterMap)[positiveDrift] * simulationClock->getTimeStepDuration_ms();//Drifts by positive drift each ms, so multiply by the time step duration
			if(weight > (*parameterMap)[maximumDrift])//Prevent weight from exceeding the maximum, which may not be the same as the maximum weight
				weight = (*parameterMap)[maximumDrift];
		}
		else{
			weight -= (*parameterMap)[negativeDrift] * simulationClock->getTimeStepDuration_ms();//Drifts by positive drift each ms, so multiply by the time step duration
			if(weight < (*parameterMap)[minimumDrift])//Prevent weight from exceeding the minimum, which may not be the same as the minimum weight
				weight = (*parameterMap)[minimumDrift];
		}

		//Normalise the weight in case the drift parameters have been set incorrectly.
		normaliseWeight();
	}
}


//----------------------------------------------------------------------------------------
//----------------------------------- PRIVATE METHODS ------------------------------------
//----------------------------------------------------------------------------------------

/*! Calculates the current state of the weight. Each time step the weight drifts by a certain amount
	but to save calculating this all the time, it is only calculated when a spike is received or when
	the weights are viewed or saved or when the synapse is in full update mode.
	When a spike is received by this synapse we do not want to drift the weight for the current
	time step since the weight will be updated elsewhere, so only calculate the weight change
	for the intervening time steps when a spike was not received and the synapse was not updated.
	NOTE: This method should only be called in learning mode. */
void STDP1Synapse::calculateWeight(bool spikeReceived){
	//Update the current time
	currentTime = simulationClock->getSimulationTime();

	if(lastUpdateTime == currentTime)//Check to see if neuron has been updated already
		return;

	if(weight > (*parameterMap)[driftThreshold] ){
		if(spikeReceived)//Do not drift the weight for the current time step because a spike has been received
			weight += (*parameterMap)[positiveDrift] * (currentTime - lastUpdateTime - simulationClock->getTimeStepDuration_ms());
		else//Drift weight for the current time step as well
			weight += (*parameterMap)[positiveDrift] * (currentTime - lastUpdateTime);

		//Prevent weight from exceeding the maximum, which may not be the same as the maximum weight
		if(weight > (*parameterMap)[maximumDrift])
			weight = (*parameterMap)[maximumDrift];
	}
	else{
		if(spikeReceived)//Do not drift the weight for the current time step because a spike has been received
			weight -= (*parameterMap)[negativeDrift] * (currentTime - lastUpdateTime - simulationClock->getTimeStepDuration_ms());
		else//Drift weight for the current time step as well
			weight -= (*parameterMap)[negativeDrift] * (currentTime - lastUpdateTime);

		//Prevent weight from exceeding the minimum, which may not be the same as the minimum weight
		if(weight < (*parameterMap)[minimumDrift])
			weight = (*parameterMap)[minimumDrift];
	}

	//Normalise the weight in case the drift parameters have been set incorrectly.
	normaliseWeight();

	//Record the last update time
	lastUpdateTime = currentTime;
}


/*! Check that all the necessary parameters are in the map. */
bool STDP1Synapse::checkParameters(){

	if(parameterMap->count(calciumThreshUpLow) != 1){
		cerr<<"STDP1Synapse ["<<preSynapticNeuronID<<", "<<postSynapticNeuron->getNeuronID()<<"]: PARAMETER calciumThreshUpLow NOT FOUND WITH KEY \""<<calciumThreshUpLow<<"\""<<endl;
		return false;
	}

	if(parameterMap->count(calciumThreshUpHigh) != 1){
		cerr<<"STDP1Synapse ["<<preSynapticNeuronID<<", "<<postSynapticNeuron->getNeuronID()<<"]: PARAMETER calciumThreshUpHigh NOT FOUND WITH KEY \""<<calciumThreshUpHigh<<"\""<<endl;
		return false;
	}

	if(parameterMap->count(calciumThreshDownLow) != 1){
		cerr<<"STDP1Synapse ["<<preSynapticNeuronID<<", "<<postSynapticNeuron->getNeuronID()<<"]: PARAMETER calciumThreshDownLow NOT FOUND WITH KEY \""<<calciumThreshDownLow<<"\""<<endl;
		return false;
	}

	if(parameterMap->count(calciumThreshDownHigh) != 1){
		cerr<<"STDP1Synapse ["<<preSynapticNeuronID<<", "<<postSynapticNeuron->getNeuronID()<<"]: PARAMETER calciumThreshDownHigh NOT FOUND WITH KEY \""<<calciumThreshDownHigh<<"\""<<endl;
		return false;
	}

	if(parameterMap->count(weightChangeThreshold) != 1){
		cerr<<"STDP1Synapse ["<<preSynapticNeuronID<<", "<<postSynapticNeuron->getNeuronID()<<"]: PARAMETER weightChangeThreshold NOT FOUND WITH KEY \""<<weightChangeThreshold<<"\""<<endl;
		return false;
	}

	if(parameterMap->count(weightIncreaseAmnt) != 1){
		cerr<<"STDP1Synapse ["<<preSynapticNeuronID<<", "<<postSynapticNeuron->getNeuronID()<<"]: PARAMETER weightIncreaseAmnt NOT FOUND WITH KEY "<<weightIncreaseAmnt<<endl;
		return false;
	}

	if(parameterMap->count(weightDecreaseAmnt) != 1){
		cerr<<"STDP1Synapse ["<<preSynapticNeuronID<<", "<<postSynapticNeuron->getNeuronID()<<"]: PARAMETER weightDecreaseAmnt NOT FOUND WITH KEY \""<<weightDecreaseAmnt<<"\""<<endl;
		return false;
	}

	if(parameterMap->count(driftThreshold) != 1){
		cerr<<"STDP1Synapse ["<<preSynapticNeuronID<<", "<<postSynapticNeuron->getNeuronID()<<"]: PARAMETER driftThreshold NOT FOUND WITH KEY \""<<driftThreshold<<"\""<<endl;
		return false;
	}

	if(parameterMap->count(positiveDrift) != 1){
		cerr<<"STDP1Synapse ["<<preSynapticNeuronID<<", "<<postSynapticNeuron->getNeuronID()<<"]: PARAMETER positiveDrift NOT FOUND WITH KEY \""<<positiveDrift<<"\""<<endl;
		return false;
	}

	if(parameterMap->count(negativeDrift) != 1){
		cerr<<"STDP1Synapse ["<<preSynapticNeuronID<<", "<<postSynapticNeuron->getNeuronID()<<"]: PARAMETER negativeDrift NOT FOUND WITH KEY \""<<negativeDrift<<"\""<<endl;
		return false;
	}

	if(parameterMap->count(maximumDrift) != 1){
		cerr<<"STDP1Synapse ["<<preSynapticNeuronID<<", "<<postSynapticNeuron->getNeuronID()<<"]: PARAMETER maximumDrift NOT FOUND WITH KEY \""<<maximumDrift<<"\""<<endl;
		return false;
	}

	if(parameterMap->count(minimumDrift) != 1){
		cerr<<"STDP1Synapse ["<<preSynapticNeuronID<<", "<<postSynapticNeuron->getNeuronID()<<"]: PARAMETER minimumDrift NOT FOUND WITH KEY \""<<minimumDrift<<"\""<<endl;
		return false;
	}

	if(parameterMap->count(learning) != 1){
		cerr<<"STDP1Synapse ["<<preSynapticNeuronID<<", "<<postSynapticNeuron->getNeuronID()<<"]: PARAMETER learning NOT FOUND WITH KEY \""<<learning<<"\""<<endl;
		return false;
	}

	if(parameterMap->count(disable) != 1){
		cerr<<"STDP1Synapse ["<<preSynapticNeuronID<<", "<<postSynapticNeuron->getNeuronID()<<"]: PARAMETER disable NOT FOUND WITH KEY \""<<disable<<"\""<<endl;
		return false;
	}

	//Print parameters in debug mode
	#ifdef SYNAPSE_PARAMETERS_DEBUG
		printParameters();
	#endif//SYNAPSE_PARAMETERS_DEBUG

	return true;
}


/*! Prevent weight from going out of range.
	Note that this is flooring the weight at zero rather than MIN_DOUBLE_WEIGHT
	because it is assumed that only excitatory connections will be subject to learning
	and should not turn into inhibitory connections during the learning process. */
void STDP1Synapse::normaliseWeight(){
	if(weight > MAX_DOUBLE_WEIGHT)
		weight = MAX_DOUBLE_WEIGHT;
	else if(weight < 0)
		weight = 0;
}


/*! Prints out the parameters for debugging. */
void STDP1Synapse::printParameters(){
		cout<<"------------------ SETTING STDP1Synapse PARAMETERS -----------------------"<<endl;
		cout<<"calciumThreshUpLow = "<<(*parameterMap)[calciumThreshUpLow]<<endl;
		cout<<"calciumThreshUpHigh= "<<(*parameterMap)[calciumThreshUpHigh]<<endl;
		cout<<"calciumThreshDownLow = "<<(*parameterMap)[calciumThreshDownLow]<<endl;
		cout<<"calciumThreshDownHigh = "<<(*parameterMap)[calciumThreshDownHigh]<<endl;
		cout<<"weightChangeThreshold = "<<(*parameterMap)[weightChangeThreshold]<<endl;
		cout<<"weightIncreaseAmnt = "<<(*parameterMap)[weightIncreaseAmnt]<<endl;
		cout<<"weightDecreaseAmnt = "<<(*parameterMap)[weightDecreaseAmnt]<<endl;
		cout<<"driftThreshold = "<<(*parameterMap)[driftThreshold]<<endl;
		cout<<"positiveDrift = "<<(*parameterMap)[positiveDrift]<<endl;
		cout<<"negativeDrift = "<<(*parameterMap)[negativeDrift]<<endl;
		cout<<"maximumDrift = "<<(*parameterMap)[maximumDrift]<<endl;
		cout<<"minimumDrift = "<<(*parameterMap)[minimumDrift]<<endl;
		cout<<"Learning mode = "<<(*parameterMap)[learning]<<endl;
		cout<<"Disable = "<<(*parameterMap)[disable]<<endl;
		cout<<"---------------------------------------------------------------------------"<<endl;
}


