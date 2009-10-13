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

#ifndef NEURONGROUPHOLDER_H
#define NEURONGROUPHOLDER_H

//Qt includes
#include <qstring.h>


/*! Container holding the cubic volume enclosing a region of space.
	Used to view an area containing a neural network. */
struct ClippingVolume{
	float minX;
	float maxX;
	float minY;
	float maxY;
	float minZ;
	float maxZ;
};


//-------------------------- Neuron Group Holder --------------------------
/*! Class to hold neuron groups for display purposes by the Network Viewer
	class. The contents of this overlap with the Neuron Group class, which
	is used to pass information about neuron groups between methods. */
//-------------------------------------------------------------------------

class NeuronGroupHolder {

	public:
		//============================ METHODS =====================================
		NeuronGroupHolder();
		~NeuronGroupHolder();


		//=========================== VARIABLES ====================================
		/*! ID of the first neuron. This is used for rapid access to X, Y, Z 
			coordinates.*/
		unsigned int startNeuronID;
		unsigned int *neuronIDArray;
		float *xPosArray;
		float *yPosArray;
		float *zPosArray;
		unsigned short neuronType;
		unsigned int numberOfNeurons;
		QString name;
		unsigned int width;
		unsigned int length;
		unsigned int depth;
		unsigned int neuronSpacing;
		int xPos;
		int yPos;
		int zPos;
		ClippingVolume clippingVolume;


	private:
		//============================== METHODS ==============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		NeuronGroupHolder (const NeuronGroupHolder&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		NeuronGroupHolder operator = (const NeuronGroupHolder&);

};


#endif//NEURONGROUPHOLDER_H

