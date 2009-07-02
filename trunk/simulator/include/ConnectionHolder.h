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

#ifndef CONNECTIONHOLDER_H
#define CONNECTIONHOLDER_H


//------------------------ Connection Holder -------------------------------
/*! 	All the connections for each neuron are stored in a single 
	connectionArray. The connection holder struct points to a part of this 
	array and it is these connection holders that are passed to the various 
	tasks when a neuron fires. By passing pointers in this way, the list of 
	connection IDs only has to be copied from the connectionArray once
	directly into the message by pvm_pkuint(). */
//--------------------------------------------------------------------------

struct ConnectionHolder {

	/*! Pointer to part of the neuron's connectionArray. */
	unsigned short *connIDArray; 
	
	/*! Count of the number of connections. Each connection is a pair of numbers: 
		the from and to neuronID. */
	int numConnIDs;

	/*! The delay of the connections pointed to by this connection holder. */
	unsigned short delay;

};


#endif //CONNECTIONHOLDER_H


