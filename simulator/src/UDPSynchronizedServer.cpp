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
#include "UDPSynchronizedServer.h"
#include "SpikeStreamSimulation.h"

//Other includes
#include <iostream>
#include <mysql++.h>
using namespace std;
using namespace mysqlpp;


/*! Step counter is a 15 bit number whose max value is defined here. */
#define STEP_COUNTER_MAX 32768;


/*! Constructor. */
UDPSynchronizedServer::UDPSynchronizedServer(DBInterface* dbInter, unsigned int neurGrpWidth){
	//Store reference to database handling class
	deviceDBInterface = dbInter;

	//Store details about neuron group
	neuronGrpWidth = neurGrpWidth;

	//Socket starts closed
	socketOpen = false;
}


/*! Destructor. */
UDPSynchronizedServer::~UDPSynchronizedServer(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING UDP SYNCHRONIZED SERVER"<<endl;
	#endif//MEMORY_DEBUG

	//Close socket
	closeDevice();
}


//--------------------------------------------------------------------------
//--------------------------- PUBLIC METHODS -------------------------------
//--------------------------------------------------------------------------

/*! Closes the socket. */
bool UDPSynchronizedServer::closeDevice(){
	close(socketHandle);
	socketOpen = false;
	return true;
}


/*! Opens the UDP socket. 
	Adapted from: http://ntrg.cs.tcd.ie/undergrad/4ba2/multicast/antony/index.html. */
bool UDPSynchronizedServer::openSocket(string address, int port){
	/* Create what looks like an ordinary UDP socket */
	socketHandle=socket(AF_INET, SOCK_DGRAM, 0);
	if (socketHandle < 0) {
		SpikeStreamSimulation::systemError("DeviceManager: ERROR OPENING SOCKET");
		return false;
	}

     /* Set up destination address */
     memset(&socketAddress, 0, sizeof(socketAddress));
     socketAddress.sin_family=AF_INET;
     socketAddress.sin_addr.s_addr=inet_addr(address.data());
     socketAddress.sin_port=htons(port);

	//If we have reached this point,the socket is open
	socketOpen = true;
	return true;
}


/*! Sends spike data to the receiving socket. */
bool UDPSynchronizedServer::sendSpikeData(){
	/* Check that socket is still open. An error in the send will set socketOpen
		to false without attempting to close the socket */
	if(!socketOpen)
		return false;

	//Create array to hold the spikes. Each spike takes four bytes.
	int bufferSize = (neuronVectorPtr->size() * 4) + 2;//Four bytes per event + 2 bytes synchronization
	unsigned char charBuffer[ bufferSize ];

	//Add the first two bytes containing the timestep and delay flag
	unsigned short stepCounter = SpikeStreamSimulation::simulationClock->getTimeStep() % STEP_COUNTER_MAX;

	//Shift step counter left to leave space for the delay flag 
	stepCounter <<= 1;

	//Set delay flag to 1 if one of the layers in the simulation has delayed in the last time step
	if(simulationSynchronizationDelay()){
		stepCounter += 1;
	}

	//Pack the step counter into the array
	charBuffer[0] = (unsigned char)stepCounter;//0 is the LSB
	stepCounter >>= 8;
	charBuffer[1] = (unsigned char)stepCounter;//1 is the MSB

	//Work through vector of firing neurons and fill buffer for sending over network
	unsigned int xPos, yPos;
	unsigned int bufferCounter = 2;
	for(vector<unsigned int>::iterator iter = neuronVectorPtr->begin(); iter != neuronVectorPtr->end(); ++iter){

		/* Sort out x position. The neuron vector contains the full 32 bit neuron id,
			so need to subtract the start position of the neuron group and turn it into
			a coordinate. */
		xPos = (*iter - startNeuronID) % neuronGrpWidth;
		if(xPos > 255){
			SpikeStreamSimulation::systemError("UDPSynchronizedServer: X POSITION OUT OF RANGE: ", xPos);
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
			SpikeStreamSimulation::systemError("UDPSynchronizedServer: Y POSITION OUT OF RANGE: ", yPos);
			//Leave socket open since this is ok, but return false because of data error
			return false;
		}
		charBuffer[bufferCounter] = (unsigned char) yPos;
		++bufferCounter;

		//FIXME Add time, for the moment, this is not used, so add zeros here
		for(int i=0; i<2; ++i){
			charBuffer[bufferCounter] = 0;
			++bufferCounter;
		}
	}

	//Broadcast buffer over network
	#ifdef SEND_NETWORK_MSG_DEBUG
		cout<<"Sending message: size of = "<<sizeof(charBuffer)<<" buffer size = "<<bufferSize<<endl;
	#endif//SEND_NETWORK_MESSAGE_DEBUG

	int sendResult = sendto(socketHandle, charBuffer, bufferSize, 0, (struct sockaddr *) &socketAddress, sizeof(socketAddress));
	if (sendResult < 0) {
		SpikeStreamSimulation::systemError("DeviceManager: ERROR SENDING MESSAGE ");
		perror("sendto");
		socketOpen = false;
		return false;
	}

	//If we have reached this point, everything should be ok
	return true;
}


/*! Passes a reference to the neuron vector in SpikeStreamSimulation to this class.
	This contains the ids of the currently firing neurons. */
void UDPSynchronizedServer::setNeuronVector(vector<unsigned int> *neurVectPtr, unsigned int startNeurID){
	neuronVectorPtr = neurVectPtr;
	startNeuronID = startNeurID;
}


/*! Returns true if there is 1 or more entries in the synchronization delay database
	These entries indicate that another task is delaying itself to synchronize
	up with an external device. */
bool UDPSynchronizedServer::simulationSynchronizationDelay(){
	try{
		Query deviceQuery = deviceDBInterface->getQuery();
		deviceQuery.reset();
		deviceQuery<<"SELECT * FROM SynchronizationDelay";
                StoreQueryResult syncRes = deviceQuery.store();
		if(syncRes.size() > 0)
			return true;
		return false;
	}
	catch (const BadQuery& er) {// Handle any query errors
		ostringstream errorStrStream;
		errorStrStream<<"Bad query when checking for synchronization delay: \""<<er.what()<<"\"";
		SpikeStreamSimulation::systemError(errorStrStream.str());
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown checking for synchronization delay: \""<<er.what()<<"\"";
		SpikeStreamSimulation::systemError(errorStrStream.str());
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		ostringstream errorStrStream;
		errorStrStream<<"Exception thrown checking for synchronization delay: \""<<er.what()<<"\"";
		SpikeStreamSimulation::systemError(errorStrStream.str());
	}
}



