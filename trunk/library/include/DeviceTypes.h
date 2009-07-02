/***************************************************************************
 *   SpikeStream Library                                                   *
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

//-------------------------- Device Types ----------------------------------
/*! Holds defines and descriptions for the different devices that can be
	plugged into the neural networks.

	NOTE Input devices should always be odd numbers; output devices should 
	always be even numbers. */
//--------------------------------------------------------------------------

#ifndef DEVICETYPES_H
#define DEVICETYPES_H

//Other includes
#include <string>
using namespace std;


/*! Unknown device.*/
#define UNKNOWN_DEVICE_VALUE 0

/*! Unknown device description.*/
#define UNKNOWN_DEVICE_DESC "Unknown device"


/*! Unsynchronized network input device using UDP.*/
#define ASYNC_UDP_NET_IN_DEVICE_VALUE 1

/*! Unsynchronized UDP network input device description.*/
#define ASYNC_UDP_NET_IN_DEVICE_DESC "Asynchronous UDP network input"

/*! Unsynchronized UDP network output device using UDP.*/
#define ASYNC_UDP_NET_OUT_DEVICE_VALUE 2

/*! Unsynchronized UDP network output device description.*/
#define ASYNC_UDP_NET_OUT_DEVICE_DESC "Asynchronous UDP network output"


/*! Synchronized network input device using UDP.*/
#define SYNC_UDP_NET_IN_DEVICE_VALUE 3

/*! Synchronized UDP network input device description.*/
#define SYNC_UDP_NET_IN_DEVICE_DESC "Synchronized UDP network input"

/*! Synchronized network output device using UDP.*/
#define SYNC_UDP_NET_OUT_DEVICE_VALUE 4

/*! Synchronized UDP network output device description.*/
#define SYNC_UDP_NET_OUT_DEVICE_DESC "Synchronized UDP network output"


/*! Synchronized network input device using TCP.*/
#define SYNC_TCP_NET_IN_DEVICE_VALUE 5

/*! Synchronized TCP network input device description.*/
#define SYNC_TCP_NET_IN_DEVICE_DESC "Synchronized TCP network input"

/*! Synchronized network output device using TCP.*/
#define SYNC_TCP_NET_OUT_DEVICE_VALUE 6

/*! Synchronized TCP network output device description.*/
#define SYNC_TCP_NET_OUT_DEVICE_DESC "Synchronized TCP network output"


/*! Synchronized network input device carrying visual information over TCP
	This needs to be a different device because vision needs two bytes to 
	encode its position. */
#define SYNC_TCP_NET_VISION_IN_DEVICE_VALUE 7

/*! Description of synchronized network input device carrying visual 
	information over TCP. This needs to be a different device because 
	vision needs two bytes to encode its position. */
#define SYNC_TCP_NET_VISION_IN_DEVICE_DESC "Synchronized TCP network vision input"


class DeviceTypes {

	public:
		//========================== VARIABLES ==============================
		/*! Unsynchronized UDP input device.*/
		static const unsigned short aSyncUDPNetworkInput = ASYNC_UDP_NET_IN_DEVICE_VALUE;

		/*! Unsynchronized UDP output device.*/
		static const unsigned short aSyncUDPNetworkOutput = ASYNC_UDP_NET_OUT_DEVICE_VALUE;

		/*! Synchronized UDP input device.*/
		static const unsigned short syncUDPNetworkInput = SYNC_UDP_NET_IN_DEVICE_VALUE;

		/*! Synchronized UDP output device.*/
		static const unsigned short syncUDPNetworkOutput = SYNC_UDP_NET_OUT_DEVICE_VALUE;

		/*! Synchronized TCP input device.*/
		static const unsigned short syncTCPNetworkInput = SYNC_TCP_NET_IN_DEVICE_VALUE;

		/*! Synchronized TCP output device.*/
		static const unsigned short syncTCPNetworkOutput = SYNC_TCP_NET_OUT_DEVICE_VALUE;

		/*! Synchronized TCP input device.*/
		static const unsigned short syncTCPNetworkVisionInput = SYNC_TCP_NET_VISION_IN_DEVICE_VALUE;


		//=========================== METHODS ===============================
		static char* getDescription(unsigned short type);
		static unsigned short getType(string description);
		static bool isInputDevice(unsigned int type);
		static bool isOutputDevice(unsigned int type);

};


#endif //DEVICETYPES_H

