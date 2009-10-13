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

//SpikeStream includes
#include "ParameterTable.h"
#include "Debug.h"

//Other includes
#include <iostream>
using namespace std;


/*! Constructor. */
ParameterTable::ParameterTable(QWidget* parent, unsigned short tID, string paramTableName) : Q3Table(parent) {
	//Store information about this neuron or synapse type
	typeID = tID;
	parameterTableName = paramTableName;

	//Connect signals to slots
	connect (this, SIGNAL(clicked(int, int, int, const QPoint &)), this, SLOT(tableClicked(int, int, int, const QPoint &)));
	connect (this, SIGNAL(valueChanged(int, int)), this, SLOT(tableValueChanged(int, int)));
}


/*! Destructor. */
ParameterTable::~ParameterTable(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING PARAMETER TABLE"<<endl;
	#endif//MEMORY_DEBUG
}


//------------------------------------------------------------------------
//-------------------------- PUBLIC METHODS ------------------------------
//------------------------------------------------------------------------

/*! Returns the name of the database table that contains the parameters for this 
	neuron or synapse type. */
string ParameterTable::getParameterTableName(){
	return parameterTableName;
}


//------------------------------------------------------------------------
//------------------------------ SIGNALS ---------------------------------
//------------------------------------------------------------------------

/*! Emits a signals containing the id of the neuron or synapse type. */
void ParameterTable::tableClicked(int row, int col, int, const QPoint &){
	emit parameterTableClicked(row, col, typeID);
}


/*! Emits a signal containing the id of the neuron or synapse type. */
void ParameterTable::tableValueChanged(int row, int col){
	emit parameterTableValueChanged(row, col, typeID);
}



