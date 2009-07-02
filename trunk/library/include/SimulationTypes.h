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
 
//------------------------ Simulation Types --------------------------------
/*! Definitions of the different simulation types, which are currently pattern,
	live and no input. Used to control the behaviour of the simulation task. 
	Different tasks may be running in different simulation modes, for example
	one neuron group may be receiving pattern input, whereas another may be
	receiving input from other neuron groups. */
//--------------------------------------------------------------------------

#ifndef SIMULATIONTYPES_H
#define SIMULATIONTYPES_H


/*! Neuron group takes input from sensors (camera, touch sensor, etc.) 
	and/or gives output to external devices */
#define LIVE_SIMULATION 1


/*! Neuron group has no external input or output connections. */
#define NO_INPUT_SIMULATION 2


/*! Neuron group takes input from patterns. */
#define PATTERN_SIMULATION 3


#endif //SIMULATIONTYPES_H


