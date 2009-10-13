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
#include "ConnectionTableItem.h"
#include "Debug.h"

//Other includes
#include <iostream>
using namespace std;


/*! Constructor. */
ConnectionTableItem::ConnectionTableItem(Q3Table * table, EditType et, const QString & text ) : Q3TableItem(table, et, text ){
	parentTable = table;
}


/*! Destructor. */
ConnectionTableItem::~ ConnectionTableItem(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING CONNECTION TABLE ITEM"<<endl;
	#endif//MEMORY_DEBUG
}


//--------------------------------------------------------------------------
//----------------------- PROTECTED METHODS --------------------------------
//--------------------------------------------------------------------------

/*! Paints virtual connections a light grey to make them distinctive. */
void ConnectionTableItem::paint( QPainter *p, const QColorGroup &cg, const QRect &cr, bool selected ){
    QColorGroup g( cg );
    if (parentTable->text(row(), 3) == "Virtual")
        g.setColor( QColorGroup::Text, QColor(150, 150, 150) );
    Q3TableItem::paint( p, g, cr, selected );
}


