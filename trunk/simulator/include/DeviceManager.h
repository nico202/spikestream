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

#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

//SpikeStream includes
#include "Neuron.h"
#include "DBInterface.h"
#include "UDPSynchronizedClient.h"
#include "UDPSynchronizedServer.h"
#include "TCPSynchronizedClient.h"
#include "TCPSynchronizedServer.h"

//Other includes
#include <google/dense_hash_map> 
using HASH_NAMESPACE::hash;
using GOOGLE_NAMESPACE::dense_hash_map;


//----------------------------- Device Manager -----------------------------
/*! Used to get input from an external device, such as SIMNOS or CRONOS, or 
	to send output to an external device and update the neurons' firing
	state appropriately. */
//--------------------------------------------------------------------------

class DeviceManager {

	public:
		DeviceManager(unsigned int devID, unsigned int neurGrpID, double devFiringMode, DBInterface* deviceDB, DBInterface *networkDB);
		DeviceManager();
		~DeviceManager();
		bool closeDevice();
		bool fetchData();
		unsigned int getDeviceType();
		unsigned int getExternalComputeTime_us();
		bool getExternalSyncDelay();
		bool isInputDevice();
		bool isOutputDevice();
		void setNeuronArray(Neuron **neurArr);
		void setNeuronUpdateMap(dense_hash_map<unsigned int, bool, hash<unsigned int> >* neurUdtMp);
		void setNeuronVector(vector<unsigned int> *neurVectPtr, unsigned int startNeuronID);
		bool updateDevice();
		void updateNeurons();


	private:
		//=========================== VARIABLES =============================
		//References to the database interfaces
		DBInterface *deviceDBInterface;
		DBInterface *networkDBInterface;

		/*! Device ID.*/
		unsigned int deviceID;

		/*! Device type.*/
		unsigned int deviceType;

		/*! Is the device open or not.*/
		bool deviceOpen;

		//Information about the neuron group
		unsigned int neuronGrpWidth;
		unsigned int neuronGrpLength;
		unsigned int startNeuronID;
		unsigned int neuronGrpID;

		/*! Reference to the neuron array in SpikeStreamSimulation.*/
		Neuron **neuronArray;

		/*! Pointer to the map holding all the neurons that have received a spike or
			been changed during this timestep. Neurons in this map are updated when 
			all spikes have been received. Map is used rather than vector to avoid 
			filtering out multiple spikes to the same neuron. The key is the full 
			neuronID.*/
		dense_hash_map<unsigned int, bool, hash<unsigned int> >* neuronUpdateMap;

		/*! Is the pattern rotated wrt the neuron group or not?.*/
		//FIXME NOT IMPLEMENTED YET.
		bool rotatePattern;

		/*! Holds the neurons that need to be fired at each time point.*/
		dense_hash_map<unsigned int, bool, hash<unsigned int> > deviceBuffer[NUMBER_OF_DELAY_VALUES];

		/*! Records the current active position in the device buffer.*/
		unsigned int bufferCounter;

		//Clients and servers to send and receive messages over Ethernet
		UDPSynchronizedClient* udpSyncClient;
		UDPSynchronizedServer* udpSyncServer;
		TCPSynchronizedClient* tcpSyncClient;
		TCPSynchronizedServer* tcpSyncServer;

		/*! Is the input pattern from an external device used to directly
			fire the neurons or not?*/
		bool directFiringMode;

		/*! If we are in synaptic firing mode (i.e. !directFiringMode), this variable
			specifies the weight.*/
		double synapticWeight;


		//=========================== METHODS =================================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		DeviceManager (const DeviceManager&);

		/*! Declare assignment private so it cannot be used.*/
		DeviceManager operator = (const DeviceManager&);

		void clearSynchronizationDelay();
		void fillDeviceBuffer();
		void loadDeviceInformation();

};


#endif //DEVICEMANAGER_H


