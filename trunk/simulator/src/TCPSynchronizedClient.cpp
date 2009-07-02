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
#include "TCPSynchronizedClient.h"
#include "SpikeStreamSimulation.h"
#include "DeviceMessages.h"
#include "PerformanceTimer.h"

/* Thread includes. Note that this needs to come after the 
	SpikeStream includes to avoid strange error about int and braces */
#include <pthread.h>


/*! Size of the array used to store received spikes.
	This has enough space for more than 5000 spikes, each of which takes 
	12 bytes to store. 		return false;*/
#define SPIKE_BUFFER_SIZE 60000


/*! Size of the receive buffer used to unpack spikes from the network. */
#define MESSAGE_BUFFER_SIZE 1048576


/*! Function that is called in a separate thread to connect to the socket. */
void* startTCPClientConnectThread(void* tmpTcpSyncClient){
	//Connect to the external device.
	TCPSynchronizedClient* tcpSyncClient = (TCPSynchronizedClient*) tmpTcpSyncClient;//Local copy of reference to class for clarity
    if (connect(tcpSyncClient->socketHandle, (struct sockaddr *) &tcpSyncClient->externalDevAddr, sizeof(tcpSyncClient->externalDevAddr)) < 0){
        SpikeStreamSimulation::systemError("TCPSynchronizedClient: CONNECTION TO SERVER FAILED");
	}
	else{//Successful connection to socket
		tcpSyncClient->socketConnected = true;
	}
}


/*! Constructor. */
TCPSynchronizedClient::TCPSynchronizedClient(unsigned int neurGrpWidth, unsigned int neurGrpLength){
	//Store details about neuron group
	neuronGrpWidth = neurGrpWidth;
	neuronGrpLength = neurGrpLength;
	
	//Initialise variables
	socketOpen = false;
	twoByteCoordinates = false;

	//Set up array to hold spikes
	spikeCount = 0;
	spikeBuffer = new unsigned int[SPIKE_BUFFER_SIZE];

	//Set up message buffer
	msgBuffer = new unsigned char[MESSAGE_BUFFER_SIZE];

	//Set up array holding confirmation that device data has been received
	requestDataMsgArray[0] = SPIKESTREAM_DATA_ACK_MSG;
}


/*! Destructor. */
TCPSynchronizedClient::~TCPSynchronizedClient(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING TCP SYNCHRONIZATION CLIENT"<<endl;
	#endif//MEMORY_DEBUG

	//Close socket
	closeDevice();

	//Free memory for thread spike buffer
	delete [] spikeBuffer;
	delete [] msgBuffer;
}


//---------------------------------------------------------------------------
//----------------------------- PUBLIC METHODS ------------------------------
//---------------------------------------------------------------------------

/*! Closes the socket. */
bool TCPSynchronizedClient::closeDevice(){
	if(socketOpen){
		close(socketHandle);
		socketOpen = false;
	}
	return true;
}


/*! Returns true if the socket is open. */
bool TCPSynchronizedClient::deviceOpen(){
	return socketOpen;
}


/*! Waits for a message containing data from the external device and unpacks the data. */
bool TCPSynchronizedClient::fetchData(){
	/* Check that socket is still open. An error in the receive will set socketOpen
		to false without attempting to close the socket */
	if(!socketOpen)
		return false;

	//Initialise spike count
	spikeCount = 0;

	#ifdef RECEIVE_DEVICE_DATA_DEBUG
		cout<<"TCPSynchronizedClient: Waiting for data ..."<<endl;
	#endif//RECEIVE_DEVICE_DATA_DEBUG

    /* Receive the data back from the server */
	//Receive the first four bytes specifying the number of spikes in the message
	//Receive these 1 at a time since am not sure if they will all come at once
	unsigned char messageSizeArray[1];
	unsigned int numberOfMsgSpikes = 0;
    unsigned int totalBytesReceived = 0, bytesReceived = 0;
    while (totalBytesReceived < 4){

        /* Receive up to the buffer size (minus 1 to leave space for
           a null terminator) bytes from the sender */
        if ((bytesReceived = recv(socketHandle, messageSizeArray, 1, 0)) <= 0){
			SpikeStreamSimulation::systemError("TCPSynchronizedClient: ERROR RECEIVNG SIZE OF MESSAGE");
			socketOpen = false;
			return false;
		}

 		//Assemble the count of the number of message spikes in the message
		unsigned int tmpPartMsgSpikeCount = messageSizeArray[0];
		numberOfMsgSpikes += tmpPartMsgSpikeCount << 8*totalBytesReceived;//Shift to correct position in the number

	// Keep track of total bytes 
      	totalBytesReceived += bytesReceived;   
    }

	#ifdef RECEIVE_DEVICE_DATA_DEBUG
		cout<<"TCPSynchronizedClient: Data received. Number of spikes in message = "<<numberOfMsgSpikes<<endl;
	#endif//RECEIVE_DEVICE_DATA_DEBUG

	/* Now receive the rest of the message
		Each receive call may contain all of the message or only part of it */
	unsigned int msgBufferSize = 4 * numberOfMsgSpikes;
	unsigned char *msgBufferPtr = msgBuffer;
	totalBytesReceived = 0;
	bytesReceived = 0;
    while (totalBytesReceived < msgBufferSize ){
		//Receive all or part of the spike data
		if ((bytesReceived = recv(socketHandle, msgBufferPtr, msgBufferSize - totalBytesReceived, 0)) <= 0){
			SpikeStreamSimulation::systemError("TCPSynchronizedClient: ERROR WHEN RECEIVING MAIN MESSAGE DATA");
			socketOpen = false;
			return false;
		}

		//Add up number of bytes received
		totalBytesReceived += bytesReceived;

		//Increase the pointer to the message buffer array so that it points to empty part of it
		msgBufferPtr += bytesReceived;
	}

	//Print out the message buffer for debug
	#ifdef PRINT_RECEIVE_DEVICE_DATA_MSG
		printMessageBuffer(msgBuffer, msgBufferSize);
	#endif//PRINT_RECEIVE_DEVICE_DATA_MSG

	//Unpack spikes
	for(unsigned int i=0; i < msgBufferSize; i += 4){
		if(twoByteCoordinates){//The X and Y positions of each spike are encoded by two bytes.
			//Stuff for two byte conversion
			unsigned int xVal = msgBuffer[i+1] & 255;
			xVal <<= 8;
			xVal += msgBuffer[i] & 255;
	
			unsigned int yVal = msgBuffer[i+3] & 255;
			yVal <<= 8;
			yVal += msgBuffer[i + 2] & 255;

			//Check X and Y are within the range of this neuron group
			if(xVal > neuronGrpWidth){
				SpikeStreamSimulation::systemError("TCPSynchronizedClient: X POSITION OUT OF RANGE ", xVal);
				//Leave socket open since this is ok, but return false because of data error
				return false;
			}
			if(yVal > neuronGrpLength){
				SpikeStreamSimulation::systemError("TCPSynchronizedClient: Y POSITION OUT OF RANGE ", yVal);
				//Leave socket open since this is ok, but return false because of data error
				return false;
			}
	
			//Store X and Y position in spike buffer
			spikeBuffer[spikeCount * 3] =  xVal;//X position
			spikeBuffer[spikeCount * 3 + 1] = yVal; // Y position
	
			//Set delay even if we are not using it.
			spikeBuffer[spikeCount * 3 + 2] = 0;
		}
		else{//The X and Y positions of each spike are encoded by a single byte
			//Check X and Y are within the range of this neuron group
			if(msgBuffer[i] > neuronGrpWidth){
				SpikeStreamSimulation::systemError("TCPSynchronizedClient: X POSITION OUT OF RANGE ", msgBuffer[i]);
				//Leave socket open since this is ok, but return false because of data error
				return false;
			}
			if(msgBuffer[i+1] > neuronGrpLength){
				SpikeStreamSimulation::systemError("TCPSynchronizedClient: Y POSITION OUT OF RANGE ", msgBuffer[i+1]);
				//Leave socket open since this is ok, but return false because of data error
				return false;
			}

			//Store X and Y position in spike buffer
			spikeBuffer[spikeCount * 3] =  msgBuffer[i];//X position
			spikeBuffer[spikeCount * 3 + 1] = msgBuffer[i+1]; // Y position

			/*Next two bytes are the timestamp.
				This is in units of 0.01 ms
				MSB is second position	*/
			unsigned int time = msgBuffer[i+3];//MSB
			time <<= 8;
			time += msgBuffer[i+2];//LSB
	
			//FIXME COME UP WITH SOMETHING BETTER HERE LINKED TO SIMULATION TIME OR SOMETHING
			unsigned int delay = time / 10;//Convert to 0.1 millisec sections
			if(delay > NUMBER_OF_DELAY_VALUES)
				spikeBuffer[spikeCount * 3 + 2] = 0;
			else
				spikeBuffer[spikeCount * 3 + 2] = delay;
		}

		//Increase the record of the number of spikes in buffer
		++spikeCount;
	}

	#ifdef RECEIVE_DEVICE_DATA_DEBUG
		cout<<"TCPSynchronizedClient: Spikes unpacked into SpikeBuffer"<<endl;
	#endif//RECEIVE_DEVICE_DATA_DEBUG


	/* Send a message requesting data to the device */
    if (send(socketHandle, requestDataMsgArray, 1, 0) != 1){
        SpikeStreamSimulation::systemError("TCPSynchronizedClient: SENDING OF REQUEST_DEVICE_DATA_MSG TO SERVER FAILED");
		socketOpen = false;
		return false;
	}

	//cout<<"-------------- DONE ----------------"<<endl;

	//If we have reached this point everything should be ok
	return true;
}


/*! Opens the TCP socket.
	The connection to the socket is handled by a separate thread so that a failed connection can be
	timed out.
	Adapated from: http://cs.baylor.edu/~donahoo/practical/CSockets/textcode.html. */
bool TCPSynchronizedClient::openSocket(string externalDeviceIPAddr, int externalDevicePort){
	cout<<"TCPSynchronizedClient: Connecting to "<<externalDeviceIPAddr<<" on port "<<externalDevicePort<<endl;

    /* Create a reliable, stream socket using TCP */
	socketHandle = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketHandle < 0){
        SpikeStreamSimulation::systemError("TCPSynchronizedClient: ERROR CREATING SOCKET");
		return false;
	}

    /* Construct the server address structure */
    memset(&externalDevAddr, 0, sizeof(externalDevAddr));     /* Zero out structure */
    externalDevAddr.sin_family      = AF_INET;             /* Internet address family */
    externalDevAddr.sin_addr.s_addr = inet_addr(externalDeviceIPAddr.data());   /* Server IP address */
    externalDevAddr.sin_port        = htons(externalDevicePort); /* Server port */


    /* Establish the connection to the server. Do this as a separate thread so it can be timed out.*/
	//Socket connected is used to check whether we have the connection established
	socketConnected = false;

	//Declare thread to make connection
	pthread_t connectThread;
	int res = pthread_create(&connectThread, NULL, startTCPClientConnectThread, (void*)this);
	if(res != 0){
		SpikeStreamSimulation::systemError("TCPSynchronizedClient: ERROR CREATING CONNECTION THREAD");
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
			SpikeStreamSimulation::systemError("TCPSynchronizedClient: Timeout while waiting for thread to connect to device");
			return false;
		}
	}


	//If we have got this far, socket is open
	socketOpen = true;
	return true;
}


/*! Vision data is potentially larger than 255, so need to use two bytes to encode
	the coordinates within a vision map, which replaces the timing information. */
void TCPSynchronizedClient::setTwoByteCoords(bool twoBtCds){
	twoByteCoordinates = twoBtCds;
}


//-------------------------------------------------------------------------------
//---------------------------- PRIVATE METHODS ----------------------------------
//-------------------------------------------------------------------------------

/*! Prints out the message buffer received from the external device
	for debugging. */
void TCPSynchronizedClient::printMessageBuffer(unsigned char* msgBuffer, unsigned int msgBufSize){
	cout<<"-------------------------- START MESSAGE BUFFER ----------------------------"<<endl;
	for(unsigned int i=0; i<msgBufSize; i+=4)
		printf("%d, %d, %d, %d\n", msgBuffer[i], msgBuffer[i+1], msgBuffer[i+2], msgBuffer[i+3]);
	cout<<"------------------------ END MESSAGE BUFFER ----------------------------"<<endl<<endl;
}


