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

#ifndef CONNECTIONPARAMETERTABLE_H
#define CONNECTIONPARAMETERTABLE_H

//Qt includes
#include <q3table.h>


//---------------------- Connection Parameter Table ------------------------
/*! Inherits from QTable to give access to endEdit method. This is called
	by ConnectionPropertiesDialog when the ok button is pressed to make sure
	that all cells are no longer in the editing state. */
//--------------------------------------------------------------------------

class ConnectionParameterTable : public Q3Table {
	Q_OBJECT

	public:
		ConnectionParameterTable(int numRows, int numCols, QWidget* parent);
		~ConnectionParameterTable();
		void endAllEditing();


	private:
	//========================== METHODS ============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		ConnectionParameterTable (const ConnectionParameterTable&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		ConnectionParameterTable operator = (const ConnectionParameterTable&);

};


#endif//CONNECTIONPARAMETERTABLE_H


