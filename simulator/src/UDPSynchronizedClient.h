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

#ifndef UDPSYNCHRONIZEDCLIENT_H
#define UDPSYNCHRONIZEDCLIENT_H

//Other includes
#include <arpa/inet.h>
#include <string>
using namespace std;


//---------------------- UDP Synchronized Client ---------------------------
/*! Thread that listens for incoming UDP messages and calculates the update
	time between them to enable the simulation to change its update rate
	to match that of the external device. */
//--------------------------------------------------------------------------

class UDPSynchronizedClient {

	public:
		UDPSynchronizedClient(unsigned int neurGrpWidth, unsigned int neurGrpLength);
		~UDPSynchronizedClient();
		bool clientThreadRunning();
		bool closeDevice();
		bool deviceOpen();
		unsigned int getExternalComputeTime_us();
		bool getExternalSyncDelay();
		void lockMutex();
		bool openSocket(string groupAddress, int port);
		void run();//Public so it can be started by thread
		void start();
		void stop();
		void unlockMutex();


		//========================== VARIABLES ==============================
		/*! Number of spikes in the spike buffer.*/
		unsigned int spikeCount;

		/*! Buffer holding the spikes that have been received by this class from
			the external device.*/
		unsigned int* spikeBuffer;


	private:
		//========================== VARIABLES ==============================
		/*! Neuron group width.*/
		unsigned int neuronGrpWidth;
		
		/*! The first ID in the neuron group.*/
		unsigned int neuronGrpLength;

		/*! Controls whether the thread is running.*/
		bool threadRunning;

		/*! Records whether the socket has been opened.*/
		bool socketOpen;

		/*! Thread to listen for incoming packets from broadcast group.*/
		pthread_t clientThread;

		/*! External device is delaying its time steps.*/
		bool externalSyncDelay;

		/*! Compute time of external device.*/
		unsigned int externalComputeTime_us;

		/*! Counter to record the number of time steps that have taken place 
			in the absence of input from the external process.*/
		unsigned int timeStepsNoMessagesCount;

		/*! Integer used to access the socket.*/
		int socketHandle;

		/*! Address of the socket.*/
		struct sockaddr_in socketAddress;

		/*! Buffer used to receive messages from the network.*/
		unsigned char* messageBuffer;


		//=========================== METHODS ===============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		UDPSynchronizedClient (const UDPSynchronizedClient&);

		/*! Declare assignment private so it cannot be used.*/
		UDPSynchronizedClient operator = (const UDPSynchronizedClient&);

};


#endif//UDPSYNCHRONIZEDCLIENT_H





