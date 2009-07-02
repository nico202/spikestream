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
#include "DeviceManager.h"
#include "Debug.h"
#include "Utilities.h"
#include "DeviceTypes.h"
#include "SimulationClock.h"
#include "SpikeStreamSimulation.h"

//Other includes
#include <math.h>
#include <errno.h>
#include <sys/shm.h>
#include <iostream>
#include <mysql++.h>
using namespace mysqlpp;
using namespace std;

//#define DEVICE_FIRING_MODE_DEBUG
//#define DEVICE_DEBUG

/*! Main constructor. */
DeviceManager::DeviceManager(unsigned int devID, unsigned int neurGrpID, double devFiringMode, DBInterface* devDBInter, DBInterface *netDBInter){
	//Store database references
	deviceDBInterface = devDBInter;
	networkDBInterface = netDBInter;

	//Store information about device and neuron group
	deviceID = devID;
	neuronGrpID = neurGrpID;

	//Initialise variables
	deviceType = UNKNOWN_DEVICE_VALUE;
	deviceOpen = false;

	//Initialise device buffers
	for(int i=0; i<NUMBER_OF_DELAY_VALUES; ++i){
		deviceBuffer[i].set_empty_key(EMPTY_NEURON_ID_KEY);
		deviceBuffer[i].set_deleted_key(DELETED_NEURON_ID_KEY);
	}

	//Load up information about this device
	try{
		loadDeviceInformation();
	}
	catch (const BadQuery& er) {// Handle any query errors
		ostringstream errorStrStream;
		errorStrStream<<"Bad query when loading device information: \""<<er.what()<<"\"";
		SpikeStreamSimulation::systemError(errorStrStream.str());
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown loading device information: \""<<er.what()<<"\"";
		SpikeStreamSimulation::systemError(errorStrStream.str());
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown loading device information: \""<<er.what()<<"\"";
		SpikeStreamSimulation::systemError(errorStrStream.str());
	}


	//Extract information about the firing mode - deviceType should be initialised at this point
	if(DeviceTypes::isInputDevice(deviceType)){
		if(devFiringMode == INPUT_DIRECT_FIRING_MODE){
			directFiringMode = true;
			synapticWeight = 0.0;
			#ifdef DEVICE_FIRING_MODE_DEBUG
				cout<<"DEVICE FIRING MODE: Input device. Direct firing mode"<<endl;
			#endif//DEVICE_FIRING_MODE_DEBUG
		}
		else if (devFiringMode == OUTPUT_FIRING_MODE){
			SpikeStreamSimulation::systemError("Device firing mode is set to output device, but this is an input device!");
		}
		else{
			directFiringMode = false;
			synapticWeight = devFiringMode;
			#ifdef DEVICE_FIRING_MODE_DEBUG
				cout<<"DEVICE FIRING MODE: Input device. Synaptic firing mode with weight = "<<synapticWeight<<endl;
			#endif//DEVICE_FIRING_MODE_DEBUG
		}
	}
	else{//Should not be using these if it is an output device.
		directFiringMode = false;
		synapticWeight = 0.0;
		#ifdef DEVICE_FIRING_MODE_DEBUG
			cout<<"DEVICE FIRING MODE: Output device. No firing mode set"<<endl;
		#endif//DEVICE_FIRING_MODE_DEBUG
	}

	//Remove all entries from the synchronization delay database
	clearSynchronizationDelay();
}


/*! Default constructor. Used when the device manager will not be used but might 
	have methods called on it. */
// FIXME ELIMINATE THIS AT SOME POINT
DeviceManager::DeviceManager(){
	//Initialise variables
	deviceDBInterface = NULL;
	deviceID = 0;
	neuronGrpID = 0;
	deviceType = UNKNOWN_DEVICE_VALUE;
	deviceOpen = false;
}


/*! Destructor. */
DeviceManager::~DeviceManager(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING DEVICE MANAGER"<<endl;
	#endif//MEMORY_DEBUG	

	if(deviceOpen)
		closeDevice();

	/*Remove all entries from the synchronization delay database
		Only do this if it is a meaningful device. Otherwise dBInterface may not exist */
	if(deviceDBInterface != NULL)
		clearSynchronizationDelay();
}


//-------------------------------------------------------------------------
//-------------------------- PUBLIC METHODS -------------------------------
//-------------------------------------------------------------------------

/*! Does anything else that is needed to reset device. Returns true if device
	closes successfully. */
bool DeviceManager::closeDevice(){
	if(deviceOpen){
		//Thread listening on socket for incoming messages
		if(deviceType == DeviceTypes::syncUDPNetworkInput){
			if(udpSyncClient->closeDevice()){
				deviceOpen = false;
				return true;
			}
		}
		else if (deviceType == DeviceTypes::syncUDPNetworkOutput){
			if(udpSyncServer->closeDevice()){
				deviceOpen = false;
				return true;
			}
		}
		else if(deviceType == DeviceTypes::syncTCPNetworkInput || deviceType == DeviceTypes::syncTCPNetworkVisionInput){
			if(tcpSyncClient->closeDevice()){
				deviceOpen = false;
				return true;
			}
		}
		else if(deviceType == DeviceTypes::syncTCPNetworkOutput){
			if(tcpSyncServer->closeDevice()){
				deviceOpen = false;
				return true;
			}
		}
		else{
			SpikeStreamSimulation::systemError("DeviceManager: ATTEMPTING TO CLOSE AN UNRECOGNIZED DEVICE TYPE");
			return false;
		}
	}
	return true;
}


/*! Instructs the device to download data from the external device or if data is being 
	obtained asynchronously it returns true if the thread is still running to do this. */
bool DeviceManager::fetchData(){
	if(deviceType == DeviceTypes::syncTCPNetworkInput || deviceType == DeviceTypes::syncTCPNetworkVisionInput){
		return tcpSyncClient->fetchData();//Get data from external device
	}
	else if (deviceType == DeviceTypes::syncUDPNetworkInput){
		return udpSyncClient->clientThreadRunning();//Check client thread is still running
	}
	else{
		SpikeStreamSimulation::systemError("DeviceManager: ATTEMPTING TO RECEIVE DATA FROM AN UNRECOGNIZED DEVICE TYPE");
		return false;
	}
}


/*! Returns the type of this device. */
unsigned int DeviceManager::getDeviceType(){
	return deviceType;
}


/*! When synchronizing to an external device this returns the time
	between time steps of the external device. */
unsigned int DeviceManager::getExternalComputeTime_us(){
	if(deviceType == DeviceTypes::syncUDPNetworkInput)
		return udpSyncClient->getExternalComputeTime_us();
	else{
		SpikeStreamSimulation::systemError("DeviceManager: CALLING GET EXTERNAL COMPUTE TIME WHEN IT IS NOT APPROPRIATE");
		return 0;
	}
}


/*! When synchronizing to an external device, this returns whether the external
	device is delaying or not. */
bool DeviceManager::getExternalSyncDelay(){
	if(deviceType == DeviceTypes::syncUDPNetworkInput)
		return udpSyncClient->getExternalSyncDelay();
	else{
		SpikeStreamSimulation::systemError("DeviceManager: CALLING EXTERNAL SYNC DELAY WHEN IT IS NOT APPROPRIATE");
		return false;
	}
}


/*! Returns true if this is an input device - i.e. provides data TO SpikeStream
	Uses the same method as DeviceTypes method to determine this. */
bool DeviceManager::isInputDevice(){
	if(deviceType % 2 == 1)
		return true;
	return false;
}


/*! Returns true if this is an output device - i.e. receives data FROM SpikeStream
	Uses the same method as DeviceTypes method to determine this. */
bool DeviceManager::isOutputDevice(){
	if(deviceType % 2 == 0)
		return true;
	return false;
}


/*! Sets the neuron array for this device manager. */
void DeviceManager::setNeuronArray(Neuron **neurArr){
	neuronArray = neurArr;
}


/*! Sets a reference to the map used to record which neurons have been changed during
	the timestep. */
void DeviceManager::setNeuronUpdateMap(dense_hash_map<unsigned int, bool, hash<unsigned int> >* neurUdtMp){
	neuronUpdateMap = neurUdtMp;
}


/*! Passes a reference to the vector containing the currently firing neurons
	to the device, which passes it on to other classes as necessary. */
void DeviceManager::setNeuronVector(vector<unsigned int> *neurVectPtr, unsigned int startNeurID){
	startNeuronID = startNeurID;
	if (deviceType == DeviceTypes::syncUDPNetworkOutput){
		udpSyncServer->setNeuronVector(neurVectPtr, startNeurID);
	}
	else if(deviceType == DeviceTypes::syncTCPNetworkOutput){
		tcpSyncServer->setNeuronVector(neurVectPtr, startNeurID);
	}
}


/*! Sends the data from SpikeStream to the external device. */
bool DeviceManager::updateDevice(){
	/*Output spikes to network. Want to output the x and y values of the spike events
		and the time of the events. */
	if (deviceType == DeviceTypes::syncUDPNetworkOutput){
		return udpSyncServer->sendSpikeData();
	}
	else if (deviceType == DeviceTypes::syncTCPNetworkOutput){
		return tcpSyncServer->sendSpikeData();
	}
	else{
		SpikeStreamSimulation::systemError("DeviceManager: ATTEMPTING TO SEND DATA TO AN UNRECOGNIZED DEVICE TYPE");
		return false;
	}
}


/*! Updates neurons with data from device. */
void DeviceManager::updateNeurons(){
	//Extract the data and fill the device buffer
	fillDeviceBuffer();

	/* Fire all the neurons at the current position in the buffer, either directly or
		by injecting the specified synaptic current. */
	for(dense_hash_map<unsigned int, bool, hash<unsigned int> >::iterator iter = deviceBuffer[bufferCounter].begin(); iter != deviceBuffer[bufferCounter].end(); ++iter){
		if(directFiringMode)
			neuronArray[iter->first]->fireNeuron();
		else{
			neuronArray[iter->first]->changePostSynapticPotential(synapticWeight, 0);
			(*neuronUpdateMap)[ iter->first + startNeuronID ] = true;
		}
	}
	
	//Reset currently active buffer and advance the buffer
	deviceBuffer[bufferCounter].clear();//Empty the buffer's vector
	++bufferCounter;
	bufferCounter = bufferCounter % NUMBER_OF_DELAY_VALUES;
}


//-------------------------------------------------------------------------
//------------------------- PRIVATE METHODS -------------------------------
//-------------------------------------------------------------------------

/*! Empties the SynchronizationDelay database. */
void DeviceManager::clearSynchronizationDelay(){
	try{
		Query deviceQuery = deviceDBInterface->getQuery();
		deviceQuery.reset();
		deviceQuery<<"DELETE FROM SynchronizationDelay";
		deviceQuery.execute();
	}
	catch (const BadQuery& er) {// Handle any query errors
		ostringstream errorStrStream;
		errorStrStream<<"Bad query when clearing synchronization delay: \""<<er.what()<<"\"";
		SpikeStreamSimulation::systemError(errorStrStream.str());
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown clearing synchronization delay: \""<<er.what()<<"\"";
		SpikeStreamSimulation::systemError(errorStrStream.str());
	}
}


/*! Fills up device buffer with data obtained from an external device. */
void DeviceManager::fillDeviceBuffer(){
	if(deviceType == DeviceTypes::syncUDPNetworkInput){
		//Lock the mutex so that the spike client cannot change threadSpikeBuffer whilst it is being loaded
		udpSyncClient->lockMutex();
	
		//Fill the device buffer
		for(unsigned int i=0; i<udpSyncClient->spikeCount; ++i){
	
			//Extract x, y and delay values
			unsigned int xPos = udpSyncClient->spikeBuffer[i * 3];
			unsigned int yPos = udpSyncClient->spikeBuffer[i * 3 + 1];
			unsigned int delay = udpSyncClient->spikeBuffer[i * 3 + 2];
	
			//Calculate position in array and add to device buffer
			int arrayIndex = xPos + yPos*neuronGrpWidth;
			int insertionPosition = (bufferCounter + delay) % NUMBER_OF_DELAY_VALUES;
			deviceBuffer[insertionPosition][arrayIndex] = true;//Add array position to device buffer at delay after buffer counter
		}
	
		//Reset spike count
		udpSyncClient->spikeCount = 0;
	
		//Unlock the mutex so that spike client can continue to fill it
		udpSyncClient->unlockMutex();
	}
	else if(deviceType == DeviceTypes::syncTCPNetworkInput || deviceType == DeviceTypes::syncTCPNetworkVisionInput){
		/* NOTE Fetching of the data is done by SpikeStreamSimulation class
			by calling fetchData() in this class */
		for(unsigned int i=0; i<tcpSyncClient->spikeCount; ++i){

			//Extract x, y and delay values
			unsigned int xPos = tcpSyncClient->spikeBuffer[i * 3];
			unsigned int yPos = tcpSyncClient->spikeBuffer[i * 3 + 1];
			unsigned int delay = tcpSyncClient->spikeBuffer[i * 3 + 2];

			//Calculate position in array and add to device buffer
			int arrayIndex = xPos + yPos*neuronGrpWidth;
			int insertionPosition = (bufferCounter + delay) % NUMBER_OF_DELAY_VALUES;
			deviceBuffer[insertionPosition][arrayIndex] = true;//Add array position to device buffer at delay after buffer counter
		}
	}
	else{
		SpikeStreamSimulation::systemError("DeviceManager: DEVICE TYPE NOT RECOGNIZED");
	}
}


/*! Loads up information about the device that this class has to manage.
	Should only be called once when device manager initialises. 
	NOTE: Exceptions should be handled by the calling class. */
void DeviceManager::loadDeviceInformation(){
	deviceOpen = false;

	#ifdef DEVICE_DEBUG
		cout<<"DeviceManager: Loading Device information"<<endl;
	#endif//DEVICE_DEBUG

	/* Get the width and length of the neuron group. */
	Query networkQuery = networkDBInterface->getQuery();
	networkQuery.reset();
	networkQuery<<"SELECT Width, Length FROM NeuronGroups WHERE NeuronGrpID = "<<neuronGrpID;
        StoreQueryResult networkResults = networkQuery.store();
	Row networkRow(*networkResults.begin());
	neuronGrpWidth = Utilities::getUInt((std::string)networkRow["Width"]);
	neuronGrpLength = Utilities::getUInt((std::string)networkRow["Length"]);

	//Get information about the device
	Query deviceQuery = deviceDBInterface->getQuery();
	deviceQuery.reset();
	deviceQuery<<"SELECT IPAddress, Port, TotalNumColumns, TotalNumRows, Type FROM Devices WHERE DeviceID = "<<deviceID;
        StoreQueryResult deviceResults = deviceQuery.store();
	Row deviceRow(*deviceResults.begin());
	deviceType = Utilities::getUInt((std::string)deviceRow["Type"]);
	unsigned int deviceWidth = Utilities::getUInt((std::string)deviceRow["TotalNumColumns"]);
	unsigned int deviceLength = Utilities::getUInt((std::string)deviceRow["TotalNumRows"]);
	unsigned int devicePort = Utilities::getUInt((std::string)deviceRow["Port"]);

	#ifdef DEVICE_DEBUG
		cout<<"Creating device with type="<<deviceType<<"; IPAddress="<<deviceRow["IPAddress"]<<"; Port="<<devicePort<<"; deviceWidth="<<deviceWidth<<"; deviceLength="<<deviceLength<<endl;
	#endif//DEVICE_DEBUG

	//Open up appropriate input or output port
	if(deviceType == DeviceTypes::syncUDPNetworkInput){//Listen for incoming spikes from network and adjusts its time to match that of the external source
		//Create a client to listen for synchronized udp input
		udpSyncClient = new UDPSynchronizedClient(neuronGrpWidth, neuronGrpLength);

		//Open the socket on the client and start thread running
		if(udpSyncClient->openSocket((std::string)deviceRow["IPAddress"], devicePort))
			udpSyncClient->start();
		else
			return;
	}
	else if(deviceType == DeviceTypes::syncUDPNetworkOutput){
		udpSyncServer = new UDPSynchronizedServer(deviceDBInterface, neuronGrpWidth);
		if(!udpSyncServer->openSocket((std::string)deviceRow["IPAddress"], devicePort))
			return;
	}
	else if(deviceType == DeviceTypes::syncTCPNetworkInput || deviceType == DeviceTypes::syncTCPNetworkVisionInput){
		//Create client to request data from device
		tcpSyncClient = new TCPSynchronizedClient(neuronGrpWidth, neuronGrpLength);

		/* If it is a vision input set device to process incoming data as two byte
			coordinates */
		if(deviceType == DeviceTypes::syncTCPNetworkVisionInput){
			tcpSyncClient->setTwoByteCoords(true);
		}
		
		//Open socket
		if(!tcpSyncClient->openSocket((std::string)deviceRow["IPAddress"], devicePort))
			return;
	}
	else if(deviceType == DeviceTypes::syncTCPNetworkOutput){
		tcpSyncServer = new TCPSynchronizedServer(neuronGrpWidth);
		if(!tcpSyncServer->openSocket((std::string)deviceRow["IPAddress"], devicePort))
			return;
	}
	else {
		SpikeStreamSimulation::systemError("DeviceManager: DEVICE TYPE NOT RECOGNIZED");
	}

	//Sort out whether the input should be rotated or not
	if(neuronGrpWidth == deviceWidth && neuronGrpLength == deviceLength){
		rotatePattern = false;//FIXME ROTATE PATTERN NOT IMPLEMENTED
	} 
	else if(neuronGrpWidth == deviceLength && neuronGrpLength == deviceWidth){
		rotatePattern = true;//FIXME ROTATE PATTERN NOT IMPLEMENTED
		SpikeStreamSimulation::systemError("DeviceManager: ROTATE PATTERN NOT IMPLEMENTED.\nDEVICE WIDTH AND LENGTH DO NOT MATCH NEURON GROUP WIDTH AND LENGTH");
		return;
	}
	else{
		SpikeStreamSimulation::systemError("DeviceManager: DEVICE WIDTH AND LENGTH DO NOT MATCH NEURON GROUP WIDTH AND LENGTH");
		return;
	}

	//Device is now open
	deviceOpen = true;

	#ifdef DEVICE_DEBUG
		cout<<"DeviceManager: Device information loaded"<<endl;
	#endif//DEVICE_DEBUG
}


