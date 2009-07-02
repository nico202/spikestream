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

#ifndef SPIKESTREAMSIMULATION_H
#define SPIKESTREAMSIMULATION_H

//SpikeStream includes
#include "DBInterface.h"
#include "Neuron.h"
#include "Synapse.h"
#include "TaskHolder.h"
#include "DeviceManager.h"
#include "PatternManager.h"
#include "ClassLoader.h"
#include "SimulationClock.h"
#include "PerformanceTimer.h"

//Other includes
#include <stack>
#include <google/dense_hash_map> 
using HASH_NAMESPACE::hash;
using GOOGLE_NAMESPACE::dense_hash_map;


/*! Struct to hold messages that arrive early out of sequence.*/
struct EarlyMessage {
	int senderTID;
	int bufferID;
	int timeStep;
};


/*! Struct to hold statistics about simulation.*/
struct Statistics {
	int startTimeStep;
	int neuronFireTotal;
	int spikeTotal;
};


//---------------------- SpikeStream Simulation ----------------------------
/*! The main class for the control of the simulation. This class is spawned 
	from an initiating parent class which acts as the central node for the 
	simulation. Each instance of this class is responsible for simulating 
	part of the neural network - generally a neuron group. */

/* FIXME AT SOME POINT SPLIT THIS HUGE CLASS UP INTO SUB CLASSES, SUCH AS
	NEURON MANAGER, SYNAPSE MANAGER MESSAGE HANDLER ETC. */
//--------------------------------------------------------------------------

class SpikeStreamSimulation {

	public:
		SpikeStreamSimulation();
		SpikeStreamSimulation(int argc, char **argv);
		~SpikeStreamSimulation();
		static unsigned int getNeuronGrpID();
		static void systemError(const char* message);
		static void systemError(const string &message);
		static void systemError_double(const string &message, double messageData1);
		static void systemError_int(const string &message, int messageData1);
		static void systemError(const char *message, int messageData1);
		static void systemInfo(const char *message);
		static void systemInfo(const char *message, bool messageData1);
		static void systemInfo(const char *message, int messageData1);


		//======================== PUBLIC VARIABLES ======================
		/*! Clock for the simulation.*/
		static SimulationClock *simulationClock;

	
		#ifdef RECORD_STATISTICS

			/*! Period in microseconds in over which statistics are gathered 
				before simulation is stopped.*/
			#define STATS_MONITORING_PERIOD 300000000

			/*! Struct to hold statistics.*/
			static Statistics statistics;

			/*! Timer for monitoring period.*/
			PerformanceTimer statisticsTimer;

		#endif//RECORD_STATISTICS
		
	

	private:
		//========================= VARIABLES ============================
		/*! Task ID of this task.*/
		static int thisTaskID;
		
		/*! Task ID of the task that spawned this task.*/
		static int parentTaskID;

		/*! In the run method this controls whether the simulation should stop.*/
		bool stop;

		/*! When the simulation has an error it enters error state, waits 
			for an exit message and then exits. This is to enable the Simulation 
			Manager to clean up properly.*/
		static bool errorState;
		
		/*! Records whether all of the simulation data has been loaded.*/
		bool simulationDataLoaded;

		/*! Records whether the initial messages have been sent so that 
			simulation can keep running.*/
		bool simulationStarted;

		/*! Records whether the simulation is running or not.*/
		bool simulationRunning;
		
		/*! ID of the neuron group that is being simulated by this task.*/
		static unsigned int neuronGrpID;
		
		/*! The type of neuron that is being simulated. This corresponds
			to an entry in the NeuronTypes database and a class library
			dynamically loaded at run time.*/
		unsigned short neuronType;
		
		/*! Map holding all of the synapses. The key in the outer dense_hash_map
			is the from neuron ID. The key in the inner dense_hash_map is the to 
			neuron id. References to neurons are held within each synapse.*/
		dense_hash_map<unsigned int, dense_hash_map<unsigned int, Synapse*, hash<unsigned int> >*, hash<unsigned int> > synapseMap;

		/*! Map holding the synapse parameters for each of the connection groups
			Key is the connection group id.*/
		map<unsigned int, map<string, double>* > connParameterMap;
		
		/*! Array of all the neurons - held as references
			Although the neuron references are also held in the synapses they need to be 
			updated once all the post synaptic membrane potentials have been calculated.*/
		Neuron** neuronArray;
		
		/*! Records the id of the first neuron in the group. Used to access a
			neuron by neuronID in the neuronArray.*/
		unsigned int startNeuronID;
		
		/*! Size of the neuron array.*/
		unsigned int numberOfNeurons;

		/*! Width of the neuron group being simulated by this task.*/
		unsigned int neuronGrpWidth;

		/*! Length of the neuron group being simulated by this task.*/
		unsigned int neuronGrpLength;

		/*! Keeps track of how many spike messages have been received. The spike list is sent
			when all spike messages have been received from other tasks.*/
		unsigned int spikeMessageCount;

		/*! The sum of the taskIDs of the tasks that will be sending spike messages 
			to this task within each update cycle.*/
		unsigned int spikeMessageTotal;
		
		/*! Map holding references to all the task holders that handle spike
			messages. Each task holder class is responsible for sending spikes 
			to a particular task and any additional monitoring tasks
			as well. Neurons hold a list of spike task holders that they use to 
			add their spikes to the spike buffers held in this class.
			The key of this map is the destinationTaskID.
			The data of this map is the TaskHolder that sends messages.*/
		map<int, TaskHolder*> spikeTaskHolderMap;

		/*! Reference to a neuron task holder that holds the list of firing
			neurons in this simulation and sends messages to any task that
			want this information.*/
		NeuronTaskHolder *neuronTaskHolder;
		
		/*! Holds the link between a connection group and a task.
			The key is the connGrpID; the data is the taskID.*/
		map<unsigned int, int> taskConnGrpMap;

		/*! Holds the link between neuronGrpID and task ID
			The key is the neuronGrpID; the data is the taskID.*/
		map<unsigned int, int> taskNeurGrpMap;

		/*! Holds the start neuron ID for each task processing neurons 
			The key is the task id, the data is the start neuron id of that
			neuron group.*/
		map<int, unsigned int> startNeurIDTaskMap;

		/*! Defines what type of simulation is being run in this task.
			These are defined in SimulationTypes.h.*/
		unsigned int simulationType;
		
		//References to the classes that handle database access
		DBInterface *networkDBInterface;
		DBInterface *patternDBInterface;
		DBInterface *deviceDBInterface;

		/*! Reference to class for handling devices.*/
		DeviceManager *deviceManager;

		/*! Reference to class for handling patterns.*/
		PatternManager *patternManager;

		/*! Reference to class loader, which is used to create new neurons 
			and synapses by dynamically loading them from libraries.*/
		ClassLoader *classLoader;

		/*! How many time steps each pattern will be presented for.*/
		unsigned int timeStepsPerPattern;
		
		/*! Used for synchronization - sets whether this layer has
			delayed itself in the last time step.*/
		bool delayInLastTimeStep;

		/*! Holds the messages received one time step out of sequence.*/
		stack<EarlyMessage> earlyMessageStack;

		/*! Controls how the task id is displayed.*/
		static const bool printTIDHex = true;

		/*! Map holding all the neurons that have received a spike during this timestep
			Neurons in this map are updated when all spikes have been received 
			Map is used rather than vector to avoid filtering out multiple spikes
			to the same neuron. The key is the full neuronID.*/
		dense_hash_map<unsigned int, bool, hash<unsigned int> > neuronUpdateMap;

		/*! Used to calculate the from neuron id from spike messages
			stored here to avoid constant redeclaration.*/
		unsigned int unpkFromNeurID;

		/*! Used to calculate the to neuron id from spike messages
			stored here to avoid constant redeclaration.*/
		unsigned int unpkToNeurID;

		/*! Array to unpack the list of spikes into.*/
		unsigned int* unpackArray;

		/*! Used to get information about PVM functions. Stored here both to save
			redelcaring it and because it is sometimes not used when PVM_DEBUG is not
			set and so generates a warning.*/
		int info;

		/*! Determines whether the compute time for each timestep is calculated.*/
		bool calculateComputeTime;

		/*! Time structure to measure how long each processing loop takes.*/
		timeval startComputeTimeStruct;

		/*! Time structure to measure how long each processing loop takes.*/
		timeval endComputeTimeStruct;

		/*! Time taken for each time step.*/
		unsigned int timeStepComputeTime_us;

		/*! The minimum time step duration. Used to slow the simulation down.*/
		unsigned int minTimeStepDuration_us;

		/*! Controls whether noise is injected into the neurons.*/
		bool noiseEnabled;

		/*! Percent of neurons fired at each time step by noise function.*/
		double percentNeurons_noise;

		/*! Set to true when the percentage of neurons selected for noise is random.*/
		bool randomPercentNoise;

		/*! Are the noise neurons fired directly or by injecting synaptic current?.*/
		bool directFiring_noise;

		/*! Amount of synaptic current used for neurons fired by noise.*/
		double synapticWeight_noise;

		/*! Controls whether all neuron classes are updated at each time step.*/
		bool updateAllNeurons;

		/*! Controls whether all synapse classes are updated at each time step.*/
		bool updateAllSynapses;

		/*! Controls whether we are sending neuron data back to the main application
			for monitoring purposes.*/
		bool neuronMonitorMode;

		/*! The neurons that are being monitored. Use map to filter out duplicates.*/
		dense_hash_map<unsigned int, bool, hash<unsigned int> >  neuronMonitorMap;

		/*! Controls whether we are sending synapse data back to the main application
			for monitoring purposes.*/
		bool synapseMonitorMode;

		/*! Map holding reference to the synapses that are being monitored. The key is 
			an unsigned int	array that is dynamically allocated on the heap.*/
		map<unsigned int*, Synapse*, synapseKeyCompare> synapseMonitorMap;


		//======================== METHODS ==============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		SpikeStreamSimulation (const SpikeStreamSimulation&);

		/*! Declare assignment private so it cannot be used.*/
		SpikeStreamSimulation operator = (const SpikeStreamSimulation&);

		void addReceivingTask_Neurons(int taskID);
		void addReceivingTask_Spikes(int taskID);
		void addTaskIDToDatabase();
		void cleanUpSimulation();
		void fireRandomNeurons(double noiseAmount);
		void fireRandomNeurons_synaptic(double noiseAmount);
		void fireSpecifiedNeurons();
		double getRandomPercentage();
		void injectNoise();
		bool loadGlobalParameters();
		bool loadNeuronParameters();
		void loadNeurons();
		bool loadNoiseParameters();
		void loadPreSynapticNeuronMaps();
		void loadSimulationData();
		bool loadSynapseParameters();
		void loadSynapses();
		void loadTasks();
		void printSynapseMap();
		static void printTID();
		static void printTID(int taskID);
		void processSpikeList(unsigned int senderTID, int msgTimeStep);
		void reloadWeights();
		void removeReceivingTask_Neurons(int removeTaskID);
		void removeReceivingTask_Spikes(int removeTaskID);
		void run();
		void saveViewWeights();
		void saveWeights();
		static bool sendMessage(int taskID, int msgtag);//Static so it can be called from systemError()
		static bool sendMessage(int taskID, int msgtag, const char* msgdata);//Static so it can be called from systemError()
		static bool sendMessage(int taskID, int msgtag, unsigned int data1, const char* msgdata);
		bool sendMessage(int taskID, int msgtag, unsigned int data1, unsigned int data2, const char* msgdata);
		bool sendNeuronData(int taskID, int msgtag, unsigned int neuronID, double* dataArray, int arrayLength);
		void sendSpikeMessages();
		bool sendSynapseData(int taskID, int msgtag, unsigned int fromNeurID, unsigned int toNeurID, double* dataArray, int arrayLength);
		void setDelayInLastTimeStep(bool dly);
		void setMaxBufferSize();
		void setMinTimeStepDuration(int senderTID);
		void setUpdateMode(int senderTID);
		void startNeuronMonitoring(int senderTID, bool restart);
		void startSimulation();
		void startSynapseMonitoring(int senderTID, bool restart);
		void stepSimulation();
		void stopSimulation();
		void stopNeuronMonitoring(int senderTID);
		void stopSynapseMonitoring(int senderTID);

};


#endif//SPIKESTREAMSIMULATION_H

