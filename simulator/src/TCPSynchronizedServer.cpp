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
#include "TCPSynchronizedServer.h"
#include "SpikeStreamSimulation.h"
#include "DeviceMessages.h"

/* Thread includes. Note that this needs to come after the 
	SpikeStream includes to avoid strange error about int and braces */
#include <pthread.h>


/*! Function that is called in a separate thread to connect to the socket. */
void* startTCPServerConnectThread(void* tmpTcpSyncServer){
	//Connect to the external device.
	TCPSynchronizedServer* tcpSyncServer = (TCPSynchronizedServer*) tmpTcpSyncServer;//Local copy of reference to class for clarity
    if (connect(tcpSyncServer->socketHandle, (struct sockaddr *) &tcpSyncServer->socketAddress, sizeof(tcpSyncServer->socketAddress)) < 0){
        SpikeStreamSimulation::systemError("TCPSynchronizedServer: CONNECTION TO SERVER FAILED");
	}
	else{//Successful connection to socket
		tcpSyncServer->socketConnected = true;
	}
}


/*! Constructor. */
TCPSynchronizedServer::TCPSynchronizedServer(unsigned int neurGrpWidth){
	socketOpen = false;
	neuronGrpWidth = neurGrpWidth;
}


/*! Destructor. */
TCPSynchronizedServer::~TCPSynchronizedServer(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING TCP SYNCHRONIZED SERVER"<<endl;
	#endif//MEMORY_DEBUG

	//Close socket
	closeDevice();
}


//-------------------------------------------------------------------------
//------------------------- PUBLIC METHODS --------------------------------
//-------------------------------------------------------------------------

/*! Closes the socket. */
bool TCPSynchronizedServer::closeDevice(){
	if(socketOpen){
		close(socketHandle);
		socketOpen = false;
	}
	return true;
}


/*! Opens the socket
	Adapted from: http://ntrg.cs.tcd.ie/undergrad/4ba2/multicast/antony/index.html. */
bool TCPSynchronizedServer::openSocket(string ipAddress, int port){
    /* Create a reliable, stream socket using TCP */
	socketHandle = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketHandle < 0){
		SpikeStreamSimulation::systemError("TCPSynchronizedServer: ERROR CREATING SOCKET");
		return false;
	}

    /* Construct the address structure */
    memset(&socketAddress, 0, sizeof(socketAddress));     /* Zero out structure */
    socketAddress.sin_family      = AF_INET;             /* Internet address family */
    socketAddress.sin_addr.s_addr = inet_addr(ipAddress.data());   /* Server IP address */
    socketAddress.sin_port        = htons(port); /* Server port */


    /* Establish the connection to the server. Do this as a separate thread so it can be timed out.*/
	//Socket connected is used to check whether we have the connection established
	socketConnected = false;

	//Declare thread to make connection
	pthread_t connectThread;
	int res = pthread_create(&connectThread, NULL, startTCPServerConnectThread, (void*)this);
	if(res != 0){
		SpikeStreamSimulation::systemError("TCPSynchronizedServer: ERROR CREATING CONNECTION THREAD");
		return false;
	}

	//Wait until connection has been established or until we have timed out
	int timeoutCount = 0;
	while(!socketConnected){

		//Sleep while waiting for thread to connect to socket.
		sleep(1);

		//Check to see if we have been sleeping for too long
		++timeoutCount;
		if(timeoutCount >= 5){
			//No point in waiting for the thread to rejoin since it is locked. Just display error and return false.
			SpikeStreamSimulation::systemError("TCPSynchronizedServer: Timeout while waiting for thread to connect to device");
			return false;
		}
	}

	//Socket has opened successfully
	socketOpen = true;
	return true;
}


/*! Sends spike data to the receiving socket. */
bool TCPSynchronizedServer::sendSpikeData(){
	/* Check that socket is still open. An error in the send will set socketOpen
		to false without attempting to close the socket */
	if(!socketOpen)
		return false;

	//Create array to hold the spikes. Each spike takes four bytes.
	int bufferSize = (neuronVectorPtr->size() * 4) + 4;//Four bytes per event + 4 bytes to record number of spikes in packet
	unsigned char charBuffer[ bufferSize ];//Buffer to be sent

	//Add the first two bytes containing the number of spikes in the message
	unsigned int numberOfSpikes = neuronVectorPtr->size();

	//Add the number of spikes in the message
	charBuffer[0] = (unsigned char)numberOfSpikes;//0 is the LSB
	numberOfSpikes >>= 8;
	charBuffer[1] = (unsigned char)numberOfSpikes;//1 is the next significant bit
	numberOfSpikes >>= 8;
	charBuffer[2] = (unsigned char)numberOfSpikes;//2 is the next significant bit
	numberOfSpikes >>= 8;
	charBuffer[3] = (unsigned char)numberOfSpikes;//3 is the most significant bit

	//Work through vector of firing neurons and fill buffer for sending over network
	unsigned int xPos, yPos;
	unsigned int bufferCounter = 4;
	for(vector<unsigned int>::iterator iter = neuronVectorPtr->begin(); iter != neuronVectorPtr->end(); ++iter){

		/* Sort out x position. The neuron vector contains the full 32 bit neuron id,
			so need to subtract the start position of the neuron group and turn it into
			a coordinate. */
		xPos = (*iter - startNeuronID) % neuronGrpWidth;
		if(xPos > 255){
			SpikeStreamSimulation::systemError("TCPSynchronizedServer: X POSITION OUT OF RANGE: ", xPos);
			//Leave socket open since this is ok, but return false because of data error
			return false;
		}
		charBuffer[bufferCounter] = (unsigned char) xPos;
		++bufferCounter;

		/* Sort out y position. The neuron vector contains the full 32 bit neuron id,
			so need to subtract the start position of the neuron group and turn it into
			a coordinate. */
		yPos = (*iter - startNeuronID) / neuronGrpWidth;
		if(yPos > 255){
			SpikeStreamSimulation::systemError("TCPSynchronizedServer: Y POSITION OUT OF RANGE: ", yPos);
			//Leave socket open since this is ok, but return false because of data error
			return false;
		}
		charBuffer[bufferCounter] = (unsigned char) yPos;
		++bufferCounter;

		//FIXME Add time, for the moment, this is not used, so add zeros here
		for(int i=0; i<2; ++i){
			charBuffer[bufferCounter] = 11;
			++bufferCounter;
		}
	}

	//Print buffer if require
	#ifdef PRINT_SEND_DEVICE_DATA_MSG
		cout<<"---------------------------------------------------------------------------"<<endl;
		for(int i=0; i<bufferSize; i += 4){
			printf("SEND BUFFER: %d, %d, %d, %d\n", charBuffer[i], charBuffer[i+1], charBuffer[i+2], charBuffer[i+3]);
		}
		cout<<"---------------------------------------------------------------------------"<<endl;
		cout.flush();
	#endif//PRINT_SEND_DEVICE_DATA_MSG

	//Send message to device
	#ifdef SEND_DEVICE_DATA_DEBUG
		cout<<"TCPSynchronizedServer: Sending message size = "<<bufferSize<<endl;
	#endif//SEND_DEVICE_DATA_DEBUG

	int sendResult = send(socketHandle, charBuffer, bufferSize, 0);
	if (sendResult != bufferSize){
		SpikeStreamSimulation::systemError("TCPSynchronizedServer: ERROR SENDING MESSAGE WITH SPIKE DATA. INCORRECT NUMBER OF BYTES SENT");
		socketOpen = false;
		return false;
	}

	#ifdef SEND_DEVICE_DATA_DEBUG
		cout<<"TCPSynchronizedServer: Data message sent to external device"<<endl;
	#endif//SEND_DEVICE_DATA_DEBUG

	//Wait for acknowledgement of 1 byte
	unsigned char receiveAckArray[1];
	receiveAckArray[0] = 0;
    int bytesReceived = 0;
	bytesReceived = recv(socketHandle, receiveAckArray, 1, 0);
	if(bytesReceived != 1){
		SpikeStreamSimulation::systemError("TCPSynchronizedClient: ERROR WHEN RECEIVNG DEVICE_DATA_ACK_MSG MESSAGE");
		socketOpen = false;
		return false;
	}

	//Check message is correct
	if(receiveAckArray[0] != DEVICE_DATA_ACK_MSG){
		SpikeStreamSimulation::systemError("TCPSynchronizedClient: INCORRECT DATA ACKNOWLEDGEMENT MESSAGE RECEIVED", (unsigned int)receiveAckArray[0]);
		//Not an error in the socket, but the data received so leave socket open, but return false to flag error
		return false;
	}
	else{
		#ifdef SEND_DEVICE_DATA_DEBUG
			cout<<"TCPSynchronizedServer: Data acknowlegement message received"<<endl;
		#endif//SEND_DEVICE_DATA_DEBUG
	}

	//If we have reached this point everything should be ok
	return true;
}


/*! Passes a reference to the vector of firing neurons held in SpikeStreamSimulation. */
void TCPSynchronizedServer::setNeuronVector(vector<unsigned int> *neurVectPtr, unsigned int startNeurID){
	neuronVectorPtr = neurVectPtr;
	startNeuronID = startNeurID;
}


