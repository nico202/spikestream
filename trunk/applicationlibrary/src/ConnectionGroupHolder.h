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

#ifndef CONNECTIONGROUPHOLDER_H
#define CONNECTIONGROUPHOLDER_H


//---------------------- Connection Group Holder ---------------------------
/*! Class to hold connection groups for display purposes that is mainly used 
	by the network viewer. References in this class are allocated to arrays 
	by the network viewer. These arrays are then cleaned up by this class 
	when it is destroyed. */
//--------------------------------------------------------------------------

class ConnectionGroupHolder {
	
	public:
		//======================= METHODS ===========================
		ConnectionGroupHolder();
		~ConnectionGroupHolder();
	
	
		//====================== VARIABLES ==========================
		unsigned int *fromNeuronIDArray;
		float *fromXArray;
		float *fromYArray;
		float *fromZArray;
		unsigned int *toNeuronIDArray;
		float *toXArray;
		float *toYArray;
		float *toZArray;
		unsigned int connectionType;
		unsigned short synapseType;
		char *weightArray;//Store as char type for efficiency, but will not be chars
		unsigned int numberOfConnections;


	private:
		//======================= METHODS ============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		ConnectionGroupHolder(const ConnectionGroupHolder&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		ConnectionGroupHolder operator = (const ConnectionGroupHolder&);

};


#endif//CONNECTIONGROUPHOLDER_H
