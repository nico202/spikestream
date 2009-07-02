/***************************************************************************
 *   SpikeStream Archiver                                                  *
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
#include "Debug.h"
#include "PVMMessages.h"
#include "SpikeStreamArchiver.h"
#include "GlobalVariables.h"
#include "Utilities.h"

//Other includes
#include <ctime>
#include <stdlib.h>
#include <pvm3.h>
#include <iostream>
using namespace std;
using namespace mysqlpp;


//Declare static variables
int SpikeStreamArchiver::parentTaskID = 0;
bool SpikeStreamArchiver::errorState = false;


/*! Constructor */
SpikeStreamArchiver::SpikeStreamArchiver(int argc, char **argv){
	//Get task id and parent task id
	myTaskID = pvm_mytid();
	parentTaskID = pvm_parent();

	//Initialise variables
	networkDataStored = false;

	//Set up direct routing
	pvm_setopt( PvmRoute, PvmRouteDirect );

	//Initialize array for receiving spikes
	unpackArray = new unsigned int[MAX_NUMBER_OF_SPIKES];

	/* Read arguments from array. 
		These include host, username and password for database and NeuronGrpID
		Use options: -h [hostname] -d [database] -u [username]  -p [password] 
		with spaces in between to avoid strict ordering. argv[0] is the executable name, so start
		from 1. */
	char *neuralNetworkHost, *neuralNetworkUser, *neuralNetworkPassword, *neuralNetworkDatabase;
	char *archiveHost, *archiveUser, *archivePassword, *archiveDatabase;

	for(int i=1; i<argc; i += 2){
		if(argv[i][0] == '-'){//This line contains an option
			if(Utilities::cStringEquals(argv[i], "-nnh", 4)){//Extract neural network host information
				neuralNetworkHost = argv[i+1];
			}
			else if(Utilities::cStringEquals(argv[i], "-nnu", 4)) {//Extract nerual network user information
				neuralNetworkUser = argv[i+1];
			}
			else if(Utilities::cStringEquals(argv[i], "-nnp", 4)){// Extract password information
				neuralNetworkPassword = argv[i+1];
			}
			else if(Utilities::cStringEquals(argv[i], "-nnd", 4)){// Extract database information
				neuralNetworkDatabase = argv[i+1];
			}
			else if(Utilities::cStringEquals(argv[i], "-ah", 4)){//Extract neural network host information
				archiveHost = argv[i+1];
			}
			else if(Utilities::cStringEquals(argv[i], "-au", 4)) {//Extract nerual network user information
				archiveUser = argv[i+1];
			}
			else if(Utilities::cStringEquals(argv[i], "-ap", 4)){// Extract password information
				archivePassword = argv[i+1];
			}
			else if(Utilities::cStringEquals(argv[i], "-ad", 4)){// Extract database information
				archiveDatabase = argv[i+1];
			}
			else if(Utilities::cStringEquals(argv[i], "-an", 4)){// Extract archive name
				// Copy archive name
				Utilities::safeCStringCopy(archiveName, argv[i+1], MAX_DATABASE_NAME_LENGTH);
			}
			else if(Utilities::cStringEquals(argv[i], "-at", 4)){
				if(Utilities::cStringEquals(argv[i+1], "Neurons", 30))
					archiveType = FIRING_NEURON_ARCHIVE;
				else if(Utilities::cStringEquals(argv[i+1], "Spikes", 20))
					archiveType = SPIKE_ARCHIVE;
				else{
					cerr<<"SpikeStreamArchiver: ARCHIVE TYPE NOT RECOGNIZED: \""<<argv[i+1]<<"\". SETTING TO ARCHIVE FIRING NEURONS"<<endl;
					archiveType = FIRING_NEURON_ARCHIVE;
				}
			}
		}
		else{
			cerr<<"SpikeStreamArchiver: Invalid command line arguments!"<<endl;
			return;
		}
	}

	//Output database parameters to check them
	#ifdef COMMAND_LINE_PARAMETERS_DEBUG
		cout<<"Neural network database parameters: "<<neuralNetworkHost<<", "<<neuralNetworkUser<<", "<<neuralNetworkPassword<<", "<<neuralNetworkDatabase<<endl;
		cout<<"Archive database parameters: "<<archiveHost<<", "<<archiveUser<<", "<<archivePassword<<", "<<archiveDatabase<<endl;
	#endif//COMMAND_LINE_PARAMETERS_DEBUG

	//Create a new neural network database interface
	neuralNetworkDBInterface = new DBInterface(neuralNetworkHost, neuralNetworkUser, neuralNetworkPassword, neuralNetworkDatabase);
	if(!neuralNetworkDBInterface->connectToDatabase(true)){//Use exceptions
		systemError("FAILED TO CONNECT TO THE NEURAL NETWORK DATABASE");//This sets up the error state, so will only respond to exit message now
	}

	//Create a new archive database interface
	archiveDBInterface = new DBInterface(archiveHost, archiveUser, archivePassword, archiveDatabase);
	if(!archiveDBInterface->connectToDatabase(true)){//Use exceptions
		systemError("FAILED TO CONNECT TO NEURAL ARCHIVE DATABASE");//This sets up the error state, so will only respond to exit message now
	}

	/* Load up a list connecting neuron group ids to task ids. The neuron archiver
		should be started after all the simulation tasks have started so all the task
		IDs should be in the database */
	loadTaskIDs();

	/* Store an XML file recording the essential details of the neural network. 
		This is to enable the archive to be played back even after the network has changed */
	archiveNeuralNetwork();

	/*Send confirmation message to parent that archive has started. */
	sendMessage(parentTaskID, ARCHIVE_TASK_STARTED_MSG, simulationStartTime);

	//Give user feedback about initialization of task
	cout<<"Archiver task successfully started with taskID "<<myTaskID<<endl;

	//Start main run method of task
	stop = false;

	run();
}


/*! Destructor */
SpikeStreamArchiver::~SpikeStreamArchiver(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING NEURON ARCHIVER. TASK ID: "<<myTaskID<<endl;
	#endif//MEMORY_DEBUG

	//Delete any classes allocated from the heap
	delete archiveDBInterface;
	delete neuralNetworkDBInterface;

	//Delete unpack array
	delete [] unpackArray;
}


/*! Main run method */
void SpikeStreamArchiver::run(){
	while(!stop){
		//Blocking receive - waiting for message from other tasks
		int bufID = pvm_recv(-1, -1);
		if(bufID < 0){
			pvm_perror("SpikeStreamArchiver: MESSAGE RECEIVE ERROR");
			systemError("SpikeStreamArchiver: MESSAGE RECEIVE ERROR");
		}
		else if (bufID > 0){
			int bytes, msgtag, senderTID;
			int info = pvm_bufinfo(bufID, &bytes, &msgtag, &senderTID);//Get info about message
			if(info < 0){
				pvm_perror("SpikeStreamArchiver: PROBLEM GETTING BUFFER INFO");
				systemError("SpikeStreamArchiver: PROBLEM GETTING BUFFER INFO");
			}
			else if(errorState){//When there has been an error, task only responds to an exit msg
				if(msgtag == EXIT_MSG){
					#ifdef MESSAGE_DEBUG
						cout<<"Task "<<myTaskID<<": EXIT_MSG "<<bytes<<" bytes received from "<<senderTID<<endl;
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
							cout<<"Task "<<myTaskID<<": SPIKE_LIST_MSG "<<bytes<<" bytes received from "<<senderTID<<endl;
						#endif//MESSAGE_DEBUG
						processSpikeList(senderTID);
					break;
					case (FIRING_NEURON_LIST_MSG):
						#ifdef MESSAGE_DEBUG
							cout<<"Task "<<myTaskID<<": FIRING_NEURON_LIST_MSG "<<bytes<<" bytes received from "<<senderTID<<endl;
						#endif//MESSAGE_DEBUG
						processFiringNeuronList(senderTID);
					break;
					case (START_ARCHIVING_MSG):
						#ifdef MESSAGE_DEBUG
							cout<<"Task "<<myTaskID<<": START_ARCHIVING_MSG "<<bytes<<" bytes received from "<<senderTID<<endl;
						#endif//MESSAGE_DEBUG
						startArchiving();
					break;
					case (STOP_ARCHIVING_MSG):
						#ifdef MESSAGE_DEBUG
							cout<<"Task "<<myTaskID<<": STOP_ARCHIVING_MSG "<<bytes<<" bytes received from "<<senderTID<<endl;
						#endif//MESSAGE_DEBUG
						stopArchiving();
					break;
					case (EXIT_MSG):
						#ifdef MESSAGE_DEBUG
							cout<<"Task "<<myTaskID<<": EXIT_MSG "<<bytes<<" bytes received from "<<senderTID<<endl;
						#endif//MESSAGE_DEBUG
						stop = true;//Exits the archiver from the run loop
					break;
					default:
						cout<<"Task "<<myTaskID<<"; UNRECOGNIZED MESSAGE "<<bytes<<" bytes received from "<<senderTID<<endl;
				}
			}
		}
	}

	//Do any remaining tasks prior to exiting
	cleanUpArchiver();
}


//---------------------------------------------------------------------------------
//-------------------------- PUBLIC STATIC METHODS --------------------------------
//---------------------------------------------------------------------------------

/*! Writes an error message to the standard output and sends a message to the parent task 
	with the error message. Also puts archiver into error state, in which it only responds
	to exit message so that SimulationManager can shut simulation down cleanly.*/
void SpikeStreamArchiver::systemError(const char *message){
	errorState = true;//Put the program into error state when it stops operating and waits for exit message.
	cerr<<"SpikeStreamArchiver: "<<message<<endl;
	sendMessage(parentTaskID, ERROR_MSG, message);
}


/*! Writes an error message to the standard output and sends a message to the parent task 
	with the error message. Also puts archiver into error state, in which it only responds
	to exit message so that SimulationManager can shut simulation down cleanly.*/
void SpikeStreamArchiver::systemError(const char *message, int messageData1){
	errorState = true;//Put the program into error state when it stops operating and waits for exit message.
	ostringstream tempStr;
	tempStr<<message<<messageData1;
	cerr<<"SpikeStreamArchiver: "<<message<<messageData1<<endl;
	sendMessage(parentTaskID, ERROR_MSG, tempStr.str().data());
}


/*! Writes an error message to the standard output and sends a message to the parent task 
	with the error message. Also puts archiver into error state, in which it only responds
	to exit message so that SimulationManager can shut simulation down cleanly.*/
void SpikeStreamArchiver::systemError(const string &message){
	errorState = true;//Put the program into error state when it stops operating and waits for exit message.
	cerr<<"SpikeStreamArchiver: "<<message<<endl;
	sendMessage(parentTaskID, ERROR_MSG, message.data());
}


//---------------------------------------------------------------------------------
//------------------------------ PRIVATE METHODS ----------------------------------
//---------------------------------------------------------------------------------

/*! Store an XML file recording the essential details of the neural network. 
	This is to enable the archive to be played back even after the network has changed 
	For the moment just store the neuron groups */
bool SpikeStreamArchiver::archiveNeuralNetwork(){
	//Get the time in seconds at which this is being archived
	simulationStartTime = time(NULL);

	string xmlString = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
	xmlString += "<neural_network>";

	try{
		Query networkQuery = neuralNetworkDBInterface->getQuery();
		networkQuery.reset();
		networkQuery<<"SELECT * FROM NeuronGroups";
                StoreQueryResult result = networkQuery.store();
                for(StoreQueryResult::iterator iter = result.begin(); iter != result.end(); ++iter){
			Row row(*iter);
	
			//First get start neuron group ID
			unsigned int neuronGrpID = Utilities::getUInt(row["NeuronGrpID"]);
			networkQuery.reset();
			networkQuery<<"SELECT MIN(NeuronID) FROM Neurons WHERE NeuronGrpID = "<<neuronGrpID;
                        StoreQueryResult startNeuronRes = networkQuery.store();
			Row startNeuronRow(*startNeuronRes.begin());//Should only be one row
	
			xmlString += "<neuron_group id=\"";
			xmlString +=(std::string)row["NeuronGrpID"];
			xmlString += "\">";
	
			xmlString += "<name>";
			xmlString += (std::string)row["Name"];
			xmlString += "</name>";
	
			xmlString += "<start_neuron_id>";
			xmlString += (std::string)startNeuronRow["MIN(NeuronID)"];
			xmlString += "</start_neuron_id>";
	
			xmlString += "<width>";
			xmlString += (std::string)row["Width"];
			xmlString += "</width>";
	
			xmlString += "<length>";
			xmlString += (std::string)row["Length"];
			xmlString += "</length>";
	
			xmlString += "<location>";
			xmlString += (std::string)row["X"];
			xmlString += ",";
			xmlString += (std::string)row["Y"];
			xmlString += ",";
			xmlString += (std::string)row["Z"];
			xmlString += "</location>";
		
			xmlString += "<spacing>";
			xmlString += (std::string)row["Spacing"];
			xmlString += "</spacing>";
	
			xmlString += "<neuron_type>";
			xmlString += (std::string)row["NeuronType"];
			xmlString += "</neuron_type>";
	
			xmlString += "</neuron_group>";
		}
	
		//Finish off XML string
		xmlString += "</neural_network>";

		//Write XML model to database	
		Query archiveQuery = archiveDBInterface->getQuery();
		archiveQuery.reset();
		archiveQuery<<"INSERT INTO NetworkModels(StartTime, ArchiveType, Name, NeuronGroups) VALUES ( "<<simulationStartTime<<", "<<archiveType<<", \""<<archiveName<<"\", "<<mysqlpp::quote<<xmlString<<" )";
		archiveQuery.exec();
	}
	catch (const BadQuery& er) {// Handle any query errors
		ostringstream errorStrStream;
		errorStrStream<<"Bad query when archiving neural network: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
		return false;
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown archiving neural network: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
		return false;
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown archiving neural network: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
		return false;
	}

	//Everything ok if we have reached this point
	return true;
}


/*! Called just before exiting run method to do any final tasks, such as storing spike lists
	or deleting network model if no spike lists have been stored. */
void SpikeStreamArchiver::cleanUpArchiver(){
	cout<<"Cleaning up archiver"<<endl;
	//Archive any half completed timeStepHolders
	for(map<unsigned int, TimeStepHolder>::iterator iter = timeStepMap.begin(); iter != timeStepMap.end(); ++iter){
		//Finish off xml string
		iter->second.xmlString += "</network_pattern>";

		/* Store string in database. */
		storeNetworkData(iter->first, iter->second.xmlString);
	}

	//Delete network model if no data has been stored for it
	if(!networkDataStored){
		try{
			Query archiveQuery = archiveDBInterface->getQuery();
			archiveQuery.reset();
			archiveQuery<<"DELETE FROM NetworkModels WHERE StartTime = "<<simulationStartTime;
			archiveQuery.execute();
		}
		catch (const BadQuery& er) {// Handle any query errors
			ostringstream errorStrStream;
			errorStrStream<<"Bad query when deleting empty network models: \""<<er.what()<<"\"";
			systemError(errorStrStream.str());
		}
		catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
			ostringstream errorStrStream;
			errorStrStream<<"Exception thrown deleting empty network models: \""<<er.what()<<"\"";
			systemError(errorStrStream.str());
		}
		catch(std::exception& er){// Catch-all for any other exceptions
			ostringstream errorStrStream;
			errorStrStream<<"Exception thrown deleting empty network models: \""<<er.what()<<"\"";
			systemError(errorStrStream.str());
		}
	}

	//Inform main application that clean up is complete
	sendMessage(parentTaskID, TASK_EXITED_MSG);
	cout<<"SpikeStreamArchiver: Finished storing data, now exiting pvm and invoking destructor"<<endl;

	//Exit from pvm, destructor should be automatically invoked
	pvm_exit();
}


/*! Converts an unsigned integer to a string. */
string SpikeStreamArchiver::getString(unsigned int uInt){
	stringstream tempStringStr;
	tempStringStr<<uInt;
	return tempStringStr.str();
}


/*! Fills the map connecting NeuronGrpIDs and TaskIDs. */
bool SpikeStreamArchiver::loadTaskIDs(){
	spikeMessageTotal = 0;
	try{
		Query networkQuery = neuralNetworkDBInterface->getQuery();
		networkQuery.reset();
		networkQuery<<"SELECT NeuronGrpID, TaskID FROM NeuronGroups";
                StoreQueryResult taskResult = networkQuery.store();
                for(StoreQueryResult::iterator iter = taskResult.begin(); iter != taskResult.end(); ++iter){
			Row taskRow(*iter);
			int tempTaskID = Utilities::getInt(taskRow["TaskID"]);
			unsigned int tempNeuronGrpID = Utilities::getUInt(taskRow["NeuronGrpID"]);
			
			// Add entry storing the link between taskID and neuronGrpID
			taskToNeuronGrpMap[tempTaskID] = tempNeuronGrpID;
	
			// Add taskID to spike message total to track whether all spike messages have arrived
			spikeMessageTotal += tempTaskID;
		}
	
		/* Work through the neuron groups and load up the start neuron ID for each task. 
			This is used to decompress the messages. */
		for(map<int, unsigned int>::iterator iter = taskToNeuronGrpMap.begin(); iter != taskToNeuronGrpMap.end(); ++iter){
			networkQuery.reset();
			networkQuery<<"SELECT MIN(NeuronID) FROM Neurons WHERE NeuronGrpID = "<<iter->second;
                        StoreQueryResult minNeurIDRes = networkQuery.store();
			Row minNeurIDRow(*minNeurIDRes.begin());//Should be only one row
			startNeurIDTaskMap[iter->first] = Utilities::getUInt((std::string)minNeurIDRow["MIN(NeuronID)"]);
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		ostringstream errorStrStream;
		errorStrStream<<"Bad query when loading task IDs: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
		return false;
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown loading task IDs: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
		return false;
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown loading task IDs: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
		return false;
	}

	//If we have reached this point everything should be ok
	return true;
}


/*! Processes a list of firing neurons from the task simulating a neuron group. 
	Need to receive all the neuron groups, which is checked using task ids.
	For each neuron group add the firing neurons to the archive. */
void SpikeStreamArchiver::processFiringNeuronList(int senderTID){
	//Create a string stream for writing neuron ids to
	stringstream firingNeuronStringStr;

	//Get the time step of the message
	int info = pvm_upkuint(&messageTimeStep, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamArchiver: ERROR EXTRACTING MESSAGE TIME STEP; TASK ID = "<<pvm_mytid()<<"; messageTimeStep: "<<messageTimeStep<<endl;
			return;
		}
	#endif//PVM_DEBUG

	//See if we already have a timeStepHolder for this time step
	if(timeStepMap.find(messageTimeStep) != timeStepMap.end()){
		//Have found one so increase spike message count for map
		timeStepMap[messageTimeStep].spikeMessageCount += senderTID;
	}
	/* Do not have a timeStepHolder for this map, so create one and 
		initialise spikeMessageCount and xmlString */
	else{
		//Initialise spikeMessageCount
		timeStepMap[messageTimeStep].spikeMessageCount = senderTID;

		//Start creating the XML file
		timeStepMap[messageTimeStep].xmlString = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
		timeStepMap[messageTimeStep].xmlString += "<network_pattern>";
	}

	// Extract the number of neurons in the message
	info = pvm_upkuint(&numberOfSpikes, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamArchiver: ERROR NUMBER OF FIRING NEURONS FROM MESSAGE; TASK ID = "<<pvm_mytid()<<"; numberOfSpikes: "<<numberOfSpikes<<endl;
			return;
		}
	#endif//PVM_DEBUG

	//Unpack the list of firing neurons. No problem with duplicates here.
	info = pvm_upkuint(unpackArray, numberOfSpikes, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamArchiver: ERROR UNPACKING FIRING NEURON IDS FROM MESSAGE; TASK ID = "<<pvm_mytid()<<"; numberOfSpikes: "<<numberOfSpikes<<endl;
			return;
		}
	#endif//PVM_DEBUG

	//Now add list of firing neurons to string stream
        for(unsigned int i=0; i<numberOfSpikes; ++i){
		firingNeuronStringStr<<unpackArray[i];
		if(i != numberOfSpikes - 1)//don't add comma for the last number
			firingNeuronStringStr<<",";
	}

	#ifdef FIRING_NEURON_DEBUG
		cout<<"SpikeStreamArchiver: Number of firing neurons processed: "<<numberOfSpikes<<" from sender: "<<senderTID<<" at timeStep: "<<messageTimeStep<<endl;
	#endif//FIRING_NEURON_DEBUG

	//Add string with firing neuron information to xml string
	timeStepMap[messageTimeStep].xmlString += "<neuron_group id=\"";
	timeStepMap[messageTimeStep].xmlString += getString(taskToNeuronGrpMap[senderTID]);
	timeStepMap[messageTimeStep].xmlString += "\">";
	timeStepMap[messageTimeStep].xmlString += firingNeuronStringStr.str();//firingNeuronString;
	timeStepMap[messageTimeStep].xmlString += "</neuron_group>";

	//Check the time step map to see if any of them is complete
	for(map<unsigned int, TimeStepHolder>::iterator iter = timeStepMap.begin(); iter != timeStepMap.end(); ++iter){
		if(iter->second.spikeMessageCount == spikeMessageTotal){//All messages have been received for this time step
			
			//Finish off xml string
			iter->second.xmlString += "</network_pattern>";

			//Store string in database
			if(storeNetworkData(iter->first, iter->second.xmlString)){
				//Remove timeStepHolder from map if it has been successfully stored
				timeStepMap.erase(iter);
			}

			//Should only be one timeStepHolder completed for each time step so break here
			break;
		}
	}
}


/*! Processes a list of spikes from the task simulating the neuron group that is being monitored. 
	Need to receive all the neuron groups. This is checked using task ids.
	For each neuron group add the firing neurons to the archive. */
void SpikeStreamArchiver::processSpikeList(int senderTID){
		//Get the time step of the message
	int info = pvm_upkuint(&messageTimeStep, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamArchiver: ERROR EXTRACTING MESSAGE TIME STEP; TASK ID = "<<pvm_mytid()<<"; messageTimeStep: "<<messageTimeStep<<endl;
			return;
		}
	#endif//PVM_DEBUG

	//See if we already have a timeStepHolder for this time step
	if(timeStepMap.find(messageTimeStep) != timeStepMap.end()){
		//Have found one so increase spike message count for map
		timeStepMap[messageTimeStep].spikeMessageCount += senderTID;
	}

	/* Do not have a timeStepHolder for this map, so create one and 
		initialise spikeMessageCount, unix time and xmlString */
	else{
		//Initialise spikeMessageCount
		timeStepMap[messageTimeStep].spikeMessageCount = senderTID;

		//Start creating the XML file
		timeStepMap[messageTimeStep].xmlString = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
		timeStepMap[messageTimeStep].xmlString += "<network_pattern>";
	}

	// Extract the number of spikes in the message
	info = pvm_upkuint(&numberOfSpikes, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"NetworkMonitor: ERROR NUMBER OF FIRING NEURONS FROM MESSAGE; TASK ID = "<<pvm_mytid()<<"; numberOfSpikes: "<<numberOfSpikes<<endl;
			return;
		}
	#endif//PVM_DEBUG

	//Unpack the spikes into the firingNeuronMap to filter out duplicates
	firingNeuronMap.clear();

	//Unpack the list of spikes 
	info = pvm_upkuint(unpackArray, numberOfSpikes, 1);//Unpack from and to neuron ids as two shorts compressed into an integer
	#ifdef PVM_DEBUG
		if(info < 0){
			systemError("NetworkMonitor: ERROR UNPACKING UNSIGNED INT FROM MESSAGE. NUMBER OF SPIKES = ", numberOfSpikes);
			return;
		}
	#endif//PVM_DEBUG
	
	//Process the spikes to get the from ids
	for(unsigned int i=0; i<numberOfSpikes; ++i){
		/* Add the from neuron ID to the from key.
			Only need the from neuron id because we are monitoring spikes from a layer*/
		unpkFromNeurID = (unsigned short) unpackArray[i];
		unpkFromNeurID += startNeurIDTaskMap[senderTID];;

		//Add it to the firing neuron map
		firingNeuronMap [ unpkFromNeurID ] = true;
	}


	//Now add list of firing neurons to string buffer
	stringstream firingNeuronStringStr;
	bool firstTime = true;
	for(map<unsigned int, bool>::iterator iter = firingNeuronMap.begin(); iter != firingNeuronMap.end(); ++iter){
		if(!firstTime){
			firingNeuronStringStr<<",";
		}
		firingNeuronStringStr<<iter->first;
		firstTime = false;
	}

	//Add string with firing neuron information to xml string
	timeStepMap[messageTimeStep].xmlString += "<neuron_group id=\"";
	timeStepMap[messageTimeStep].xmlString += getString(taskToNeuronGrpMap[senderTID]);
	timeStepMap[messageTimeStep].xmlString += "\">";
	timeStepMap[messageTimeStep].xmlString += firingNeuronStringStr.str();
	timeStepMap[messageTimeStep].xmlString += "</neuron_group>";


	//Check the time step map to see if any of them is complete
	for(map<unsigned int, TimeStepHolder>::iterator iter = timeStepMap.begin(); iter != timeStepMap.end(); ++iter){
		if(iter->second.spikeMessageCount == spikeMessageTotal){//All messages have been received for this time step
			
			//Finish off xml string
			iter->second.xmlString += "</network_pattern>";

			//Store string in database
			if(storeNetworkData(iter->first, iter->second.xmlString)){
				//Remove timeStepHolder from map if it has been successfully stored
				timeStepMap.erase(iter);
			}

			//Should only be one timeStepHolder completed for each time step so break here
			break;
		}
	}
}

/*! Sends a message without any contents. */
bool SpikeStreamArchiver::sendMessage(int taskID, int msgtag){
	//Initialise the buffer
	int info = pvm_initsend(PvmDataDefault);
	#ifdef PVM_DEBUG
		if(info < 0){
			cout<<"SpikeStreamArchiver: Init send error: "<<info<<" with task "<<taskID<<" and msgtag: "<<msgtag<<endl;
			return false;
		}
	#endif//PVM_DEBUG

	//Send the message
	info = pvm_send(taskID, msgtag);
	#ifdef PVM_DEBUG
		if(info < 0){
			cout<<"SpikeStreamArchiver: Send error: "<<info<<" with task "<<taskID<<" and msgtag: "<<msgtag<<endl;
			return false;
		}
	#endif//PVM_DEBUG
	return true;
}


/*! Sends a message with a single integer. */
bool SpikeStreamArchiver::sendMessage(int taskID, int msgtag, unsigned int msgInteger){
	//Initialise buffer
	int info = pvm_initsend(PvmDataDefault);
	#ifdef PVM_DEBUG
		if(info < 0){
			cout<<"SpikeStreamArchiver: Init send error: "<<info<<" with task "<<taskID<<" and msgtag: "<<msgtag<<endl;
			return false;
		}
	#endif//PVM_DEBUG

	//Pack the integer
	info = pvm_pkuint(&msgInteger, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamArchiver: ERROR PACKING INTEGER INTO MESSAGE"<<endl;
			return false;
		}
	#endif//PVM_DEBUG
	
	//Send the message
	info = pvm_send(taskID, msgtag);
	#ifdef PVM_DEBUG
		if(info < 0){
			cout<<"SpikeStreamArchiver: Send error: "<<info<<" with task "<<taskID<<" and msgtag: "<<msgtag<<endl;
			return false;
		}
	#endif//PVM_DEBUG
	return true;
}


/*! Sends a message containing a char string. */
bool SpikeStreamArchiver::sendMessage(int taskID, int msgtag, const char* charArray){
	//First find the length of the char array (+1 so that other end does not have to add 1 for null character)
	unsigned int arrayLength = strlen(charArray) + 1;

	//Initialise the buffer
	int info = pvm_initsend(PvmDataDefault);
	#ifdef PVM_DEBUG
		if(info<0){
			cerr<<"SpikeStreamArchiver: Init send error: tag: "<<msgtag<<" to: "<<taskID<<" containing: "<<charArray<<endl;
			return false;
		}
	#endif//PVM_DEBUG

	//Pack the length of the char array
	info = pvm_pkuint(&arrayLength, 1, 1);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamArchiver: ERROR PACKING MESSAGE LENGTH INTO MESSAGE"<<endl;
			return false;
		}
	#endif //PVM_DEBUG

	//Pack the char array
	info = pvm_pkstr((char*)charArray);
	#ifdef PVM_DEBUG
		if(info < 0){
			cerr<<"SpikeStreamArchiver: ERROR PACKING  CHAR* INTO MESSAGE"<<endl;
			return false;
		}
	#endif //PVM_DEBUG

	//Send message
	info = pvm_send(taskID, msgtag);
	#ifdef PVM_DEBUG
		if(info<0){
			cerr<<"NeuronSimulation: Message send error: tag: "<<msgtag<<" to: "<<taskID<<" containing: "<<charArray<<endl;
			return false;
		}
	#endif //PVM_DEBUG
	return true;
}


/*! Sends a message to all simulation tasks requesting them to start sending spike messages. */
void SpikeStreamArchiver::startArchiving(){
	for(map<int, unsigned int>::iterator iter = taskToNeuronGrpMap.begin(); iter != taskToNeuronGrpMap.end(); ++iter){
		//Send a different message depending on the archive type
		if(archiveType == FIRING_NEURON_ARCHIVE)
			sendMessage(iter->first, REQUEST_FIRING_NEURON_DATA_MSG);
		else if (archiveType == SPIKE_ARCHIVE)
			sendMessage(iter->first, REQUEST_SPIKE_DATA_MSG);
	}
}


/*! Sends a message to all simulation taks requesting them to stop sending spike messages. */
void SpikeStreamArchiver::stopArchiving(){
	for(map<int, unsigned int>::iterator iter = taskToNeuronGrpMap.begin(); iter != taskToNeuronGrpMap.end(); ++iter){
		if(archiveType == FIRING_NEURON_ARCHIVE)
			sendMessage(iter->first, CANCEL_FIRING_NEURON_DATA_MSG);
		else if (archiveType == SPIKE_ARCHIVE)
			sendMessage(iter->first, CANCEL_SPIKE_DATA_MSG);
	}
}


/*! Stores the network data in the database. */
bool SpikeStreamArchiver::storeNetworkData(unsigned int timeStep, string xmlString){
	try{
		Query archiveQuery = archiveDBInterface->getQuery();
		archiveQuery.reset();
		archiveQuery<<"INSERT INTO NetworkData VALUES ( "<<simulationStartTime<<", "<<timeStep<<", "<<mysqlpp::quote<<xmlString<<" )";
		//Only preview when using xpvm, otherwise this could crash.
		bool queryResult = archiveQuery.exec();
		if(!queryResult){
			systemError("SpikeStreamArchiver: UNSUCCESSFUL MYSQLPP QUERY STORING NETWORK DATA");
			return false;
		}

		/* Record that network data has been stored. 
			Model file will be deleted if no network data is stored for it */
		networkDataStored = true;
	}
	catch (const BadQuery& er) {// Handle any query errors
		ostringstream errorStrStream;
		errorStrStream<<"Bad query when storing network data: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
		return false;
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown storing network data: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
		return false;
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown storing network data: \""<<er.what()<<"\"";
		systemError(errorStrStream.str());
		return false;
	}

	//If we have reached this point everything should be ok
	return true;
}




