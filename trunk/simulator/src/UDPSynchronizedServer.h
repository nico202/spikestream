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

#ifndef UDPSYNCHRONIZEDSERVER_H
#define UDPSYNCHRONIZEDSERVER_H

//SpikeStream includes
#include "DBInterface.h"

//Other includes
#include <arpa/inet.h>


//--------------------- UDP Synchronized Server ---------------------------
/*! Sends spikes out to external device using UDP synchronized method
	Does not technically operate as a server, but called such because it is
	a provider of spikes to the external device, rather than a receiver. */
//-------------------------------------------------------------------------

class UDPSynchronizedServer {

	public:
		UDPSynchronizedServer(DBInterface* devDBInter, unsigned int neurGrpWidth);
		~UDPSynchronizedServer();
		bool closeDevice();
		bool openSocket(string addr, int pt);
		bool sendSpikeData();
		void setNeuronVector(vector<unsigned int> *neurVectPtr, unsigned int startNeurID);


	private:
		//============================ VARIABLES ==============================
		/*! Reference to database handling class. */
		DBInterface* deviceDBInterface;

		/*! Integer used to access the socket. */
		int socketHandle;

		/*! Address of the socket. */
		struct sockaddr_in socketAddress;

		/*! Records whether socket is open. */
		bool socketOpen;

		/*! Width neuron group. */
		unsigned int neuronGrpWidth;

		/*! First ID in neuron group. */
		unsigned int startNeuronID;

		/*! Vector of currently firing neurons. */
		vector<unsigned int> *neuronVectorPtr;


		//============================ METHODS ================================
		/*! Declare copy constructor private so it cannot be used inadvertently. */
		UDPSynchronizedServer (const UDPSynchronizedServer&);

		/*! Declare assignment private so it cannot be used. */
		UDPSynchronizedServer operator = (const UDPSynchronizedServer&);

		bool simulationSynchronizationDelay();

};


#endif//UDPSYNCHRONIZEDSERVER_H


