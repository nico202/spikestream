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

//SpikeStream includes
#include "STDP1Neuron.h"
#include "SimulationClock.h"
#include "Debug.h"
#include "STDP1Synapse.h"

//Other includes
#include <cmath>
#include <iostream>
using namespace std;


//Defines for debugging
//#define NEURON_MODEL_DEBUG
//#define NEURON_METHOD_DEBUG
//#define NEURON_PARAMETERS_DEBUG


/*! Defines whether we sent membrane potential to SpikeStream Application
	to draw a real time graph. */
#define MONITOR_MEMBRANE_POTENTIAL

/*! Defines whether we sent calcium concentration to SpikeStream Application
	to draw a real time graph. */
#define MONITOR_CALCIUM_CONC


//List static variables here so they can be accessed and initialise them
double STDP1Neuron::threshold = 0.0;
double STDP1Neuron::refractoryPeriod_millisec = 0.0;
double STDP1Neuron::membraneTimeConstant_millisec = 0.0;
double STDP1Neuron::refractoryParam_m = 0.0;
double STDP1Neuron::refractoryParam_n = 0.0;
bool STDP1Neuron::learningMode = false;
double STDP1Neuron::calciumIncreaseAmnt = 0.0;
double STDP1Neuron::calciumDecayRate = 0.0;
double STDP1Neuron::minPostsynapticPotential = 0.0;


/*! Function used to create a Neuron class when library is dynamically loaded. */
extern "C" {
	Neuron* getClass(){
		return new STDP1Neuron;
	}
}


/*! Constructor. */
STDP1Neuron::STDP1Neuron() : Neuron() {
	//Initialise dynamic variables and set default values
	pspTotal = 0;
	membranePotential = 0;
	currentTime = 0.0;
	lastUpdateTime = 0.0;
	timeSinceLastFire = 0.0;
	oldLearningMode = false;
	calciumConc = 0.0;
	finalStateUpdateTime = -1.0;

	//Initialise MonitorData structure depending on what we are monitoring
	int count = 0;
	#ifdef MONITOR_MEMBRANE_POTENTIAL
		count++;
	#endif//MONITOR_MEMBRANE_POTENTIAL
	#ifdef MONITOR_CALCIUM_CONC
		count++;
	#endif//MONITOR_CALCIUM_CONC

	monitorData.dataArray = new double[count];
	monitorData.length = count;
}


/*! Destructor. */
STDP1Neuron::~STDP1Neuron(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING STDP1 NEURON"<<endl;
	#endif//MEMORY_DEBUG

	delete [] monitorData.dataArray;
}


//---------------------------------------------------------------------------------------
//-------------------------------- PUBLIC METHODS ---------------------------------------
//---------------------------------------------------------------------------------------

/*! Called to calculate the final membrane state and decide whether the neuron should fire or not. 
	In event-based updating mode, this has to be called after all spikes have been received because 
	a set of excitatory messages could be received from one layer and later a set of inhibitory messages 
	from another layer. In full neuron update mode, this method is called at each timestep. */
void STDP1Neuron::calculateFinalState(){
	#ifdef NEURON_MODEL_DEBUG
		cout<<"STDP1Neuron ["<<neuronID<<"]: Calculating final state."<<endl;
	#endif//NEURON_METHOD_DEBUG

	//Update the neuron if it has not been updated already
	update();

	//Calculate the membrane potential taking refractory period into account
	calculateMembranePotential();

	/* Call all the synapses that connect to this neuron and instruct them to carry out STDP learning
		This needs to be carried out at this point after all the spikes have arrived in the timestep
		and the membrane potential is final for this timestep and before the neuron fires, which
		reduces the membrane potential back to zero. Synapses do not connect to more than one neuron
		so there is no danger of calling the same method twice on the same synapse. This method will not
		be called on all synapses in a layer if all the neurons are not updated in a layer. */
	if(learningMode){
		for(vector<void*>::iterator iter = preSynapseVector.begin(); iter != preSynapseVector.end(); ++iter){
			((STDP1Synapse*)*iter)->updateWeight(membranePotential, calciumConc);
		}
	}

	//Fire neuron if membrane potential is greater than the threshold
	if(membranePotential > threshold){
		//Call method to send spikes etc.
		fireNeuron();

		//Once neuron has fired, it clears its history
		pspTotal = 0;
		membranePotential = 0;

		//Increase calcium concentration
		calciumConc += calciumIncreaseAmnt;
	}

	/* Record time at which final state has been calculated. This enables us to call this method
		when requesting monitor data so that the monitor data is up to date. */
	finalStateUpdateTime = simulationClock->getSimulationTime();
}


/*! Called by a synapse when it receives a spike and decides to pass it on to the neuron 
	Will be using dynamic synapses, so spikes may not always be passed on. */
void STDP1Neuron::changePostSynapticPotential(double amount, unsigned int preSynapticNeuronID){

	//Update any changes in the neuron's state.
	update();

	/* Add new spike to the total. 
		This spike has just arrived, so it has not decayed and is multiplied by 1 */
	pspTotal += amount;

	/* Make sure that this has not made the pspTotal less than the minimum */
	if(pspTotal < minPostsynapticPotential)
		pspTotal = minPostsynapticPotential;

	#ifdef NEURON_MODEL_DEBUG
		cout<<"STDP1Neuron ["<<neuronID<<"]: Changing post synaptic potential at time "<<simulationClock->getSimulationTime()<<" with timestep "<<simulationClock->getTimeStep()<<". pspTotal = "<<pspTotal<<endl;
	#endif//NEURON_METHOD_DEBUG
}


/*! Returns a description of this class for debugging purposes. 
	Invoking method has the responsibility of deleting the description string. */
const string* STDP1Neuron::getDescription(){
	string* tempstr = new string("STDP1 Neuron");
	return tempstr;
}


/*! Returns a NeuronData structure containing a pointer to an array containing the current values 
	of the monitored data items in the same order as they were listed in the XML file. */
MonitorData* STDP1Neuron::getMonitoringData(){

	//Check that the final state has been calculated for this neuron
	if(finalStateUpdateTime != simulationClock->getSimulationTime())
		calculateFinalState();

	#ifdef MONITOR_MEMBRANE_POTENTIAL
		monitorData.dataArray[0] = membranePotential;
	#endif//MONITOR_MEMBRANE_POTENTIAL

	#ifdef MONITOR_CALCIUM_CONC
		monitorData.dataArray[1] = calciumConc;
	#endif//MONITOR_CALCIUM_CONC

	return &monitorData;
}


/*! Returns a string containing the data that is output by this neuron in monitoring mode in XML format. */ 
string STDP1Neuron::getMonitoringInfo(){
	string xmlString = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
	xmlString += "<monitor_info>";

	#ifdef MONITOR_MEMBRANE_POTENTIAL
		xmlString += "<data><description>Membrane Potential</description><range_high>2</range_high><range_low>-8</range_low></data>";
	#endif//MONITOR_MEMBRANE_POTENTIAL

	#ifdef MONITOR_CALCIUM_CONC
		xmlString += "<data><description>Calcium Concentration</description><range_high>50</range_high><range_low>0</range_low></data>";
	#endif//MONITOR_CALCIUM_CONC

	xmlString += "</monitor_info>";
	return xmlString;
}


/*! Called when the parameters have been statically changed. 
	In this neuron implementation it resets the calcium to zero when it is turned on. */
void STDP1Neuron::parametersChanged(){
	#ifdef NEURON_METHOD_DEBUG
		cout<<"STDP1Neuron ["<<neuronID<<"]: Parameters changed."<<endl;//Should be called on every neuron
	#endif//NEURON_METHOD_DEBUG

	//Reset calcium concentration if learning mode is being turned on
	if(learningMode == true && oldLearningMode == false){//Learning is being turned on
		calciumConc = 0.0;//Reset calcium concentration
	}
	oldLearningMode = learningMode;
}


/*! Sets the neuron parameters. */
bool STDP1Neuron::setParameters(map<string, double> paramMap){
	#ifdef NEURON_METHOD_DEBUG
		cout<<"STDP1Neuron ["<<neuronID<<"]: Setting parameters"<<endl;//Should be called on a single neuron
	#endif//NEURON_METHOD_DEBUG

	if(paramMap.count(string("Threshold")))
		threshold = paramMap[string("Threshold")];
	else{
		cerr<<"STDP1Neuron ["<<neuronID<<"]: CANNOT FIND \"Threshold\" IN PARAMETER MAP."<<endl;
		return false;
	}

	if(paramMap.count(string("RefractoryPeriod")))
		refractoryPeriod_millisec = paramMap[string("RefractoryPeriod")];
	else{
		cerr<<"STDP1Neuron ["<<neuronID<<"]: CANNOT FIND \"RefractoryPeriod\" IN PARAMETER MAP"<<endl;
		return false;
	}

	if(paramMap.count(string("MembraneTimeConstant")))
		membraneTimeConstant_millisec = paramMap[string("MembraneTimeConstant")];
	else{
		cerr<<"STDP1Neuron ["<<neuronID<<"]: CANNOT FIND \"MembraneTimeConstant\" IN PARAMETER MAP"<<endl;
		return false;
	}

	if(paramMap.count(string("RefractoryParamM")))
		refractoryParam_m = paramMap[string("RefractoryParamM")];
	else{
		cerr<<"STDP1Neuron ["<<neuronID<<"]: CANNOT FIND \"RefractoryParamM\" IN PARAMETER MAP"<<endl;
		return false;
	}

	if(paramMap.count(string("RefractoryParamN")))
		refractoryParam_n = paramMap[string("RefractoryParamN")];
	else{
		cerr<<"STDP1Neuron ["<<neuronID<<"]: CANNOT FIND \"RefractoryParamN\" IN PARAMETER MAP"<<endl;
		return false;
	}

	if(paramMap.count(string("CalciumIncreaseAmnt")))
		calciumIncreaseAmnt = paramMap[string("CalciumIncreaseAmnt")];
	else{
		cerr<<"STDP1Neuron ["<<neuronID<<"]: CANNOT FIND \"CalciumIncreaseAmnt\" IN PARAMETER MAP"<<endl;
		return false;
	}

	if(paramMap.count(string("CalciumDecayRate")))
		calciumDecayRate = paramMap[string("CalciumDecayRate")];
	else{
		cerr<<"STDP1Neuron ["<<neuronID<<"]: CANNOT FIND \"CalciumDecayRate\" IN PARAMETER MAP"<<endl;
		return false;
	}

	if(paramMap.count(string("MinPostsynapticPotential")))
		minPostsynapticPotential = paramMap[string("MinPostsynapticPotential")];
	else{
		cerr<<"STDP1Neuron ["<<neuronID<<"]: CANNOT FIND \"MinPostsynapticPotential\" IN PARAMETER MAP"<<endl;
		return false;
	}

	if(paramMap.count(string("Learning"))){
		if(paramMap[string("Learning")] == 1.0)
			learningMode = true;
		else if(paramMap[string("Learning")] == 0.0)
			learningMode = false;
		else{
			cerr<<"STDP1Neuron ["<<neuronID<<"]: \"Learning\" HAS INCORRECT VALUE IN PARAMETER MAP: "<<paramMap[string("Learning")]<<endl;
			return false;
		}
	}
	else{
		cerr<<"STDP1Neuron ["<<neuronID<<"]: CANNOT FIND Learning IN PARAMETER MAP"<<endl;
		return false;
	}

	#ifdef NEURON_PARAMETERS_DEBUG
		printParameters();
	#endif//NEURON_PARAMETERS_DEBUG

	//All parameters have been set ok
	return true;
}


//----------------------------------------------------------------------------------------
//----------------------------------- PRIVATE METHODS ------------------------------------
//----------------------------------------------------------------------------------------

/*! Calculates the current state of the membrane potential, taking the refractory period into
	account. This is generally called just before calculating the final state of the neuron
	to evaluate whether it should fire, but in learning mode, need to calculate the membrane
	potential prior to processing the spike if it is a voltage dependent learning rule.
	NOTE this may create problems if a large number of positive synapses are always processed
	prior to a large number of negative synapses. */
void STDP1Neuron::calculateMembranePotential(){
	#ifdef NEURON_METHOD_DEBUG
		cout<<"STDP1Neuron ["<<neuronID<<"]:Calculating membrane potential."<<endl;
	#endif//NEURON_METHOD_DEBUG

	/* In refractory period neuron cannot fire so membrane potential is zero
		Ignoring spikes that arrive within refractory period by resetting pspTotal. */
	if(timeSinceLastFire <= refractoryPeriod_millisec){
		#ifdef NEURON_MODEL_DEBUG
			cout<<"Time "<<currentTime<<" In refractory period: NeuronID = "<<neuronID<<"; timeSinceLastFire = "<<timeSinceLastFire<<endl;
		#endif//NEURON_MODEL_DEBUG

		//Reset state of neuron during refractory period
		pspTotal = 0;
		membranePotential = 0;

		return;
	}

	/* Calculate membrane potential. This is the total contributions of previous spikes combined
		with the refractory nature of the neuron */
	membranePotential  = pspTotal - exp ( refractoryParam_n - pow(timeSinceLastFire, refractoryParam_m));


	//Output debugging information
	#ifdef NEURON_MODEL_DEBUG
		cout<<"Time "<<currentTime<<" Calculating membrane potential: NeuronID = "<<neuronID<<"; timeSinceLastFire = "<<timeSinceLastFire<<" pspTotal = " <<pspTotal<<" membranePotential = "<<membranePotential<<"; refractory period = "<<refractoryPeriod_millisec<<"; param N = "<<refractoryParam_n<<" param M = "<<refractoryParam_m<<"; threshold = "<<threshold;
		cout<<" Last update time "<<lastUpdateTime<<" current time "<<currentTime<<endl;
	#endif//NEURON_MODEL_DEBUG
}


/*! Prints out the parameters for debugging. */
void STDP1Neuron::printParameters(){
	cout<<"============================ STDP1 PARAMETERS =================================="<<endl;
	cout<<"Threshold = "<<threshold<<endl;
	cout<<"Refractory period = "<<refractoryPeriod_millisec<<endl;
	cout<<"Membrane time constant = "<<membraneTimeConstant_millisec<<endl;
	cout<<"Refractory parameter M = "<<refractoryParam_m<<endl;
	cout<<"Refractory parameter N = "<<refractoryParam_n<<endl;
	cout<<"CalciumIncreaseAmnt = "<<calciumIncreaseAmnt<<endl;
	cout<<"CalciumDecayRate = "<<calciumDecayRate<<endl;
	cout<<"Learning mode = "<<learningMode<<endl;
	cout<<"================================================================================="<<endl;
}


/*! Called to retrospectively calculate how the neuron has changed since its last update.
	Should be called before carrying out any changes on the neuron's state and before
	reporting any parameters of the neuron. */
void STDP1Neuron::update(){
	#ifdef NEURON_METHOD_DEBUG
		cout<<"STDP1Neuron ["<<neuronID<<"]: Updating."<<endl;
	#endif//NEURON_METHOD_DEBUG

	//Update the current time
	currentTime = simulationClock->getSimulationTime();

	if(lastUpdateTime == currentTime)//Check to see if neuron has been updated already
		return;


	/* Update the time interval since the neuron last fired */
	timeSinceLastFire = currentTime - neuronFireTime;
	//In debug mode run an extra check that the time interval is not negative
	#ifdef NEURON_MODEL_DEFBUG
		if(timeSinceLastFire < 0.0){
			cerr<<"STDP1Neuron ["<<neuronID<<"]: TIME SINCE LAST NEURON FIRE SHOULD NOT BE LESS THAN ZERO"<<endl;
			return;
		}
	#endif//NEURON_MODEL_DEBUG


	/* Decay post synaptic total from spikes received in previous time steps
		-1 * (currentTime - lastUpdateTime) is the same as (lastUpdateTime - currentTime), 
		which should be a negative number */
	pspTotal = pspTotal * exp( (lastUpdateTime - currentTime) / membraneTimeConstant_millisec );

	//Calculate the current calcium potential
	calciumConc *= exp( (lastUpdateTime - currentTime) / calciumDecayRate );

	//Record the last update time
	lastUpdateTime = currentTime;
}


