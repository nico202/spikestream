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
#include "PatternTypes.h"


//--------------------------------------------------------------------------
//------------------------ STATIC PUBLIC METHODS ---------------------------
//--------------------------------------------------------------------------

/*! Returns a description of the pattern type. */
char* PatternTypes::getDescription(unsigned short type){
	switch(type){
		case STATIC_PATTERN_VALUE: return STATIC_PATTERN_DESC;
		case TEMPORAL_PATTERN_VALUE: return TEMPORAL_PATTERN_DESC;
		default: return UNKNOWN_PATTERN_DESC;
	}
}


/*! Returns the type that corresponds to a given description. */
unsigned short PatternTypes::getType(string description){
	if(description == STATIC_PATTERN_DESC)
		return STATIC_PATTERN_VALUE;
	else if (description == TEMPORAL_PATTERN_DESC)
		return TEMPORAL_PATTERN_VALUE;
	return UNKNOWN_PATTERN_VALUE;
}

