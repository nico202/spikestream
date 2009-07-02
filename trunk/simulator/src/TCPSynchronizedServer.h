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

#ifndef TCPSYNCHRONIZEDSERVER_H
#define TCPSYNCHRONIZEDSERVER_H

//SpikeStream includes
#include "DBInterface.h"

//Other includes
#include <arpa/inet.h>


//---------------------- TCP Synchronized Server ---------------------------
/*! Sends spikes out to external device using TCP synchronization method.
	Does not technically operate as a server, but called such because it is
	a provider of spikes to the external device, rather than a receiver. */
//--------------------------------------------------------------------------

class TCPSynchronizedServer {

	public:
		TCPSynchronizedServer(unsigned int neurGrpWidth);
		~TCPSynchronizedServer();
		bool closeDevice();
		bool openSocket(string ipAddress, int port);
		bool sendSpikeData();
		void setNeuronVector(vector<unsigned int> *neurVectPtr, unsigned int startNeurID);


		//=============================== VARIABLES ==============================
		/*! Integer used to access the socket.*/
		int socketHandle;

		/*! Address of the socket.*/
		struct sockaddr_in socketAddress;

		/*! Records whether socket has been successfully connected.*/
		bool socketConnected;


	private:
		//=============================== VARIABLES ==============================
		/*! Neuron group width.*/
		unsigned int neuronGrpWidth;

		/*! First ID of neuron group.*/
		unsigned int startNeuronID;

		/*! Records whether socket is open.*/
		bool socketOpen;

		/*! Reference to the vector of currently firing neurons held in 
			SpikeStreamSimulation.*/
		vector<unsigned int> *neuronVectorPtr;


		//=============================== METHODS =================================
		/*! Declare copy constructor private so it cannot be used inadvertently. */
		TCPSynchronizedServer (const TCPSynchronizedServer&);

		/*! Declare assignment private so it cannot be used. */
		TCPSynchronizedServer operator = (const TCPSynchronizedServer&);

};


#endif//TCPSYNCHRONIZEDSERVER_H


