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
 
//--------------------------- Device Messages ------------------------------
/*! Defines types of TCP or UDP messages that are sent and received over IP 
	to and from external devices.

	NOTE Any changes to these values have to be coordinated with SIMNOS and 
	other external devices. */
//--------------------------------------------------------------------------

#ifndef DEVICEMESSAGES_H
#define DEVICEMESSAGES_H


/*! TCP message sent from SpikeStream to device acknowledging receipt of 
	data from the device.*/
#define SPIKESTREAM_DATA_ACK_MSG 1


/*! TCP message sent by device to SpikeStream acknowledging receipt of data.*/
#define DEVICE_DATA_ACK_MSG 3


#endif//DEVICEMESSAGES_H
