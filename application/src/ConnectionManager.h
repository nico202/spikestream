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

#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

//SpikeStream includes
#include "DBInterface.h"
#include "GlobalVariables.h"
#include "ConnectionHolder.h"
#include "SpikeStreamApplication.h"

//Qt includes
#include <qstring.h>
#include <q3progressdialog.h>

//Other includes
#include <map>
#include <string>
using namespace std;


//----------------------- Connection Manager ------------------------------
/*! Creates different types of connection in the Connections and Connection
	Groups databases. Controlled by Connection Widget.*/
//-------------------------------------------------------------------------

class ConnectionManager {

	public:
		ConnectionManager(DBInterface* netDBInter, DBInterface* devDBInter);
		~ConnectionManager();
		int createConnections(ConnectionHolder);
		void deleteConnections(vector<ConnectionHolder>);


	private:
		//=========================== VARIABLES =====================================
		/*! Reference to network database handling class.*/
		DBInterface *networkDBInterface;

		/*! Reference to device database handling class.*/
		DBInterface *deviceDBInterface;
		
		/*! Progress dialog to give feedback about progress of connection creation.*/
		Q3ProgressDialog *progressDialog;

		/*! Short version of the reference to QApplication to update it during
			large connection creations.*/
		SpikeStreamApplication *spikeStrApp;


		//============================ METHODS ======================================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		ConnectionManager(const ConnectionManager&);
		/*! Declare assignment private so it cannot be used inadvertently.*/
		ConnectionManager operator = (const ConnectionManager&);

		bool connectionExists(mysqlpp::Query&, unsigned int, unsigned int);
		void createOffCentreOnSurroundConns(ConnectionHolder&, int&);
		void createOnCentreOffSurroundConns(ConnectionHolder&, int&);
		void createSIMNOSComponentConns(ConnectionHolder&, int&);
		void createSimpleCortexConns(ConnectionHolder&, int&);
		void createTopographicConns(ConnectionHolder&, int&);
		void createUnstructuredConns(ConnectionHolder&, int&);
		void createUnstructuredExInhibConnections(ConnectionHolder& connHolder, int& connectionCount);
		bool evaluateConnectionProbability(double, double, double);
		void fillRandomSelectionVector(vector<unsigned int> &ranSelVect, unsigned int startNeuronID, unsigned int endNeuronID, double connectionDensity);
		unsigned short getDelay(unsigned short, unsigned short);
		double getDistance(int, int, int, int);
		double getNormalRandom();
		short getWeight(double, double, double);
		bool isExcitatoryNeuron(unsigned int neuronID, double excitatoryPercentage);

};


#endif//CONNECTIONMANAGER_H
