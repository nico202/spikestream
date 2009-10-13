/***************************************************************************
 *   SpikeStream Application                                               *
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
#include "SimulationManager.h"
#include "SimulationWidget.h"
#include "Debug.h"
#include "PVMMessages.h"
#include "ConnectionType.h"
#include "Utilities.h"
#include "SpikeStreamMainWindow.h"

//Qt includes
#include <qapplication.h>
#include <qmessagebox.h>
#include <qtimer.h>

//Other includes
#include <signal.h>
#include <stdio.h>
#include "pvm3.h"
#include <mysql++.h>
#include <iostream>
using namespace std;
using namespace mysqlpp;


//Declare static variables
bool SimulationManager::stop;


/*! Constructor */
SimulationManager::SimulationManager(DBInterface *netDBInter, DBInterface *archDBInter, DBInterface *pattDBInter, DBInterface *devDBInter, QWidget *simWidg){
	//Store a reference to the DBInterface
	networkDBInterface = netDBInter;
	archiveDBInterface = archDBInter;
	patternDBInterface = pattDBInter;
	deviceDBInterface = devDBInter;

	//Set up a short version of this reference
	spikeStrApp = SpikeStreamMainWindow::spikeStreamApplication;

	//Store reference to parent to display progress and busy dialog
	simulationWidget = simWidg;

	//Create busy dialog
	busyDialog = new BusyDialog(simulationWidget, QString("Simulation"));

	//Graphics are not loading at this point
	graphicsLoading = false;

	//Initialise stop to true
	stop = true;

	//Initialise simulationRunning to false
	simulationRunning = false;

	//Set default max load time - should be set in config file.
	maxNeurDataLoadTime_sec = 200;
}


/*! Destructor
	Simulation should have been stopped by SpikeStremMainWindow before
	any deleting is done. Otherwise the networkDBInterface becomes unavailable for 
	resetting the database.*/
SimulationManager::~SimulationManager(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING SIMULATION MANAGER"<<endl;
	#endif//MEMORY_DEBUG
}


//-------------------------------------------------------------------------
//------------------------- PUBLIC METHODS --------------------------------
//-------------------------------------------------------------------------

/*! Clears the record of a simulation error and resets the error message. */
void SimulationManager::clearSimulationError(){
	simError = false;
	simulationErrorMsg = "";
}


/*! Stops the simulation.*/
bool SimulationManager::destroySimulation(){
	cout<<"Destroying simulation"<<endl;
	//Show busy dialog
	busyDialog->showDialog(QString("Cleaning up simulation, please wait"));

	// Set stop to true to exit from run method. When this exits the simulation is cleaned up
	stop = true;
	
	simulationRunning = false;

	return true;
}


/*! Fires a specified neuron by sending a message to the appropriate task. */
void SimulationManager::fireNeuron(unsigned int neuronGrpID, unsigned int neuronID){
	unsigned int tempArray[1];
	tempArray[0] = neuronID;
	sendMessage(neuronGrpTaskMap[neuronGrpID], FIRE_SPECIFIED_NEURONS_MSG, tempArray, 1);
}


/*! Returns whether there was an error generated during clean up of 
	the simulation. */
bool SimulationManager::getCleanUpError(){
	return cleanUpError;
}


/*! Returns a description of errors generated during clean up of the simulation.*/
QString SimulationManager::getCleanUpErrorMsg(){
	return cleanUpErrorMsg;
}


/*! Returns error messages generated during initialisation of the simulation. */
QString SimulationManager::getInitErrorMsg(){
	return initErrorMsg;
}


/*! Returns errors generated during the simulation run. */
QString SimulationManager::getSimulationErrorMsg(){
	return simulationErrorMsg;
}


/*! Returns true if the view weights have been saved. */
bool SimulationManager::getViewWeightsSaved(){
	return viewWeightsSaved;
}


/*! Returns true if the loading of weights is complete. */
bool SimulationManager::getWeightsLoaded(){
	return synapseWeightsLoaded;
}


/*! Returns true if the weights have all been saved. */
bool SimulationManager::getWeightsSaved(){
	return weightsSaved;
}


/*! Initialises the simulation */
StartSimRes SimulationManager::initialiseSimulation(QString archiveName, map<unsigned int, unsigned int> patternInputMap, map<unsigned int, unsigned int> deviceInOutMap, map<unsigned int, double>deviceFiringModeMap, map<const char*, unsigned int> parameterMap){
	//Create StartSimRes to return result of initialisation
	StartSimRes startSimRes;
	startSimRes.canceled = false;
	startSimRes.started = false;

	//Reset the error messages
	initErrorMsg = "";
	simError = false;
	simulationErrorMsg = "";

	//Show message dialog with information about what is going on
	Q3ProgressDialog *progressDialog = new Q3ProgressDialog("Starting simulation", "Cancel", 6, simulationWidget, "Creating virtual connections", true);
	progressDialog->setMinimumDuration(0);
	//Process events and quit if cancelled
	spikeStrApp->processEvents();
	if(progressDialog->wasCancelled()){
		cleanUpSimulation();
		startSimRes.canceled = true;
		return startSimRes;
	}

	//Initialise data that might have been left over from a previous simulation
	archiveTaskID = -1;
	neuronGrpTaskMap.clear();
	neuronGrpTaskVector.clear();

	//Set weights saved to true since they are being loaded up from the database
	weightsSaved = true;

	/* Create virtual connections for all neuron groups that are not reciprocally connected
		This is needed in the present version to avoid synchronization errors */
	if(!createTempVirtualConnections()){
		initErrorMsg += "Failed to create virtual connections.\n";
		cleanUpSimulation();
		progressDialog->cancel();
		return startSimRes;	
	}
	progressDialog->setProgress(1);
	progressDialog->setLabelText("Starting PVM");
	spikeStrApp->processEvents();
	if(progressDialog->wasCancelled()){
		cleanUpSimulation();
		startSimRes.canceled = true;
		return startSimRes;
	}

	//Start pvm daemon if it is not already running
	if(!startPvm()){
		initErrorMsg += "PVM start failed.\n";
		cleanUpSimulation();
		progressDialog->cancel();
		return startSimRes;
	}
	progressDialog->setProgress(2);
	progressDialog->setLabelText("Spawning tasks");
	spikeStrApp->processEvents();
	if(progressDialog->wasCancelled()){
		cleanUpSimulation();
		startSimRes.canceled = true;
		return startSimRes;
	}

	cout<<"Starting neuron simulation. Parent task id = "<<thisTaskID<<endl;

	//Spawn tasks to simulate neuron groups
	if(!spawnNeuronGroupTasks()){
		initErrorMsg += "Spawn tasks failed.\n";
		cleanUpSimulation();
		progressDialog->cancel();
		return startSimRes;
	}
	progressDialog->setProgress(3);
	progressDialog->setLabelText("Checking tasks");
	spikeStrApp->processEvents();
	if(progressDialog->wasCancelled()){
		cleanUpSimulation();
		startSimRes.canceled = true;
		return startSimRes;
	}

	//Check that tasks have written their task id to the database
	if(!checkNeuronGroupTasks()){
		initErrorMsg += "Launch of tasks could not be checked.\n";
		cleanUpSimulation();
		progressDialog->cancel();
		return startSimRes;
	}
	progressDialog->setProgress(4);
	progressDialog->setLabelText("Loading neuron data");
	spikeStrApp->processEvents();
	if(progressDialog->wasCancelled()){
		cleanUpSimulation();
		startSimRes.canceled = true;
		return startSimRes;
	}

	//Instruct tasks to load up neuron data
	StartSimRes startNeurRes = loadNeuronData(patternInputMap, deviceInOutMap, deviceFiringModeMap, parameterMap, progressDialog);
	if(!startNeurRes.started && !startNeurRes.canceled){//Error loading up neuron data
		initErrorMsg += "Neuron data could not be loaded.\n";
		cleanUpSimulation();
		progressDialog->cancel();
		return startSimRes;	
	}
	else if(startNeurRes.canceled){
		cleanUpSimulation();
		startSimRes.canceled = true;
		return startSimRes;
	}
	progressDialog->setProgress(5);
	progressDialog->setLabelText("Starting archive task");
	spikeStrApp->processEvents();

	//Spawn the archive task to record neuron data
	if(!spawnArchiveTask(archiveName, parameterMap)){
		initErrorMsg += "Error spawning archive task.\n";
		cleanUpSimulation();
		progressDialog->cancel();
		return startSimRes;
	}
	
	//If have reached this point, simulation should have started ok
	#ifdef SIMULATION_LOAD_DEBUG
		cout<<"Finished initialisation of simulation"<<endl;
	#endif//SIMULATION_LOAD_DEBUG

	//Start this thread running
	stop = false;
	start();

	//Clear progress dialog and return successful start information
	progressDialog->setProgress(6);
	spikeStrApp->processEvents();
	delete progressDialog;
	startSimRes.started = true;
	return startSimRes;
}


/*! Injects an amount of noise into a neuron group. */
void SimulationManager::injectNoise(unsigned int neuronGrpID, int amount){
	int neurGrpTaskID = neuronGrpTaskMap[neuronGrpID];
	sendMessage(neurGrpTaskID, INJECT_NOISE_MSG, amount);
}


/*! Static method used by other classes to query whether the simulation is 
	initialised or not. */
bool SimulationManager::isInitialised(){
	if (stop)
		return false;
	return true;
}


/*! Returns true if the simulation is running. */
bool SimulationManager::isRunning(){
	return simulationRunning;
}


/*! Instructs the tasks running neuron groups to reload their weights from the database.
	This task sends the reload weights instructions and then the run method is used to receive
	the confirmation messages, which set synapseWeightsSaved to true when they have all been received. */
bool SimulationManager::loadWeights(){
	//Check to see if there are any tasks before continuing
	if(neuronGrpTaskMap.empty()){
		synapseWeightsLoaded = true;
		return true;
	}

	//Instruct tasks to load weights
	synapseWeightsLoaded = false;
	weightsLoadedAcknowledgementMap.clear();
	for(map<unsigned int, int>::iterator iter = neuronGrpTaskMap.begin(); iter != neuronGrpTaskMap.end(); ++iter){
		bool messageSent = sendMessage(iter->second, LOAD_WEIGHTS_MSG);
		if(!messageSent){
			return false;
		}
		//Store task ids in a map to check that they have all been received
		weightsLoadedAcknowledgementMap[iter->second] = false;
	}
	return true;
}


/*! Instructs the simulation manager to start monitoring a neuron group.
	This involves sending a message to the appropriate task. When the spike
	lists	are received they are automatically passed to the appropriate 
	network monitor. */
void SimulationManager::monitorNeuronGroup(unsigned int neuronGrpID, bool monitorNeurons){
	if(isInitialised() && !graphicsLoading){//Don't want to try to send stray messages when not initialised or when graphics are loading
		int neurGrpTaskID = neuronGrpTaskMap[neuronGrpID];
		if(monitorNeurons)
			sendMessage(neurGrpTaskID, REQUEST_FIRING_NEURON_DATA_MSG);
		else
			sendMessage(neurGrpTaskID, REQUEST_SPIKE_DATA_MSG);
	}
}


/*! Sends a message to a neuron group requesting an XML file containing information about
	the variables that are to be monitored in the neuron. Different neuron implementations
	have different variables, so need to get this information each time.*/
bool SimulationManager::startNeuronMonitoring(unsigned int neuronGrpID, unsigned int neuronID, bool restart){
	if(!restart){//Do not have the neuron info
		//Send message to neuron group requesting the monitor neuron information.
		return sendMessage_unsigned(neuronGrpTaskMap[neuronGrpID], REQUEST_MONITOR_NEURON_INFO_MSG, neuronID);
	}
	else{//Want to restart the monitoring
		return sendMessage_unsigned(neuronGrpTaskMap[neuronGrpID], START_MONITORING_NEURON_MSG, neuronID);
	}
}


/*! Sends a message to a neuron group requesting an XML file containing information about
	the variables that are to be monitored in the neuron. Different neuron implementations
	have different variables, so need to get this information each time. */
bool SimulationManager::startSynapseMonitoring(unsigned int neuronGrpID, unsigned int fromNeuronID, unsigned int toNeuronID, bool restart){
	if(!restart){//Do not have the neuron info
		//Send message to neuron group requesting the monitor neuron information.
		return sendMessage(neuronGrpTaskMap[neuronGrpID], REQUEST_MONITOR_SYNAPSE_INFO_MSG, fromNeuronID, toNeuronID);
	}
	else{//Want to restart the monitoring
		return sendMessage(neuronGrpTaskMap[neuronGrpID], START_MONITORING_SYNAPSE_MSG, fromNeuronID, toNeuronID);
	}
}


/*! Sends a message to a neuron group instructing it to stop sending neuron data messages. */
bool SimulationManager::stopNeuronMonitoring(unsigned int neuronGrpID, unsigned int neuronID){
	//Send message to neuron group requesting the monitor neuron information.
	return sendMessage_unsigned(neuronGrpTaskMap[neuronGrpID], STOP_MONITORING_NEURON_MSG, neuronID);	
}


/*! Sends a message to a neuron group instructing it to stop sending neuron data messages. */
bool SimulationManager::stopSynapseMonitoring(unsigned int neuronGrpID, unsigned int fromNeuronID, unsigned int toNeuronID){
	//Send message to neuron group requesting the monitor neuron information.
	return sendMessage(neuronGrpTaskMap[neuronGrpID], STOP_MONITORING_SYNAPSE_MSG, fromNeuronID, toNeuronID);	
}


/*! Instructs the tasks to save a tempory copy of their weights to the database so that they
	can be easily viewed. This could also be done by requesting the weights from a neuron group, 
	which would save database space, but would be more messy to implement */
bool SimulationManager::saveViewWeights(){
	//Check to see if there are any tasks before continuing
	if(neuronGrpTaskMap.empty()){
		viewWeightsSaved = true;
		return true;
	}

	//Instruct tasks to save the weights
	viewWeightsSaved = false;
	viewWeightsSavedAcknowledgementMap.clear();
	for(map<unsigned int, int>::iterator iter = neuronGrpTaskMap.begin(); iter != neuronGrpTaskMap.end(); ++iter){
		bool messageSent = sendMessage(iter->second, SAVE_VIEW_WEIGHTS_MSG);
		if(!messageSent){
			return false;
		}
		//Store task ids in a map to check that they have all been received
		viewWeightsSavedAcknowledgementMap[iter->second] = false;
	}
	return true;
}


/*! Instructs the tasks running neuron groups to write their weights to the database.
	This task sends the save weights instructions and then the run method is used to receive
	the confirmation methods which set synapseWeightsSaved to true when they have all been received. */
bool SimulationManager::saveWeights(){
	//Check to see if there are any tasks before continuing
	if(neuronGrpTaskMap.empty()){
		weightsSaved = true;
		return true;
	}

	//Instruct tasks to save weights
	weightsSaved = false;
	weightsSavedAcknowledgementMap.clear();
	for(map<unsigned int, int>::iterator iter = neuronGrpTaskMap.begin(); iter != neuronGrpTaskMap.end(); ++iter){
		bool messageSent = sendMessage(iter->second, SAVE_WEIGHTS_MSG);
		if(!messageSent){
			return false;
		}
		//Store task ids in a map to check that they have all been received
		weightsSavedAcknowledgementMap[iter->second] = false;
	}
	return true;
}


/*! Controls whether the simulation runs at less than its maximum speed
	This is done by instructing the tasks to sleep for the appropriate
	duration every time step. */
void SimulationManager::setFrameRate(int fr){
	if(isInitialised()){
		//Maximum frame rate, set time step sleep interval to be zero
		unsigned int minTimeStepDuration;
		if(fr < 0)
			minTimeStepDuration = 0;
		else{//Need to divide 1000000 by the frame rate to get the mimimum time step duration in microseconds
			minTimeStepDuration = 1000000 / fr;
		}
		for(map<unsigned int, int>::iterator iter = neuronGrpTaskMap.begin(); iter != neuronGrpTaskMap.end(); ++iter){
			bool messageSent = sendMessage(iter->second, SET_MIN_TIMESTEP_DURATION_US_MSG, minTimeStepDuration);
			if(!messageSent){
				showErrorMessage("SimulationManager: PROBLEM SENDING SET_MIN_TIMESTEP_DURATION_US_MSG MESSAGE");
			}
		}
	}
}


/*! Sends a message to each task instructing it to load the global parameters. */
bool SimulationManager::setGlobalParameters(){
	if(isInitialised()){
		for(map<unsigned int, int>::iterator iter = neuronGrpTaskMap.begin(); iter != neuronGrpTaskMap.end(); ++iter){
			bool messageSent = sendMessage(iter->second, LOAD_GLOBAL_PARAMETERS_MSG);
			if(!messageSent){
				return false;
			}
			//Could think about checking with acknowledgement message, but this would have to be done in the run method
		}
	}
	return true;
}


/*! Called at the end of initialisation to instruct this class that all graphics loading
	is complete. */
//	FIXME DESIGNED TO FILTER OUT STRAY MESSAGES, BUT NOT PROPERLY TESTED YET.
void SimulationManager::setGraphicsLoading(bool gl){
	graphicsLoading = gl;
}


/*! Sets the maximum time that the simulation manager will wait for tasks to load their
	data. */
void SimulationManager::setMaxNeurDataLoadTime_sec(int mNeurDataLoadTime){
	maxNeurDataLoadTime_sec = mNeurDataLoadTime;
}


/*! Passes a reference to the monitor area to enable the simulation manager 
	to send spike messages to the network monitor for display.
	The incoming map references the network monitors by neuron group id, but 
	need to store them by task id so that messages can be directly routed to 	
	them. */
void SimulationManager::setNetworkMonitors(map<unsigned int, NetworkMonitor*> nwMonMap){
	networkMonitorMap.clear();
	for(map<unsigned int, NetworkMonitor*>::iterator iter = nwMonMap.begin(); iter != nwMonMap.end(); ++iter){
		networkMonitorMap[neuronGrpTaskMap[iter->first]] = iter->second;
	}
}


/*! Passes a reference to the neuron monitor map to enable the simulation manager
	to pass data to the neuron monitors. Only pass a reference because the 
	contents of the map are changed each time the simulation starts. */
void SimulationManager::setNeuronMonitors(map<unsigned int, MonitorDataPlotter*> *neurMonMap){
	neuronMonitorMap = neurMonMap;
}


/*! Send message to each task instructing it to load its parameters. */
bool SimulationManager::setNeuronParameters(){
	if(isInitialised()){
		for(map<unsigned int, int>::iterator iter = neuronGrpTaskMap.begin(); iter != neuronGrpTaskMap.end(); ++iter){
			bool messageSent = sendMessage(iter->second, LOAD_NEURON_PARAMETERS_MSG);
			if(!messageSent){
				return false;
			}
			//Could think about checking with acknowledgement message, but this would have to be done in the run method
		}
	}
	return true;
}


/*! Instructs each task to load its noise parameters from the database. */
bool SimulationManager::setNoiseParameters(){
	if(isInitialised()){
		for(map<unsigned int, int>::iterator iter = neuronGrpTaskMap.begin(); iter != neuronGrpTaskMap.end(); ++iter){
			bool messageSent = sendMessage(iter->second, LOAD_NOISE_PARAMETERS_MSG);
			if(!messageSent){
				return false;
			}
			//Could think about checking with acknowledgement message, but this would have to be done in the run method
		}
	}
	return true;
}


/*! Passes a reference to the neuron monitor map to enable the simulation manager
	to pass data to the neuron monitors. Only pass a reference because the 
	contents of the map are changed each time the simulation starts. */
void SimulationManager::setSynapseMonitors(map<unsigned int*, MonitorDataPlotter*, synapseKeyCompare> *synMonMap){
	synapseMonitorMap = synMonMap;
}


/*! Sends a message to each task instructing it to load its synapse parameters. */
bool SimulationManager::setSynapseParameters(){
	if(isInitialised()){
		for(map<unsigned int, int>::iterator iter = neuronGrpTaskMap.begin(); iter != neuronGrpTaskMap.end(); ++iter){
			bool messageSent = sendMessage(iter->second, LOAD_SYNAPSE_PARAMETERS_MSG);
			if(!messageSent){
				return false;
			}
			//Could think about checking with acknowledgement message, but this would have to be done in the run method
		}
	}
	return true;
}


/*! Sets the update mode for the simulation.
	Most efficient is event-driven, but may want to update the neuron or 
	synapse classes every time step to simulate spontaneous behaviour. */
void SimulationManager::setUpdateMode(bool updateNeurons, bool updateSynapses){
	unsigned int updateInt = 0;
	if(!updateNeurons && !updateSynapses)
		updateInt = 1;
	else if (updateNeurons && !updateSynapses)
		updateInt = 2;
	else if (!updateNeurons && updateSynapses)
		updateInt = 3;
	else
		updateInt = 4;

	for(map<unsigned int, int>::iterator iter = neuronGrpTaskMap.begin(); iter != neuronGrpTaskMap.end(); ++iter){
		bool messageSent = sendMessage(iter->second, SET_UPDATE_MODE_MSG, updateInt);
		if(!messageSent){
			showErrorMessage("SimulationManager: PROBLEM SENDING SET_UPDATE_MODE MESSAGE");
		}
	}
}


/*! Returns true if there has been an error in the simulation. */
bool SimulationManager::simulationError(){
	return simError;
}


/*! Sends a message to the archiver instructing it to start recording. */
bool SimulationManager::startRecording(){
	bool messageSent = sendMessage(archiveTaskID, START_ARCHIVING_MSG);
	if(!messageSent)
		return false;
	return true;
}


/*! Sends a message to all tasks instructing them to start sending spike messages. */
bool SimulationManager::startSimulation(){
	#ifdef TRANSPORT_DEBUG
		cout<<"Transport: Start simulation"<<endl;
	#endif//TRANSPORT_DEBUG

	//Work through running tasks and send a message instructing them to start
	for(map<unsigned int, int>::iterator iter = neuronGrpTaskMap.begin(); iter != neuronGrpTaskMap.end(); ++iter){
		bool messageSent = sendMessage(iter->second, START_SIMULATION_MSG);
		if(!messageSent){
			simulationRunning = false;
			return false;
		}
	}
	weightsSaved = false;//Some of the tasks could be in learning mode so weights could be changing at this point
	simulationRunning = true;
	return true;
}


/*! Sends messages instructing simulation to advance one step and then stop. */
bool SimulationManager::stepSimulation(){
	#ifdef TRANSPORT_DEBUG
		cout<<"Transport: Step simulation"<<endl;
	#endif//TRANSPORT_DEBUG

	//Work through running tasks and send a message instructing them to advance one step and then pause
	for(map<unsigned int, int>::iterator iter = neuronGrpTaskMap.begin(); iter != neuronGrpTaskMap.end(); ++iter){
		bool messageSent = sendMessage(iter->second, STEP_SIMULATION_MSG);
		if(!messageSent){
			return false;
		}
	}
	weightsSaved = false;//Some of the tasks could be in learning mode so weights could be changing at this point
	simulationRunning = false; //Simulation should have successfully stopped after step FIXME NEED A CONFIRMATION MESSAGE TO BE SURE THAT THIS IS REALLY TRUE
	return true;
}


/*! Stops monitoring a neuron group. */
void SimulationManager::stopMonitoringNeuronGroup(unsigned int neuronGrpID, bool monitorNeurons){
	if(isInitialised() && !graphicsLoading){//Don't want to try to send stray messages when not initialised or when graphics are loading
		int neurGrpTaskID = neuronGrpTaskMap[neuronGrpID];
		if(monitorNeurons)
			sendMessage(neurGrpTaskID, CANCEL_FIRING_NEURON_DATA_MSG);
		else
			sendMessage(neurGrpTaskID, CANCEL_SPIKE_DATA_MSG);
	}
}


/*! Sends a message to the archiver instructing it to stop recording spike patterns. */
bool SimulationManager::stopRecording(){
	bool messageSent = sendMessage(archiveTaskID, STOP_ARCHIVING_MSG);
	if(!messageSent)
		return false;
	return true;
}


/*! Sends a message to all tasks instructing them to stop sending spike messages. */
bool SimulationManager::stopSimulation(){
	#ifdef TRANSPORT_DEBUG
		cout<<"Transport: Stop simulation"<<endl;
	#endif//TRANSPORT_DEBUG

	//Work through running tasks and send a message instructing them to start
	for(map<unsigned int, int>::iterator iter = neuronGrpTaskMap.begin(); iter != neuronGrpTaskMap.end(); ++iter){
		bool messageSent = sendMessage(iter->second, STOP_SIMULATION_MSG);
		if(!messageSent){
			return false;
		}
	}
	simulationRunning = false; //Simulation has been successfully stopped FIXME NEED A CONFIRMATION MESSAGE TO BE SURE THAT THIS IS REALLY TRUE
	return true;
}


//-----------------------------------------------------------------------
//---------------- PROTECTED METHODS INHERITED FROM QTHREAD -------------
//-----------------------------------------------------------------------

/*! Main run method. */
void SimulationManager::run(){
	struct timeval tmout;
	tmout.tv_sec = 5;
	tmout.tv_usec = 0;

	#ifdef TRANSPORT_DEBUG
		cout<<"Transport: run"<<endl;
	#endif//TRANSPORT_DEBUG

	while(!stop){
		//Timeout receive - waiting for message from other task.
		int bufID = pvm_trecv(-1, -1, &tmout);
		if(bufID < 0)
			cerr<<"Receive error: "<<bufID<<endl;
		else if (bufID > 0){
			int numBytes, msgtag, senderTid;
			int info = pvm_bufinfo(bufID, &numBytes, &msgtag, &senderTid);//Get info about message
			if(info < 0)
				pvm_perror("SimulationManager.cpp: PROBLEM GETTING BUFFER INFO");
			else{
				switch(msgtag){
					case(SPIKE_LIST_MSG):
						#ifdef MESSAGE_DEBUG
							cout<<"Task "<<thisTaskID<<": SPIKE_LIST_MSG "<<numBytes<<" bytes received from "<<senderTid<<endl;
						#endif//MESSAGE_DEBUG
						networkMonitorMap[senderTid]->processSpikeList();
					break;
					case(FIRING_NEURON_LIST_MSG):
						#ifdef MESSAGE_DEBUG
							cout<<"Task "<<thisTaskID<<": FIRING_NEURON_LIST_MSG "<<numBytes<<" bytes received from "<<senderTid<<endl;
						#endif//MESSAGE_DEBUG
						networkMonitorMap[senderTid]->processFiringNeuronList();
					break;
					case(MONITOR_NEURON_INFO_MSG):
						#ifdef MESSAGE_DEBUG
							cout<<"Task "<<thisTaskID<<": MONITOR_NEURON_INFO_MSG "<<numBytes<<" bytes received from "<<senderTid<<endl;
						#endif//MESSAGE_DEBUG
						unpackMonitorNeuronInfo();
					break;
					case(MONITOR_NEURON_DATA_MSG):
						#ifdef MESSAGE_DEBUG
							cout<<"Task "<<thisTaskID<<": MONITOR_NEURON_DATA_MSG "<<numBytes<<" bytes received from "<<senderTid<<endl;
						#endif//MESSAGE_DEBUG
						unpackMonitorNeuronData();
					break;
					case(MONITOR_SYNAPSE_INFO_MSG):
						#ifdef MESSAGE_DEBUG
							cout<<"Task "<<thisTaskID<<": MONITOR_SYNAPSE_INFO_MSG "<<numBytes<<" bytes received from "<<senderTid<<endl;
						#endif//MESSAGE_DEBUG
						unpackMonitorSynapseInfo();
					break;
					case(MONITOR_SYNAPSE_DATA_MSG):
						#ifdef MESSAGE_DEBUG
							cout<<"Task "<<thisTaskID<<": MONITOR_SYNAPSE_DATA_MSG "<<numBytes<<" bytes received from "<<senderTid<<endl;
						#endif//MESSAGE_DEBUG
						unpackMonitorSynapseData();
					break;
					case(TEST_MSG):
						#ifdef MESSAGE_DEBUG
							cout<<"Task "<<thisTaskID<<": TEST_MSG "<<numBytes<<" bytes received from "<<senderTid<<endl;
						#endif//MESSAGE_DEBUG
					break;
					case(WEIGHT_SAVE_SUCCESS_MSG):
						#ifdef MESSAGE_DEBUG
							cout<<"Task "<<thisTaskID<<": WEIGHT_SAVE_SUCCESS_MSG "<<numBytes<<" bytes received from "<<senderTid<<endl;
						#endif//MESSAGE_DEBUG
						updateWeightsSavedState(senderTid);
					break;
					case(VIEW_WEIGHTS_SAVE_SUCCESS_MSG):
						#ifdef MESSAGE_DEBUG
							cout<<"Task "<<thisTaskID<<": VIEW_WEIGHTS_SAVE_SUCCESS_MSG "<<numBytes<<" bytes received from "<<senderTid<<endl;
						#endif//MESSAGE_DEBUG
						updateViewWeightsSavedState(senderTid);
					break;
					case(LOAD_WEIGHTS_SUCCESS_MSG):
						#ifdef MESSAGE_DEBUG
							cout<<"Task "<<thisTaskID<<": LOAD_WEIGHTS_SUCCESS_MSG "<<numBytes<<" bytes received from "<<senderTid<<endl;
						#endif//MESSAGE_DEBUG
						updateWeightsLoadedState(senderTid);
					break;
					case(ERROR_MSG):
						#ifdef MESSAGE_DEBUG
							cout<<"Task "<<thisTaskID<<": ERROR_MSG "<<numBytes<<" bytes received from "<<senderTid<<endl;
						#endif//MESSAGE_DEBUG
						showErrorMessage(senderTid);
					break;
					case(INFORMATION_MSG):
						#ifdef MESSAGE_DEBUG
							cout<<"Task "<<thisTaskID<<": INFORMATION_MSG "<<numBytes<<" bytes received from "<<senderTid<<endl;
						#endif//MESSAGE_DEBUG
						showInformationMessage();
					break;
					default:
						cerr<<"Task "<<thisTaskID<<": *UNRECOGNIZED MESSAGE* msgtag = "<<msgtag<<" size "<<numBytes<<" bytes received from "<<senderTid<<endl;
				}
			}
		}
		else if (bufID ==0)
			cout<<"Checking for messages"<<endl;//Shows that it is alive
	}
	
	//When it has exited the while loop need to clean up the simulation before exiting the thread
	cleanUpSimulation();
}


//------------------------------------------------------------------------
//-------------------------- PRIVATE METHODS -----------------------------
//------------------------------------------------------------------------

/*! When all tasks have been spawned this method check that each task has entered its 
	task id in the Neuron Group table. */
bool SimulationManager::checkNeuronGroupTasks(){
	try{
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"SELECT NeuronGrpID, TaskID FROM NeuronGroups";
                StoreQueryResult neuronGrpRes = query.store();
                for(StoreQueryResult::iterator neuronGrpIter = neuronGrpRes.begin(); neuronGrpIter != neuronGrpRes.end(); ++neuronGrpIter){
			Row neuronGrpRow(*neuronGrpIter);
			unsigned int neuronGrpID = Utilities::getUInt((std::string)neuronGrpRow["NeuronGrpID"]);
			int dbTaskID = Utilities::getInt((std::string)neuronGrpRow["TaskID"]);
			if(neuronGrpTaskMap[neuronGrpID] != dbTaskID){//Error somewhere, possibly because task cannot connect to database
				cerr<<"SimulationManager: TASKID NOT CORRECT IN DATABASE. ABORTING SIMULATION"<<endl;
				return false;
			}
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"SimulationManager: MYSQL QUERY EXCEPTION \""<<er.what()<<"\" checking neuron group tasks."<<endl;
		return false;
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"SimulationManager: MYSQL EXCEPTION \""<<er.what()<<"\" checking neuron group tasks."<<endl;
		return false;
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"SimulationManager: STD EXCEPTION \""<<er.what()<<"\" checking neuron group tasks."<<endl;
		return false;
	}
	return true;
}


/*! Unpacks an error message received during clean up, writes it to cerr
	and stores it for simulation widget to display. */
void SimulationManager::cleanUpErrorMsgReceived(int senderTID){
	//Get the length of the message
	unsigned int messageLength;
	int info = pvm_upkuint(&messageLength, 1, 1);
	if(info < 0){
		cerr<<"SimulationManager: ERROR EXTRACTING CHAR* LENGTH"<<endl;
		return;
	}

	//Unpack the message
	char charArray[messageLength];
 	pvm_upkstr(charArray);

	//Write error message to the console
	cerr<<"SimulationManager: CLEAN UP ERROR MESSAGE FROM "<<senderTID<<": "<<charArray<<endl;

	//Add to clean up error string for display
	cleanUpErrorMsg += "ERROR MESSAGE FROM ";
	cleanUpErrorMsg += QString::number(senderTID) += ": ";
	cleanUpErrorMsg += charArray;
	cleanUpErrorMsg += "\n";
	cleanUpError = true;
}


/*! Cleans up the simulation after it has exited from the run method. */
void SimulationManager::cleanUpSimulation(){
	#ifdef SIMULATION_CLEAN_UP_DEBUG
		cout<<"SimulationManager: Cleaning up simulation"<<endl;
	#endif//SIMULATION_CLEAN_UP_DEBUG

	//Create timeout for the archiver
	struct timeval archiveTimeout;
	archiveTimeout.tv_sec = 20;//Should not take more than 20 seconds for tasks to clean up
	archiveTimeout.tv_usec = 0;

	//Timeout for ordinary tasks - runs in a loop
	struct timeval taskTimeout;
	taskTimeout.tv_sec = 0;
	taskTimeout.tv_usec = 200000;

	/* Initialise variable to record errors. Want to keep cleaning up in the 
		face of errors to reduce damage and maximise information to the user. */
	cleanUpError = false;	
	cleanUpErrorMsg = "";

	//Instruct any started simulation tasks to exit
	for(map<unsigned int, int>::iterator iter = neuronGrpTaskMap.begin(); iter != neuronGrpTaskMap.end(); ++iter){
		#ifdef SIMULATION_CLEAN_UP_DEBUG
			cout<<"Sending exit message to task "<<iter->second<<endl;
		#endif//SIMULATION_CLEAN_UP_DEBUG

		if(!sendMessage(iter->second, EXIT_MSG)){
			cerr<<"SimulationManager: ERROR KILLING SIMULATION TASK "<<iter->second<<endl;
			cleanUpErrorMsg += "Error sending exit message to simulation task. TID=";
			cleanUpErrorMsg += QString::number(iter->second) += ".\n";
			cleanUpError = true;
		}
	}

	/* Wait for tasks to send task exited messages */
	vector<int> tempTaskVector = neuronGrpTaskVector;// Temporary copy of vector of taskIDs processing neuron grps
	int timeoutCount = 0;
	while(!tempTaskVector.empty()){// Need to wait for all tasks to send exit message
		int bufID = pvm_trecv(-1, -1, &taskTimeout);// Blocking receive waiting for message from any task
		if(bufID < 0){
			cleanUpError = true;	
			cleanUpErrorMsg = "RECEIVE ERROR WAITING FOR TASK_EXITED_MSG.\n";
			cerr<<"SimulationManager: RECEIVE ERROR WAITING FOR TASK_EXITED_MSG."<<endl;
			break;//Quit waiting for messages
		}
		else if (bufID == 0){
			++timeoutCount;//Receive has timed out
			if(timeoutCount > 30){//30 x 200000 = 6 second timeout
				cleanUpError = true;
				cleanUpErrorMsg += "TIMEOUT WHILST WAITING FOR TASK_EXITED_MSG.\n";
				cerr<<"SimulationManager: TIMEOUT WHILST WAITING FOR TASK_EXITED_MSG."<<endl;
				break;//Quit waiting
			}
		}
		else if (bufID > 0){
			timeoutCount = 0;//Reset timeout count
			int bytes, msgtag, msgTaskID;
			int info = pvm_bufinfo(bufID, &bytes, &msgtag, &msgTaskID);//Get info about message
			if(info < 0){
				pvm_perror("SimulationManager: PROBLEM GETTING BUFFER INFO");
				cleanUpError = true;
				cleanUpErrorMsg += "PVM ERROR GETTING BUFFER INFORMATION";
			}
			else{
				if(msgtag == TASK_EXITED_MSG){
					/* Work through vector and remove entry if it can be found.
						I have done it this way to avoid potential duplicate messages from the
						same task, which a simple count would overlook. */
					for(vector<int>::iterator iter = tempTaskVector.begin(); iter != tempTaskVector.end(); ++iter){
						if(*iter == msgTaskID){
							#ifdef SIMULATION_CLEAN_UP_DEBUG
								cout<<"Task "<<msgTaskID<<" clean up complete."<<endl;
							#endif//SIMULATION_CLEAN_UP_DEBUG
							tempTaskVector.erase(iter);
							break;
						}
					}
				}
				else if(msgtag == ERROR_MSG){
					cleanUpErrorMsgReceived(msgTaskID);
					break;
				}
				else if(msgtag == INFORMATION_MSG){
					showInformationMessage();
				}
				else{
					pvm_perror("SimulationManager: UNEXPECTED MESSAGE");
					cleanUpErrorMsg += "PVM ERROR: UNEXPECTED MESSAGE";
					cleanUpError = true;
					break;
				}
			}
		}
	}

	//Delete all temporary virtual connections from the database
	#ifdef SIMULATION_CLEAN_UP_DEBUG
		cout<<"Deleting temp virtual connections from database"<<endl;
	#endif//SIMULATION_CLEAN_UP_DEBUG

	deleteTempVirtualConnections();

	#ifdef SIMULATION_CLEAN_UP_DEBUG
		cout<<"Temporary virtual connections deleted"<<endl;
	#endif//SIMULATION_CLEAN_UP_DEBUG
	
	//Reset all taskIDs in Neuron Group Table to -1
	#ifdef SIMULATION_CLEAN_UP_DEBUG
		cout<<"Resetting task ids in database"<<endl;
	#endif//SIMULATION_CLEAN_UP_DEBUG

	try {
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"UPDATE NeuronGroups SET TaskID = -1";
		query.execute();
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"SimulationManager: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		cleanUpErrorMsg += "Bad query resetting task IDs.";
		cleanUpError = true;
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"SimulationManager: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		cleanUpErrorMsg += "Exception thrown resetting task IDs.";
		cleanUpError = true;
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"SimulationManager: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		cleanUpErrorMsg += "Exception thrown resetting task IDs";
		cleanUpError = true;
	}

	#ifdef SIMULATION_CLEAN_UP_DEBUG
		cout<<"Task ids in database reset"<<endl;
	#endif//SIMULATION_CLEAN_UP_DEBUG

	//Send message to archive task instructing it to exit
	if(archiveTaskID > 0){//Check archive task id is likely to be valid
		#ifdef SIMULATION_CLEAN_UP_DEBUG
			cout<<"Sending message to exit archive task "<<archiveTaskID<<endl;
		#endif//SIMULATION_CLEAN_UP_DEBUG

		if(!sendMessage(archiveTaskID, EXIT_MSG)){
			cerr<<"SimulationManager: ERROR KILLING ARCHIVE TASK "<<archiveTaskID<<endl;
			cleanUpErrorMsg += "Error sending exit message to archive task. TID=";
			cleanUpErrorMsg += QString::number(archiveTaskID) += ".\n";
			cleanUpError = true;
		}
		//Check that archive task is exiting ok
		#ifdef SIMULATION_CLEAN_UP_DEBUG
			cout<<"Checking that archive task has cleaned up using trecv"<<endl;
		#endif//SIMULATION_CLEAN_UP_DEBUG

		int bufferID = pvm_trecv(archiveTaskID, -1, &archiveTimeout);
		if(bufferID < 0){
			pvm_perror("SimulationManager: RECEIVE ERROR WHEN KILLING ARCHIVE TASK: ");
			cleanUpErrorMsg += "Error receiving confirmation of exit message from archive task. TID=";
			cleanUpErrorMsg += QString::number(archiveTaskID) += ".\n";
			cleanUpError = true;
		}
		else if (bufferID == 0){
			cerr<<"SimulationManager: TIMEOUT WHILE WAITING FOR ARCHIVER TO CLEAN UP."<<endl;
			cleanUpErrorMsg += "Time out while waiting for archiver to clean up. TID=";
			cleanUpErrorMsg += QString::number(archiveTaskID) += ".\n";
			cleanUpError = true;
		}
		else{
			int bytes, msgtag, msgTaskID;
			int info = pvm_bufinfo(bufferID, &bytes, &msgtag, &msgTaskID);//Get info about message
			if(info < 0){
				pvm_perror("SimulationManager: PROBLEM GETTING BUFFER INFO");
				cleanUpError = true;
				cleanUpErrorMsg += "PVM ERROR GETTING BUFFER INFORMATION";
			}
			else{
				if(msgtag == TASK_EXITED_MSG){
					#ifdef SIMULATION_CLEAN_UP_DEBUG
						cout<<"Archive task cleaned up successfully."<<endl;
					#endif//SIMULATION_CLEAN_UP_DEBUG
				}
				else if(msgtag == ERROR_MSG){
					cleanUpErrorMsg += "Error message from Archiver during clean up. TID=";
					cleanUpErrorMsg += QString::number(archiveTaskID) += ".\nArchiver should be cleaned up automatically.\n";
					cleanUpError = true;
				}
				else{
					cleanUpErrorMsg += "Unrecognized message from Archiver. TID=";
					cleanUpErrorMsg += QString::number(archiveTaskID) += ".\nArchiver should be cleaned up automatically.\n";
					cleanUpError = true;
				}
			}
		}
	}

	//Delete all information about tasks
	archiveTaskID = -1;
	neuronGrpTaskMap.clear();
	neuronGrpTaskVector.clear();

	//Exit from Pvm without shutting it down
	#ifdef SIMULATION_CLEAN_UP_DEBUG
		cout<<"Exiting from PVM"<<endl;
	#endif//SIMULATION_CLEAN_UP_DEBUG

	int info = pvm_exit();
	if(info < 0 ){
		pvm_perror("SimulationManager: ERROR EXITING FROM PVM");
		cleanUpErrorMsg += "Error exiting from PVM.\n";
		cleanUpError = true;
	}

	#ifdef SIMULATION_CLEAN_UP_DEBUG
		if(!cleanUpError) cout<<"Successfully exited from PVM"<<endl;
	#endif//SIMULATION_CLEAN_UP_DEBUG

	//Hide dialog indicating that clean up is in progress
	busyDialog->hide();

	#ifdef SIMULATION_CLEAN_UP_DEBUG
		cout<<"SimulationManager: Simulation cleaned up"<<endl;
	#endif//SIMULATION_CLEAN_UP_DEBUG
}


/*! Looks through connection group table and creates a virtual connection
	in the opposite direction to every connection that is not reciprocal.
	This makes sure that all of the simulation groups send and receive
	a message at each simulation time step and thus keep in step. */
bool SimulationManager::createTempVirtualConnections(){
	/* First delete any stray temporary virtual connections in case the previous
		simulation crashed */
	if (!deleteTempVirtualConnections())
		return false;

	//Query the database
	try{
		Query query = networkDBInterface->getQuery();
		query<<"SELECT FromNeuronGrpID, ToNeuronGrpID FROM ConnectionGroups";
                StoreQueryResult result = query.store();
		
		//Create a map to hold the connections
		vector< pair<unsigned int, unsigned int> > tempConnVector;
		
		//Load the connections into a vector
                for(StoreQueryResult::iterator iter = result.begin(); iter != result.end(); ++iter){
			Row row(*iter);
			unsigned int fromGrpID = Utilities::getUInt((std::string)row["FromNeuronGrpID"]);
			unsigned int toGrpID = Utilities::getUInt((std::string)row["ToNeuronGrpID"]);
			pair<unsigned int, unsigned int> tempPair;
			tempPair.first = fromGrpID;
			tempPair.second = toGrpID;
			tempConnVector.push_back(tempPair);
		}
	
		//Work through the 
		for(vector< pair<unsigned int, unsigned int> >::iterator iter1 = tempConnVector.begin(); iter1 != tempConnVector.end(); ++iter1){
			//Look for a connection in which the to group = the from group and vice versa
			bool reciprocalConnectionFound = false;
			for(vector< pair<unsigned int, unsigned int> >::iterator iter2 = tempConnVector.begin(); iter2 != tempConnVector.end(); ++iter2){
				if(iter1->first == iter2->second && iter2->first == iter1->second){
					reciprocalConnectionFound = true;
					break;
				}
			}
			if(!reciprocalConnectionFound){
				query.reset();
				query<<"INSERT INTO ConnectionGroups (FromNeuronGrpID, ToNeuronGrpID, ConnType) VALUES ("<<iter1->second<<", "<<iter1->first<<", "<<ConnectionType::TempVirtual<<")";
				try{
					query.execute();
				}
				catch (const Exception& er) {// Catch-all for MySQL++ exceptions
					cerr << "MySQL++ Error: " << er.what() << endl;
					return false;
				}
			}
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"SimulationManager: MYSQL QUERY EXCEPTION \""<<er.what()<<"\" creating temporary virtual connections."<<endl;
		return false;
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"SimulationManager: MYSQL EXCEPTION \""<<er.what()<<"\" creating temporary virtual connections."<<endl;
		return false;
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"SimulationManager: STD EXCEPTION \""<<er.what()<<"\" creating temporary virtual connections."<<endl;
		return false;
	}
	return true;
}


/*! Deletes all the temporary virtual connections from the connection table. */
bool SimulationManager::deleteTempVirtualConnections(){
	try{
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"DELETE FROM ConnectionGroups WHERE ConnType = "<<ConnectionType::TempVirtual;
		query.execute();
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"SimulationManager: MYSQL QUERY EXCEPTION \""<<er.what()<<"\" deleting temporary virtual connections."<<endl;
		return false;
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"SimulationManager: MYSQL EXCEPTION \""<<er.what()<<"\" deleting temporary virtual connections."<<endl;
		return false;
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"SimulationManager: STD EXCEPTION \""<<er.what()<<"\" deleting temporary virtual connections."<<endl;
		return false;
	}
	return true;
}


/*! Diplays and records error messages received during initialisation of the
	simulation. */
void SimulationManager::initErrorMsgReceived(int senderTID){
	//Get the length of the message
	unsigned int messageLength;
	int info = pvm_upkuint(&messageLength, 1, 1);
	if(info < 0){
		cerr<<"SimulationManager: ERROR EXTRACTING CHAR* LENGTH."<<endl;
		return;
	}

	//Unpack the message
	char charArray[messageLength];
 	info = pvm_upkstr(charArray);
	if(info < 0){
		cerr<<"SimulationManager: ERROR UNPACKING CHAR* MESSAGE."<<endl;
		return;
	}

	//Write error message to the console
	cerr<<"SimulationManager: INIT ERROR MESSAGE FROM "<<senderTID<<": "<<charArray<<endl;

	//Add to initErrorMsg string for display
	initErrorMsg += "ERROR MESSAGE FROM ";
	initErrorMsg += QString::number(senderTID) += ": ";
	initErrorMsg += charArray;
	initErrorMsg += "\n";
}


/*! Instructs tasks to load up their data. */
StartSimRes SimulationManager::loadNeuronData(map<unsigned int, unsigned int> patternInputMap, map<unsigned int, unsigned int> deviceInOutMap, map<unsigned int, double>deviceFiringModeMap, map<const char*, unsigned int> parameterMap, Q3ProgressDialog* progressDialog){
	//Initialise result of this operation
	StartSimRes startSimRes;
	startSimRes.canceled = false;
	startSimRes.started = false;

	/* Create a timeout to detect if neuronsimulation task does not load neuron data within 
		a specified time. */
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 200000;//Runs in a loop checking for messages every 0.2 seconds

	//Work through running tasks and send a message instructing them to load neuron data
	for(map<unsigned int, int>::iterator iter = neuronGrpTaskMap.begin(); iter != neuronGrpTaskMap.end(); ++iter){
		/* See if this neuron group receives an input or is connected to an output.
			Within a single simulation type some layers may be running with an input or 
			pattern and others with no input */
		if(patternInputMap.count(iter->first) > 0){//Determine what type of simulation it is and pack message appropriately
			int numberOfPatternParameters = 3;
			unsigned int parameterArray[numberOfPatternParameters];
			parameterArray[0] = PATTERN_SIMULATION;
			parameterArray[1] = patternInputMap[iter->first]; //Pattern ID
			parameterArray[2] = parameterMap["TimeStepsPerPattern"];
			bool messageSent = sendMessage(iter->second, LOAD_SIMULATION_DATA_MSG, parameterArray, numberOfPatternParameters);
			if(!messageSent)
				return startSimRes;
		}
		else if(deviceInOutMap.count(iter->first) > 0){
			int numberOfLiveParameters = 3;
			unsigned int parameterArray[numberOfLiveParameters];
			parameterArray[0] = LIVE_SIMULATION;
			parameterArray[1] = deviceInOutMap[iter->first];//Device ID
			/*Convert firing mode to unsigned int to save messing around with separate doubles etc.
				Need to reverse this procedure when unpacking the firing mode. */
			double tmpDevFiringMode = deviceFiringModeMap[iter->first];//Number between -1 and OUTPUT_FIRING_MODE with 0.1 resolution
			tmpDevFiringMode += 1.0;//Number between 0 and OUTPUT_FIRING_MODE with 0.1 resolution
			tmpDevFiringMode *= 10.0;//Number between 0 and OUTPUT_FIRING_MODE with 1 resolution
			parameterArray[2] = (unsigned int)rint(tmpDevFiringMode);
			bool messageSent = sendMessage(iter->second, LOAD_SIMULATION_DATA_MSG, parameterArray, numberOfLiveParameters);
			if(!messageSent)
				return startSimRes;
		}
		else{//Neuron group is not connected to anything so it is running in no input mode
			int numberOfNoInputParameters = 1;
			unsigned int parameterArray[numberOfNoInputParameters];
			parameterArray[0] = NO_INPUT_SIMULATION;
			bool messageSent = sendMessage(iter->second, LOAD_SIMULATION_DATA_MSG, parameterArray, numberOfNoInputParameters);
			if(!messageSent)
				return startSimRes;
		}

		//Process events and quit if cancelled
		spikeStrApp->processEvents();
		if(progressDialog->wasCancelled()){
			startSimRes.canceled = true;
			return startSimRes;
		}
	}
	
	/* Wait for tasks to send loading complete message */
	vector<int> tempTaskVector = neuronGrpTaskVector;// Temporary copy of vector of taskIDs processing neuron grps

	//Set up timer to check for messages every 0.2 seconds
	timeout.tv_sec = 0;
	timeout.tv_usec = 200000;

	//Count how many timeouts we have done - throw an error when timeoutCount / 5 > maxNeurDataLoadTime_sec
	int timeoutCount = 0;
	while(!tempTaskVector.empty()){// Need to wait for all tasks to load
		int bufID = pvm_trecv(-1, -1, &timeout);// Blocking receive waiting for message from any task
		if(bufID < 0){
			cerr<<"SimulationManager: RECEIVE ERROR: "<<bufID<<endl;
			return startSimRes;
		}
		else if (bufID == 0){
			//Process events and quit if cancelled
			spikeStrApp->processEvents();
			if(progressDialog->wasCancelled()){
				startSimRes.canceled = true;
				return startSimRes;
			}
			++timeoutCount;
			if(( timeoutCount / 5) > maxNeurDataLoadTime_sec){
				initErrorMsg += "TIMEOUT WHILST LOADING NEURON DATA.\n";
				cerr<<"SimulationManager: TIMEOUT WHILST WAITING FOR LOADING COMPLETE MESSAGES"<<endl;
				return startSimRes;
			}
		}
		else if (bufID > 0){
			timeoutCount = 0;//Reset timeout count
			int bytes, msgtag, msgTaskID;
			int info = pvm_bufinfo(bufID, &bytes, &msgtag, &msgTaskID);//Get info about message
			if(info < 0)
				pvm_perror("SimulationManager: PROBLEM GETTING BUFFER INFO");
			else{
				if(msgtag == SIMULATION_LOADING_COMPLETE_MSG){
					/* Work through vector and remove entry if it can be found.
						I have done it this way to avoid potential duplicate messages from the
						same task, which a simple count would overlook. */
					for(vector<int>::iterator iter = tempTaskVector.begin(); iter != tempTaskVector.end(); ++iter){
						if(*iter == msgTaskID){
							#ifdef SIMULATION_LOAD_DEBUG
								cout<<"Task "<<msgTaskID<<" loading complete"<<endl;
							#endif//SIMULATION_LOAD_DEBUG
							tempTaskVector.erase(iter);
							break;
						}
					}
				}
				else if(msgtag == ERROR_MSG){
					initErrorMsgReceived(msgTaskID);
					return startSimRes;
				}
				else if(msgtag == INFORMATION_MSG){
					showInformationMessage();
				}
				else{
					pvm_perror("SimulationManager: UNEXPECTED MESSAGE");
					return startSimRes;
				}
			}
		}
	}
	startSimRes.started = true;
	return startSimRes;
}


/*! Sends a message that does not contain data. */
bool SimulationManager::sendMessage(int taskID, int msgtag){
	int bufID = pvm_initsend(PvmDataDefault);
	if(bufID<0){
		cerr<<"SimulationManager: Init send error: "<<endl;
		return false;
	}
	int info = pvm_send(taskID, msgtag);
	if(info < 0){
		cerr<<"SimulationManager: MESSAGE SENDING ERROR TO "<<taskID<<" WITH MSGTAG "<<msgtag<<endl;
		return false;
	}
	return true;
}


/*! Sends a message containing a single integer. */
bool SimulationManager::sendMessage(int taskID, int msgtag, int msgData){
	//Initialise buffer
	int bufID = pvm_initsend(PvmDataDefault);
	if(bufID<0){
		cerr<<"SimulationManager: Init send error: "<<endl;
		return false;
	}

	//Pack unsigned int data
	int info = pvm_pkint(&msgData, 1, 1);
	if(info < 0){
		cerr<<"SimulationManager: ERROR PACKING UNSIGNED INT INTO MESSAGE"<<endl;
		return false;
	}

	//Send message
	info = pvm_send(taskID, msgtag);
	if(info < 0){
		cerr<<"SimulationManager: MESSAGE SENDING ERROR TO "<<taskID<<" WITH MSGTAG "<<msgtag<<endl;
		return false;
	}
	return true;
}


/*! Sends a message containing a single unsigned integer. */
bool SimulationManager::sendMessage_unsigned(int taskID, int msgtag, unsigned int msgData){
	//Initialise buffer
	int bufID = pvm_initsend(PvmDataDefault);
	if(bufID<0){
		cerr<<"SimulationManager: Init send error: "<<endl;
		return false;
	}

	//Pack unsigned int data
	int info = pvm_pkuint(&msgData, 1, 1);
	if(info < 0){
		cerr<<"SimulationManager: ERROR PACKING UNSIGNED INT INTO MESSAGE"<<endl;
		return false;
	}

	//Send message
	info = pvm_send(taskID, msgtag);
	if(info < 0){
		cerr<<"SimulationManager: MESSAGE SENDING ERROR TO "<<taskID<<" WITH MSGTAG "<<msgtag<<endl;
		return false;
	}
	return true;
}


/*! Sends a message containing two integers. */
bool SimulationManager::sendMessage(int taskID, int msgtag, unsigned int msgData1, unsigned int msgData2){
	//Initialise buffer
	int bufID = pvm_initsend(PvmDataDefault);
	if(bufID<0){
		cerr<<"SimulationManager: Init send error: "<<endl;
		return false;
	}

	//Pack unsigned int data
	int info = pvm_pkuint(&msgData1, 1, 1);
	if(info < 0){
		cerr<<"SimulationManager: ERROR PACKING UNSIGNED INT INTO MESSAGE"<<endl;
		return false;
	}
	info = pvm_pkuint(&msgData2, 1, 1);
	if(info < 0){
		cerr<<"SimulationManager: ERROR PACKING UNSIGNED INT INTO MESSAGE"<<endl;
		return false;
	}

	//Send message
	info = pvm_send(taskID, msgtag);
	if(info < 0){
		cerr<<"SimulationManager: MESSAGE SENDING ERROR TO "<<taskID<<" WITH MSGTAG "<<msgtag<<endl;
		return false;
	}
	return true;
}


/*! Sends a message with an array of unsigned integers. */
bool SimulationManager::sendMessage(int taskID, int msgtag, unsigned int *uIntArray, int arrayLength){
	//Initialise buffer
	int bufID = pvm_initsend(PvmDataDefault);
	if(bufID<0){
		pvm_perror("SimulationManager: INIT SEND ERROR ");
		return false;
	}

	//Pack length of the array
	int info = pvm_pkint(&arrayLength, 1, 1);
	if(info < 0){
		pvm_perror("SimulationManager: ERROR PACKING ARRAY LENGTH INTO MESSAGE");
		return false;
	}

	//Pack unsigned int array data
	info = pvm_pkuint(uIntArray, arrayLength, 1);
	if(info < 0){
		pvm_perror("SimulationManager: ERROR PACKING UNSIGNED INT ARRAY INTO MESSAGE");
		return false;
	}

	//Send message
	info = pvm_send(taskID, msgtag);
	if(info < 0){
		cerr<<"SimulationManager: MESSAGE SENDING ERROR TO "<<taskID<<" WITH MSGTAG "<<msgtag<<endl;
		return false;
	}
	return true;
}


/*! Sends a message with an array of doubles . */
bool SimulationManager::sendMessage(int taskID, int msgtag, double doubleArray [], int arrayLength){
	//Initialise buffer
	int bufID = pvm_initsend(PvmDataDefault);
	if(bufID<0){
		cerr<<"SimulationManager: Init send error: "<<endl;
		return false;
	}

	//Pack double data
	int info = pvm_pkdouble(doubleArray, arrayLength, 1);
	if(info < 0){
		cerr<<"SimulationManager: ERROR PACKING DOUBLES INTO MESSAGE"<<endl;
		return false;
	}

	//Send message
	info = pvm_send(taskID, msgtag);
	if(info < 0){
		cerr<<"SimulationManager: MESSAGE SENDING ERROR TO "<<taskID<<" WITH MSGTAG "<<msgtag<<endl;
		return false;
	}
	return true;
}


/*! Unpacks an error message, writes it to cerr and records that an error message
	has been received. Only the first error message is recorded to prevent a large
	number of duplicates. */
void SimulationManager::showErrorMessage(int senderTID){
	//Get the length of the message
	unsigned int messageLength;
	int info = pvm_upkuint(&messageLength, 1, 1);
	if(info < 0){
		cerr<<"SimulationManager: ERROR EXTRACTING CHAR* LENGTH"<<endl;
		return;
	}
	//Unpack the message
	char charArray[messageLength];
 	pvm_upkstr(charArray);

	//Write error message to the console
	cerr<<"ERROR MESSAGE FROM "<<senderTID<<". MESSAGE: "<<charArray<<endl;

	/* Only add to string if it is the first message. 
		Otherwise could get large number of errors arriving in between 
		checks when the first one is the most important */
	if(!simError){
		simulationErrorMsg += "ERROR MESSAGE FROM ";
		simulationErrorMsg += QString::number(senderTID) += ": ";
		simulationErrorMsg += charArray;
		simulationErrorMsg += "\n";
	}

	//Flag the error
	simError = true;
}


/*! Called from within this class when an error has been generated.
	Only the first error message is recorded to prevent a large
	number of duplicates. */
void SimulationManager::showErrorMessage(const char* msg){
	//Write error message to the console
	cerr<<msg<<endl;

	/* Add to error message for display if it is the first error message
		Otherwise a large number of error messages could become unmanageable */
	if(!simError){
		simulationErrorMsg += msg;
		simulationErrorMsg += "\n";
	}
	
	//Flag the error
	simError = true;
}


/*! Unpacks an information message and writes it to system out. */
void SimulationManager::showInformationMessage(){
	//Get the length of the message
	unsigned int messageLength;
	int info = pvm_upkuint(&messageLength, 1, 1);
	if(info < 0){
		cerr<<"SimulationManager: ERROR EXTRACTING CHAR* LENGTH"<<endl;
		return;
	}

	//Unpack the message
	char charArray[messageLength];
 	pvm_upkstr(charArray);

	//Write the message to the console
	cout<<charArray<<endl;
}


/*! Spawns the archive task with the information it needs. */
bool SimulationManager::spawnArchiveTask(const QString &archiveName, map<const char*, unsigned int> parameterMap){
	//Set up char array to hold command line arguments for tasks
	DBParameters neuralNetworkDBParameters = networkDBInterface->getDBParameters();
	char *commandLineArray[21];
	//Add parameters for nerual network database
	commandLineArray[0] = "-nnh";//neural network DB host
	commandLineArray[1] = neuralNetworkDBParameters.host;
	commandLineArray[2] = "-nnu";//neural network DB user
	commandLineArray[3] = neuralNetworkDBParameters.user;
	commandLineArray[4] = "-nnp";//neural network DB password
	commandLineArray[5] = neuralNetworkDBParameters.password;
	commandLineArray[6] = "-nnd";//neural network DB database
	commandLineArray[7] = neuralNetworkDBParameters.database;

	//Add parameters for archive database
	DBParameters archiveDBParameters = archiveDBInterface->getDBParameters();
	commandLineArray[8] = "-ah";//archive DB host
	commandLineArray[9] = archiveDBParameters.host;
	commandLineArray[10] = "-au";//archive DB user
	commandLineArray[11] = archiveDBParameters.user;
	commandLineArray[12] = "-ap";//archive DB password
	commandLineArray[13] = archiveDBParameters.password;
	commandLineArray[14] = "-ad";//archive DB database
	commandLineArray[15] = archiveDBParameters.database;
	
	//Add parameter to specify the archive name
	commandLineArray[16] = "-an";//archive name
	const char* archNameChar = archiveName.ascii(); 
	int archNameLength = strlen(archNameChar);
	commandLineArray[17] = new char[archNameLength + 1];//Allow space for terminating character which is not counted by strlen
	try{
		Utilities::safeCStringCopy(commandLineArray[17], archNameChar, archNameLength);
	}
	catch(std::exception& er){
		cerr<<"SimulationManager: Exception thrown copying archive name: \""<<er.what()<<"\""<<endl;
		return false;
	}

	//Determine whether to monitor spikes or firing neurons
	commandLineArray[18] = "-at";//Type of monitoring
        commandLineArray[19] = "";
        for(map<const char*, unsigned int>::iterator iter = parameterMap.begin(); iter != parameterMap.end(); ++iter){
            if(strcmp(iter->first, "ArchiveFiringNeurons")){
                commandLineArray[19] = "Neurons";
            }
            else if(strcmp(iter->first, "ArchiveSpikes")){
                commandLineArray[19] = "Spikes";
            }
        }

        if(!strcmp(commandLineArray[19], "ArchiveFiringNeurons") && !strcmp(commandLineArray[19], "ArchiveSpikes")){
		cerr<<"SimulationManager. CANNOT FIND ARCHIVE SPIKES/FIRING NEURONS PARAMETER"<<endl;
		return false;
	}

	//Finish off
	commandLineArray[20] = '\0';

	//Spawn task to handle archive
	int newTaskID;
	int numTasksLaunched = pvm_spawn("spikestreamarchiver", commandLineArray, 0, "", 1, &newTaskID);
		
	// Check that task has launched
	if (numTasksLaunched == 1) {
		//Store task ID
		archiveTaskID = newTaskID;
		
		//Wait for a confirmation message that task has launched
		#ifdef LAUNCH_TASKS_DEBUG
			cout<<"SimulationManager: Archive task "<<newTaskID<<" spawned, waiting for confirmation."<<endl;
		#endif//LAUNCH_TASKS_DEBUG

		//If archive task does not launch in 30 seconds, there is almost certainly an error
		struct timeval timeout;
		timeout.tv_sec = 60;
		timeout.tv_usec = 0;
		
		//Blocking receive - waiting for message from other task
		int bufID = pvm_trecv(newTaskID, -1, &timeout);
		if(bufID < 0){
			cerr<<"SimulationManager: RECEIVE ERROR STARTING ARCHIVE TASK: "<<bufID<<endl;
			return false;
		}
		else if(bufID == 0){
			cerr<<"SimulationManager: ATTEMPT TO START ARCHIVE TASK HAS TIMED OUT"<<endl;
			return false;
		}
		else{//Check that task has started ok or unpack error message.
			int bytes, msgtag, msgTaskID;
			int info = pvm_bufinfo(bufID, &bytes, &msgtag, &msgTaskID);//Get info about message
			if(info < 0){
				pvm_perror("SimulationManager: PROBLEM GETTING BUFFER INFO FOR ARCHIVE TASK");
				return false;
			}
			else{//Determine the type of message from the new task.
				if(msgtag == ARCHIVE_TASK_STARTED_MSG){
					/* Unpack the simulation start time from the message. 
						This is used to prevent the current archive from being deleted by the archive widget */
					unsigned int simulationStartTime = 0;
					int info = pvm_upkuint(&simulationStartTime, 1, 1);
					if(info < 0){
						cerr<<"SimulationManager: ERROR UNPACKING SIMULATION START TIME; TASK ID = "<<pvm_mytid()<<"; simulationStartTime =  "<<simulationStartTime<<endl;
						return false;
					}

					/* Inform other classes that the simulation start time has changed.
						This method should only be called within the main application thread so do not need to lock mutex.*/
					emit simulationStartTimeChanged(simulationStartTime);
			
					//If this point is reached, everything should be ok.
					#ifdef LAUNCH_TASKS_DEBUG
						cout<<"SimulationManager: Archive task "<<newTaskID<<" started."<<endl;
					#endif//LAUNCH_TASKS_DEBUG
				}
				else if(msgtag == ERROR_MSG){
					initErrorMsgReceived(msgTaskID);//Display error message
					#ifdef LAUNCH_TASKS_DEBUG
						cout<<"SimulationManager: Archive task "<<newTaskID<<" started with errors."<<endl;
					#endif//LAUNCH_TASKS_DEBUG
					return false;
				}
				else{//Unrecognized message
					cerr<<"SimulationManager: UNEXPECTED MESSAGE FROM ARCHIVE TASK "<<newTaskID<<". MESSAGE TYPE: "<<msgtag<<endl;
					return false;
				}
			}
		}
	}
	else{
		cerr<<"SimulationManager: CANNOT START ARCHIVE TASK"<<endl;
		return false;
	}

	//Clean up memory
	delete commandLineArray[17];

	//Everything should be ok if we have reached this point.
	return true;
}


/*! Spawn task to simulate neuron group.
	Pass database information to tasks in the form of arguments, rather 
	than managing separate config files.
	Each spawned task writes its task id to the database, which acts as a check 
	that they have all successfully established database communication. */
bool SimulationManager::spawnNeuronGroupTasks(){
	/* Create a timeout to detect if neuronsimulation task does not start within 
		a specified time. */
	struct timeval timeout;
	timeout.tv_sec = 30;//Should not take more than 30 seconds to spawn a task
	timeout.tv_usec = 0;

	//Set up char array to hold command line arguments for tasks
	DBParameters networkDBParameters = networkDBInterface->getDBParameters();
	DBParameters patternDBParameters = patternDBInterface->getDBParameters();
	DBParameters deviceDBParameters = deviceDBInterface->getDBParameters();
	char *commandLineArray[27];
	commandLineArray[0] = "-ng";
	commandLineArray[1] = new char[20];//Has to be long enough to hold an integer as a string.
	commandLineArray[2] = "-nnh";
        commandLineArray[3] = networkDBParameters.host;
	commandLineArray[4] = "-nnu";
	commandLineArray[5] = networkDBParameters.user;
	commandLineArray[6] = "-nnp";
	commandLineArray[7] = networkDBParameters.password;
	commandLineArray[8] = "-nnd";
	commandLineArray[9] = networkDBParameters.database;
	commandLineArray[10] = "-ph";
	commandLineArray[11] = patternDBParameters.host;
	commandLineArray[12] = "-pu";
	commandLineArray[13] = patternDBParameters.user;
	commandLineArray[14] = "-pp";
	commandLineArray[15] = patternDBParameters.password;
	commandLineArray[16] = "-pd";
	commandLineArray[17] = patternDBParameters.database;	
	commandLineArray[18] = "-dh";
	commandLineArray[19] = deviceDBParameters.host;
	commandLineArray[20] = "-du";
	commandLineArray[21] = deviceDBParameters.user;
	commandLineArray[22] = "-dp";
	commandLineArray[23] = deviceDBParameters.password;
	commandLineArray[24] = "-dd";
	commandLineArray[25] = deviceDBParameters.database;	
	commandLineArray[26] = '\0';


	//Work through the neuron groups, spawning a new process for each group.
	try{
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"SELECT NeuronGrpID FROM NeuronGroups";
                StoreQueryResult neuronGrpRes = query.store();
                for(StoreQueryResult::iterator neuronGrpIter = neuronGrpRes.begin(); neuronGrpIter != neuronGrpRes.end(); ++neuronGrpIter){
			int newTaskID;
			Row neuronGrpRow(*neuronGrpIter);
			string neuronGrpString = (std::string)neuronGrpRow["NeuronGrpID"];
			Utilities::safeCStringCopy(commandLineArray[1], neuronGrpString.c_str(), 20);
			
			//Spawn the task
                        int numTasksLaunched = pvm_spawn("spikestreamsimulator", commandLineArray, 0, "", 1, &newTaskID);
			
			// Check that task has launched
			if (numTasksLaunched == 1) {
				//Store neuronGrpIDs and task
				//These will need to be reset in the table at the end of the simulation
				unsigned int neuronGrpID = Utilities::getUInt(neuronGrpString);
				neuronGrpTaskMap[neuronGrpID] = newTaskID;
				neuronGrpTaskVector.push_back(newTaskID);
			
				//Wait for a confirmation message that task has launched
				#ifdef LAUNCH_TASKS_DEBUG
					cout<<"Task "<<newTaskID<<" spawned, waiting for confirmation."<<endl;
				#endif//LAUNCH_TASKS_DEBUG
			
				//Blocking receive with timeout waiting for message from other task
				//Better to check each task is up before running database check
				int bufID = pvm_trecv(newTaskID, -1, &timeout);
				if(bufID < 0){
					cerr<<"SimulationManager: RECEIVE ERROR WITH SIMULATION TASK: "<<bufID<<endl;
					return false;
				}
				else if(bufID == 0){
					cerr<<"SimulationManager: TIME OUT WHEN ATTEMPTING TO SPAWN SIMULATION TASK WITH ID: "<<neuronGrpID<<endl;
					return false;
				}
				else{
					int bytes, msgtag, msgTaskID;
					int info = pvm_bufinfo(bufID, &bytes, &msgtag, &msgTaskID);//Get info about message
					if(info < 0){
						pvm_perror("SimulationManager: PROBLEM GETTING BUFFER INFO FOR SIMULATION TASK");
						return false;
					}
					else{//Determine the type of message from the new task.
						if(msgtag == SIMULATION_TASK_STARTED_MSG){
							#ifdef LAUNCH_TASKS_DEBUG
								cout<<"Task "<<newTaskID<<" started for neuron group "<<neuronGrpID<<"."<<endl;
							#endif//LAUNCH_TASKS_DEBUG
						}
						else if(msgtag == ERROR_MSG){
							initErrorMsgReceived(msgTaskID);
							#ifdef LAUNCH_TASKS_DEBUG
								cout<<"Task "<<newTaskID<<" started for neuron group "<<neuronGrpID<<" with errors."<<endl;
							#endif//LAUNCH_TASKS_DEBUG
							return false;
						}
						else{//Unrecognized message
							cerr<<"SimulationManager: UNEXPECTED MESSAGE FROM SIMULATION TASK "<<newTaskID<<". MESSAGE TYPE: "<<msgtag<<endl;
							return false;
						}
					}
				}
			}
			else{
				cerr<<"SimulationManager.cpp: CANNOT START NEURON GROUP TASK"<<endl;
				return false;
			}
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"SimulationManager: MYSQL QUERY EXCEPTION \""<<er.what()<<"\" spawning neuron group tasks."<<endl;
		return false;
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"SimulationManager: MYSQL EXCEPTION \""<<er.what()<<"\" spawning neuron group tasks."<<endl;
		return false;
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		cerr<<"SimulationManager: STD EXCEPTION \""<<er.what()<<"\" spawning neuron group tasks."<<endl;
		return false;
	}

	//Delete char array created to hold neuron group
	delete commandLineArray[1];

	return true;
}


/*! Starts pvm daemon if it is not already running. */
bool SimulationManager::startPvm(){
	//First need to define PVM_ROOT environment variable if it is not alread defined
	char * pvmRoot = getenv("PVM_ROOT");
 	if (!pvmRoot){
		int putEnvRes = putenv("PVM_ROOT=/usr/lib/pvm3");
		if (putEnvRes){//Returns 0 if successful
 			cerr<<"SimulationManager.cpp: Failed to define environment variable PVM_ROOT"<<endl;
			return false;
		}
	}
	
	//Now start pvm daemon
	int info = pvm_start_pvmd(0, (char **)0, 1);
	if(info == PvmDupHost)
    	;//Do nothing, pvmd3 is already running. FIXME COULD CHECK THIS BEFORE TRYING TO START UP
	else if(info == PvmSysErr){
		cerr<<"Pvm system not responding"<<endl;
		return false;
	}

	//Store the task id of this task
	thisTaskID = pvm_mytid();
	if(thisTaskID < 0){
		pvm_perror("SimulationManager.cpp: TASK ID LESS THAN ZERO. STOPPING SIMULATION.");
		return false;
	}

	/* Set up direct routing. This increases the available bandwidth, 
		but is not scalable to more than 60 tasks */
	pvm_setopt( PvmRoute, PvmRouteDirect );
	return true;
}


/*! Unpacks monitoring data sent from a neuron class. */
void SimulationManager::unpackMonitorNeuronData(){
	unsigned int tempNeuronID, numberOfVariables;
	double msgTime;
	//Get the neuron ID
	int info = pvm_upkuint(&tempNeuronID, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SimulationManager: ERROR EXTRACTING NEURON ID; TASK ID = "<<pvm_mytid()<<"; tempNeuronID: "<<tempNeuronID<<endl;
			return;
		}
	#endif//PVM_DEBUG	

	//Get the time of the message
	info = pvm_upkdouble(&msgTime, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SimulationManager: ERROR EXTRACTING MESSAGE TIME; TASK ID = "<<pvm_mytid()<<"; msgTime: "<<msgTime<<endl;
			return;
		}
	#endif//PVM_DEBUG

	// Extract the number of variables in the message
	info = pvm_upkuint(&numberOfVariables, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SimulationManager: ERROR NUMBER OF VARIABLES FROM MESSAGE; TASK ID = "<<pvm_mytid()<<"; numberOfVariables: "<<numberOfVariables<<endl;
			return;
		}
	#endif//PVM_DEBUG

	//Unpack the list of variables 
	double tempVarArray[numberOfVariables];

	//Unpack array of doubles
	info = pvm_upkdouble(tempVarArray, numberOfVariables, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"NetworkMonitor: ERROR UNPACKING DATA ID FROM MESSAGE. NeuronID = "<<tempNeuronID<<endl;
			return;
		}
	#endif//PVM_DEBUG

	//Add the data to the appropriate neuron Monitor
	if(neuronMonitorMap->count(tempNeuronID))
		(*neuronMonitorMap)[tempNeuronID]->plotData(msgTime, tempVarArray, numberOfVariables);
	else{
		showErrorMessage("SimulationManager: Neuron ID not found in Neuron Monitor Map when unpacking data.");
		cerr<<"SimulationManager: Neuron ID not found in Neuron Monitor Map: "<<tempNeuronID<<endl;
	}
}


/*! Unpacks XML structured information about monitoring data sent from a neuron class. */
void SimulationManager::unpackMonitorNeuronInfo(){
	unsigned int tempNeuronID, charLength;

	#ifdef MONITOR_NEURON_DATA_DEBUG
		cout<<"SimulationManager: Monitor neuron info message received."<<endl;
	#endif//MONITOR_NEURON_DATA_DEBUG

	//Get the neuron ID
	int info = pvm_upkuint(&tempNeuronID, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SimulationManager: ERROR EXTRACTING NEURON ID; TASK ID = "<<pvm_mytid()<<"; tempNeuronID: "<<tempNeuronID<<endl;
			return;
		}
	#endif//PVM_DEBUG

	// Extract the length of the XML string
	info = pvm_upkuint(&charLength, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SimulationManager: ERROR EXTRACTING LENGTH OF XML STRING FROM MESSAGE; TASK ID = "<<pvm_mytid()<<"; charLength: "<<charLength<<endl;
			return;
		}
	#endif//PVM_DEBUG

	//Unpack the message
	char* charArray = new char[charLength];
 	pvm_upkstr(charArray);

	//Add the data to the appropriate neuron Monitor
	if(neuronMonitorMap->count(tempNeuronID))
		(*neuronMonitorMap)[tempNeuronID]->setUpGraphs(charArray);
	else{
		showErrorMessage("SimulationManager: Neuron ID not found in Neuron Monitor Map when unpacking info.");
		cerr<<"SimulationManager: Neuron ID not found in Neuron Monitor Map: "<<tempNeuronID<<endl;
	}
}


/*! Unpacks monitoring data sent from a synapse class. */
void SimulationManager::unpackMonitorSynapseData(){
	unsigned int tmpFromNeurID, tmpToNeurID, numberOfVariables;
	double msgTime;
	//Get the from neuron ID
	int info = pvm_upkuint(&tmpFromNeurID, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SimulationManager: ERROR EXTRACTING FROM NEURON ID; TASK ID = "<<pvm_mytid()<<"; tmpFromNeurID: "<<tmpFromNeurID<<endl;
			return;
		}
	#endif//PVM_DEBUG	

	//Get the to neuron id
	info = pvm_upkuint(&tmpToNeurID, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SimulationManager: ERROR EXTRACTING TO NEURON ID; TASK ID = "<<pvm_mytid()<<"; tmpToNeurID: "<<tmpToNeurID<<endl;
			return;
		}
	#endif//PVM_DEBUG	

	//Get the time of the message
	info = pvm_upkdouble(&msgTime, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SimulationManager: ERROR EXTRACTING MESSAGE TIME; TASK ID = "<<pvm_mytid()<<"; msgTime: "<<msgTime<<endl;
			return;
		}
	#endif//PVM_DEBUG

	// Extract the number of variables in the message
	info = pvm_upkuint(&numberOfVariables, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SimulationManager: ERROR NUMBER OF VARIABLES FROM MESSAGE; TASK ID = "<<pvm_mytid()<<"; numberOfVariables: "<<numberOfVariables<<endl;
			return;
		}
	#endif//PVM_DEBUG

	//Unpack the list of variables 
	double tempVarArray[numberOfVariables];

	//Unpack array of doubles
	info = pvm_upkdouble(tempVarArray, numberOfVariables, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"NetworkMonitor: ERROR UNPACKING DATA ID FROM MESSAGE. tmpFromNeurID = "<<tmpFromNeurID<<" tmpToNeurID = "<<tmpToNeurID<<endl;
			return;
		}
	#endif//PVM_DEBUG

	//Create a key to access the synapse monitor map
	unsigned int tmpSynapseKey[2];
	tmpSynapseKey[0] = tmpFromNeurID;
	tmpSynapseKey[1] = tmpToNeurID;

	//Add the data to the appropriate neuron Monitor
	if(synapseMonitorMap->count(tmpSynapseKey))
		(*synapseMonitorMap)[tmpSynapseKey]->plotData(msgTime, tempVarArray, numberOfVariables);
	else{
		showErrorMessage("SimulationManager: Synapse not found in Synapse Monitor Map when unpacking data.");
		cerr<<"SimulationManager: Synapse key not found in Synapse Monitor Map: "<<tmpSynapseKey[0]<<" "<<tmpSynapseKey[1]<<endl;
	}
}


/*! Unpacks XML structured information about monitoring data sent from a synapse class. */
void SimulationManager::unpackMonitorSynapseInfo(){
	unsigned int tmpFromNeurID, tmpToNeurID, charLength;

	//Get the from neuron ID
	int info = pvm_upkuint(&tmpFromNeurID, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SimulationManager: ERROR EXTRACTING FROM NEURON ID; TASK ID = "<<pvm_mytid()<<"; tmpFromNeuronID: "<<tmpFromNeurID<<endl;
			return;
		}
	#endif//PVM_DEBUG

	//Get the to neuron ID
	info = pvm_upkuint(&tmpToNeurID, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SimulationManager: ERROR EXTRACTING TO NEURON ID; TASK ID = "<<pvm_mytid()<<"; tmpToNeuronID: "<<tmpToNeurID<<endl;
			return;
		}
	#endif//PVM_DEBUG

	// Extract the length of the XML string
	info = pvm_upkuint(&charLength, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SimulationManager: ERROR EXTRACTING LENGTH OF XML STRING FROM MESSAGE; TASK ID = "<<pvm_mytid()<<"; charLength: "<<charLength<<endl;
			return;
		}
	#endif//PVM_DEBUG

	//Unpack the message
	char* charArray = new char[charLength];
 	pvm_upkstr(charArray);

	//Create a key to access the synapse monitor map
	unsigned int tmpSynapseKey[2];
	tmpSynapseKey[0] = tmpFromNeurID;
	tmpSynapseKey[1] = tmpToNeurID;

	//Add the data to the appropriate synapse Monitor
	if(synapseMonitorMap->count(tmpSynapseKey))
		(*synapseMonitorMap)[tmpSynapseKey]->setUpGraphs(charArray);
	else{
		showErrorMessage("SimulationManager: Synapse not found in Synapse Monitor Map when unpacking info.");
		cerr<<"SimulationManager: Synapse key not found in Synapse Monitor Map: "<<tmpSynapseKey[0]<<" "<<tmpSynapseKey[1]<<endl;
	}
}



/*! Records a message from a task to indicate that its view weights have been saved. */
void SimulationManager::updateViewWeightsSavedState(int taskID){
	viewWeightsSavedAcknowledgementMap.erase(taskID);
	if(viewWeightsSavedAcknowledgementMap.size() == 0)
		viewWeightsSaved = true;
}


/*! Records a message from a task to indicate that its weights have been loaded. */
void SimulationManager::updateWeightsLoadedState(int taskID){
	weightsLoadedAcknowledgementMap.erase(taskID);
	if(weightsLoadedAcknowledgementMap.size() == 0)
		synapseWeightsLoaded = true;
}


/*! Records a message from a task to indicate that its weights have been saved.*/
void SimulationManager::updateWeightsSavedState(int taskID){
	weightsSavedAcknowledgementMap.erase(taskID);
	if(weightsSavedAcknowledgementMap.size() == 0)
		weightsSaved = true;
}




