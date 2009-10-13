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

#ifndef CONNECTIONWIDGET_H
#define CONNECTIONWIDGET_H

//SpikeStream includes
#include "DBInterface.h"
#include "ConnectionManager.h"
#include "BusyDialog.h"
 
 //Qt includes
#include <qwidget.h>
#include <q3table.h>
#include <string>
#include <qstring.h>
#include <qpixmap.h>
#include <qpoint.h>


//-------------------------- Connection Widget ----------------------------
/*! Displays table with details of all connection groups with controls for
	creating new connections, deleting connections and viewing connection
	groups. Interfaces with the connection manager, which does the actual
	adding and deleting from the database. */
//-------------------------------------------------------------------------

class ConnectionWidget : public QWidget {
	Q_OBJECT
 
 	public:
 		ConnectionWidget(QWidget *parent, DBInterface* netDBInter, DBInterface* devDBInter);
		~ConnectionWidget();
		void createConnections(unsigned int fromNeurGrpID, unsigned int toNeurGrpID, unsigned int componentID, bool deviceIsFrom);
		void deleteLayerConnections(unsigned int);
		vector<unsigned int>getConnectionViewVector();
		void reloadConnections();
		void simulationDestroyed();
		void simulationInitialised();


 	private slots:
		void deleteConnections();
 		void newConnectionsButtonPressed();
		void tableClicked(int, int, int, const QPoint &);
		void tableHeaderClicked(int);


 	private:
		//=========================== VARIABLES =======================================
		/*! Reference to class handling network database.*/
		DBInterface* networkDBInterface;

		/*! Reference to class handling device database.*/
		DBInterface* deviceDBInterface;

		/*! References to class responsible for creating and deleting connections.*/
		ConnectionManager *connectionManager;
		
		//Widgets
		Q3Table *connTable;

		//Pixmaps for showing and hiding layers
		QPixmap *showPixmap;
		QPixmap *hidePixmap;
		QPixmap *paramPixmap;

		//Store location of columns that are used to access information in the table
		int connGrpIDColumn;
		int paramCol;

		//Buttons
		QPushButton *newConnButt;
		QPushButton *deleteConnButt;

		/*! Vector contains connection group IDS of the visible layers.*/
		vector<unsigned int> viewVector;

		/*! Dialog to inform user that a long operation is taking place.*/
		BusyDialog* busyDialog;
		

		//=========================== METHODS =========================================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		ConnectionWidget(const ConnectionWidget&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		ConnectionWidget operator = (const ConnectionWidget&);

		string getSynapseTypeDescription(unsigned short synapseTypeID);
		void loadAllConnections();
		void loadConnection(unsigned int);
		void removeConnectionFromTable(unsigned int);

};


#endif//CONNECTIONWIDGET_H

