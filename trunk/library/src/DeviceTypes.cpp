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

//SpikeStream includes
#include "DeviceTypes.h"


//--------------------------------------------------------------------------
//------------------------ STATIC PUBLIC METHODS ---------------------------
//--------------------------------------------------------------------------

/*! Returns a description of the device type. */
char* DeviceTypes::getDescription(unsigned short deviceType){
	switch(deviceType){
		case ASYNC_UDP_NET_IN_DEVICE_VALUE: return ASYNC_UDP_NET_IN_DEVICE_DESC;
		case ASYNC_UDP_NET_OUT_DEVICE_VALUE: return ASYNC_UDP_NET_OUT_DEVICE_DESC;
		case SYNC_UDP_NET_IN_DEVICE_VALUE: return SYNC_UDP_NET_IN_DEVICE_DESC;
		case SYNC_UDP_NET_OUT_DEVICE_VALUE: return SYNC_UDP_NET_OUT_DEVICE_DESC;
		case SYNC_TCP_NET_IN_DEVICE_VALUE: return SYNC_TCP_NET_IN_DEVICE_DESC;
		case SYNC_TCP_NET_OUT_DEVICE_VALUE: return SYNC_TCP_NET_OUT_DEVICE_DESC;
		case SYNC_TCP_NET_VISION_IN_DEVICE_VALUE: return SYNC_TCP_NET_VISION_IN_DEVICE_DESC;
		default: return UNKNOWN_DEVICE_DESC;
	}
}


/*! Returns the device type that corresponds to a given description. */
unsigned short DeviceTypes::getType(string deviceType){
	if (deviceType == ASYNC_UDP_NET_IN_DEVICE_DESC)
		return ASYNC_UDP_NET_IN_DEVICE_VALUE;
	else if (deviceType == ASYNC_UDP_NET_OUT_DEVICE_DESC)
		return ASYNC_UDP_NET_OUT_DEVICE_VALUE;
	else if(deviceType == SYNC_UDP_NET_IN_DEVICE_DESC)
		return SYNC_UDP_NET_IN_DEVICE_VALUE;
	else if(deviceType == SYNC_UDP_NET_OUT_DEVICE_DESC)
		return SYNC_UDP_NET_OUT_DEVICE_VALUE;
	else if(deviceType == SYNC_TCP_NET_IN_DEVICE_DESC)
		return SYNC_TCP_NET_IN_DEVICE_VALUE;
	else if(deviceType == SYNC_TCP_NET_OUT_DEVICE_DESC)
		return SYNC_TCP_NET_OUT_DEVICE_VALUE;
	else if(deviceType == SYNC_TCP_NET_VISION_IN_DEVICE_DESC)
		return SYNC_TCP_NET_VISION_IN_DEVICE_VALUE;
	return UNKNOWN_DEVICE_VALUE;
}


/*! Returns true if this is an input device, i.e. one that provides 
	input TO the simulation. */
bool DeviceTypes::isInputDevice(unsigned int deviceType){
	if(deviceType % 2 == 1)
		return true;
	return false;
}


/*! Returns true if this is an output device, i.e. one that receives 
	input FROM the simulation. */
bool DeviceTypes::isOutputDevice(unsigned int deviceType){
	if(deviceType % 2 == 0)
		return true;
	return false;
}

