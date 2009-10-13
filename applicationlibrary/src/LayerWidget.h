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

#ifndef LAYERWIDGET_H
#define LAYERWIDGET_H

//SpikeStream includes
#include "DBInterface.h"
#include "LayerManager.h"
#include "ConnectionWidget.h"

//Qt includes
#include <q3popupmenu.h>
#include <qpoint.h>
#include <qpixmap.h>
#include <qwidget.h>
#include <q3table.h>
#include <QString>

//Other includes
#include <string>


//-------------------------- Layer Widget --------------------------------
/*! Displays layers in a table and allows control over visibility and 
	zooming. Initialises the creation, editing and deletion of layers. */
//-------------------------------------------------------------------------

class LayerWidget : public QWidget {
	Q_OBJECT
 
	public:
 		LayerWidget(QWidget *parent, DBInterface* netDBInter, DBInterface* devDBInter);
		~LayerWidget();
		void reloadNeuronGroups();
		void setConnectionWidget(ConnectionWidget*);
		void simulationDestroyed();
		void simulationInitialised();

 
 	private slots:
		void deleteLayer();
		void editLayerProperties();
 		void newLayer();
		void tableClicked(int, int, int, const QPoint &);
		void tableHeaderClicked(int);


 	private:
		//=========================== VARIABLES =======================================
		//Database handling class
		DBInterface* networkDBInterface;
		DBInterface* deviceDBInterface;

		//References to enable communication with other classes
		ConnectionWidget *connectionWidget;
		LayerManager* layerManager;
		
		/*! Table holding all the information about the layers.*/
		Q3Table *layerTable;

		//Variables used to edit a layer
		Q3PopupMenu *layerPropertiesPopup;
		unsigned int activeRow;

		//Buttons
		QPushButton *newLayerButt;
		QPushButton *deleteButt;
		
		//Pixmaps used for zooming and showing and hiding layers
		QPixmap *showPixmap;
		QPixmap *hidePixmap;
		QPixmap *zoomToPixmap;
		QPixmap *zoomToPixmapHighlight;
		QPixmap *zoomAbovePixmapHighlight;
		
		/*! Contains the layer IDs of the visible layers. Should match the list of 
			layer IDs in the network viewer.*/
		vector<unsigned int> viewVector;
		
		//Variables to control zooming in and out of layers
		/*! Records which layer is highlighted.*/
		int zoomLayerRow;

		/*! Records how many clicks on a layer.*/
		int zoomClickCount;
		
		
		//============================ METHODS ========================================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		LayerWidget (const LayerWidget&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		LayerWidget operator = (const LayerWidget&);

		void addAllLayersToTable();
		void addLayerToTable(unsigned int);
		void clearTable();
                QString getNeuronTypeDescription(unsigned short);
		void removeLayerFromTable(unsigned int);

};


#endif//LAYERWIDGET_H
