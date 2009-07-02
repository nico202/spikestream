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
#include "SpikeStreamSimulation.h"
#include "Debug.h"
#include "PVMMessages.h"
#include "Utilities.h"
#include "SimulationTypes.h"
#include "ConnectionType.h"
#include "DeviceTypes.h"
#include "GlobalVariables.h"

//Other includes
#include "mysql++.h"
#include "pvm3.h"
#include <cmath>
#include <string>
#include <iostream>
using namespace std;
using namespace mysqlpp;


/*! Database Exception for errors connected with the database. */
class DatabaseException : public exception{
  virtual const char* what() const throw(){
    return "Database error";
  }
} databaseException;


//Declare and initialize static variables
int SpikeStreamSimulation::thisTaskID = 0;
int SpikeStreamSimulation::parentTaskID = 0;
SimulationClock* SpikeStreamSimulation::simulationClock;
bool SpikeStreamSimulation::errorState = false;
unsigned int SpikeStreamSimulation::neuronGrpID = 0;

#ifdef RECORD_STATISTICS
	Statistics SpikeStreamSimulation::statistics;
#endif//RECORD_STATISTICS


/*! Default constructor - NOT USED. */
SpikeStreamSimulation::SpikeStreamSimulation(){
}


/*! Main constructor.
	Do not return from construction during an error because we must reach
	the run() call so that simulation manager can cleanly shut the task down
	when there is an error. */
SpikeStreamSimulation::SpikeStreamSimulation(int argc, char **argv){
	/* Initialize the random number generator using the current time in microseconds
		Since each task starts at a slightly different time, this ensures that random
		activity in the different layers is independent. */
	timeval randomTimeStruct;
	gettimeofday(&randomTimeStruct, NULL);
	srand(randomTimeStruct.tv_usec);

	//Create the simulation clock
	simulationClock = new SimulationClock();

	//Create the array that will be used to receive the spikes
	unpackArray = new unsigned int[MAX_NUMBER_OF_SPIKES];

	//Set up synapse hash map by setting empty and deleted keys
	synapseMap.set_empty_key(EMPTY_NEURON_ID_KEY);
	synapseMap.set_deleted_key(DELETED_NEURON_ID_KEY);

	//Set up neuron update map by satting empty and deleted keys
	neuronUpdateMap.set_empty_key(EMPTY_NEURON_ID_KEY);
	neuronUpdateMap.set_deleted_key(DELETED_NEURON_ID_KEY);

	//Set up neuron update map by satting empty and deleted keys
	neuronMonitorMap.set_empty_key(EMPTY_NEURON_ID_KEY);
	neuronMonitorMap.set_deleted_key(DELETED_NEURON_ID_KEY);

	//Initialise variables
	delayInLastTimeStep = false;
	calculateComputeTime = false;
	neuronMonitorMode = false;
	synapseMonitorMode = false;
	
	//Set simulationDataLoaded to false
	simulationDataLoaded = false;

	//Set simulation running to false
	simulationRunning = false;
	
	//Simulation started is set to true later, when initial messages are sent from all the neuron groups
	simulationStarted = false;

	//Initialise time structs used to calculate how long each time step lasts
	gettimeofday(&startComputeTimeStruct, NULL);
	gettimeofday(&endComputeTimeStruct, NULL);
	minTimeStepDuration_us = 0;//Runs at maximum speed by default

	//Initialise noise parameters
	noiseEnabled = false;
	percentNeurons_noise = 0;
	directFiring_noise = true;
	synapticWeight_noise = 0;
	randomPercentNoise = false;

	//Default update mode is event driven
	updateAllNeurons = false;
	updateAllSynapses = false;

	//Get task id and parent task id
	thisTaskID = pvm_mytid();
	parentTaskID = pvm_parent();

	/* Set up direct routing, which increases the available bandwidth.
		NOTE This is not scalable to more than 60 tasks */
	pvm_setopt( PvmRoute, PvmRouteDirect );

	/* Read command line arguments. 
		These include host, username and password for database and NeuronGrpID
		Use options: -n [neuronGrpID] -h [hostname] -d [database] -u [username]  -p [password] 
		with spaces in between to avoid strict ordering. argv[0] is the executable name, so start
		from 1.*/
	char *neuralNetworkHost = NULL, *neuralNetworkUser = NULL, *neuralNetworkPassword = NULL, *neuralNetworkDatabase = NULL;
	char *patternHost = NULL, *patternUser = NULL, *patternPassword = NULL, *patternDatabase = NULL;
	char *deviceHost = NULL, *deviceUser = NULL, *devicePassword = NULL, *deviceDatabase = NULL;
	for(int i=1; i<argc; i += 2){
		if(argv[i][0] == '-'){//This line contains an option
			if(Utilities::cStringEquals(argv[i], "-ng", 4)){//Extract information about neuron group
				neuronGrpID = Utilities::getUInt(argv[i+1]);
			}
			else if(Utilities::cStringEquals(argv[i], "-nnh", 4)){//Extract neural network host information
				neuralNetworkHost = argv[i+1];
			}
			else if(Utilities::cStringEquals(argv[i], "-nnu", 4)) {//Extract neural network user information
				neuralNetworkUser = argv[i+1];
			}
			else if(Utilities::cStringEquals(argv[i], "-nnp", 4)){// Extract neural network password information
				neuralNetworkPassword = argv[i+1];
			}
			else if(Utilities::cStringEquals(argv[i], "-nnd", 4)){// Extract neural network database information
				neuralNetworkDatabase = argv[i+1];
			}
			else if(Utilities::cStringEquals(argv[i], "-ph", 4)){//Extract pattern host information
				patternHost = argv[i+1];
			}
			else if(Utilities::cStringEquals(argv[i], "-pu", 4)) {//Extract pattern user information
				patternUser = argv[i+1];
			}
			else if(Utilities::cStringEquals(argv[i], "-pp", 4)){// Extract pattern password information
				patternPassword = argv[i+1];
			}
			else if(Utilities::cStringEquals(argv[i], "-pd", 4)){// Extract pattern database information
				patternDatabase = argv[i+1];
			}
			else if(Utilities::cStringEquals(argv[i], "-dh", 4)){//Extract device host information
				deviceHost = argv[i+1];
			}
			else if(Utilities::cStringEquals(argv[i], "-du", 4)) {//Extract device user information
				deviceUser = argv[i+1];
			}
			else if(Utilities::cStringEquals(argv[i], "-dp", 4)){// Extract device password information
				devicePassword = argv[i+1];
			}
			else if(Utilities::cStringEquals(argv[i], "-dd", 4)){// Extract device database information
				deviceDatabase = argv[i+1];
			}
		}
		else{
			systemError("SpikeStreamSimulation: Invalid command line arguments!");
		}
	}

	//Check that all parameters have been initialised
	if( neuralNetworkHost == NULL || neuralNetworkUser == NULL || neuralNetworkPassword == NULL || neuralNetworkDatabase == NULL || patternHost == NULL || patternUser == NULL || patternPassword == NULL || patternDatabase == NULL || deviceHost == NULL || deviceUser == NULL || devicePassword == NULL || deviceDatabase == NULL){
		systemError("SpikeStreamSimulation: SOME OR ALL OF THE COMMAND LINE PARAMETERS ARE MISSING");
	}

	//Output database parameters in debug mode
	#ifdef COMMAND_LINE_PARAMETERS_DEBUG
		cout<<"Neural network database parameters: "<<neuralNetworkHost<<", "<<neuralNetworkUser<<", "<<neuralNetworkPassword<<", "<<neuralNetworkDatabase<<endl;
		cout<<"Pattern database parameters: "<<patternHost<<", "<<patternUser<<", "<<patternPassword<<", "<<patternDatabase<<endl;
		cout<<"Device database parameters: "<<deviceHost<<", "<<deviceUser<<", "<<devicePassword<<", "<<deviceDatabase<<endl;
	#endif//COMMAND_LINE_PARAMETERS_DEBUG

	//Create a new neural network database interface and connect to database
	networkDBInterface = new DBInterface(neuralNetworkHost, neuralNetworkUser, neuralNetworkPassword, neuralNetworkDatabase);
	if(!networkDBInterface->connectToDatabase(true)){//Exceptions enabled
		systemError("Failed to connect to the neural network database.");//This sets up the error state, so will only respond to exit message now
	}

	//Create a new pattern database interface and connect to database
	patternDBInterface = new DBInterface(patternHost, patternUser, patternPassword, patternDatabase);
	if(!patternDBInterface->connectToDatabase(true)){//Exceptions enabled
		systemError("Failed to connect to the pattern database.");//This sets up the error state, so will only respond to exit message now
	}

	//Create a new device database interface and connect to database
	deviceDBInterface = new DBInterface(deviceHost, deviceUser, devicePassword, deviceDatabase);
	if(!deviceDBInterface->connectToDatabase(true)){//Exceptions enabled
		systemError("Failed to connect to the device database.");//This sets up the error state, so will only respond to exit message now
	}
	
	//Add the task id of this task to the database to enable communication between tasks
	if(!errorState)
		addTaskIDToDatabase();

	//Create new class loader to load neuron and synapse classes
	if(!errorState)
		classLoader = new ClassLoader(networkDBInterface);

	/*Send confirmation message to parent, which will not check database messages until 
		confirmation from all tasks has been received. */
	sendMessage(parentTaskID, SIMULATION_TASK_STARTED_MSG);

	//Give user feedback about initialization of task
	cout<<"Simulation task started for neuron group "<<neuronGrpID<<" with taskID "<<thisTaskID<<endl;

	//Start the run method for this class	
	stop = false;
	run();
}


/*! Destructor. */
SpikeStreamSimulation::~SpikeStreamSimulation(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING NEURON SIMULATION. TASK ID: "; printTID(thisTaskID); cout<<endl;
	#endif//MEMORY_DEBUG

	//Output statistics about the simulation run if required
	#ifdef RECORD_STATISTICS
		cout<<"=========================== STATISTICS ==============================="<<endl;
		cout<<"      Total number of neurons fired: "<<statistics.neuronFireTotal<<endl;
		cout<<"      Total number of spikes: "<<statistics.spikeTotal<<endl;
		cout<<"      Number of time steps: "<<(simulationClock->getTimeStep() - statistics.startTimeStep)<<endl;
		cout<<"      "; statisticsTimer.printTime();
		cout<<"=========================== /STATISTICS ==============================="<<endl;
	#endif//RECORD_STATISTICS
	
	//Delete simulation clock
	delete simulationClock;

	/* Delete device manager: in all simulation modes there will be a device manager 
                to delete, which needs to be deleted before the databases are disconnect1654d */
	delete deviceManager;

	//Delete pattern manager if it is a pattern simulation
	if(simulationType == PATTERN_SIMULATION)
		delete patternManager;

	/* Delete database interfaces. Some of these may have been deleted
		during load time to save resources, so need to check for this */
	if(networkDBInterface != NULL)
		delete networkDBInterface;
	if(patternDBInterface != NULL)
		delete patternDBInterface;
	if(deviceDBInterface != NULL)
		delete deviceDBInterface;

	//Delete the class loader
	delete classLoader;

	//Delete neurons
	for(unsigned int i=0; i<numberOfNeurons; ++i)
		delete neuronArray[i];
	delete [] neuronArray;

	//Delete task holders
	delete neuronTaskHolder;
	for(map<int, TaskHolder*>::iterator iter = spikeTaskHolderMap.begin(); iter != spikeTaskHolderMap.end(); ++iter)
		delete iter->second;

	//Delete unpack array
	delete [] unpackArray;

	//Delete synapses and synapse keys
	for( dense_hash_map<unsigned int, dense_hash_map<unsigned int, Synapse*, hash<unsigned int> >*, hash<unsigned int> >::iterator outerIter = synapseMap.begin(); outerIter != synapseMap.end(); ++outerIter){
		for(dense_hash_map<unsigned int, Synapse*, hash<unsigned int> >::iterator innerIter = outerIter->second->begin(); innerIter != outerIter->second->end(); ++innerIter){
			//Delete Synapse
			delete innerIter->second;
		}
		//Delete inner hash map
		delete outerIter->second;
	}
	
	//Delete dynamically allocated maps for connection parameters
	for(map<unsigned int, map<string, double>* >::iterator iter = connParameterMap.begin(); iter != connParameterMap.end(); ++iter)
		delete iter->second;

	//Inform SpikeStream Application that clean up is complete
	sendMessage(parentTaskID, TASK_EXITED_MSG);

	#ifdef MEMORY_DEBUG
		cout<<">>>>>>>>>>>>>>>>>>>>>>>>>> NEURON SIMULATION FINISHED CLEANING UP <<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
	#endif//MEMORY_DEBUG

	//Flush any remaining messages before exiting
	cout.flush();

	//Exit from pvm
	pvm_exit();
}


/*! Main run method.
	This loops around receiving and processing messages until the simulation ends. */
void SpikeStreamSimulation::run(){
	while(!stop){
		//Blocking receive - waiting for messages from other tasks
		int bufID = pvm_recv(-1, -1);
		#ifdef PVM_BUFFER_DEBUG
			if(bufID > 50){
				cout<<"Run method: BUFFER ID CHECK ON RECEIVE bufID = "<<bufID<<endl;
			}
		#endif//PVM_BUFFER_DEBUG

		#ifdef PVM_DEBUG
			if(bufID < 0){
				pvm_perror("SpikeStreamSimulation: MESSAGE RECEIVE ERROR");
				systemError("Message receive error");
				return;
			}
		#endif//PVM_DEBUG

		int bytes, msgtag, senderTID, info;
		info = pvm_bufinfo(bufID, &bytes, &msgtag, &senderTID);//Get info about message
		#ifdef PVM_DEBUG
			if(info < 0){
				pvm_perror("SpikeStreamSimulation: PROBLEM GETTING BUFFER INFO");
				systemError("Problem getting buffer info.");
				return;
			}
		#endif//PVM_DEBUG

		if(errorState){//When there has been an error, task only responds to an exit msg
			if(msgtag == EXIT_MSG){
				#ifdef MESSAGE_DEBUG
					cout<<"Task "<<thisTaskID<<": EXIT_MSG "<<bytes<<" bytes received from "<<senderTID<<endl;
				#endif//MESSAGE_DEBUG
				stop = true;//Exits the archiver from the run loop
			}
			else{
				systemError("Task is in error state and will no longer carry out any operations.\nTermination of the simulation is recommended.");
			}
		}
		else{
			switch(msgtag){
				case (SPIKE_LIST_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "<<thisTaskID<<": SPIKE_LIST_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					processSpikeList(senderTID, -1);
				break;
				case(LOAD_SIMULATION_DATA_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": LOAD_NEURON_DATA_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					loadSimulationData();
				break;
				case(START_SIMULATION_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": START_SIMULATION_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					startSimulation();
				break;
				case(STOP_SIMULATION_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": STOP_SIMULATION_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					stopSimulation();
				break;
				case(STEP_SIMULATION_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": STEP_SIMULATION_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					stepSimulation();
				break;
				case(REQUEST_SPIKE_DATA_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": REQUEST_SPIKE_DATA_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					addReceivingTask_Spikes(senderTID);
				break;
				case(REQUEST_FIRING_NEURON_DATA_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": REQUEST_FIRING_NEURON_DATA_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					addReceivingTask_Neurons(senderTID);
				break;
				case(CANCEL_SPIKE_DATA_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": CANCEL_SPIKE_DATA_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					removeReceivingTask_Spikes(senderTID);
				break;
				case(CANCEL_FIRING_NEURON_DATA_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": CANCEL_SPIKE_DATA_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					removeReceivingTask_Neurons(senderTID);
				break;
				case(INJECT_NOISE_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": INJECT_NOISE_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					injectNoise();
				break;
				case(FIRE_SPECIFIED_NEURONS_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": FIRE_SPECIFIED_NEURONS_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					fireSpecifiedNeurons();
				break;
				case(LOAD_NEURON_PARAMETERS_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": LOAD_NEURON_PARAMETERS_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					loadNeuronParameters();
				break;
				case(LOAD_SYNAPSE_PARAMETERS_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": LOAD_SYNAPSE_PARAMETERS_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					loadSynapseParameters();
				break;
				case(LOAD_GLOBAL_PARAMETERS_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": LOAD_GLOBAL_PARAMETERS_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					loadGlobalParameters();
				break;
				case(LOAD_NOISE_PARAMETERS_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": LOAD_NOISE_PARAMETERS_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					loadNoiseParameters();
				break;
				case (EXIT_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": EXIT_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					stop = true;//Exit from run method and invoke the destructor
				break;
				case (SAVE_WEIGHTS_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": SAVE_WEIGHTS_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					saveWeights();
				break;
				case (LOAD_WEIGHTS_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": LOAD_WEIGHTS_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					reloadWeights();
				break;
				case (SAVE_VIEW_WEIGHTS_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": SAVE_VIEW_WEIGHTS_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					saveViewWeights();
				break;
				case (SET_MIN_TIMESTEP_DURATION_US_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": SET_MIN_TIMESTEP_DURATION_US_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					setMinTimeStepDuration(senderTID);
				break;
				case (SET_UPDATE_MODE_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": SET_UPDATE_MODE_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					setUpdateMode(senderTID);
				break;
				case (REQUEST_MONITOR_NEURON_INFO_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": REQUEST_MONITOR_NEURON_INFO_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					startNeuronMonitoring(senderTID, false);
				break;
				case (START_MONITORING_NEURON_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": START_MONITORING_NEURON_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					startNeuronMonitoring(senderTID, true);
				break;
				case (STOP_MONITORING_NEURON_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": STOP_MONITORING_NEURON_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					stopNeuronMonitoring(senderTID);
				break;
				case (REQUEST_MONITOR_SYNAPSE_INFO_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": REQUEST_MONITOR_SYNAPSE_INFO_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					startSynapseMonitoring(senderTID, false);
				break;
				case (START_MONITORING_SYNAPSE_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": START_MONITORING_SYNAPSE_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					startSynapseMonitoring(senderTID, true);
				break;
				case (STOP_MONITORING_SYNAPSE_MSG):
					#ifdef MESSAGE_DEBUG
						cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": STOP_MONITORING_SYNAPSE_MSG "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					#endif//MESSAGE_DEBUG
					stopSynapseMonitoring(senderTID);
				break;
				default:
					cout<<"Task "; printTID(thisTaskID); cout<<"; NeuronGroup: "<<neuronGrpID<<": *UNRECOGNIZED MESSAGE* MSGTAG = "<<msgtag<<"; size "<<bytes<<" bytes received from "; printTID(senderTID); cout<<endl;
					systemError("UNRECOGNIZED MESSAGE RECEIVED FROM ", senderTID);
				break;
			}
		}
	}
	//End of run method, need to clean up and exit pvm
	cleanUpSimulation();
}


//----------------------------------------------------------------------------
//--------------------------- PUBLIC METHODS ---------------------------------
//----------------------------------------------------------------------------

/*! Returns the ID of the neuron group that is being simulated by this task. */
unsigned int SpikeStreamSimulation::getNeuronGrpID(){
	return neuronGrpID;
}

/*! Writes an error message to the standard output and sends a message 
	to the parent task with the error message. */
void SpikeStreamSimulation::systemError(const char *message){
	errorState = true;//Put the program into error state when it stops operating and waits for exit message.
	cerr<<"SpikeStreamSimulation "; printTID(); cerr<<": "<<message<<endl;
	sendMessage(parentTaskID, ERROR_MSG, message);
}


/*! Writes an error message to the standard output and sends a message 
	to the parent task with the error message. */
void SpikeStreamSimulation::systemError(const char *message, int messageData1){
	errorState = true;//Put the program into error state when it stops operating and waits for exit message.
	ostringstream tempStr;
	tempStr<<message<<messageData1;
	cerr<<"SpikeStreamSimulation "; printTID(); cerr<<": "<<message<<messageData1<<endl;
	sendMessage(parentTaskID, ERROR_MSG, tempStr.str().data());
}


/*! Writes an error message to the standard output and sends a message 
	to the parent task with the error message. */
void SpikeStreamSimulation::systemError(const string &message){
	errorState = true;//Put the program into error state when it stops operating and waits for exit message.
	cerr<<"SpikeStreamSimulation "; printTID(); cerr<<": "<<message<<endl;
	sendMessage(parentTaskID, ERROR_MSG, message.data());
}


/*! Writes an error message to the standard output and sends a message 
	to the parent task with the error message. */
void SpikeStreamSimulation::systemError_double(const string &message, double messageData1){
	errorState = true;//Put the program into error state when it stops operating and waits for exit message.
	cerr<<"SpikeStreamSimulation "; printTID(); cerr<<": "<<message<<" "<<messageData1<<endl;
	ostringstream tempStr;
	tempStr<<message<<" "<<messageData1;
	sendMessage(parentTaskID, ERROR_MSG, tempStr.str().data());
}


/*! Writes an error message to the standard output and sends a message 
	to the parent task with the error message. */
void SpikeStreamSimulation::systemError_int(const string &message, int messageData1){
	errorState = true;//Put the program into error state when it stops operating and waits for exit message.
	cerr<<"SpikeStreamSimulation "; printTID(); cerr<<": "<<message<<" "<<messageData1<<endl;
	ostringstream tempStr;
	tempStr<<message<<" "<<messageData1;
	sendMessage(parentTaskID, ERROR_MSG, tempStr.str().data());
}


/*! Writes an information message to the standard output and sends a 
	message to the parent task with the message. */
void SpikeStreamSimulation::systemInfo(const char *message){
	cout<<message<<endl;
	sendMessage(parentTaskID, INFORMATION_MSG, message);
}


/*! Writes an information message to the standard output and sends a 
	message to the parent task with the message. */
void SpikeStreamSimulation::systemInfo(const char *message, bool messageData1){
	ostringstream tempStr;
	tempStr<<message<<messageData1;
	cout<<tempStr.str()<<endl;
	sendMessage(parentTaskID, INFORMATION_MSG, tempStr.str().data());
}


/*! Writes an information message to the standard output and sends a 
	message to the parent task with the message. */
void SpikeStreamSimulation::systemInfo(const char *message, int messageData1){
	ostringstream tempStr;
	tempStr<<message<<messageData1;
	cout<<tempStr.str()<<endl;
	sendMessage(parentTaskID, INFORMATION_MSG, tempStr.str().data());
}


//--------------------------------------------------------------------------
//------------------------- PRIVATE METHODS --------------------------------
//--------------------------------------------------------------------------

/*! Adds a task that will receive the firing neuron data from this task. */
void SpikeStreamSimulation::addReceivingTask_Neurons(int newTask){
	neuronTaskHolder->addReceivingTask(newTask);
}


/*! Adding a task to receive spikes is easy because it is just treated as another neuron group
	and sent all the spikes from this neuron group at each time step. */
void SpikeStreamSimulation::addReceivingTask_Spikes(int newTask){
	//Work through the task holders and add task to them
	for(map<int, TaskHolder*>::iterator iter = spikeTaskHolderMap.begin(); iter != spikeTaskHolderMap.end(); ++iter)
		iter->second->addReceivingTask(newTask);
}


/*! Inserts this task id into database. */
void SpikeStreamSimulation::addTaskIDToDatabase(){
	try{
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"UPDATE NeuronGroups SET TaskID = "<<thisTaskID<<" WHERE NeuronGrpID = "<<neuronGrpID;
                SimpleResult updateResult = query.execute();

		//Check that a single row has been updated
                if(!updateResult){
			systemError("SpikeStreamSimulation: FAILURE TO UPDATE DATABASE WITH TASK ID FOR NEURON GROUP ", neuronGrpID);
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		ostringstream errorStrStream;
		errorStrStream<<"Bad query when adding task ID to database: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown adding task ID to database: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
	}
}


/*! Cleans up after the simulation. */
void SpikeStreamSimulation::cleanUpSimulation(){
	cout<<"Cleaning up simulation"<<endl;
	deviceManager->closeDevice();//Close device
}


/*! Fires noiseAmount percentage of neurons directly by calling fireNeuron() on the neuron class. */
void SpikeStreamSimulation::fireRandomNeurons(double noiseAmount){
	#ifdef NOISE_DEBUG
		int numNeuronsFired = 0;
	#endif//NOISE_DEBUG

	//Calculate threshold to be applied to random number to decide if neuron fires or not
	int threshold = (int)rint(((double)RAND_MAX * (noiseAmount / 100.0)));

	//Work through neuron array firing neurons at random
	for(unsigned int i=0; i<numberOfNeurons; ++i){
		if(rand() < threshold){
			neuronArray[i]->fireNeuron();
			#ifdef NOISE_DEBUG
				++numNeuronsFired;
			#endif//NOISE_DEBUG
		}
	}
	#ifdef NOISE_DEBUG
		cout<<"SpikeStreamSimulation: Inject noise: noiseAmount = "<<noiseAmount<<"; number of neurons fired = "<<numNeuronsFired<<endl;
	#endif//NOISE_DEBUG
}


/*! Calls changeMembranePotential on noiseAmount percentage of neurons in this
	layer. This simulates a noisy input to the neuron group. */
void SpikeStreamSimulation::fireRandomNeurons_synaptic(double noiseAmount){
	#ifdef NOISE_DEBUG
		int numNeuronsChanged = 0;
	#endif//NOISE_DEBUG

	//Get a threshold that will be used to subsample the percentage of neurons
	int threshold = (int)rint(((double)RAND_MAX * (noiseAmount / 100.0)));

	//Work through neuron array changing the membrane potential of neurons at random
	for(unsigned int i=0; i<numberOfNeurons; ++i){
		if(rand() < threshold){
			neuronArray[i]->changePostSynapticPotential(synapticWeight_noise, 0);
			neuronUpdateMap[ i + startNeuronID ] = true;

			#ifdef NOISE_DEBUG
				++numNeuronsChanged;
			#endif//NOISE_DEBUG
		}
	}
	#ifdef NOISE_DEBUG
		cout<<"SpikeStreamSimulation: Fire random neurons (synaptic): noiseAmount = "<<noiseAmount<<"; number of neurons with membrane potential adjustment = "<<numNeuronsChanged<<endl;
	#endif//NOISE_DEBUG
}


/*! Fires the specified neurons. Usually called in respose to a FIRE_NEURON_MSG for debugging
	purposes. */
void SpikeStreamSimulation::fireSpecifiedNeurons(){
	int numberOfFiringNeurons;
	
	//Unpack number of neurons to be fired
	int info = pvm_upkint(&numberOfFiringNeurons, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			systemError("ERROR UNPACKING NUMBER OF NEURONS FROM MESSAGE");
			return;
		}
	#endif//PVM_DEBUG

	//Unpack neuron IDs
	unsigned int fireNeuronArray[numberOfFiringNeurons];
	info = pvm_upkuint(fireNeuronArray, numberOfFiringNeurons, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			systemError("ERROR UNPACKING NEURON IDS FROM MESSAGE");
			return;
		}
	#endif//PVM_DEBUG
	
	for(int i=0; i<numberOfFiringNeurons; ++i){
		#ifdef FIRING_NEURONS_EXTERNAL_DEBUG
			if((fireNeuronArray[i] < startNeuronID) || (fireNeuronArray[i] > (startNeuronID + numberOfNeurons))){
				systemError("SpikeStreamSimulation: FIRING NEURONS, NEURON ID OUT OF RANGE", fireNeuronArray[i]);
				return;
			}
			else
				cout<<"SpikeStreamSimulation: Firing neuron neuronID = "<<(fireNeuronArray[i] - startNeuronID)<<endl;
		#endif//FIRING_NEURONS_EXTERNAL_DEBUG
		neuronArray[fireNeuronArray[i] - startNeuronID]->fireNeuron();
	}
}


/*! Returns a random number between range low and range high. */
double SpikeStreamSimulation::getRandomPercentage(){
	return ( (double)rand() / (double)RAND_MAX ) * 100.0;
}


/*! Causes neurons to fire with random patterns in addition to any firing caused by their input 
	Message that calls this method contains a single integer specifying the noise. -1 means inject
	noise in a single neuron. Numbers above 0 give the percentage of neurons to be fired randomly. */
void SpikeStreamSimulation::injectNoise(){
	int noiseAmount;
	info = pvm_upkint(&noiseAmount, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			systemError("ERROR UNPACKING NOISE AMOUNT FROM MESSAGE");
			return;
		}
	#endif//PVM_DEBUG
	
	int ranNum_int = rand();
	int index_int;
	if(noiseAmount == -1){//Select 1 neuron at random
		double index_doub = (double)ranNum_int *((double)numberOfNeurons / (double)RAND_MAX);
		index_int = (int)rint(index_doub);
		neuronArray[index_int]->fireNeuron();
		#ifdef NOISE_DEBUG
			cout<<"SpikeStreamSimulation: Inject noise, firing neuron "<<(index_int + startNeuronID)<<endl;
		#endif//NOISE_DEBUG
	}
	else if(noiseAmount > 0 && noiseAmount <= 100){
		fireRandomNeurons((double)noiseAmount);
	}
	else{
		systemError("SpikeStreamSimulation: NOISE AMOUNT NOT RECOGNIZED");
	}
}


/*! Loads up the global parameters from the global parameters database. */
bool SpikeStreamSimulation::loadGlobalParameters(){
	try{
		//Query global parameters table
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"SELECT TimeStepDurationMS_val, RealTimeClock_val FROM GlobalParameters";
                StoreQueryResult paramResult = query.store();
	
		//Check the number of rows - should be 1. Quit if it is the wrong number
                if(paramResult.num_rows() == 0){
			systemError("Global Parameters table is empty and should have a single row");
			return false;
		}
                else if (paramResult.num_rows() > 1){
			systemError("Global Parameters table has more than one row and should only have a single row");
			return false;
		}
		Row parameterRow (*paramResult.begin());//Have checked that there is only 1 row
	
		//Real time clock and time step duration are mutually exclusive parameters
		unsigned int realTimeClock = Utilities::getUInt((std::string)parameterRow["RealTimeClock_val"]);
		if(realTimeClock == 0){
			//Set the time step duration
			double timeStepDurationMS = Utilities::getDouble((std::string)parameterRow["TimeStepDurationMS_val"]);
		
			//Set the synapse parameters for all basic synapses in this neuron group
			simulationClock->setTimeStepDuration(timeStepDurationMS);
	
			//Make sure live mode is false
			simulationClock->setLiveMode(false);
		}
		else{//Run simulation in real time
			simulationClock->setLiveMode(true);
		}
	
		//Print parameters for debug
		#ifdef GLOBAL_PARAMETERS_DEBUG
			cout<<"Time step duration (ms): "<<simulationClock->getTimeStepDuration_ms()<<"; Live mode = "<<realTimeClock<<endl;
		#endif//GLOBAL_PARAMETERS_DEBUG

		//Everything should be ok if we have reached this point
		return true;
	}
	catch (const BadQuery& er) {// Handle any query errors
		ostringstream errorStrStream;
		errorStrStream<<"Bad query when loading global parameters: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
		return false;
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown loading global parameters: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
		return false;
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown loading global parameters: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
		return false;
	}
}


/*! Loads the parameters for neurons from the database. */
bool SpikeStreamSimulation::loadNeuronParameters(){
	try{
		//Output debugging information
		#ifdef NEURON_PARAMETERS_DEBUG
			cout<<"Loading neuron parameters from table "<<classLoader->getNeuronParameterTableName(neuronType)<<endl;
		#endif //NEURON_PARAMETERS_DEBUG
	
		//Create a map to store all of the neuron parameters.
		map<string, double> parameterMap;
	
		//First need to get the names of all the parameters
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"SHOW COLUMNS FROM "<<classLoader->getNeuronParameterTableName(neuronType);
                StoreQueryResult showResult = query.store();
                for(StoreQueryResult::iterator iter = showResult.begin(); iter != showResult.end(); ++iter){
			Row showRow(*iter);
	
			//Get the column name
			string tempFieldName((std::string)showRow["Field"]);
		
			//If it is a parameter name
			if(tempFieldName.find("_val") != string::npos){
	
				//Create a key without the _val extension
				string finalFieldName(tempFieldName, 0, tempFieldName.find("_val"));
	
				//Store field name
				parameterMap[finalFieldName] = 0.0;
			}
		}
	
		//Extract the values of the parameters from the database
		bool firstParameter = true;
		query.reset();
		if(parameterMap.size() > 0){//There are parameters for this neuron type
			query<<"SELECT ";
			for(map<string, double>::iterator iter = parameterMap.begin(); iter != parameterMap.end(); ++iter){
				if(firstParameter){
					query<<iter->first<<"_val";
					firstParameter = false;
				}
				else{
					query<<", "<<iter->first<<"_val";
				}
			}
			query<<" FROM "<<classLoader->getNeuronParameterTableName(neuronType)<<" WHERE NeuronGrpID = "<<neuronGrpID;
		}
		else//No parameters for this neuron type
			query<<"SELECT NeuronGrpID FROM "<<classLoader->getNeuronParameterTableName(neuronType)<<" WHERE NeuronGrpID = "<<neuronGrpID;
                StoreQueryResult valueResult = query.store();
	
		//Check that there is just one row for this neuron group
		if(valueResult.size() != 1){
			systemError_int("PARAMETER TABLE \"" + classLoader->getNeuronParameterTableName(neuronType) + "\" DOES NOT CONTAIN AN ENTRY FOR NEURON GROUP ", neuronGrpID);
			return false;
		}
		Row parameterRow(*valueResult.begin());
		for(map<string, double>::iterator iter = parameterMap.begin(); iter != parameterMap.end(); ++iter){
			parameterMap[iter->first] = Utilities::getDouble((std::string)parameterRow[(iter->first + "_val").data()]);
		}

		/*Set the neuron parameters. Neuron parameters should all be static, but
			does not appear to be possible to have a method that is both static 
			and virtual so have made method virtual and not static, but which manipulates
			the static variables for the neuron parameters. */
		if(!neuronArray[0]->setParameters(parameterMap)){
			systemError("FAILED TO SET NEURON PARAMETERS");
			return false;
		}
	
	
		/* Inform neuron classes that their parameters have been changed. Needed in case they have
			learning related variables, for example, that need to be updated to a final state
			when learning is switched off. */
		for(unsigned int i=0; i<numberOfNeurons; ++i){
			neuronArray[i]->parametersChanged();	
		}
	
		#ifdef NEURON_PARAMETERS_DEBUG
			cout<<"Setting neuron parameters: "<<endl;
			for(map<string, double>::iterator iter = parameterMap.begin(); iter != parameterMap.end(); ++iter){
				cout<<iter->first<<" = "<<iter->second<<endl;
			}
		#endif //NEURON_PARAMETERS_DEBUG

		//Everything is ok if we have reached this point
		return true;
	}
	catch (const BadQuery& er) {// Handle any query errors
		ostringstream errorStrStream;
		errorStrStream<<"Bad query when loading neuron parameters: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
		return false;
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown loading neuron parameters: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
		return false;
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown loading neuron parameters: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
		return false;
	}
}


/*! Loads up the neurons.
	NOTE Exception handling should be done by the calling method. */
void SpikeStreamSimulation::loadNeurons(){
	Query generalQuery = networkDBInterface->getQuery();
	
	//Get information about neurons in neuron group
	generalQuery.reset();
	generalQuery<<"SELECT NeuronType, Width, Length FROM NeuronGroups WHERE NeuronGrpID = "<<neuronGrpID;
        StoreQueryResult result = generalQuery.store();
	Row neuronGrpInfoRow(*result.begin());//Should only be one row because neuronGrpID is unique
	neuronType = Utilities::getUShort(neuronGrpInfoRow["NeuronType"]);
	neuronGrpWidth = Utilities::getUInt(neuronGrpInfoRow["Width"]);
	neuronGrpLength = Utilities::getUInt(neuronGrpInfoRow["Length"]);
	
	/*Get the number of neurons in the group and the lowest neuronID in the group
		By assuming that the neuron group is a continuous number of neuronIDs starting at 
		neuronGrpStart it becomes easy to achieve random access to any neuron using the neuronID */
	generalQuery.reset();
	generalQuery<<"SELECT COUNT(NeuronID), MIN(NeuronID) FROM Neurons WHERE NeuronGrpID = "<<neuronGrpID;
	result = generalQuery.store();
	Row neurGrpPropRow(*result.begin());//Should only be one row
	numberOfNeurons = Utilities::getUInt(neurGrpPropRow["COUNT(NeuronID)"]);
	startNeuronID = Utilities::getUInt(neurGrpPropRow["MIN(NeuronID)"]);
	
	/* Create an array to hold the neurons. This contains neuron classes on the heap.
		Create a different type of neuron depending on neuronType */
	neuronArray = new Neuron*[numberOfNeurons];
	for(unsigned int i=0; i<numberOfNeurons; ++i){
		//Create the neuron
		neuronArray[i] = classLoader->getNewNeuron(neuronType);
	
		//Set up the necessary references.
		//FIXME WOULD BE BETTER DONE STATICALLY, BUT HAD SOME STRANGE PROBLEMS WITH THIS
		neuronArray[i]->setSimulationClock(simulationClock);
		neuronArray[i]->setSynapseMapPtr(&synapseMap);
		neuronArray[i]->setNeuronID(i + startNeuronID);
	}

	/* Work through the neurons that have been created and set up the connections for each one.*/
	Connection* tmpFastConnection = networkDBInterface->getNewConnection();
	if(!tmpFastConnection){
		throw databaseException;
	}
	Query fastQuery = tmpFastConnection->query();
	int oldDelay = -1, tempTaskID = -1, oldTempTaskID = -1;
	vector<int> tempTaskIDVector;
	for(unsigned short neuronNum = 0; neuronNum < numberOfNeurons; ++neuronNum){
		//Get number of connections from this neuron
		generalQuery.reset();
		generalQuery<<"SELECT COUNT(*) FROM Connections WHERE PreSynapticNeuronID = "<<(startNeuronID + neuronNum);
		result = generalQuery.store();
		Row connSizeRow(*result.begin());//Should only be one row
		unsigned int numberOfConnections = Utilities::getUInt(connSizeRow["COUNT(*)"]);
		
		//Create an array to hold these connections
		neuronArray[neuronNum]->connectionArray = new unsigned short[numberOfConnections * 2];//Deleted by Neuron class at end
		neuronArray[neuronNum]->numberOfConnections = numberOfConnections * 2;
	
		/* Work through connections and create connection holders pointing to different parts
			of the array. This query orders by ConnGrpID and then by Delay, so can work through
			each connection group and then through each list of delays */
		fastQuery.reset();
		fastQuery<<"SELECT PostSynapticNeuronID, Delay, ConnGrpID FROM Connections WHERE PreSynapticNeuronID = "<<(startNeuronID + neuronNum)<<" ORDER BY ConnGrpID, Delay";
                UseQueryResult neurConnRes = fastQuery.use();
		Row neurConnRow;
		if(neurConnRes){
			//Create a counter to access array
			int connectionCounter = 0;
		
			/* Work through all of the rows, adding entries to the connectionArray and 
				creating connection holders that point to parts of this array. These 
				connection holders are organised by task and delay */
                        while (neurConnRow = neurConnRes.fetch_row()){
                                //Retrieve details about the row
                                unsigned int postNeuronID = Utilities::getUInt(neurConnRow["PostSynapticNeuronID"]);
                                int delay = Utilities::getInt(neurConnRow["Delay"]);
                                unsigned int connGrpID = Utilities::getUInt(neurConnRow["ConnGrpID"]);

                                #ifdef LOAD_NEURON_DEBUG
                                        cout<<"Loading Connection into neuron: "<<startNeuronID + neuronNum<<"; postNeuronID = "<<postNeuronID<<"; delay = "<<delay<<"; connGrpID = "<<connGrpID<<endl;
                                #endif//LOAD_NEURON_DEBUG

                                // Get the task associated with the connection Grp ID
                                tempTaskID = taskConnGrpMap[connGrpID];

                                /* Add from entrie to connection array
                                        To compress the data record the relative position in the array and then add
                                        the start neuron ID to this value when the message is received.
                                        The start neuron ID of this task is stored in the taskNeurGrpMap */
                                neuronArray[neuronNum]->connectionArray[connectionCounter] = neuronNum;
                                neuronArray[neuronNum]->connectionArray[connectionCounter + 1 ] = postNeuronID - startNeurIDTaskMap [ tempTaskID ];

                                // Create connection holders organised by task and delay
                                if(tempTaskID == oldTempTaskID){//Add connection to existing vector in connectionMap
                                        #ifdef LOAD_NEURON_DEBUG
                                                cout<<"Adding connection to existing task"<<endl;
                                        #endif//LOAD_NEURON_DEBUG

                                        if(delay == oldDelay){//Add to existing connection holder

                                                /* Increase number of connections in current connection holder by
                                                        1. Have to increase it by 1 because this counts each pair of numbers in
                                                        each connection. This controls how many numbers are read from
                                                        the connection array when a message is sent. back() returns an
                                                        iterator to the last connectionHolder in the vector so need to
                                                        dereference this`iterator to get the pointer to the connection
                                                        holder. */
                                                (neuronArray[neuronNum]->connectionMap[tempTaskID].back()).numConnIDs++;
                                        }
                                        else{//Create a new connection holder to hold this new delay value
                                                // New connection holder
                                                ConnectionHolder tempConnHolder;

                                                /* Point the connection holder to the address of the current position
                                                        in the connection array. */
                                                tempConnHolder.connIDArray = &(neuronArray[neuronNum]->connectionArray[connectionCounter]);

                                                //Set the number of connections in the connection holder
                                                tempConnHolder.numConnIDs = 1;

                                                //Set the delay in the connection holder
                                                tempConnHolder.delay = delay;

                                                //Add the connection holder to the vector in the connectionMap for this taskID
                                                neuronArray[neuronNum]->connectionMap[tempTaskID].push_back(tempConnHolder);

                                                // Copy new delay value into old delay value
                                                oldDelay = delay;
                                        }
                                }
                                else{// New task
                                        #ifdef LOAD_NEURON_DEBUG
                                                cout<<"New task"<<endl;
                                        #endif//LOAD_NEURON_DEBUG

                                        /*Store this taskID.
                                                This gives me a list of tasks that this neuron needs to connect with. */
                                        tempTaskIDVector.push_back(tempTaskID);

                                        /*  Add a new vector to the connection map.
                                                This is done automatically with [] operator */
                                        neuronArray[neuronNum]->connectionMap[tempTaskID];

                                        // New connection holder
                                        ConnectionHolder tempConnHolder;

                                        /* Point the connection holder to the address of the current position
                                                in the connection array. */
                                        tempConnHolder.connIDArray = &(neuronArray[neuronNum]->connectionArray[connectionCounter]);

                                        //Set the number of connections in the connection holder
                                        tempConnHolder.numConnIDs = 1;

                                        //Set the delay in the connection holder
                                        tempConnHolder.delay = delay;

                                        //Add the connection holder to the vector in the connectionMap for this taskID
                                        neuronArray[neuronNum]->connectionMap[tempTaskID].push_back(tempConnHolder);

                                        //Store the old values of delay and task id
                                        oldTempTaskID = tempTaskID;
                                        oldDelay = delay;
                                }
                                /*Increase the counter that is keeping track of the connectionArray
                                        Needs to be increased by two since connection array stores both from and to
                                        neuron IDs */
                                connectionCounter += 2;
                        }
			
			/* All connections have been loaded into the neuron at neuronNum. Now need to create
				taskHolderArray for neuron. This holds references to all the task holder classes
				that will be used to send the neuron's spikes */
			neuronArray[neuronNum]->numberOfSpikeTaskHolders = tempTaskIDVector.size();
			neuronArray[neuronNum]->spikeTaskHolderArray = new TaskHolder*[tempTaskIDVector.size()];//Deleted by Neuron at clean up
			int tHCounter = 0;
			for(vector<int>::iterator iter = tempTaskIDVector.begin(); iter != tempTaskIDVector.end(); ++iter){
				neuronArray[neuronNum]->spikeTaskHolderArray[tHCounter] = spikeTaskHolderMap[*iter];
				++tHCounter;
			}

			//Set the neuronTaskHolder for the neuron
			neuronArray[neuronNum]->neuronTaskHolder = neuronTaskHolder;
			
			//Empty task ID vector so that it can be filled with task IDs for next neuron.
			tempTaskIDVector.clear();

			//Reset oldDelay and oldTempTaskID
			oldDelay = -1; 
			oldTempTaskID = -1;
		}
		else{
			cerr<<fastQuery.error()<<endl;
			systemError("CANNOT RETRIEVE NEURON GROUP DETAILS");
		}
		
		//Print out details of neuron for debugging.
		#ifdef LOAD_NEURON_DEBUG
			cout<<"Loaded neuron number: "<<neuronNum<<endl;
		#endif//LOAD_NEURON_DEBUG
	}
	//Close temporary connection
        tmpFastConnection->disconnect();
}


/*! Loads the parameters controlling noise from the NoiseParameters database. */
bool SpikeStreamSimulation::loadNoiseParameters(){
	try{
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"SELECT NeuronGrpID, NoiseEnabled, PercentNeurons, DirectFiring, SynapticWeight FROM NoiseParameters WHERE NeuronGrpID = "<<neuronGrpID;
                StoreQueryResult noiseParamsRes = query.store();
		Row noiseParamsRow(*noiseParamsRes.begin());//NeuronGrpID is unique
	
		//Extract whether noise is enabled or not
		unsigned int noiseEnabledVal = Utilities::getUInt((std::string)noiseParamsRow["NoiseEnabled"]);
		if(noiseEnabledVal == 0)
			noiseEnabled = false;
		else 
			noiseEnabled = true;
	
		//Extract the percentage of neurons to be fired
		percentNeurons_noise = Utilities::getDouble((std::string)noiseParamsRow["PercentNeurons"]);
		if(percentNeurons_noise > 100 && percentNeurons_noise != RANDOM_PERCENT_NEURONS_NOISE){
			systemError_double("NON RANDOM PERCENT OF NOISY NEURONS SHOULD NOT EXCEED 100: ", percentNeurons_noise);
			return false;
		}

		//Are we selecting a random percentage of neurons for the noise
		if(percentNeurons_noise == RANDOM_PERCENT_NEURONS_NOISE){
			randomPercentNoise = true;
		}
		else{
			randomPercentNoise = false;
		}
	
		//Extract whether neurons should be fired directly or via changeMembranePotential()
		unsigned int directFiringVal = Utilities::getUInt((std::string)noiseParamsRow["DirectFiring"]);
		if(directFiringVal == 0)
			directFiring_noise = false;
		else
			directFiring_noise = true;
		
		//Extract the synaptic weight for synaptic firing of neurons
		synapticWeight_noise = Utilities::getDouble((std::string)noiseParamsRow["SynapticWeight"]);
	
		#ifdef NOISE_DEBUG
			cout<<"NOISE PARAMETERS: percentNeurons_noise = "<<percentNeurons_noise<<"; directFiring_noise = "<<directFiring_noise<<"; synapticWeight_noise = "<<synapticWeight_noise<<endl;
		#endif//NOISE_DEBUG

		//Everything should be ok if we have reached this point
		return true;
	}
	catch (const BadQuery& er) {// Handle any query errors
		ostringstream errorStrStream;
		errorStrStream<<"Bad query when loading noise parameters: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
		return false;
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown loading noise parameters: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
		return false;
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown loading noise parameters: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
		return false;
	}
}


/*! Passes references to each neuron of the synapses
	that are connected to them. Used for some learning algorithms. */
void SpikeStreamSimulation::loadPreSynapticNeuronMaps(){
	//Work through all the synapses
	for( dense_hash_map<unsigned int, dense_hash_map<unsigned int, Synapse*, hash<unsigned int> >*, hash<unsigned int> >::iterator outerIter = synapseMap.begin(); outerIter != synapseMap.end(); ++outerIter){
		for(dense_hash_map<unsigned int, Synapse*, hash<unsigned int> >::iterator innerIter = outerIter->second->begin(); innerIter != outerIter->second->end(); ++innerIter){
			//Get neuron that this synapse connects to and add a reference to this synapse to its vector
			innerIter->second->postSynapticNeuron->preSynapseVector.push_back(innerIter->second);
		}
	}
}


/*! Loads data into simulation. */
void SpikeStreamSimulation::loadSimulationData(){
	/* First check to see if simulation data has already been loaded. This is to avoid the 
		potential problem of duplicate messages. Could code reload neuron data as a separate
		message if needed, although restart might be better. */
	if(simulationDataLoaded)
		return;

	/* Unpack the number of simulation parameters */
	int numberOfSimulationParameters;
	int info = pvm_upkint(&numberOfSimulationParameters, 1, 1);
	if(info < 0){
		systemError("ERROR UNPACKING NUMBER OF SIMULATION PARAMETERS FROM MESSAGE.");
		return;
	}

	/* Unpack the rest of the message to determine what sort of simulation this
		task should be carrying out. */
	info = pvm_upkuint(&simulationType, 1, 1);
	if(info < 0){
		systemError("ERROR UNPACKING SIMULATION TYPE FROM MESSAGE");;
		return;
	}
	
	//Unpack data relevant to a pattern simulation
	if(simulationType == PATTERN_SIMULATION){
		//Need to unpack the patternGrpId and the timeStepsPerPattern
		unsigned int patternGrpID;
		info = pvm_upkuint(&patternGrpID, 1, 1);
		if(info < 0){
			systemError("ERROR UNPACKING patternGrpID FROM MESSAGE.");
			return;
		}
		info = pvm_upkuint(&timeStepsPerPattern, 1, 1);
		if(info < 0){
			systemError("ERROR UNPACKING timeStepsPerPattern FROM MESSAGE.");
			return;
		}

		//Create the pattern manager
		patternManager = new PatternManager(networkDBInterface, patternDBInterface, neuronGrpID, patternGrpID);

		/* Create an empty device manager that does nothing. This is needed at present because of intermittent
			calls to the device manager during the simulation run, even when it is not being used. */
		deviceManager = new DeviceManager();

		//Free resources from device database
		delete deviceDBInterface;
		deviceDBInterface = NULL;

		#ifdef SIMULATION_DEBUG
			cout<<"Task: "; printTID(); cout<<": PATTERN_SIMULATION with pattern group "<<patternGrpID<<endl;
		#endif//SIMULATION_DEBUG
	}

	//Unpack data relevant to a live simulation
	else if(simulationType == LIVE_SIMULATION){
		//Unpack device id 
		unsigned int deviceID;
		info = pvm_upkuint(&deviceID, 1, 1);
		if(info < 0){
			systemError("ERROR UNPACKING deviceID FROM MESSAGE.");
			return;
		}

		//Unpack device firing mode
		unsigned int unpkDevFiringMode;
		info = pvm_upkuint(&unpkDevFiringMode, 1, 1);
		if(info < 0){
			systemError("ERROR UNPACKING device firing mode FROM MESSAGE.");
			return;
		}

		/* Convert back to a number between -1 and OUTPUT_FIRING_MODE with 0.1 resolution. */
		double tmpDevFiringMode = (double)unpkDevFiringMode / 10.0;
		tmpDevFiringMode -= 1.0;

		#ifdef DEVICE_FIRING_MODE_DEBUG
			cout<<"DEVICE FIRING MODE: unpacked value = "<<unpkDevFiringMode<<"; double value = "<<tmpDevFiringMode<<endl;
		#endif//DEVICE_FIRING_MODE_DEBUG

		//Create device manager
		deviceManager = new DeviceManager(deviceID, neuronGrpID, tmpDevFiringMode, deviceDBInterface, networkDBInterface);

		/* Set simulation to calculate compute time on each time step if we are receiving
			data from an external device that needs to be sychronized to */
		if(deviceManager->getDeviceType() == DeviceTypes::syncUDPNetworkInput)
			calculateComputeTime = true;

		//Free resources from pattern database
		delete patternDBInterface;
		patternDBInterface = NULL;

		#ifdef SIMULATION_DEBUG
			cout<<"Task: "<<thisTaskID<<": LIVE_SIMULATION with external device "<<deviceID<<endl;
		#endif//SIMULATION_DEBUG
	}

	//The simulation is a NO_INPUT_SIMULATION - nothing to unpack at present
	else{
		//Create an empty device manager
		deviceManager = new DeviceManager();

		//Free resources from unused databases
		delete deviceDBInterface;
		deviceDBInterface = NULL;
		delete patternDBInterface;
		patternDBInterface = NULL;

		#ifdef SIMULATION_DEBUG
			cout<<"Task: "; printTID(thisTaskID); cout<<": NO_INPUT_SIMULATION"<<endl;
		#endif//SIMULATION_DEBUG
	}

	/* Load Tasks, Neurons and Synapses. This loading order must be preserved because some of 
		the later items depend upon earlier ones. These are only called once, so do exception handling here.*/
	try{
		loadTasks();
		loadNeurons();
		loadSynapses();
		loadPreSynapticNeuronMaps();
	}
	catch (const BadQuery& er) {// Handle any query errors
		ostringstream errorStrStream;
		errorStrStream<<"Bad query when loading simulation data: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
		return;
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown loading simulation data: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
		return;
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown loading simulation data: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
		return;
	}

	/* Load up parameters and set the maximum buffer size. 
		Check that each has been performed successfully before moving on. */
	if(!loadNeuronParameters())
		return;
	
	if(!loadSynapseParameters())
		return;

	if(!loadGlobalParameters())
		return;

	if(!loadNoiseParameters())
		return;

	//Set the maximum buffer size.
	setMaxBufferSize();

	/* Set the neuron array in the pattern or device manager
		and fire the neurons for the first time */
	if(simulationType == PATTERN_SIMULATION){
		patternManager->setNeuronArray(neuronArray, numberOfNeurons);
		patternManager->fireNeurons();
	}
	else if(simulationType == LIVE_SIMULATION && deviceManager->isInputDevice()){
		deviceManager->setNeuronArray(neuronArray);
		deviceManager->setNeuronUpdateMap(&neuronUpdateMap);
		deviceManager->setNeuronVector(&neuronTaskHolder->firingNeuronVector, startNeuronID);//Use this to set the start neuron id
	}
	else if(simulationType == LIVE_SIMULATION && deviceManager->isOutputDevice()){
		deviceManager->setNeuronVector(&neuronTaskHolder->firingNeuronVector, startNeuronID);
	}

	/* Send a message to the parent task to indicate that loading is complete */
	sendMessage(parentTaskID, SIMULATION_LOADING_COMPLETE_MSG);
	
	//Set simulation clock to zero
	simulationClock->reset();
	
	// Record the fact that the simulation data has been loaded
	simulationDataLoaded = true;
}



/*! Loads up the appropriate synapse parameters. */
bool SpikeStreamSimulation::loadSynapseParameters(){
	try{
		//Output debugging information
		#ifdef SYNAPSE_PARAMETERS_DEBUG
			cout<<"SpikeStreamSimulation: Loading synapse parameters"<<endl;
		#endif //SYNAPSE_PARAMETERS_DEBUG
	
		/*Work through all of the connection groups that connect to/from this neuron group. connParameterMap holds
			a map of parameters for each connection group. */
		for (map<unsigned int, map<string, double>* >::iterator connParamMapIter = connParameterMap.begin(); connParamMapIter != connParameterMap.end(); ++connParamMapIter){
			//Clear the map if it holds parameters
			connParamMapIter->second->clear();
	
			//Get the names of all the parameters for this connection group
			//First need to get the names of all the parameters
			Query query = networkDBInterface->getQuery();
			query.reset();
			query<<"SHOW COLUMNS FROM "<<classLoader->getConnGrpParameterTableName(connParamMapIter->first);
	
			#ifdef SYNAPSE_PARAMETERS_DEBUG
				cout<<query.preview()<<endl;
			#endif //SYNAPSE_PARAMETERS_DEBUG
	
                        StoreQueryResult showResult = query.store();
                        for(StoreQueryResult::iterator iter = showResult.begin(); iter != showResult.end(); ++iter){
				Row showRow(*iter);
		
				//Get the column name
				string tempFieldName((std::string)showRow["Field"]);
			
				//If it is a parameter name
				if(tempFieldName.find("_val") != string::npos){
	
					//Create a key without the _val extension
					string finalFieldName(tempFieldName, 0, tempFieldName.find("_val"));
	
					//Store field name
					(*connParamMapIter->second)[finalFieldName] = 0.0;
				}
			}
	
			//Extract the values of the parameters from the database
			bool firstParameter = true;
			query.reset();
			query<<"SELECT ";
			for(map<string, double>::iterator iter = connParamMapIter->second->begin(); iter != connParamMapIter->second->end(); ++iter){
				if(firstParameter){
					query<<iter->first<<"_val";
					firstParameter = false;
				}
				else{
					query<<", "<<iter->first<<"_val";
				}
			}
			query<<" FROM "<<classLoader->getConnGrpParameterTableName(connParamMapIter->first)<<" WHERE ConnGrpID = "<<connParamMapIter->first;
	
			#ifdef SYNAPSE_PARAMETERS_DEBUG
				cout<<query.preview()<<endl;
			#endif //SYNAPSE_PARAMETERS_DEBUG
	
                        StoreQueryResult valueResult = query.store();
		
			//Check that there is just one row for this neuron group
			if(valueResult.size() != 1){
				ostringstream tempStr;
				tempStr<<"SYNAPSE PARAMETER TABLE \""<<classLoader->getConnGrpParameterTableName(connParamMapIter->first)<<"\" CONTAINS "<<valueResult.size()<<" ENTRIES FOR CONNECTION GROUP "<<connParamMapIter->first<<endl;
				systemError(tempStr.str());
				return false;
			}
			Row parameterRow(*valueResult.begin());
			for(map<string, double>::iterator iter = connParamMapIter->second->begin(); iter != connParamMapIter->second->end(); ++iter){
				(*connParamMapIter->second)[iter->first] = Utilities::getDouble((std::string)parameterRow[(iter->first + "_val").data()]);
			}
		}
	
		#ifdef SYNAPSE_PARAMETERS_DEBUG
			//Print out the parameters for each of the connection groups
			for (map<unsigned int, map<string, double>* >::iterator connParamMapIter = connParameterMap.begin(); connParamMapIter != connParameterMap.end(); ++connParamMapIter){
				cout<<"--------------------- Neuron Group "<<neuronGrpID<<" Parameters for Connection Group "<<connParamMapIter->first<<" -------------------"<<endl;
				for(map<string, double>::iterator iter = connParamMapIter->second->begin(); iter != connParamMapIter->second->end(); ++iter){
					cout<<iter->first<<" = "<<iter->second<<endl;
				}
			}
		#endif //SYNAPSE_PARAMETERS_DEBUG
	
		/* Synapse parameters have all been loaded up into the appropriate maps. Final thing needed is to inform
			the synapse classes that the parameters have changed. If they want to monitor this change they will have
			to create a local copy of the variable and compare it with the new value. */
		for( dense_hash_map<unsigned int, dense_hash_map<unsigned int, Synapse*, hash<unsigned int> >*, hash<unsigned int> >::iterator outerIter = synapseMap.begin(); outerIter != synapseMap.end(); ++outerIter){
			for(dense_hash_map<unsigned int, Synapse*, hash<unsigned int> >::iterator innerIter = outerIter->second->begin(); innerIter != outerIter->second->end(); ++innerIter){
				innerIter->second->parametersChanged();
			}
		}

		//If we have reached this point everything should be ok.
		return true;
	}
	catch (const BadQuery& er) {// Handle any query errors
		ostringstream errorStrStream;
		errorStrStream<<"Bad query when loading synapse parameters: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
		return false;
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown loading synapse parameters: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
		return false;
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown loading synapse parameters: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
		return false;
	}
}


/*! Loads up the synapses data from the database and creates a new synapse for each neuron-neuron
	connection. A synapse is created for each connection where the postsynaptic neuron is in this
	neuron group. Synapses are stored in nested dense_hash_maps for speed of access. The key in the
	outer map is the fromNeuronID. The key in the inner maps is the toNeuronID. 
	NOTE Exception handling should be done by the calling method. */
void SpikeStreamSimulation::loadSynapses(){
	/* Create a map to hold the link between connection group ids and synapse types.
		All connections within a connection group use the same synapse type and so
		this information is stored in the connection group table. */
	map<unsigned int, unsigned int> connSynapseTypeMap;

	// Create parameter map for each of the connection groups that this task is involved in
	Query query = networkDBInterface->getQuery();
	query.reset();
	query<<"SELECT ConnGrpID, ConnType, SynapseType FROM ConnectionGroups WHERE FromNeuronGrpID = "<<neuronGrpID<<" OR ToNeuronGrpID = "<<neuronGrpID;
        StoreQueryResult paramRes = query.store();
        for(StoreQueryResult::iterator iter = paramRes.begin(); iter != paramRes.end(); ++iter){
		//Extract information about connection group
		Row paramRow(*iter);
		unsigned int connGrpID = Utilities::getUInt(paramRow["ConnGrpID"]);
		unsigned int connType =  Utilities::getUInt(paramRow["ConnType"]);
		unsigned short synapseType = Utilities::getShort(paramRow["SynapseType"]);

		/*Create parameter maps on the heap */
		if (connType == ConnectionType::Virtual || connType == ConnectionType::TempVirtual){
			;//Do nothing - don't need parameters for virtual connections which have no synapses
		}
		else{
			//Create a new map to hold the parameters of the synapses in this connection group
			connParameterMap[connGrpID] = new map<string, double>;

			//Store the synapse type for the connection group
			connSynapseTypeMap[connGrpID] = synapseType;
		}
	}
	
	//Select all connections that have a connection to this neuron group
	Connection* tmpFastConnection = networkDBInterface->getNewConnection();
	if(!tmpFastConnection){
		throw databaseException;
	}
	Query fastQuery = tmpFastConnection->query();
	fastQuery.reset();
	fastQuery<<"SELECT PreSynapticNeuronID, PostSynapticNeuronID, Weight, ConnGrpID FROM Connections WHERE PostSynapticNeuronID >= "<<startNeuronID<<" AND PostSynapticNeuronID < "<<(startNeuronID + numberOfNeurons);
        UseQueryResult connRes = fastQuery.use();
	Row connRow;
	if(connRes){
            while (connRow = connRes.fetch_row()) {
                    //Extract the pre and post neuron ids
                    unsigned int tmpPreNeurID = Utilities::getUInt(connRow["PreSynapticNeuronID"]);
                    unsigned int tmpPostNeurID = Utilities::getUInt(connRow["PostSynapticNeuronID"]);

                    //Get weight and normalise to between -1.0 and 1.0
                    double weight = (double)Utilities::getShort(connRow["Weight"]);
                    weight /= 127.0;

                    //Get connection group id
                    unsigned int connGrpID = Utilities::getUInt(connRow["ConnGrpID"]);

                    //Create second dense hash map if needed
                    if(synapseMap.count(tmpPreNeurID) == 0){
                            synapseMap[tmpPreNeurID] = new dense_hash_map<unsigned int, Synapse*, hash<unsigned int> >();
                            synapseMap[tmpPreNeurID]->set_empty_key(EMPTY_NEURON_ID_KEY);
                    }

                    //Create the synapse
                    (*synapseMap[tmpPreNeurID])[tmpPostNeurID] = classLoader->getNewSynapse(connSynapseTypeMap[connGrpID]);

                    //Set the necessary parameters.
                    /* FIXME THESE WOULD BE MUCH BETTER HANDLED THROUGH STATIC REFERNECES IN THE Synapse
                            CLASS. HOWEVER, I HAD STRANGE PROBLEMS WITH THIS.*/
                    (*synapseMap[tmpPreNeurID])[tmpPostNeurID]->setSimulationClock(simulationClock);
                    (*synapseMap[tmpPreNeurID])[tmpPostNeurID]->setPostSynapticNeuron(neuronArray[tmpPostNeurID - startNeuronID]);
                    (*synapseMap[tmpPreNeurID])[tmpPostNeurID]->setPreSynapticNeuronID(tmpPreNeurID);
                    (*synapseMap[tmpPreNeurID])[tmpPostNeurID]->setWeight(weight);
                    (*synapseMap[tmpPreNeurID])[tmpPostNeurID]->setParameterMapReference(connParameterMap[connGrpID]);
            }
	}
	else{
		cerr<<fastQuery.error()<<endl;
		systemError("CANNOT RETRIEVE CONNECTION GROUP DETAILS");
	}

	//Close temporary connection
        tmpFastConnection->disconnect();

	//Output debugging information if necessary
	#ifdef LOAD_SYNAPSE_DEBUG
		printSynapseMap();
	#endif //LOAD_SYNAPSE_DEBUG
}


/*! Loads tasks and task holders
	Each connection group connects to a particular task and need to be able to 
	look up the task id using the connection group id in order to be able to make the
	neuron connections.
	NOTE Exception handling should be done by the calling method. */
void SpikeStreamSimulation::loadTasks(){
	//Get query
	Query query= networkDBInterface->getQuery();

	//================== Create Neuron Task Holder ======================
	//Get size of this neuron group and create neuron task holder
	query.reset();
	query<<"SELECT COUNT(*) FROM Neurons WHERE NeuronGrpID = "<<neuronGrpID;
        StoreQueryResult sizeResult = query.store();
	Row sizeRow = (*sizeResult.begin());//Single row
	unsigned int neuronGrpSize = Utilities::getUInt((std::string) sizeRow["COUNT(*)"]);
	neuronTaskHolder = new NeuronTaskHolder(thisTaskID, neuronGrpSize);
	

	//================= Load Spike Task Holders =========================
	//Get a list of NeuronGrpIDs and TaskIDs
	query.reset();
	query<<"SELECT NeuronGrpID, TaskID FROM NeuronGroups";
        StoreQueryResult taskResult = query.store();
        for(StoreQueryResult::iterator iter = taskResult.begin(); iter != taskResult.end(); ++iter){
		Row taskRow(*iter);
		int tempTaskID = Utilities::getInt(taskRow["TaskID"]);
		unsigned int tempNeuronGrpID = Utilities::getUInt(taskRow["NeuronGrpID"]);
		
		// Add entry storing the link between neuronGrpID and taskID
		taskNeurGrpMap[tempNeuronGrpID] = tempTaskID;
	}

	/* When all the links between neuron group IDs and taskIDs have been stored, need to 
		get the link between connection groups that this neuron group is involved in and the
		task ids of these connection groups. */
		
	// Select all connectionGrpIDs that this neuron group is connected to
	query.reset();
	query<<"SELECT ConnGrpID, ToNeuronGrpID FROM ConnectionGroups WHERE FromNeuronGrpID = "<<neuronGrpID;
        StoreQueryResult connGrpRes = query.store();
        for(StoreQueryResult::iterator connGrpIter = connGrpRes.begin(); connGrpIter != connGrpRes.end(); ++connGrpIter){
		Row connGrpRow(*connGrpIter);
		unsigned int connGrpID = Utilities::getUInt(connGrpRow["ConnGrpID"]);
		unsigned int toNeuronGrpID = Utilities::getUInt(connGrpRow["ToNeuronGrpID"]);
		
		/* For regular connections, add entry to taskConnGrpMap storing 
			link between connection group and taskID. This will be used later
			when setting up the neurons.
			The task ID of the toNeuronGrp needs to be looked up in the 
			taskNeurGrpMap. */
		taskConnGrpMap[connGrpID] = taskNeurGrpMap[toNeuronGrpID];
	}
		
	/* Tasks that this neuron group communicates with are managed by a task holder class
		This is loaded into a task holder map and references to task holders in this map
		are passed to individual neurons. */
	for(map<unsigned int, int>::iterator iter = taskConnGrpMap.begin(); iter != taskConnGrpMap.end(); ++iter){
		/* Check to see if task holder has already been created for this task id.
			Several connection groups can connect to the same task */
		if(spikeTaskHolderMap.find(iter->second) == spikeTaskHolderMap.end()){// Cannot find task: add new task holder
			spikeTaskHolderMap[iter->second] = new TaskHolder(thisTaskID, iter->second);
		}
	}
	
	/* Count how many spike messages should be received
		To do this, select all the unique neuron groups that connect to this neuron 
		group, including this neuron group */
	query.reset();
	query<<"SELECT DISTINCT FromNeuronGrpID FROM ConnectionGroups WHERE toNeuronGrpID = "<<neuronGrpID;//<<" AND FromNeuronGrpID != "<<neuronGrpID;
        StoreQueryResult distFromNeurIDRes = query.store();

	/* Work through these neuronGroupIDs and add the task id to the number of spike messages.
		This will give a more reliable indication about whether all the messages have arrived
		without a significant performance penalty */
	spikeMessageTotal = 0;
        for(StoreQueryResult::iterator iter = distFromNeurIDRes.begin(); iter != distFromNeurIDRes.end(); ++iter){
		Row spikeListFromNeuronRow(*iter);
		unsigned int spikeListFromNeuronID = Utilities::getUInt(spikeListFromNeuronRow["FromNeuronGrpID"]);
		spikeMessageTotal += taskNeurGrpMap[spikeListFromNeuronID];
	}

	/* Work through the neuron groups and load up the start neuron ID for each task. 
		This is used to compress the messages */
	for(map<unsigned int, int>::iterator iter = taskNeurGrpMap.begin(); iter != taskNeurGrpMap.end(); ++iter){
		query.reset();
		query<<"SELECT MIN(NeuronID) FROM Neurons WHERE NeuronGrpID = "<<iter->first;
                StoreQueryResult minNeurIDRes = query.store();
		Row minNeurIDRow(*minNeurIDRes.begin());//Should be only one row
		startNeurIDTaskMap[iter->second] = Utilities::getUInt((std::string)minNeurIDRow["MIN(NeuronID)"]);
	}

	//Reset spike message count
	spikeMessageCount = 0;

	#ifdef TASK_DEBUG
		cout<<"SpikeStreamSimulation: SpikeMessageTotal = "<<spikeMessageTotal<<endl;
	#endif//TASK_DEBUG
}


/*! Prints out the synapse map for debugging. */
void SpikeStreamSimulation::printSynapseMap(){
	cout<<"----------------------- Start Synapse Map ---------------------------------"<<endl;
	for( dense_hash_map<unsigned int, dense_hash_map<unsigned int, Synapse*, hash<unsigned int> >*, hash<unsigned int> >::iterator outerIter = synapseMap.begin(); outerIter != synapseMap.end(); ++outerIter){
		for(dense_hash_map<unsigned int, Synapse*, hash<unsigned int> >::iterator innerIter = outerIter->second->begin(); innerIter != outerIter->second->end(); ++innerIter){
			cout<<"[ "<<outerIter->first<<", "<<innerIter->first<<" } ";
			innerIter->second->print();
			cout<<endl;
		}
	}
	cout<<"------------------------- End Synapse Map ---------------------------------"<<endl;
}


/*! Prints out the task id for this task using either hexadecimal or decimal depending
	on the value of printTIDHex. */
void SpikeStreamSimulation::printTID(){
	printTID(thisTaskID);
}


/*! Prints out a task id either as a hexadecimal or decimal value depending on the
	value of printTIDHex.
	Code adapted from http://mathforum.org/library/drmath/view/54347.html. */
void SpikeStreamSimulation::printTID(int taskID_decimal){
	//Convert task id to hex and print it
	if(printTIDHex){
		char hexCharArray[8];
		unsigned long temp_value;
		for(short index=7;index>=0;index--){
			// temp_value=decimal_value/pow(16,index)
			temp_value=taskID_decimal/(1<<(index<<2));
			if(temp_value>9){
				hexCharArray[index]=(char)('A'-10+temp_value);
				// decimal_value=decimal_value%pow(16,index)
				taskID_decimal=taskID_decimal%(1<<(index<<2));
			}
			else if(temp_value>0){
				hexCharArray[index]=(char)('0'+temp_value);
				// decimal_value=decimal_value%pow(16,index)
				taskID_decimal=taskID_decimal%(1<<(index<<2));
			}
			else{
				hexCharArray[index]='0';
			}
		}
	
		//Reverse and strip off significant digits
		char finalHexCString [9];
		bool firstDigitFound = false;
		int counter = 0;
		for(int i=7; i >=0; --i){
			if(hexCharArray[i] == '0' && !firstDigitFound){
				;//Do nothing want to ignore this
			}
			else if(hexCharArray[i] != '0' && !firstDigitFound){
				firstDigitFound = true;
				finalHexCString[counter] = hexCharArray[i];
				++counter;
			}
			else{
				finalHexCString[counter] = hexCharArray[i];
				++counter;
			}
		}
		finalHexCString[counter] = '\0';
		cout<<finalHexCString;
	}
	else
		cout<<taskID_decimal;
}


/*! Called when a SPIKE_LIST_MSG is received containing spikes from another task.
	The spikes in this message are pairs of shorts with the from and to neuron ids, 
	but with the startNeuronID subtracted, so their value is relative to the neuron
	group they are from. Spike list is processed by reading these ids from the 
	message and using them to find the synapse in the map and call its update function. */
void SpikeStreamSimulation::processSpikeList(unsigned int senderTID, int msgTimeStep){

	/* During step mode, exta spike messages can arrive before the message has been sent
		so check to see if there are spike messages waiting to be sent and send them */
	int tempBufID = -1;
	if(spikeMessageCount == spikeMessageTotal){
		tempBufID = pvm_setrbuf(0);//Store buffer before sending messages. Early messages may overwrite current buffer
		#ifdef PVM_BUFFER_DEBUG
			cout<<"ProcessSpikeList1: Saving buffer with ID = "<<tempBufID<<endl;
		#endif//PVM_BUFFER_DEBUG

		sendSpikeMessages();

		#ifdef PVM_BUFFER_DEBUG
			int oldBufID = pvm_setrbuf(tempBufID);
			cout<<"ProcessSpikeList2: Saving buffer with ID = "<<oldBufID<<"; Loading buffer with ID = "<<tempBufID<<endl;
		#else
			pvm_setrbuf(tempBufID);
		#endif//PVM_BUFFER_DEBUG
	}

	//Unpack message.
	unsigned int numberOfSpikes = 0;//Number of spikes in the message
	unsigned int messageTimeStep = 0;
	if(msgTimeStep == -1){//New incoming message received in the run method
		//Extract the time step from the message. This should be one time step behind the current one or an error
		info = pvm_upkuint(&messageTimeStep, 1, 1);
		#ifdef PVM_DEBUG
			if(info < 0){
				systemError("ERROR UNPACKING TIME STEP FROM MESSAGE; SENDER TASK ID");
				cerr<<"TIME STEP ERROR IN MESSAGE FROM TASK: "; printTID(senderTID); cout<<"; messageTimeStep: "<<messageTimeStep<<endl;
				return;
			}
		#endif//PVM_DEBUG
	}
	else{//A reloaded early message
		messageTimeStep = msgTimeStep;
	}

	/* Check time step. Messages should always have a message time step 1 less than the 
		current time step. However tasks can run at different rates, so it happens sometimes that a task
		has processed the spikes from this task and sent a new spike list before other tasks have had a 
		chance to send the current spike list. In this case (when messagetimestep == this time step), 
		need to store the current receive buffer and process the message after sending the spike lists.
		When simulation clock time step is 0 do not want to subtract 1 because with an unsigned int this 
		leads to a very large number. */

	//Unexpected simulation error. Should not continue with simulation after this
	if((simulationClock->getTimeStep() > 0 && messageTimeStep < (simulationClock->getTimeStep() - 1)) || (messageTimeStep > simulationClock->getTimeStep())){

		unsigned int tempInt = simulationClock->getTimeStep() - 1;
		cout<<"SYNC PROBLEM: messageTimeStep = "<<messageTimeStep<<" simulationClock = "<<simulationClock->getTimeStep()<<" - 1= "<<tempInt<<endl;

		ostringstream tempStr;
		tempStr<<"SYNCHRONIZATION ERROR: Current time step: "<<simulationClock->getTimeStep()<<"; Message time step: "<<messageTimeStep<<" Message from: "; printTID(senderTID); cout<<endl;
		systemError(tempStr.str());
		spikeMessageCount += 10000;//Add an arbitrary number to prevent simulation from continuing
		return;
	}

	/* Message has arrive early and needs to be stored until all messages have been dealt with 
		from the previous timestep */
	else if(messageTimeStep == simulationClock->getTimeStep() ){
		#ifdef MESSAGE_DEBUG
			cout<<"SpikeStreamSimulation: Storing early message from "; printTID(senderTID); cout<<"; messageTimeStep = "<<messageTimeStep<<endl;
		#endif//MESSAGE_DEBUG

		EarlyMessage tmpEarlyMsg;
		tmpEarlyMsg.bufferID = pvm_setrbuf(0);//Instruct PVM to store message and record the buffer id of stored message
		#ifdef PVM_BUFFER_DEBUG
			cout<<"ProcessSpikeList3: Saving buffer with ID = "<<tmpEarlyMsg.bufferID<<endl;
		#endif//PVM_BUFFER_DEBUG
		tmpEarlyMsg.senderTID = senderTID;
		tmpEarlyMsg.timeStep = messageTimeStep;
		earlyMessageStack.push(tmpEarlyMsg);
		return;//Don't want to continue processing the now non existent buffer
	}


	/* Increase the count of the number of spike messages received. Will only increase the spikeMessageCount
		on messages that are 1 time step behind. This applies to both regular and early messages. */
	spikeMessageCount += senderTID;
	

	/* Only want to unpack and process the spikes if running in no input mode or if updating
		an external device. In these cases the spike message contains meaningful spikes that 
		need to be processed. When updating the neurons from an external device or pattern
		only fire the neurons when sending spike messages. */
	//NOTE HAVE KILLED THIS CONDITION SO THAT CAN INHIBIT INPUT LAYERS. DON'T KNOW IF THERE ARE UNPLEASANT REPERCUSSIONS
	//if(simulationType == NO_INPUT_SIMULATION || (simulationType == LIVE_SIMULATION && deviceManager->isOutputDevice())){

	// Extract the number of spikes in the message
	int info = pvm_upkuint(&numberOfSpikes, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR UNPACKING NUMBER OF SPIKES FROM MESSAGE; numberOfSpikes: "<<numberOfSpikes<<endl;
			systemError("ERROR UNPACKING NUMBER OF SPIKES FROM MESSAGE");
			return;
		}
	#endif//PVM_DEBUG

	//Output debugging information if required
	#ifdef SPIKE_DEBUG
		cout<<"Processing Spike list at time: "<<simulationClock->getTimeStep()<<"; Message time step = "<<messageTimeStep<<"; Number of spikes in message = "<<numberOfSpikes<<"; Spike message count = "<<spikeMessageCount<<endl;
	#endif //SPIKE_DEBUG

	//Unpack array of from and to neuron ids as two shorts compressed into an integer
	info = pvm_upkuint(unpackArray, numberOfSpikes, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			systemError("SpikeStreamSimulation: ERROR UNPACKING UNSIGNED INT FROM MESSAGE. NUMBER OF SPIKES = ", numberOfSpikes);
			return;
		}
	#endif//PVM_DEBUG

	/* Work through the unpackArray, extract information about the spikes
		and request synapses to process the spikes. */
	for(unsigned int i=0; i<numberOfSpikes; ++i){

		//Add the from neuron ID to the from key
		unpkFromNeurID = (unsigned short) unpackArray[i];
		unpkFromNeurID += startNeurIDTaskMap[senderTID];

		//Add this from neuron ID to the from key 
		unpkToNeurID = unpackArray[i] >> 16;
		unpkToNeurID  += startNeuronID;

		//Print the synapse key for debugging
		//cout<<"Received Synapse Key: [ "<<unpkFromNeurID<<", "<<unpkToNeurID<<" ]"<<endl;
				
		// Process the spike with full debug checks
		#ifdef SPIKE_DEBUG
			if(synapseMap.count(unpkFromNeurID)){//Check that from key is in map
				if(synapseMap[unpkFromNeurID]->count(unpkToNeurID)){//Check to key is in second map
					(*synapseMap[unpkFromNeurID])[unpkToNeurID]->processSpike();
					neuronUpdateMap[ unpkToNeurID ] = true;//The to neuron should be in this layer
				}
				else{
					ostringstream tempStr;
					tempStr<<"SpikeStreamSimulation: SYNAPSE TO KEY NOT FOUND IN INNER SYNAPSE MAP: [ "<<unpkFromNeurID<<", "<<unpkToNeurID<<" ]";
					systemError(tempStr.str().data());
				}
			}
			else{
				ostringstream tempStr;
				tempStr<<"SpikeStreamSimulation: SYNAPSE TO KEY NOT FOUND IN OUTER SYNAPSE MAP: [ "<<unpkFromNeurID<<", "<<unpkToNeurID<<" ]";
				systemError(tempStr.str().data());
			}

		//Process the spike with less checks
		#else
			(*synapseMap[unpkFromNeurID])[unpkToNeurID]->processSpike();
			neuronUpdateMap[ unpkToNeurID ] = true;//The to neuron should be in the layer processed by this task
		#endif//SPIKE_DEBUG
	}
	//}

	//Clean up buffer if necessary
	if(tempBufID != -1)
		pvm_freebuf(tempBufID);

	
	/* If all spike messages have been received, send spike message to other tasks 
		If simulationRunning is false, the method will exit here and the messages will be sent either
		when step is pressed again or when a spike list is received. */
	if(spikeMessageCount == spikeMessageTotal && simulationRunning){
		#ifdef TRANSPORT_DEBUG
			cout<<"Process spike list: Sending spike messages"<<endl;
		#endif//TRANSPORT_DEBUG
		sendSpikeMessages();
	}
	else if(spikeMessageCount == spikeMessageTotal && !simulationRunning){
		#ifdef TRANSPORT_DEBUG
			cout<<"Process spike list: all spikes received but not sending messags because simulation not running"<<endl;
		#endif//TRANSPORT_DEBUG
	}
	else{
		#ifdef TRANSPORT_DEBUG
			cout<<"Process spike list: not all spikes received; waiting for more spikes"<<endl;
		#endif//TRANSPORT_DEBUG
	}
}


/*! Reloads the weights of the synapses from the database. */
void SpikeStreamSimulation::reloadWeights(){
	try{
		#ifdef RELOAD_WEIGHTS_DEBUG
			cout<<"SpikeStreamSimulation: Reloading weights."<<endl;
		#endif//RELOAD_WEIGHTS_DEBUG
	
		//Select all connections that have a connection to this neuron group
		Connection* tmpFastConnection = networkDBInterface->getNewConnection();
		if(!tmpFastConnection){
			throw databaseException;
		}
		Query fastQuery = tmpFastConnection->query();
		fastQuery.reset();
		fastQuery<<"SELECT PreSynapticNeuronID, PostSynapticNeuronID, Weight FROM Connections WHERE PostSynapticNeuronID >= "<<startNeuronID<<" AND PostSynapticNeuronID < "<<(startNeuronID + numberOfNeurons);
                UseQueryResult connRes = fastQuery.use();
		Row connRow;
		unsigned int tmpPreNeurID, tmpPostNeurID;
		if(connRes){
                    while (connRow = connRes.fetch_row()) {

                            //Extract pre and post neuron ids
                            tmpPreNeurID = Utilities::getUInt(connRow["PreSynapticNeuronID"]);
                            tmpPostNeurID = Utilities::getUInt(connRow["PostSynapticNeuronID"]);

                            //Extract the weight
                            double weight = (double)Utilities::getShort(connRow["Weight"]);
                            weight /= 127.0;//Normalise to between -1.0 and 1.0

                            /* Check that key is in map in debug mode. */
                            #ifdef RELOAD_WEIGHTS_DEBUG
                                    if(synapseMap.count(tmpPreNeurID)){//Check that from key is in map
                                            if(!synapseMap[tmpPreNeurID]->count(tmpPostNeurID)){//Check to key is in second map
                                                    cerr<<"SpikeStreamSimulation: CANNOT FIND SYNAPSE KEY: "<<tmpPreNeurID<<" " <<tmpPostNeurID<<endl;
                                                    systemError("Cannot find key in synapse map when reloading weights.");
                                                    return;
                                            }
                                    }
                                    else{
                                            cerr<<"SpikeStreamSimulation: CANNOT FIND SYNAPSE KEY: "<<tmpPreNeurID<<" " <<tmpPostNeurID<<endl;
                                            systemError("Cannot find key in synapse map when reloading weights.");
                                            return;
                                    }
                            #endif//RELOAD_WEIGHTS_DEBUG

                            //Set weight in synapse map
                            (*synapseMap[tmpPreNeurID])[tmpPostNeurID]->setWeight(weight);
                    }
		}
		else{
			cerr<<fastQuery.error()<<endl;
			systemError("CANNOT RETRIEVE CONNECTION GROUP DETAILS WHILST LOADING WEIGHTS");
		}

		#ifdef RELOAD_WEIGHTS_DEBUG
			cout<<"SpikeStreamSimulation: Weights reloaded"<<endl;
		#endif//RELOAD_WEIGHTS_DEBUG
	
		//Close temporary connection
                tmpFastConnection->disconnect();

		//Send acknowledgement message
		sendMessage(parentTaskID, LOAD_WEIGHTS_SUCCESS_MSG);
	}
	catch (const BadQuery& er) {// Handle any query errors
		ostringstream errorStrStream;
		errorStrStream<<"Bad query when reloading weights: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown reloading weights: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown reloading weights: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
	}
}


/*! Removes the specified task from the list of tasks that receive firing
	neuron data. */
void SpikeStreamSimulation::removeReceivingTask_Neurons(int removeTaskID){
	neuronTaskHolder->removeReceivingTask(removeTaskID);
}


/*! Removes the specified task from the list of tasks that receive the spike messages
	from this task. */
void SpikeStreamSimulation::removeReceivingTask_Spikes(int removeTaskID){
	//Work through the task holders and remove task from them
	for(map<int, TaskHolder*>::iterator iter = spikeTaskHolderMap.begin(); iter != spikeTaskHolderMap.end(); ++iter)
		iter->second->removeReceivingTask(removeTaskID);
}


/*! Writes the weights of this neuron group into TempWeight field of the database
	so that they can be viewed by the user during a simulation run. */
void SpikeStreamSimulation::saveViewWeights(){
	try{
		#ifdef SAVE_WEIGHTS_DEBUG
			cout<<"SpikeStreamSimulation: Saving view weights."<<endl;
		#endif //SAVE_WEIGHTS_DEBUG
	
		//Sort out the database stuff
		Query query = networkDBInterface->getQuery();
	
		//Work through the synapses in this layer and save the weights in a temporary location
		for( dense_hash_map<unsigned int, dense_hash_map<unsigned int, Synapse*, hash<unsigned int> >*, hash<unsigned int> >::iterator outerIter = synapseMap.begin(); outerIter != synapseMap.end(); ++outerIter){
			for(dense_hash_map<unsigned int, Synapse*, hash<unsigned int> >::iterator innerIter = outerIter->second->begin(); innerIter != outerIter->second->end(); ++innerIter){
				query.reset();
				query<<"UPDATE Connections SET TempWeight = "<<innerIter->second->getShortWeight()<<" WHERE PreSynapticNeuronID = "<<outerIter->first<<" AND PostSynapticNeuronID = "<<innerIter->first;
				query.execute();
			}
		}

		#ifdef SAVE_WEIGHTS_DEBUG
			cout<<"SpikeStreamSimulation: View weights saved"<<endl;
		#endif //SAVE_WEIGHTS_DEBUG
	
		//Send acknowledgement message
		sendMessage(parentTaskID, VIEW_WEIGHTS_SAVE_SUCCESS_MSG);
	}
	catch (const BadQuery& er) {// Handle any query errors
		ostringstream errorStrStream;
		errorStrStream<<"Bad query when saving view weights: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown saving view weights: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown saving view weights: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
	}
}


/*! Writes the weights of this neuron group into the database, overwriting their current values. */
void SpikeStreamSimulation::saveWeights(){
	try{
		#ifdef SAVE_WEIGHTS_DEBUG
			cout<<"SpikeStreamSimulation: Saving weights."<<endl;
		#endif //SAVE_WEIGHTS_DEBUG
	
		//Sort out the database stuff
		Query query = networkDBInterface->getQuery();
	
		//Save the weights
		for( dense_hash_map<unsigned int, dense_hash_map<unsigned int, Synapse*, hash<unsigned int> >*, hash<unsigned int> >::iterator outerIter = synapseMap.begin(); outerIter != synapseMap.end(); ++outerIter){
			for(dense_hash_map<unsigned int, Synapse*, hash<unsigned int> >::iterator innerIter = outerIter->second->begin(); innerIter != outerIter->second->end(); ++innerIter){
				query.reset();
				query<<"UPDATE Connections SET Weight = "<<innerIter->second->getShortWeight()<<" WHERE PreSynapticNeuronID = "<<outerIter->first<<" AND PostSynapticNeuronID = "<<innerIter->first;
				query.execute();
			}
		}
	
		#ifdef SAVE_WEIGHTS_DEBUG
			cout<<"SpikeStreamSimulation: Weights saved"<<endl;
		#endif //SAVE_WEIGHTS_DEBUG

		//Send acknowledgement message
		sendMessage(parentTaskID, WEIGHT_SAVE_SUCCESS_MSG);
	}
	catch (const BadQuery& er) {// Handle any query errors
		ostringstream errorStrStream;
		errorStrStream<<"Bad query when saving weights: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown saving weights: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown saving weights: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
	}
}


/*! Sends a message without any contents. */
bool SpikeStreamSimulation::sendMessage(int taskID, int msgtag){
	//Initialise the buffer
	int info = pvm_initsend(PvmDataDefault);
	#ifdef PVM_DEBUG
		if(info < 0){
			cout<<"SpikeStreamSimulation: Init send error: "<<info<<" with task "<<taskID<<" and msgtag: "<<msgtag<<endl;
			return false;
		}
	#endif //PVM_DEBUG

	//Send the message
	info = pvm_send(taskID, msgtag);
	#ifdef PVM_DEBUG
		if(info < 0){
			cout<<"SpikeStreamSimulation: Send error: "<<info<<" with task "<<taskID<<" and msgtag: "<<msgtag<<endl;
			return false;
		}
	#endif //PVM_DEBUG

	return true;
}


/*! Sends a message containing a char string. */
bool SpikeStreamSimulation::sendMessage(int taskID, int msgtag, const char* charArray){
	//First find the length of the char array (+1 so that other end does not have to add 1 for null character)
	unsigned int arrayLength = strlen(charArray) + 1;

	//Initialise the buffer
	int info = pvm_initsend(PvmDataDefault);
	#ifdef PVM_DEBUG
		if(info<0){
			cerr<<"SpikeStreamSimulation: Init send error: tag: "<<msgtag<<" to: "<<taskID<<" containing: "<<charArray<<endl;
			systemError("Init send error");
			return false;
		}
	#endif //PVM_DEBUG

	//Pack the length of the char array
	info = pvm_pkuint(&arrayLength, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR PACKING MESSAGE LENGTH INTO MESSAGE; TASK ID = "<<thisTaskID<<endl;
			systemError("ERROR PACKING MESSAGE LENGTH INTO MESSAGE");
			return false;
		}
	#endif //PVM_DEBUG

	//Pack the char array
	info = pvm_pkstr((char*)charArray);
	#ifdef PVM_DEBUG
		if(info < 0){
			systemError("ERROR PACKING  CHAR* INTO MESSAGE");
			return false;
		}
	#endif //PVM_DEBUG

	//Send the message
	info = pvm_send(taskID, msgtag);
	#ifdef PVM_DEBUG
		if(info<0){
			cerr<<"SpikeStreamSimulation: Message send error: tag: "<<msgtag<<" to: "<<taskID<<" containing: "<<charArray<<endl;
			systemError("PVM message send error");
			return false;
		}
	#endif //PVM_DEBUG
	return true;
}


/*! Sends a message containing an unsigned int and a char string. */
bool SpikeStreamSimulation::sendMessage(int taskID, int msgtag, unsigned int intData, const char* charArray){
	//Find the length of the char array (+1 so that other end does not have to add 1 for null character)
	unsigned int arrayLength = strlen(charArray) + 1;

	//Initialise the buffer
	int info = pvm_initsend(PvmDataDefault);
	#ifdef PVM_DEBUG
		if(info<0){
			cerr<<"SpikeStreamSimulation: Init send error: tag: "<<msgtag<<" to: "<<taskID<<" containing: "<<charArray<<endl;
			systemError("PVM init send error.");
			return false;
		}
	#endif //PVM_DEBUG

	//Pack the unsigned int
	info = pvm_pkuint(&intData, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR PACKING UNSIGNED INT INTO MESSAGE; TASK ID = "<<thisTaskID<<endl;
			systemError("ERROR PACKING UNSIGNED INT INTO MESSAGE");
			return false;
		}
	#endif //PVM_DEBUG

	//Pack the length of the char array
	info = pvm_pkuint(&arrayLength, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR PACKING MESSAGE LENGTH INTO MESSAGE; TASK ID = "<<thisTaskID<<endl;
			systemError("ERROR PACKING MESSAGE LENGTH INTO MESSAGE");
			return false;
		}
	#endif //PVM_DEBUG

	//Pack the char array
	info = pvm_pkstr((char*)charArray);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR PACKING  CHAR* INTO MESSAGE; TASK ID = "<<thisTaskID<<endl;
			systemError("ERROR PACKING  CHAR* INTO MESSAGE");
			return false;
		}
	#endif //PVM_DEBUG

	//Send the message
	info = pvm_send(taskID, msgtag);
	#ifdef PVM_DEBUG
		if(info<0){
			cerr<<"SpikeStreamSimulation: Message send error: tag: "<<msgtag<<" to: "<<taskID<<" containing: "<<charArray<<endl;
			systemError("PVM message send error.");
			return false;
		}
	#endif //PVM_DEBUG
	return true;
}


/*! Sends a message containing two unsigned ints and a char string. */
bool SpikeStreamSimulation::sendMessage(int taskID, int msgtag, unsigned int msgData1, unsigned int msgData2, const char* charArray){
	//Find the length of the char array (+1 so that other end does not have to add 1 for null character)
	unsigned int arrayLength = strlen(charArray) + 1;

	//Initialise the buffer
	int info = pvm_initsend(PvmDataDefault);
	#ifdef PVM_DEBUG
		if(info<0){
			cerr<<"SpikeStreamSimulation: Init send error: tag: "<<msgtag<<" to: "<<taskID<<" containing: "<<charArray<<endl;
			systemError("PVM init send error.");
			return false;
		}
	#endif //PVM_DEBUG

	//Pack the unsigned int
	info = pvm_pkuint(&msgData1, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR PACKING FIRST UNSIGNED INT INTO MESSAGE; TASK ID = "<<thisTaskID<<endl;
			systemError("ERROR PACKING FIRST UNSIGNED INT INTO MESSAGE.");
			return false;
		}
	#endif //PVM_DEBUG

	//Pack the unsigned int
	info = pvm_pkuint(&msgData2, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR PACKING SECOND UNSIGNED INT INTO MESSAGE; TASK ID = "<<thisTaskID<<endl;
			systemError("ERROR PACKING SECOND UNSIGNED INT INTO MESSAGE.");
			return false;
		}
	#endif //PVM_DEBUG

	//Pack the length of the char array
	info = pvm_pkuint(&arrayLength, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR PACKING MESSAGE LENGTH INTO MESSAGE; TASK ID = "<<thisTaskID<<endl;
			systemError("ERROR PACKING MESSAGE LENGTH INTO MESSAGE.");
			return false;
		}
	#endif //PVM_DEBUG

	//Pack the char array
	info = pvm_pkstr((char*)charArray);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR PACKING  CHAR* INTO MESSAGE; TASK ID = "<<thisTaskID<<endl;
			systemError("ERROR PACKING  CHAR* INTO MESSAGE.");
			return false;
		}
	#endif //PVM_DEBUG

	//Send the message
	info = pvm_send(taskID, msgtag);
	#ifdef PVM_DEBUG
		if(info<0){
			cerr<<"SpikeStreamSimulation: Message send error: tag: "<<msgtag<<" to: "<<taskID<<" containing: "<<charArray<<endl;
			systemError("PVM message send error.");
			return false;
		}
	#endif //PVM_DEBUG
	return true;
}


/*! Sends a message containing an unsigned integer followed by a float array. */
bool SpikeStreamSimulation::sendNeuronData(int taskID, int msgtag, unsigned int neuronID, double* dataArray, int arrayLength){
	//Initialise the buffer
	int info = pvm_initsend(PvmDataDefault);
	#ifdef PVM_DEBUG
		if(info<0){
			cerr<<"SpikeStreamSimulation: Init send error: tag: "<<msgtag<<" to: "<<taskID<<endl;
			systemError("PVM init send error.");
			return false;
		}
	#endif //PVM_DEBUG

	//Pack the neuron ID
	info = pvm_pkuint(&neuronID, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR PACKING UNSIGNED INT INTO MESSAGE; TASK ID = "<<thisTaskID<<endl;
			systemError("ERROR PACKING UNSIGNED INT INTO MESSAGE.");
			return false;
		}
	#endif //PVM_DEBUG

	//Pack the time
	double currTim = simulationClock->getSimulationTime();
	info = pvm_pkdouble(&currTim, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR PACKING DOUBLE INTO MESSAGE; TASK ID = "<<thisTaskID<<endl;
			systemError("ERROR PACKING DOUBLE INTO MESSAGE.");
			return false;
		}
	#endif //PVM_DEBUG

	//Pack the length of the double array
	info = pvm_pkint(&arrayLength, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR PACKING DOUBLE ARRAY LENGTH INTO MESSAGE; TASK ID = "<<thisTaskID<<endl;
			systemError("ERROR PACKING FLOAT ARRAY LENGTH INTO MESSAGE.");
			return false;
		}
	#endif //PVM_DEBUG

	//Pack the double array
	info = pvm_pkdouble(dataArray, arrayLength, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR PACKING DOUBLE ARRAY INTO MESSAGE; TASK ID = "<<thisTaskID<<endl;
			systemError("ERROR PACKING DOUBLE ARRAY INTO MESSAGE.");
			return false;
		}
	#endif //PVM_DEBUG

	//Send the message
	info = pvm_send(taskID, msgtag);
	#ifdef PVM_DEBUG
		if(info<0){
			cerr<<"SpikeStreamSimulation: Message send error: tag: "<<msgtag<<" to: "<<taskID<<endl;
			systemError("PVM message send error.");
			return false;
		}
	#endif //PVM_DEBUG
	return true;
}

/*! Sends a message containing an unsigned integer followed by a float array. */
bool SpikeStreamSimulation::sendSynapseData(int taskID, int msgtag, unsigned int fromNeurID, unsigned int toNeurID, double* dataArray, int arrayLength){
	//Initialise the buffer
	int info = pvm_initsend(PvmDataDefault);
	#ifdef PVM_DEBUG
		if(info<0){
			cerr<<"SpikeStreamSimulation: Init send error: tag: "<<msgtag<<" to: "<<taskID<<endl;
			systemError("PVM init send error.");
			return false;
		}
	#endif //PVM_DEBUG

	//Pack the from neuron ID
	info = pvm_pkuint(&fromNeurID, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR PACKING UNSIGNED INT INTO MESSAGE; TASK ID = "<<thisTaskID<<endl;
			systemError("ERROR PACKING UNSIGNED INT INTO MESSAGE.");
			return false;
		}
	#endif //PVM_DEBUG

	//Pack the to neuron ID
	info = pvm_pkuint(&toNeurID, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR PACKING UNSIGNED INT INTO MESSAGE; TASK ID = "<<thisTaskID<<endl;
			systemError("ERROR PACKING UNSIGNED INT INTO MESSAGE");
			return false;
		}
	#endif //PVM_DEBUG

	//Pack the time
	double currTim = simulationClock->getSimulationTime();
	info = pvm_pkdouble(&currTim, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR PACKING DOUBLE INTO MESSAGE; TASK ID = "<<thisTaskID<<endl;
			systemError("ERROR PACKING DOUBLE INTO MESSAGE.");
			return false;
		}
	#endif //PVM_DEBUG

	//Pack the length of the double array
	info = pvm_pkint(&arrayLength, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR PACKING DOUBLE ARRAY LENGTH INTO MESSAGE; TASK ID = "<<thisTaskID<<endl;
			systemError("ERROR PACKING DOUBLE ARRAY LENGTH INTO MESSAGE");
			return false;
		}
	#endif //PVM_DEBUG

	//Pack the double array
	info = pvm_pkdouble(dataArray, arrayLength, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR PACKING DOUBLE ARRAY INTO MESSAGE; TASK ID = "<<thisTaskID<<endl;
			systemError("ERROR PACKING DOUBLE ARRAY INTO MESSAGE");
			return false;
		}
	#endif //PVM_DEBUG

	//Send the message
	info = pvm_send(taskID, msgtag);
	#ifdef PVM_DEBUG
		if(info<0){
			cerr<<"SpikeStreamSimulation: Message send error: tag: "<<msgtag<<" to: "<<taskID<<endl;
			systemError("PVM message send error.");
			return false;
		}
	#endif //PVM_DEBUG
	return true;
}



/*! Instructs the task holders to send the spike messages to the other tasks.
	At this point messages should have been received from all the tasks connecting
	to this task and all of the spikes should have been processed. However, the final
	state of the neurons still needs to be calculated. */
void SpikeStreamSimulation::sendSpikeMessages(){
	/* When recording statistics, want to bring simulation to a halt after STATS_MONITORING_PERIOD
		This is done by creating an error which will stop the simulation until the simulation is 
		destroyed, which will print out the statistics. */
	#ifdef RECORD_STATISTICS
		if(statisticsTimer.getTime_usec() > STATS_MONITORING_PERIOD){
			statisticsTimer.stop();
			systemError("Statistical monitoring period has expired");
			return;
		}
	#endif//RECORD_STATISTICS

	/*In noise mode, add noise to the layer either by firing neurons directly
		or by changing the membrane potential using the supplied synapse weight */
	if(noiseEnabled){
		if(directFiring_noise){//Fire a percentage of the neurons at random
			if(randomPercentNoise)
				fireRandomNeurons(getRandomPercentage());
			else
				fireRandomNeurons(percentNeurons_noise);
		}
		else{//Call changePostSynapticPotential on a random percentage of neurons
			if(randomPercentNoise)
				fireRandomNeurons_synaptic(getRandomPercentage());
			else
				fireRandomNeurons_synaptic(percentNeurons_noise);
		}
	}


	/* In full synapse update mode, need to work through all the synapses and update them at 
		each time step. Otherwise synapses should update themselves when they process a spike. 
		Also need to update all neurons that these synapses are connected to.*/
	if(updateAllSynapses){
		for( dense_hash_map<unsigned int, dense_hash_map<unsigned int, Synapse*, hash<unsigned int> >*, hash<unsigned int> >::iterator outerIter = synapseMap.begin(); outerIter != synapseMap.end(); ++outerIter){
			for(dense_hash_map<unsigned int, Synapse*, hash<unsigned int> >::iterator innerIter = outerIter->second->begin(); innerIter != outerIter->second->end(); ++innerIter){
				innerIter->second->calculateFinalState();
				neuronUpdateMap[innerIter->first] = true;
			}
		}
	}

	/* In event driven update mode need to work through the neurons that have received a spike to 
		calculate whether they should fire or not. This can't be done as messages are received 
		because the neuron could receive a lot of excitatory messages from one layer, which would 
		fire it, before receiving other inhibitory messages from another layer, which would prevent 
		it from firing. The only way around this  is to wait for all messages and then calculate the 
		final membrane potential. */

	if(!updateAllNeurons){//Only update neurons that have received a spike
		for(dense_hash_map<unsigned int, bool, hash<unsigned int> >::iterator iter = neuronUpdateMap.begin(); iter != neuronUpdateMap.end(); ++iter ){
			neuronArray[ iter->first - startNeuronID ]->calculateFinalState();
		}
	}
	/* In full neuron update mode, need to work through all the neurons and update them at 
		each time step.*/
	else{
		for(unsigned int i=0; i< numberOfNeurons; ++i)
			neuronArray[i]->calculateFinalState();
	}
	//Empty the update map in all modes
	neuronUpdateMap.clear();

	
	/* Send monitoring data - draws graphs of data sent by neuron and synapse classes. 
		In neuron monitor mode, need to extract parameters from the neuron and send them to the main application. */
	if(neuronMonitorMode){
		for(dense_hash_map<unsigned int, bool, hash<unsigned int> >::iterator iter = neuronMonitorMap.begin(); iter != neuronMonitorMap.end(); ++iter){
			//Send array of floats containing the monitoring data to the parent application
			sendNeuronData(parentTaskID, MONITOR_NEURON_DATA_MSG, iter->first, neuronArray[iter->first - startNeuronID]->getMonitoringData()->dataArray, neuronArray[iter->first - startNeuronID]->getMonitoringData()->length);
		}
	}
	/* In synapse monitor mode, need to extract parameters from the neuron and send them to the main application. */
	if(synapseMonitorMode){
		for(map<unsigned int*, Synapse*, synapseKeyCompare>::iterator iter = synapseMonitorMap.begin(); iter != synapseMonitorMap.end(); ++iter){
			//Send array of floats containing the monitoring data to the parent application
			sendSynapseData(parentTaskID, MONITOR_SYNAPSE_DATA_MSG, iter->first[0], iter->first[1], iter->second->getMonitoringData()->dataArray, iter->second->getMonitoringData()->length);
		}
	}


	/* To slow down the simulation for easy visualisation and to synchronize with external
		devices, it may be necessary to sleep for a bit. */
	if(calculateComputeTime){
		//Calculate how long this time step has taken. End time is larger than start time
		gettimeofday(&endComputeTimeStruct, NULL);
		timeStepComputeTime_us = 1000000 * (endComputeTimeStruct.tv_sec - startComputeTimeStruct.tv_sec) + endComputeTimeStruct.tv_usec - startComputeTimeStruct.tv_usec;

		/* When using synchronized UDP, sleep behaviour depends on the interval between messages
			received from the other class and whether a delay flag has been set in messages
			received from the other class. */	
		if( (simulationType == LIVE_SIMULATION) && (deviceManager->getDeviceType() == DeviceTypes::syncUDPNetworkInput) ){
			if(deviceManager->getExternalSyncDelay() == false){//Will sleep if the other process is not sleeping
				
				//Sleep for the difference between my compute time and the external compute time
				if(deviceManager->getExternalComputeTime_us() > timeStepComputeTime_us){
					unsigned int delayAmount = (unsigned int) rint((double)(deviceManager->getExternalComputeTime_us() - timeStepComputeTime_us) / 1.0);//Could reduce sleep time by small amount to reduce overshoot
					setDelayInLastTimeStep(true);
					usleep( delayAmount );
				}
				/* Simulation is running slower than external device, so no need to sleep.*/
				else{
					setDelayInLastTimeStep(false);
				}
			}
			else{//Other process is sleeping, so do not want to sleep or indicate that we are sleeping
				setDelayInLastTimeStep(false);
			}
		}

		/* Non UDP sync LIVE_SIMULATION, NO_INPUT_SIMULATION or PATTERN_SIMULATION modes.
			Just have simple sleep without informing	other tasks that we are delaying.*/
		else if (minTimeStepDuration_us > timeStepComputeTime_us){
			//Sleep for the time difference between current time step duration and min time step duration
			usleep(minTimeStepDuration_us - timeStepComputeTime_us);
		}

		//Get time at start of computation loop
		gettimeofday(&startComputeTimeStruct, NULL);
	}


	/* Communicate with external devices. Simulation will be stopped if there
		is an error with an external device. */
	if(simulationType == LIVE_SIMULATION){
		if(deviceManager->isOutputDevice()){
			if(!deviceManager->updateDevice()){//Update output device and check for errors
				simulationRunning = false;
			}
		}
		else if(deviceManager->isInputDevice()){
			if(!deviceManager->fetchData()){//Download data from input device and check for errors
				simulationRunning = false;
			}
		}
	}


	//Reset spike message count 
	spikeMessageCount = 0;


	/* Work through the spike task holders and send the current buffer to other tasks including
		this task */
	for(map<int, TaskHolder*>::iterator iter = spikeTaskHolderMap.begin(); iter != spikeTaskHolderMap.end(); ++iter){
		iter->second->sendSpikeMessages();
	}


	//Send firing neuron messages to external tasks
	neuronTaskHolder->sendFiringNeuronMessages();


	/* Have now sent all messages, so increase the simulation time step. */
	simulationClock->advance();


	//Deal with any messages that arrived early
	while(earlyMessageStack.size() > 0){

		//Get early message
		EarlyMessage earlyMsgPtr = earlyMessageStack.top();

		//Extract the information about the early message
		int msgTimeStep = earlyMsgPtr.timeStep;
		int sender = earlyMsgPtr.senderTID;
		int bufferID = earlyMsgPtr.bufferID;

		/* Reload the receive buffer for this message. 
			The set buffer method stores the current buffer. We need to store this 
			buffer number so that we can delete it. */
		int oldBufID = pvm_setrbuf(bufferID);
		#ifdef PVM_BUFFER_DEBUG
			cout<<"SendSpikeMessages1: Saving buffer with ID = "<<oldBufID<<"; Loading buffer with ID = "<<bufferID<<endl;
		#endif//PVM_BUFFER_DEBUG

		//Free memory from the old buffer, which we do not need.
		pvm_freebuf(oldBufID);
		#ifdef PVM_BUFFER_DEBUG
			cout<<"SendSpikeMessages2: Deleting buffer with ID = "<<oldBufID<<endl;
		#endif//PVM_BUFFER_DEBUG

		/* Remove message from stack before caling processSpikeList. Otherwise in some situations, 
			processSpikeList calls sendSpikeMessages, which in turn calls processSpikeList and so on */
		earlyMessageStack.pop();

		#ifdef MESSAGE_DEBUG
			cout<<"SpikeStreamSimulation: Reloading earlier message with timestep = "<<msgTimeStep<<"; senderTID = "; printTID(sender); cout<<endl;
		#endif//MESSAGE_DEBUG

		//Call process spike list for this buffer
		processSpikeList(sender, msgTimeStep);

		//Free memory from saved buffer that we have restored to the memory
		pvm_freebuf(bufferID);
		#ifdef PVM_BUFFER_DEBUG
			cout<<"SendSpikeMessages3: Deleting buffer with ID = "<<bufferID<<endl;
		#endif//PVM_BUFFER_DEBUG

		#ifdef MESSAGE_DEBUG
			cout<<"SpikeStreamSimulation: Finished reloading earlier message with timestep = "<<msgTimeStep<<"; senderTID = "; printTID(sender); cout<<endl;
		#endif//MESSAGE_DEBUG
	}


	//In PATTERN_SIMULATION mode, advance the pattern and fire neurons
	if(simulationType == PATTERN_SIMULATION){
		//Change the pattern if it has been exposed for sufficient time
		if(simulationClock->getTimeStep() % timeStepsPerPattern == 0)
			patternManager->loadPatternData();

		//Fire the neurons according to the pattern
		patternManager->fireNeurons();
	}

	/*In LIVE_SIMULATION mode, update neurons from device data that was fetched
		earlier. */
	else if(simulationType == LIVE_SIMULATION && deviceManager->isInputDevice()){
		deviceManager->updateNeurons();
	}
}


/*! Used when synchronizing to an external device using UDP.
	Adds or deletes an entry in the SynchronizationDelay database to set whether the
	simulation is delaying itself for synchronization or not. Need to use a database
	for this to enable communiation between tasks on potentially separate machines. */
void SpikeStreamSimulation::setDelayInLastTimeStep(bool dly){
	if(dly == delayInLastTimeStep)//Nothing has changed, so do nothing
		return;

	try{
		if(dly){//Delay has been switched on, so need to add this neuron group to Synchronization delay database
			Query deviceQuery = deviceDBInterface->getQuery();
			deviceQuery.reset();
			deviceQuery<<"INSERT INTO SynchronizationDelay (NeuronGrpID) VALUES ( "<<neuronGrpID<<" )";
			deviceQuery.execute();
		}
		else{
			Query deviceQuery = deviceDBInterface->getQuery();
			deviceQuery.reset();
			deviceQuery<<"DELETE FROM SynchronizationDelay WHERE NeuronGrpID = "<<neuronGrpID;
			deviceQuery.execute();
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		ostringstream errorStrStream;
		errorStrStream<<"Bad query when setting delay in last timestep: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown setting delay in last timestep: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown setting delay in last timestep: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
	}

	delayInLastTimeStep = dly;
}	


/*! Once neurons have been loaded, need to calculate the maximum size of the message 
	buffers in the task holders. This will be used to reserve space for the vectors
	to save increasing this at runtime. Another option would be to use arrays, but 
	this may not be significantly faster. */
void SpikeStreamSimulation::setMaxBufferSize(){
	//Create map that will be used to count the number of potential messages in each buffer for each task
	map<int, int*> messageCountMap;
	
	//Create arrays to count number of messages in each buffer
	for(map<int, TaskHolder*>::iterator iter = spikeTaskHolderMap.begin(); iter != spikeTaskHolderMap.end(); ++iter){
		//Create integer array to count the number of potential messages in the message buffer
		messageCountMap[iter->first] = new int[NUMBER_OF_DELAY_VALUES];
		//Initialise the array
		for(int i=0; i<NUMBER_OF_DELAY_VALUES; ++i)
			messageCountMap[iter->first][i] = 0;
	}
	
	//Work through neurons and calculate how many messages they could add to each buffer
	for(unsigned int i=0; i<numberOfNeurons; i++){
		/* Iterate through each of the neuron's connection maps, counting the number of 
			potential messages for each task. */
		for(map<int, vector<ConnectionHolder> >::iterator taskIter = neuronArray[i]->connectionMap.begin(); taskIter != neuronArray[i]->connectionMap.end(); ++taskIter){
			//Work through vector at this location in map
			for(vector<ConnectionHolder>::iterator connHoldIter =  taskIter->second.begin(); connHoldIter != taskIter->second.end(); ++connHoldIter){
				messageCountMap[taskIter->first][connHoldIter->delay]++;
			}
		}
	}
	
	/* Find the largest potential message buffer size for each task holder, reserve space
		in the vectors in the message buffers and delete the arrays that were created.*/
	for(map<int, int*>::iterator iter = messageCountMap.begin(); iter != messageCountMap.end(); ++iter){
		int maxBufferSize = 0;
		for(int i=0; i<NUMBER_OF_DELAY_VALUES; ++i){
			if(iter->second[i] > maxBufferSize)
				maxBufferSize = iter->second[i];
		}
		spikeTaskHolderMap[iter->first]->setMaxBufferSize(maxBufferSize);
		delete [] iter->second;
	}
}


/*! Sets the minimum duration of a time step in microseconds.
	Used to slow the simulation down. */
void SpikeStreamSimulation::setMinTimeStepDuration(int senderTID){

	//Unpack the new minimum time step duration from message
	unsigned int tmpMinTSDuration = 0;//Unpack to a temp in case the unpacking goes wrong.
	info = pvm_upkuint(&tmpMinTSDuration, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR UNPACKING MINIMUM TIME STEP DURATION FROM MESSAGE"; 
			printTID(senderTID);
			cerr<<". tmpMinTSDuration = "<<tmpMinTSDuration<<endl;
			systemError("ERROR UNPACKING MINIMUM TIME STEP DURATION FROM MESSAGE.");
			return;
		}
	#endif//PVM_DEBUG

	//Store min timestep duration
	minTimeStepDuration_us = tmpMinTSDuration;

	//Set the boolean to calculate the compute time appropriately
	if( (simulationType == LIVE_SIMULATION) && (deviceManager->getDeviceType() == DeviceTypes::syncUDPNetworkInput) )
		calculateComputeTime = true;//In this mode will always be calculating the compute time
	else if(minTimeStepDuration_us > 0)
		calculateComputeTime = true;//Calculate compute time if min time step duration has been set greater than 0
	else
		calculateComputeTime = false;//Zero minimum time step duration so no point in calculating compute time
}


/*! Sets whether the updates of the neuron and synapse classes are event driven, or whether
	all neuron and/or synapse classes are updated at each time step. */
void SpikeStreamSimulation::setUpdateMode(int senderTID){

	//Unpack the new update mode from message
	unsigned int newUpdateMode = 0;//Unpack to a temp in case the unpacking goes wrong.
	info = pvm_upkuint(&newUpdateMode, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR UNPACKING UPDATE MODE FROM MESSAGE"; 
			printTID(senderTID);
			cerr<<". newUpdateMode = "<<newUpdateMode<<endl;
			systemError("ERROR UNPACKING UPDATE MODE FROM MESSAGE.");
			return;
		}
	#endif//PVM_DEBUG

	//Set the update mode
	switch(newUpdateMode){
		case 1: updateAllNeurons = false; updateAllSynapses = false; break;
		case 2: updateAllNeurons = true; updateAllSynapses = false; break;
		case 3: updateAllNeurons = false; updateAllSynapses = true; break;
		case 4: updateAllNeurons = true; updateAllSynapses = true; break;
		default: systemError("Update mode not recognized: ", newUpdateMode);
	}
}


/*! Adds the neuron to the neuron monitoring map and sends back XML file that is used to 
	create the graphical plot of the neuron's data. */
void SpikeStreamSimulation::startNeuronMonitoring(int senderTID, bool restart){
	//We are now in neuron monitoring mode
	neuronMonitorMode = true;

	//Unpack the id of the neuron that is to be monitored from message
	unsigned int neurID = 0;//Unpack to a temp in case the unpacking goes wrong.
	info = pvm_upkuint(&neurID, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR UNPACKING NEURON ID FROM MESSAGE"; 
			printTID(senderTID);
			cerr<<". neurID = "<<neurID<<endl;
			systemError("ERROR UNPACKING NEURON ID FROM MESSAGE.");
			return;
		}
	#endif//PVM_DEBUG

	//Check that this is a valid key must be greater than the start neuron ID and l
	if( (neurID < startNeuronID) || neurID >= (startNeuronID + numberOfNeurons) ){
		cerr<<"SpikeStreamSimulation: Neuron ID supplied for neuron monitoring is invalid. neurID = "<<neurID<<endl;
		systemError("Neuron ID supplied for neuron monitoring is invalid.");
		return;
	}
	
	//Add neuron id to map. If it is already monitoring this statement will have no effect.
	neuronMonitorMap[neurID] = true;

	if(!restart){
		//Send XML data about the variables back to the main application
		sendMessage(parentTaskID, MONITOR_NEURON_INFO_MSG, neurID, neuronArray[neurID - startNeuronID]->getMonitoringInfo().data());
	}
	//Otherwise, data will be sent at every update step
}


/*! Adds the synapse to the synapse monitoring map and sends back XML file that is used to 
	create the graphical plot of the synapse's data. */
void SpikeStreamSimulation::startSynapseMonitoring(int senderTID, bool restart){

	//Unpack the id of the synapse that is to be monitored from message
	unsigned int fromNeurID = 0, toNeurID = 0;
	info = pvm_upkuint(&fromNeurID, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR UNPACKING FROM NEURON ID FROM MESSAGE"; 
			printTID(senderTID);
			cerr<<". neurID = "<<fromNeurID<<endl;
			systemError("ERROR UNPACKING FROM NEURON ID FROM MESSAGE.");
			return;
		}
	#endif//PVM_DEBUG	

	info = pvm_upkuint(&toNeurID, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR UNPACKING FROM NEURON ID FROM MESSAGE"; 
			printTID(senderTID);
			cerr<<". neurID = "<<toNeurID<<endl;
			systemError("ERROR UNPACKING FROM NEURON ID FROM MESSAGE.");
			return;
		}
	#endif//PVM_DEBUG

	//Check that this is a valid key
	if(synapseMap.count(fromNeurID)){
		if(!synapseMap[fromNeurID]->count(toNeurID)){
			cerr<<"SpikeStreamSimulation: To neuron key supplied for synapse monitoring is invalid. fromNeurID = "<<fromNeurID<<"; toNeurID = "<<toNeurID<<endl;
			systemError("Key supplied for synapse monitoring is invalid.");
			return;
		}
	}
	else{
		cerr<<"SpikeStreamSimulation: From neuron key supplied for synapse monitoring is invalid. fromNeurID = "<<fromNeurID<<"; toNeurID = "<<toNeurID<<endl;
		systemError("Key supplied for synapse monitoring is invalid.");
		return;
	}

	//Create array to act as key
	unsigned int* keyArray = new unsigned int[2];
	keyArray[0] = fromNeurID;
	keyArray[1] = toNeurID;
	
	//Add reference to synapse class to map. If it is already monitoring this statement will have no effect.
	synapseMonitorMap[keyArray] = (*synapseMap[fromNeurID])[toNeurID];

	if(!restart){
		//Send XML data about the variables back to the main application
		sendMessage(parentTaskID, MONITOR_SYNAPSE_INFO_MSG, fromNeurID, toNeurID, synapseMonitorMap[keyArray]->getMonitoringInfo().data());
	}
	//Otherwise, data will be sent at every update step

	//We are now in synapse monitoring mode
	synapseMonitorMode = true;
}


/*! In order to send a spike list, each task must have received a spike list and so unless they start
	sending spike lists to each other the whole thing will never begin. This method sends the current spike
	list to all recipients to initialise simulation. */
void SpikeStreamSimulation::startSimulation(){
	/* When monitoring statistics, need to start a timer to monitor for a particular period.*/
	#ifdef RECORD_STATISTICS
		if(simulationClock->getTimeStep() > 500){//Need to get neural network in stable state before doing stats
			//Start timer of statistical monitoring period
			statisticsTimer.start("Record statistics");

			//Initialise statistics structure
			statistics.neuronFireTotal = 0;
			statistics.startTimeStep = simulationClock->getTimeStep();
			statistics.spikeTotal = 0;
		}
	#endif//RECORD_STATISTICS

	simulationRunning = true;//Simulation is now running

	//Only want to send initial messages once. Simulation should run continuously after that.
	if(!simulationStarted){
		#ifdef TRANSPORT_DEBUG
			cout<<"Play simulation - starting: Sending spike messages"<<endl;
		#endif//TRANSPORT_DEBUG

		//Send spikes
		sendSpikeMessages();
		
		//Set simulation started to true. This information is needed in case step simulation is pressed before play
		simulationStarted = true;
	}

	// Simulation has started and there should be messages ready to be sent
	else{
		/* If all spike messages have been received, update neurons and send spike message to other tasks */
		if(spikeMessageCount == spikeMessageTotal){
			#ifdef TRANSPORT_DEBUG
				cout<<"Play simulation - already running: Sending spike messages"<<endl;
			#endif//TRANSPORT_DEBUG

			//Send spikes
			sendSpikeMessages();
		}
		else{
			#ifdef TRANSPORT_DEBUG
				cout<<"Play simulation: spikeMessageCount != spikeMessageTotal; spikeMessageCount = "<<spikeMessageCount<<"; spikeMessageTotal = "<<spikeMessageTotal<<endl;
			#endif//TRANSPORT_DEBUG
		}
	}
}


/*! Advances the simulation one step and then pauses it. */
void SpikeStreamSimulation::stepSimulation(){
	//This prevents simulation from sending message when it has processed a message
	simulationRunning = false;

	//Simulation cannot be stepped unless it has started
	if(!simulationStarted){
		#ifdef TRANSPORT_DEBUG
			cout<<"Step simulation - starting up: sending spike messages"<<endl;
		#endif//TRANSPORT_DEBUG

		//Send spikes
		sendSpikeMessages();
		
		//Set simulation started to true. This information is needed in case step simulation is pressed before play
		simulationStarted = true;
	}

	// Simulation has started and there should be messages ready to be sent
	else{
		/* If all spike messages have been received, update neurons and send spike message to other tasks */
		if(spikeMessageCount == spikeMessageTotal){
			#ifdef TRANSPORT_DEBUG
				cout<<"Step simulation - already running: sending spike messages"<<endl;
			#endif//TRANSPORT_DEBUG

			//Send spikes
			sendSpikeMessages();
		}
		else{
			#ifdef TRANSPORT_DEBUG
				cout<<"Step simulation - already running: not sending spike messages because spike message count less than number of Spike Messages"<<endl;
			#endif//TRANSPORT_DEBUG
		}
	}
}


/*! Stops the sending of monitor data for the specified neuron. */
void SpikeStreamSimulation::stopNeuronMonitoring(int senderTID){
	//Unpack the id of the neuron that is no longer to be monitored
	unsigned int neurID = 0;
	info = pvm_upkuint(&neurID, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR UNPACKING NEURON ID FROM MESSAGE"; 
			printTID(senderTID);
			cerr<<". neurID = "<<neurID<<endl;
			systemError("ERROR UNPACKING NEURON ID FROM MESSAGE.");
			return;
		}
	#endif//PVM_DEBUG

	//Remove neuron id from map
	neuronMonitorMap.erase(neurID);

	//Switch off neuron monitor mode if no neurons are being monitored
	if(neuronMonitorMap.empty())
		neuronMonitorMode = false;
}


/*! Stops the simulation running without destroying it. When it has processed all of its 
	incoming spike lists it waits before sending spike messages until it receives the start simulation 
	message again. */
void SpikeStreamSimulation::stopSimulation(){
	#ifdef TRANSPORT_DEBUG
		cout<<"Stop simulation - setting simulationRunning to false"<<endl;
	#endif//TRANSPORT_DEBUG

	simulationRunning = false;//Prevent spike messages from being sent
}


/*! Stops the sending of monitoring data for the specified synapse. */
void SpikeStreamSimulation::stopSynapseMonitoring(int senderTID){
	//Unpack the id of the synapse that is no longer to be monitored
	unsigned int fromNeurID = 0, toNeurID = 0;
	int info = pvm_upkuint(&fromNeurID, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR UNPACKING FROM NEURON ID FROM MESSAGE"; 
			printTID(senderTID);
			cerr<<". neurID = "<<fromNeurID<<endl;
			systemError("ERROR UNPACKING FROM NEURON ID FROM MESSAGE.");
			return;
		}
	#endif//PVM_DEBUG

	info = pvm_upkuint(&toNeurID, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamSimulation: ERROR UNPACKING TO NEURON ID FROM MESSAGE"; 
			printTID(senderTID);
			cerr<<". neurID = "<<toNeurID<<endl;
			systemError("ERROR UNPACKING TO NEURON ID FROM MESSAGE.");
			return;
		}
	#endif//PVM_DEBUG

	//Create an array to act as the key
	unsigned int tempArray[2];
	tempArray[0] = fromNeurID;
	tempArray[1] = toNeurID;

//	for(map<unsigned int*, Synapse*, synapseKeyCompare>::iterator iter = synapseMonitorMap.begin()


	//Check key is in map
	if(synapseMonitorMap.count(tempArray)){
		//Store the address of the key
		unsigned int* tmpKey = synapseMonitorMap.find(tempArray)->first;

		//Remove the address of the key array from the map
		synapseMonitorMap.erase(tempArray);

		//Deletes the array that is used as a key to this location in the map
		delete [] tmpKey;
	}
	else{
		systemError("SpikeStreamSimulation: Cannot find synapse key in map when stopping monitoring.");
		cerr<<"Cannot find synapse key in map when stopping monitoring:"<<tempArray[0]<<" "<<tempArray[1]<<endl;
	}

	//Switch off synapse monitor mode if no synapses are being monitored
	if(synapseMonitorMap.empty())
		synapseMonitorMode = false;
}


