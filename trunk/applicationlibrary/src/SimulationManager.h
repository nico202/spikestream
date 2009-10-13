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

#ifndef SIMULATIONMANAGER_H
#define SIMULATIONMANAGER_H

//SpikeStream includes
#include "DBInterface.h"
#include "NetworkMonitor.h"
#include "SimulationTypes.h"
#include "BusyDialog.h"
#include "MonitorDataPlotter.h"
#include "GlobalVariables.h"

//Qt includes
#include <qthread.h>
#include <qwidget.h>
#include <q3progressdialog.h>
#include <qstring.h>


/*! Struct holding the outcome of an attempt to initialise the simulation. 
	The simulation may have started or the initialisation may have been 
	cancelled by the user. */
struct StartSimRes {
	bool started;
	bool canceled;
};


//------------------------ Simulation Manager ------------------------------
/*! Initiates and controls the simulation using PVM. */
//--------------------------------------------------------------------------
/* FIXME WOULD BE GREATLY IMPROVED IF THE INITIALISATION TASKS WERE CARRIED
	OUT BY A SEPARATE THREAD TO ENABLE THEM TO BE CLEANLY CANCELLED. */

class SimulationManager : public QThread {
	Q_OBJECT

	public: 
		SimulationManager(DBInterface *networkDBInterface, DBInterface *archiveDBInterface, DBInterface *patternDBInterface, DBInterface *deviceDBInterface, QWidget *simulationWidget);
		~SimulationManager();
		void clearSimulationError();
		bool destroySimulation();
		void fireNeuron(unsigned int neuronGrpID, unsigned int neuronID);
		bool getCleanUpError();
		QString getCleanUpErrorMsg();
		QString getInitErrorMsg();
		QString getSimulationErrorMsg();
		bool getViewWeightsSaved();
		bool getWeightsLoaded();
		bool getWeightsSaved();
		StartSimRes initialiseSimulation(QString archiveName, map<unsigned int, unsigned int> patternInputMap, map<unsigned int, unsigned int> deviceInOutMap, map<unsigned int, double>deviceFiringModeMap, map<const char*, unsigned int> paramMap);
		void injectNoise(unsigned int neuronGrpID, int amount);
		static bool isInitialised();
		bool isRunning();
		bool loadWeights();
		void monitorNeuronGroup(unsigned int neuronGrpID, bool monitorType);
		bool saveViewWeights();
		bool saveWeights();
		void setFrameRate(int fr);
		bool setGlobalParameters();
		void setGraphicsLoading(bool gl);
		void setMaxNeurDataLoadTime_sec(int mNeurData);
		void setNetworkMonitors(map<unsigned int, NetworkMonitor*> nwMonitorMap);
		void setNeuronMonitors(map<unsigned int, MonitorDataPlotter*> *neurMonMap);
		bool setNeuronParameters();
		void setSynapseMonitors(map<unsigned int*, MonitorDataPlotter*, synapseKeyCompare> *synMonMap);
		bool setNoiseParameters();
		bool setSynapseParameters();
		void setUpdateMode(bool, bool);
		bool simulationError();
		bool startNeuronMonitoring(unsigned int neuronGrpID, unsigned int neuronID, bool restart);
		bool startRecording();
		bool startSimulation();
		bool startSynapseMonitoring(unsigned int neuronGrpID, unsigned int fromNeuronID, unsigned int toNeuronID, bool restart);
		bool stepSimulation();
		void stopMonitoringNeuronGroup(unsigned int neuronGrpID, bool monitorType);
		bool stopNeuronMonitoring(unsigned int neuronGrpID, unsigned int neuronID);
		bool stopRecording();
		bool stopSimulation();
		bool stopSynapseMonitoring(unsigned int neurGrpID, unsigned int fromNeurID, unsigned int toNeurID);


	signals:
		void simulationStartTimeChanged(unsigned int newSimStartTime);


	protected:
		//Method inherited from QThread
		void run();


	private:
		//===================== VARIABLES ===============================
		/*! The task id of the SpikeStream Application.*/
		int thisTaskID;

		//References to classes for database handling
		DBInterface *networkDBInterface;
		DBInterface *archiveDBInterface;
		DBInterface *patternDBInterface;
		DBInterface *deviceDBInterface;

		/*! Reference to SimulationWidget stored as QWidget to avoid include problems.*/
		QWidget *simulationWidget; 

		/*! Short version of the reference to the main QApplication.*/
		SpikeStreamApplication *spikeStrApp;

		/*! Task id of archive.*/
		int archiveTaskID;
		
		/*! Records which task is processing which neuron group
			The key is the neuron group ID, the data is the task ID.*/
		map<unsigned int, int> neuronGrpTaskMap;
		
		/*! Links tasks to network monitors that are running in this process
			The key is the taskID.*/
		map<int, NetworkMonitor*> networkMonitorMap;
		
		/*! Complete list of all the tasks that are processing neuron groups.*/
		vector<int> neuronGrpTaskVector;

		/*! Used to control run loop. Made static so that isInitialised() can be static.*/
		static bool stop;

		/*! Records whether an error was generated during the cleaning up of a simulation.*/
		bool cleanUpError;

		/*! Error messages generated during simulation clean up.*/
		QString cleanUpErrorMsg;

		/*! Error messages generated during simulation initialisation.*/
		QString initErrorMsg;

		/*! Records whether there have been errors during the running of the simulation.*/
		bool simError;

		/*! Stores error messages generated during a simulation run.*/
		QString simulationErrorMsg;

		/*! Records whether the simulation manager is in its run loop.*/
		bool simulationRunning;

		/*! Used to keep track of whether weights have been saved or not.*/
		bool weightsSaved;

		/*! Used to record confirmtion messages about weights being saved.*/
		map<int, bool> weightsSavedAcknowledgementMap;

		/*! Used to keep track of whether weights have been saved to TempWeight 
			for viewing. */
		bool viewWeightsSaved;

		/*! Used to record confirmtion messages about view weights being saved.*/
		map<int, bool> viewWeightsSavedAcknowledgementMap;

		/*! Used to keep track of whether weights have been reloaded or not.*/
		bool synapseWeightsLoaded;

		/*! Used to record confirmtion messages about weights being reloaded.*/
		map<int, bool> weightsLoadedAcknowledgementMap;

		/*! Dialog shown to indicate a process taking place whose duration is not known.*/
		BusyDialog *busyDialog;

		/*! Records when the graphics is loading to avoid sending stray messages.*/
		//FIXME NOT SURE IF THIS IS WORKING
		bool graphicsLoading;

		/*! Pointer to the map holding the neuron monitors.*/
		map<unsigned int, MonitorDataPlotter*> *neuronMonitorMap;

		/*! Pointer to the map holding the synapse monitors.*/
		map<unsigned int*, MonitorDataPlotter*, synapseKeyCompare> *synapseMonitorMap;

		/*! The amount of time the simulation manager will wait for tasks to load their neuron data
			This variable is set in the config file. */
		int maxNeurDataLoadTime_sec;


		//========================= METHODS ==============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		SimulationManager (const SimulationManager&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		SimulationManager operator = (const SimulationManager&);

		bool checkNeuronGroupTasks();
		void cleanUpErrorMsgReceived(int senderTID);
		void cleanUpSimulation();
		bool createTempVirtualConnections();
		bool deleteTempVirtualConnections();
		void initErrorMsgReceived(int senderTID);
		StartSimRes loadNeuronData(map<unsigned int, unsigned int> patternInputMap, map<unsigned int, unsigned int> deviceInOutMap, map<unsigned int, double>deviceFiringModeMap, map<const char*, unsigned int> paramMap, Q3ProgressDialog* progressDialog);
		bool sendMessage(int taskID, int msgtag);
		bool sendMessage(int taskID, int msgtag, unsigned int*, int arrayLength);
		bool sendMessage(int taskID, int msgtag, int msgData);
		bool sendMessage_unsigned(int taskID, int msgtag, unsigned int msgData);
		bool sendMessage(int taskID, int msgtag, unsigned int msgData1, unsigned int msgData2);
		bool sendMessage(int taskId, int msgtag, double [], int arrayLength);
		void showErrorMessage(int senderTID);
		void showErrorMessage(const char* msg);
		void showInformationMessage();
		bool spawnArchiveTask(const QString& archiveName, map<const char*, unsigned int>);
		bool spawnNeuronGroupTasks();
		bool startPvm();
		void unpackMonitorNeuronData();
		void unpackMonitorNeuronInfo();
		void unpackMonitorSynapseData();
		void unpackMonitorSynapseInfo();
		void updateViewWeightsSavedState(int taskID);
		void updateWeightsLoadedState(int taskID);
		void updateWeightsSavedState(int taskID);

};


#endif//SIMULATIONMANAGER_H


