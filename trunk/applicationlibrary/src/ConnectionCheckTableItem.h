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

#ifndef CONNECTIONCHECKTABLEITEM_H
#define CONNECTIONCHECKTABLEITEM_H

//Qt includes
#include <q3table.h>
#include <qstring.h>


//--------------------- Connection Check Table Item ------------------------
/*! Used to paint virtual connection table items grey to differentiate them 
	from ordinary connections. */
//--------------------------------------------------------------------------

class ConnectionCheckTableItem : public Q3CheckTableItem {

	public:
		ConnectionCheckTableItem( Q3Table * table, const QString & text );
		~ConnectionCheckTableItem();


	protected:
		void paint(QPainter *p, const QColorGroup &cg, const QRect &cr, bool selected);


	private:
		//========================= VARIABLES ===========================
		/*! Reference to the table that contains this widget.*/
		Q3Table *parentTable;


		//========================== METHODS ============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		ConnectionCheckTableItem(const ConnectionCheckTableItem&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		ConnectionCheckTableItem operator = (const ConnectionCheckTableItem&);

};


#endif //CONNECTIONCHECKTABLEITEM_H


