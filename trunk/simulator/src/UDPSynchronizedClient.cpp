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

//Reentrant needs to be defined for threads
#ifndef _REENTRANT
	#define _REENTRANT
#endif

//SpikeStream includes
#include "UDPSynchronizedClient.h"
#include "SpikeStreamSimulation.h"

/* Thread includes. Note that this needs to come after the 
	SpikeStream includes to avoid strange error about int and braces */
#include <pthread.h>

//Other includes
#include <errno.h>
#include <math.h>


/*! Size of the receive buffer used to unpack spikes from the network. */
#define MESSAGE_BUFFER_SIZE 1048576

/*! Size of the array used to store received spikes.
	This has enough space for more than 5000 spikes, each of which takes 
	12 bytes to store. */
#define SPIKE_BUFFER_SIZE 60000

/*! Step counter is a 15 bit number whose max value is defined here. */
#define STEP_COUNTER_MAX 32768

/*! Maximum time that we will slow down to accommodate external device 
	when no messages have been received. */
#define MAX_EXTERNAL_COMPUTE_TIME_US 500000


/*! Have to define mutex here because of define that is used to 
	initialise it. */
pthread_mutex_t threadMutex = PTHREAD_MUTEX_INITIALIZER;


/*! Function that thread for this class calls when it starts running
	This function calls the run method of the class. */
void* startThreadFunction(void* udpSyncClient){
	((UDPSynchronizedClient*)udpSyncClient)->run();
}


/*! Constructor. */
UDPSynchronizedClient::UDPSynchronizedClient(unsigned int neurGrpWidth, unsigned int neurGrpLength){
	//Store details about neuron group
	neuronGrpWidth = neurGrpWidth;
	neuronGrpLength = neurGrpLength;

	//Initialise variables
	socketOpen = false;
	threadRunning = false;
	externalSyncDelay = false;
	externalComputeTime_us = 0;
	timeStepsNoMessagesCount = 0;

	//Set up array to hold spikes
	spikeCount = 0;
	spikeBuffer = new unsigned int[SPIKE_BUFFER_SIZE];

	//Set up buffer to receive messages from the network
	messageBuffer = new unsigned char[MESSAGE_BUFFER_SIZE];
}


/*! Destructor. */
UDPSynchronizedClient::~UDPSynchronizedClient(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING UDP SYNCHRONIZATION CLIENT"<<endl;
	#endif//MEMORY_DEBUG

	//Close socket and terminate thread if necessary
	closeDevice();

	//Free memory for spike buffer and message buffer
	delete [] spikeBuffer;
	delete [] messageBuffer;
}


//-----------------------------------------------------------------------------
//--------------------------- PUBLIC METHODS ----------------------------------
//-----------------------------------------------------------------------------

/*! Returns true if the thread is still in its run method and in the state of 
	receiving UDP data. */
bool UDPSynchronizedClient::clientThreadRunning(){
	return threadRunning;
}


/*! Closes the socket and/or stops the thread. */
bool UDPSynchronizedClient::closeDevice(){
	//Create bool to record close errors
	bool closeOk = true;

	//Stop the run method
	threadRunning = false;

	//Wait for thread to rejoin
	void *thread_result;
	int joinResult = pthread_join(clientThread, &thread_result);
	if(joinResult != 0){
		SpikeStreamSimulation::systemError("UDPSynchronizedClient: Thread join failed");
		//Better not to return here so that socket can be closed before returning false
		closeOk = false;		
	}
	else{
		SpikeStreamSimulation::systemInfo("UDPSynchronizedClient: Thread successfully rejoined");
	}

	//Close the socket
	close(socketHandle);
	socketOpen = false;

	//Return the outcome
	return closeOk;
}


/*! Returns true if socket is open or thread is running. */
bool UDPSynchronizedClient::deviceOpen(){
	if(socketOpen || threadRunning)
		return true;
	return false;
}


/*! This method is called at each time step to get the current 
	external compute time. It increases a counter each time it is
	called which is used to increase the external compute time
	in the absence of any messages from the other processes,
	up to a maximum. This prevents the external compute time
	remaining at a low value when there is no external input. */
unsigned int UDPSynchronizedClient::getExternalComputeTime_us(){
	//Increase the record of time steps without messages
	++timeStepsNoMessagesCount;
	if(timeStepsNoMessagesCount <= 10)
		return externalComputeTime_us;//We are roughly in sync give or take a few lost messages

	//Have not received a message for more than 10 time steps, so want to slow down a bit
	unsigned int newExternalComputeTime_us = timeStepsNoMessagesCount * externalComputeTime_us;
	if(newExternalComputeTime_us < MAX_EXTERNAL_COMPUTE_TIME_US)//Don't want to stop completely
		return newExternalComputeTime_us;
	return MAX_EXTERNAL_COMPUTE_TIME_US;
}


/*! Returns true if the external device is delaying itself. */
bool UDPSynchronizedClient::getExternalSyncDelay(){
	return externalSyncDelay;
}


/*! Used by external class to lock this thread's mutex and prevent it
	accessing shared data. */
void UDPSynchronizedClient::lockMutex(){
	pthread_mutex_lock( &threadMutex );
}


/*! Opens a socket to receive spikes
	Adapted from http://ntrg.cs.tcd.ie/undergrad/4ba2/multicast/antony/index.html. */
bool UDPSynchronizedClient::openSocket(string groupAddress, int port){
    struct ip_mreq multicastRequest;
    struct sockaddr_in multicastAddr; /* Multicast Address */

    /* Create a best-effort datagram socket using UDP */
    if ((socketHandle = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
		SpikeStreamSimulation::systemError("UDPSynchronizedClient: ERROR IN socket");
		return false;
	}

    /* Allow multiple sockets to use the same PORT number */
	unsigned int multipleListeners = 1; 
    if (setsockopt(socketHandle, SOL_SOCKET, SO_REUSEADDR, &multipleListeners, sizeof(multipleListeners)) < 0) {
		SpikeStreamSimulation::systemError("UDPSynchronizedClient: ERROR Reusing ADDR failed");
		return false;
	}

	// Set up socket to timeout after half a second so that it can be shut down easily
	struct timeval tmout;
	tmout.tv_sec = 0;
	tmout.tv_usec = 500000;
    if (setsockopt(socketHandle, SOL_SOCKET, SO_RCVTIMEO, &tmout, sizeof(tmout)) < 0) {
		SpikeStreamSimulation::systemError("UDPSynchronizedClient: ERROR SETTING SOCKET TIMEOUT ");
		return false;
	}

    /* Construct bind structure */
    memset(&multicastAddr, 0, sizeof(multicastAddr));   /* Zero out structure */
    multicastAddr.sin_family = AF_INET;                 /* Internet address family */
    multicastAddr.sin_addr.s_addr = htonl(INADDR_ANY);  /* Any incoming interface */
    multicastAddr.sin_port = htons(port);      /* Multicast port */

    /* Bind to the multicast port */
	//FIXME IS THIS REALLY NECESSARY?
    if (bind(socketHandle, (struct sockaddr *) &multicastAddr, sizeof(multicastAddr)) < 0){
		SpikeStreamSimulation::systemError("UDPSynchronizedClient: ERROR IN bind");
		return false;
	}

    /* Specify the multicast group */
    multicastRequest.imr_multiaddr.s_addr = inet_addr(groupAddress.data());

    /* Accept multicast from any interface */
    multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);

    /* Join the multicast address */
    if (setsockopt(socketHandle, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *) &multicastRequest, sizeof(multicastRequest)) < 0){
		SpikeStreamSimulation::systemError("UDPSynchronizedClient: ERROR JOINING MULTICAST");
		return false;
	}

	//Socket has been successfully opened
	socketOpen = true;
	return true;
}


/*! Run method for this class, called by the thread that is started. */
void UDPSynchronizedClient::run(){
	//Only start run method if the socket is open
	if(!socketOpen){
		threadRunning = false;
		return;
	}

	//Set up variables for use in this method
	unsigned int interMessageTime_us = 0, externalStepCtr = 0, oldExternalStepCtr = 0;
	timeval currentTimeStruct, oldTimeStruct;
	oldTimeStruct.tv_sec = 0;
	oldTimeStruct.tv_usec = 0;

	bool firstMessage = true;
	unsigned int addressLength = sizeof(socketAddress);

	cout<<"UDPSynchronizedClient: Thread entering main loop"<<endl;

	//Main run loop
	while(threadRunning){
		//Receive from network
		int numBytes= recvfrom(socketHandle, messageBuffer, MESSAGE_BUFFER_SIZE, 0, (struct sockaddr *) &(socketAddress), &addressLength);
		if(numBytes < 2) {//Minimum of two bytes in this synchronization method
			if(errno == EAGAIN || errno == EWOULDBLOCK){
				#ifdef RECEIVE_DEVICE_DATA_DEBUG
					SpikeStreamSimulation::systemInfo("UDPSynchronizedClient thread: Socket timed out");
				#endif//RECEIVE_DEVICE_DATA_DEBUG
			}
			else{
				SpikeStreamSimulation::systemError("UDPSynchronizedClient: ERROR IN recvfrom EXITING RUN METHOD.");
				threadRunning = false;
			}
		}
		else{//Greater than or equal to 2 bytes received
			//Reset the counter recording time steps without messages
			timeStepsNoMessagesCount = 0;

			#ifdef RECEIVE_DEVICE_DATA_DEBUG
				SpikeStreamSimulation::systemInfo("UDPSynchronizedClient: Message received");
			#endif//RECEIVE_DEVICE_DATA_DEBUG

			//Lock mutex so that device manager cannot change or read buffer whilst it is being filled with spikes
			pthread_mutex_lock( &threadMutex );

			//Record time at which message arrived
			gettimeofday(&currentTimeStruct, NULL);
			if(firstMessage)//This is the first message that has been received
				oldTimeStruct = currentTimeStruct;

			//Run a check on the packet. Should be a multiple of 4 bytes + 2 synchronization bytes
			if( (numBytes - 2) % 4 != 0){
				SpikeStreamSimulation::systemError("UDPSynchronizedClient: INCORRECT NUMBER OF BYTES IN SPIKE PACKET ", numBytes);
				threadRunning = false;
				return;
			}

			//Each event is 4 bytes long and starts with the X and Y address and time difference
			//Each packet starts with two bytes containing the synchronization information
			//Check to see if this number of spikes will fit into buffer
			if(spikeCount + (( numBytes - 2 ) / 4 ) > ( SPIKE_BUFFER_SIZE / 12 )){//Each spike is 12 bytes with X, Y and time delay
				cout<<"UDPSynchronizedClient: Shared memory for spike buffer is full. Resetting spike count. SpikeCount = "<<spikeCount<<"; numBytes/6 = "<<(numBytes/6)<<"; Max spikes = "<<((SPIKE_BUFFER_SIZE - 4) / 8)<<endl;
				spikeCount = 0;
			}

			#ifdef RECEIVE_DEVICE_DATA_DEBUG
				cout<<"UDPSynchronizedClient: Message contains "<<numBytes<<" bytes"<<endl;
			#endif//RECEIVE_DEVICE_DATA_DEBUG

			//Read synchronization information
			//Read delay flag. This is the last bit of the second byte
			if(messageBuffer[0] & 1)
				externalSyncDelay = true;
			else
				externalSyncDelay = false;

			//Read in step counter. First position is the LSB + delay, second position is the MSB
			externalStepCtr = (unsigned char)messageBuffer[1];//Load MSB
			externalStepCtr <<= 8;
			externalStepCtr += (unsigned char)messageBuffer[0];//Add LSB + delay flag
			externalStepCtr >>= 1;//Get rid of delay flag

			//If first time, old external step ctr has to be set the same
			if(firstMessage){
				oldExternalStepCtr = externalStepCtr;
				firstMessage = false;
			}

			//Work out counter change and throw away out of sequence messages
			int stepCtrChange = 0;
			if(externalStepCtr > oldExternalStepCtr)
				stepCtrChange = externalStepCtr - oldExternalStepCtr;
			else if(oldExternalStepCtr - externalStepCtr > 30000)//Counter has probably gone round
				stepCtrChange = STEP_COUNTER_MAX - oldExternalStepCtr +  externalStepCtr;
			oldExternalStepCtr = externalStepCtr;//Have extracted the change, so record old value

			if(stepCtrChange > 0){//External device has increased its step
				//Calculate time between messages
				interMessageTime_us = 1000000 * (currentTimeStruct.tv_sec - oldTimeStruct.tv_sec) + ( currentTimeStruct.tv_usec - oldTimeStruct.tv_usec);
				oldTimeStruct = currentTimeStruct;

				//Calculate time of external time step
				externalComputeTime_us = (unsigned int)rint((double)interMessageTime_us / (double)stepCtrChange);

				//Read spikes from message, starting from the third byte
				for(int i=2; i < numBytes; i += 4){

					//Check X and Y are within the range of this neuron group
					if(messageBuffer[i] > neuronGrpWidth){
						SpikeStreamSimulation::systemError("UDPSynchronizedClient: X POSITION OUT OF RANGE", messageBuffer[i]);
						threadRunning = false;
						return;
					}
					if(messageBuffer[i+1] > neuronGrpLength){
						SpikeStreamSimulation::systemError("UDPSynchronizedClient: Y POSITION OUT OF RANGE", messageBuffer[i+1]);
						threadRunning = false;
						return;
					}

					//Store X and Y in the spike buffer
					spikeBuffer[spikeCount * 3] = messageBuffer[i];//X position
					spikeBuffer[spikeCount * 3 + 1] = messageBuffer[i+1]; // Y position
	
					/*Next two bytes are the timestamp.
						This is in units of 0.01 ms
						MSB is second position	*/
					unsigned int time = messageBuffer[i+3];//MSB
					time <<= 8;
					time += messageBuffer[i+2];//LSB

					//FIXME COME UP WITH SOMETHING BETTER HERE LINKED TO SIMULATION TIME OR SOMETHING
					unsigned int delay = time / 10;//Convert to 0.1 millisec sections
					if(delay > NUMBER_OF_DELAY_VALUES)
						spikeBuffer[spikeCount * 3 + 2] = 0;
					else
						spikeBuffer[spikeCount * 3 + 2] = delay;
	
					//printf("Delay = %d \n", spikeBuffer[(*spikeCount) * 3 + 2]);
	
					//Increase the record of the number of spikes in buffer
					++spikeCount;
				}
			}

			//Unlock mutex so that device manager can load spikes
			pthread_mutex_unlock( &threadMutex );
		}
	}
	SpikeStreamSimulation::systemInfo("UDPSynchronizedClient: Exiting spike client thread");
}


/*! Starts the thread running and listening for messages. */
void UDPSynchronizedClient::start(){
	//Set running to true for main while loop
	threadRunning = true;

	//Start thread with reference to this class so that it can store spikes
	int res = pthread_create(&clientThread, NULL, startThreadFunction, (void*)this);
	if(res != 0){
		SpikeStreamSimulation::systemError("UDPSynchronizedClient: ERROR CREATING CLIENT THREAD");
	}
}


/*! Stops the thread from running. */
void UDPSynchronizedClient::stop(){
	threadRunning = false;
}


/*! Used by external applications to unlock the thread's mutex. */
void UDPSynchronizedClient::unlockMutex(){
	pthread_mutex_unlock( &threadMutex );
}


