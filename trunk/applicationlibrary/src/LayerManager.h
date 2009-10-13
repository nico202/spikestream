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

#ifndef LAYERMANAGER_H
#define LAYERMANAGER_H

//SpikeStream includes
#include "ConnectionWidget.h"
#include "DBInterface.h"
#include "NeuronGroup.h"

//Qt includes
#include <qwidget.h>
#include <qstring.h>
#include <q3progressdialog.h>

//Other includes
#include <mysql++.h>
#include <vector>
using namespace std;


//-------------------------- Layer Manager --------------------------------
/*! Manages the creation and deletion of layers.*/
//-------------------------------------------------------------------------

class LayerManager {

	public:
		LayerManager(DBInterface* netDBInter, DBInterface* devDBInter);
		~LayerManager();
		int createLayer(const NeuronGroup &neuronGrp);
		void deleteLayers(vector<unsigned int> layerID);
		void setConnectionWidget(ConnectionWidget* connWidg);
		void setNeuronGrpName(const unsigned int neuronGrpID, QString name);
		bool setNeuronGrpSpacingPosition(const unsigned int neuronGrpID, const NeuronGroup&, const NeuronGroup&);
		void setNeuronGrpType(const unsigned int neuronGrpID, unsigned short neuronType);


	private:
		//========================== VARIABLES ===========================
		//Reference to database interfaces
		DBInterface *networkDBInterface;
		DBInterface *deviceDBInterface;

		/*! Reference to connection widget enables layer manager to instruct 
			it to create connections when creating SIMNOS component layers.*/
		ConnectionWidget* connectionWidget;
		
		/*! Progress dialog to display progress.*/
		Q3ProgressDialog *progressDialog;


		//=========================== METHODS ============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		LayerManager (const LayerManager&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		LayerManager operator = (const LayerManager&);

		int create2DRectangularLayer(const NeuronGroup &neuronGrp);
		int create3DRectangularLayer(const NeuronGroup &neuronGrp);
		int createSIMNOSComponentLayer(const NeuronGroup &neuronGrp);
		bool evaluatePlaceNeuronProbability(double density);

};


#endif//LAYERMANAGER_H
