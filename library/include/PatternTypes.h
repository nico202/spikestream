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

#ifndef PATTERNTYPES_H
#define PATTERNTYPES_H

//Other includes
#include <string>
using namespace std;


/*! Static patterns consist of 1s and 0s */
#define STATIC_PATTERN_VALUE 1

/*! Description used for static patterns. */
#define STATIC_PATTERN_DESC "Static"



/*! Temporal patterns are either -1 for non-firing neurons or a delay value 
	between 0 and up to around 200 */
#define TEMPORAL_PATTERN_VALUE 10

/*! Description used for temporal patterns. */
#define TEMPORAL_PATTERN_DESC "Temporal"



/*! Unknown pattern type. */
#define UNKNOWN_PATTERN_VALUE 0

/*! Description used for unknown pattern type. */
#define UNKNOWN_PATTERN_DESC "Unknown pattern"

 
//------------------------ Pattern Types ----------------------------------
/*! Definitions of the different pattern types
	Currently static and temporal patterns.
	Static patterns consist of 1s and 0s, defining whether neurons fire or not
	Temporal patterns consist of -1 if the neuron never fires and otherwise
	a value for the delay value of the neuron's firing. */
//---------------------------------------------------------------------------*/

class PatternTypes {

	public:
		static char* getDescription(unsigned short type);
		static unsigned short getType(string description);

};


#endif //PATTERNTYPES_H


