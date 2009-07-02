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
#include "Synapse.h"
//#include "Debug.h"

//Other includes
#include <iostream>
using namespace std;


/*! Construtor. */
Synapse::Synapse(){
	//Initialise monitoring data
	monitorData.dataArray = NULL;
	monitorData.length = 0;
}


/*! Destructor. */
Synapse::~ Synapse(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING SYNAPSE"<<endl;
	#endif//MEMORY_DEBUG		
}


//------------------------------------------------------------------------
//------------------------- PUBLIC METHODS -------------------------------
//------------------------------------------------------------------------

/*! Returns a MonitorData structure containing a pointer to an array containing the current values 
	of the monitored data items in the same order as they were listed in the XML file. This data
	structure also contains the length of the array. */
MonitorData* Synapse::getMonitoringData(){
	return &monitorData;
}

/*! Returns a string containing the data that is output by this neuron in monitoring mode in XML format.
	The implementation in Neuron does not contain any monitored parameters. Overload this method if you 
	want to send actual information back. */
string Synapse::getMonitoringInfo(){
	string xmlString = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
	xmlString += "<monitor_info>";
	xmlString += "</monitor_info>";
	return xmlString;
}


/*! Returns the id of the neuron that connects to this synapse
	from another task. This can be used to access the synapse
	from the synapse map. */
unsigned int Synapse::getPresynapticNeuronID(){
	return preSynapticNeuronID;
}


/*! Sets the weight of this synapse. Public to allow it to be dynamically
	modified. */
void Synapse::setWeight(double wei){
	weight = wei;

	#ifdef LOAD_SYNAPSE_DEBUG
		if(weight > MAX_DOUBLE_WEIGHT || weight < MIN_DOUBLE_WEIGHT)
			cerr<<"Synapse: WEIGHT IS OUT OF RANGE: "<<weight<<endl;
	#endif //LOAD_SYNAPSE_DEBUG
}


//------------------------------------------------------------------------
//-------------------------- PRIVATE METHODS -----------------------------
//------------------------------------------------------------------------

/*! Prints out information about this synapse for debugging. */
void Synapse::print(){
	cout<<"["<<preSynapticNeuronID<<"->"<<postSynapticNeuron->getNeuronID()<<": "<<weight<<"]";
}


/*! Stores a reference to a map containing parameters for this synapse. */
void Synapse::setParameterMapReference(map<string, double>* paramMap){
	parameterMap = paramMap;
}


/*! Sets the neuron that this synapse connects to. */
void Synapse::setPostSynapticNeuron(Neuron *neuron){
	postSynapticNeuron = neuron;
}


/*! Sets the id of the neuron that connects to this synapse
	from another task. This can be used to access the synapse
	from the synapse map. */
void Synapse::setPreSynapticNeuronID(unsigned int preSynNeurID){
	preSynapticNeuronID = preSynNeurID;
}


/*! Passes a reference to the simulation clock. */
/* FIXME THIS WOULD BE BETTER STATIC, BUT HAD LINKING PROBLEMS
	WITH DYNAMIC LIBRARIES, WHICH WOULD NEED TO BE SOLVED FIRST. */
void Synapse::setSimulationClock(SimulationClock* simClock){
	simulationClock = simClock;
}

