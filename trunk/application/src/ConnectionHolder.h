/***************************************************************************
 *   SpikeStream Application                                               *
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

#ifndef CONNECTIONHOLDER_H
#define CONNECTIONHOLDER_H


//----------------------- Connection Holder --------------------------------
/*! Used to hold and pass the parameters of a connection group. */
//--------------------------------------------------------------------------

struct ConnectionHolder {

	unsigned int fromLayerID;
	unsigned int toLayerID;
	unsigned short connectionType;
	unsigned short minDelay;
	unsigned short maxDelay;
	unsigned int connectionGrpID;
	unsigned int synapseType;
	map<string, double> paramMap;
	
	//Parameters for SIMNOS connections
	unsigned int componentID;
	bool deviceIsFrom;

};


#endif//CONNECTIONHOLDER_H

