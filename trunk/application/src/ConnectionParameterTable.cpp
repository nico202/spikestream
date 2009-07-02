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
#include "ConnectionParameterTable.h"

//Other includes
#include <iostream>
using namespace std;


/*! Constructor. */
ConnectionParameterTable::ConnectionParameterTable(int numRows, int numCols, QWidget* parent) : Q3Table(numRows, numCols, parent) {
}


/*! Destructor. */
ConnectionParameterTable::~ConnectionParameterTable(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING CONNECTION PARAMETER TABLE"<<endl;
	#endif//MEMORY_DEBUG
}


/*! Ends editing on all cells. */
void ConnectionParameterTable::endAllEditing(){
	for(int i=0; i<numRows(); ++i)
		for(int j=0; j<numCols(); ++j)
			Q3Table::endEdit( i, j, true, true );
}


