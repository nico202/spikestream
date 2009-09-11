/***************************************************************************
 *   SpikeStream Library                                                   *
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

//--------------------------  Global Variables  ----------------------------
/*! Definitions of variables that apply across the whole application. */
//--------------------------------------------------------------------------

#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

//Other includes
#include <cstring>


/*! Defines of the different archive types. 
	A firing neurons archive records the firing state of the neurons at any point in time
	This is different from recording the spikes sent out from a neuron group, which are 
	generally spread out in time because of the delay.*/
#define FIRING_NEURON_ARCHIVE 1
#define SPIKE_ARCHIVE 10


/*! Defines of the firing mode of a device. Input spikes from an external device can either
	fire the neuron directly or they can inject synaptic current into the neuron.
	When this value is set between -1.0 and 1.0 it is synaptic input mode. */
#define INPUT_DIRECT_FIRING_MODE 100.0
#define OUTPUT_FIRING_MODE 200.0


/*! Reserved neuron ids used to initialise dense_hash_map.
	Auto increment always starts with 10 so a neuron ID of zero or 1 should not occur.*/
#define EMPTY_NEURON_ID_KEY 0
#define DELETED_NEURON_ID_KEY 1


/*! The maximum length of a name in the database.*/
#define MAX_DATABASE_NAME_LENGTH 250

/*! The maximum length of a name in the database.*/
#define MAX_DATABASE_DESCRIPTION_LENGTH 250


/*! Monitor windows are added in the constructor.
	This is the size of the monitorWindowArray: this number of 
	dock windows will be added to the dock area.*/
#define MAX_NUMBER_MONITOR_WINDOWS 100


/*! The maximum number of spikes that can be received in a single packet
	This is the size of the receive array.*/
#define MAX_NUMBER_OF_SPIKES 1000000


/*! Maximum number of attempts to reconnect to the database. */
#define MAX_NUMBER_RECONNECT_ATTEMPTS 10


/*! File extension for saving archive file of databases.*/
#define NEURON_APPLICATION_FILE_EXTENSION ".sql.tar.gz"


/*! The number of delay values available to the simulation.
	These are created using a buffer to hold the spikes.*/
#define NUMBER_OF_DELAY_VALUES 256


/*! Number used to code for the percentage of neurons being selected
	randomly for the noise at each timestep. Used when maximum entropy
	is required in the layer. */
#define RANDOM_PERCENT_NEURONS_NOISE 1000

/*! First valid automatically generated ID in NeuralNetwork database */
#define START_NEURALNETWORK_ID 1


/*! First valid automatically generated ID in NeuralNetwork database */
#define START_NEURONGROUP_ID 1


/*! First valid automatically generated ID in NeuralNetwork database */
#define START_NEURON_ID 1

/*! First valid automatically generated ID in NeuralNetwork database */
#define START_CONNECTION_ID 1


/*! First valid automatically generated ID in NeuralNetwork database */
#define START_CONNECTIONGROUP_ID 1


/*! Default threshold for lazy loading */
#define LAZY_LOADING_THRESHOLD 10000

/*! Commenting or uncommenting this define determines whether 
	statistics are recorded for the simulation.*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//#define RECORD_STATISTICS
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/*! Function for comparing two char keys in a hash map.*/
struct charKeyCompare{
  bool operator()(const char* key1, const char* key2) const{
    return strcmp(key1, key2) < 0;
  }
};


/*! Comparison function for synapseMonitorMap.
	Need a comparison function that returns true if s1 is less than s2, and false otherwise.
	This is made similar to an alphabetical comparison so that if the first byte in s1 is less than
	the first byte in s2. the whole int[] s1 is rated as less than s2.*/
struct synapseKeyCompare {
	bool operator()(const unsigned int* key1, const unsigned int* key2) const {
		if(key1[0] < key2[0])
			return true; //key1[0] >= key2[0]
		else if (key1[0] == key2[0])
			if(key1[1] < key2[1])
				return true;//key1[0] == key2[0] && key1[1] < key2[1]
		return false;
	}
};


/*! Data structure containing monitored data from the neuron or synapse.*/
struct MonitorData {
	double* dataArray;
	int length;
};


/*! Random seed used to initialise random number generator.
	Use the same seed each time to get reproducible results.*/
static const unsigned int randomSeed = 719253438;


#endif //GLOBALVARIABLES_H

