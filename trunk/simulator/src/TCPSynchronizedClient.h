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

#ifndef TCPSYNCHRONIZEDCLIENT_H
#define TCPSYNCHRONIZEDCLIENT_H

#include "PerformanceTimer.h"

//Other includes
#include <arpa/inet.h>
#include <string>
using namespace std;


//-------------------- TCP Synchronized Client ----------------------------
/*! Client that requests data from an external device using TCP
	This client blocks until it receives the data. */

/* FIXME BLOCKING RECEIVE THROWS AN EXCEPTION WHEN OTHER SOCKET IS CLOSED
	NEED TO TEST THIS MORE AND SHUT DOWN CLEANLY
	FIXME MAKE OPEN SOCKET RUN AS A SEPARATE THREAD TO AVOID BLOCKING. */
//-------------------------------------------------------------------------

class TCPSynchronizedClient {

	public:
		TCPSynchronizedClient(unsigned int neurGrpWidth, unsigned int neurGrpLength);
		~TCPSynchronizedClient();
		bool closeDevice();
		bool deviceOpen();
		bool fetchData();
		bool openSocket(string groupAddress, int port);
		void setTwoByteCoords(bool);


		//========================== VARIABLES ==============================
		/*! Holds the number of spikes in the spike buffer.*/
		unsigned int spikeCount;

		/*! Holds the spikes that have been received from the network.*/
		unsigned int* spikeBuffer;
	
		/*! Integer used to access the socket.*/
		int socketHandle;

		/*! Address of the socket.*/
		struct sockaddr_in externalDevAddr;

		/*! Records whether socket has been successfully connected.*/
		bool socketConnected;

	private:
		//========================== VARIABLES ==============================
		/*! Neuron group width.*/
		unsigned int neuronGrpWidth;

		/*! Neuron group length.*/
		unsigned int neuronGrpLength;

		/*! Buffer to unpack message into.*/
		unsigned char *msgBuffer;

		/*! Records whether the socket has been opened.*/
		bool socketOpen;

		/*! Array holding the request for data from the device. This is sent
			to the device to request the data. */
		unsigned char requestDataMsgArray[1];

		/*! Vision coordinates are too big to fit in a single byte. Set this
			to true when it is a vision connection. */
		bool twoByteCoordinates;


		//=========================== METHODS ===============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		TCPSynchronizedClient (const TCPSynchronizedClient&);

		/*! Declare assignment private so it cannot be used.*/
		TCPSynchronizedClient operator = (const TCPSynchronizedClient&);

		void printMessageBuffer(unsigned char* msgBuf, unsigned int msgBufSize);

};


#endif//TCPSYNCHRONIZEDCLIENT_H

