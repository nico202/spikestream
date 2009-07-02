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

#ifndef NEURONGROUP_H
#define NEURONGROUP_H

//Qt includes
#include <qstring.h>


//----------------------------- Neuron Group -------------------------------
/*! Used to pass the parameters of a neuron group between different methods
	and classes. Overlaps with NeuronGroupHolder, but this is just a simple
	struct without destructors etc. */
//--------------------------------------------------------------------------

struct NeuronGroup {
	unsigned int neuronGrpID;
	QString name;
	unsigned int startNeuronID;
	unsigned int width;
	unsigned int length;
	int xPos;
	int yPos;
	int zPos;
	unsigned int spacing;
	unsigned int neuronType;
	unsigned short neuronGrpType;

	//Used when creating layers corresponding to SIMNOS components
	unsigned int componentID;
	unsigned int deviceNeuronGrpID;

};


#endif //NEURONGROUP_H
