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

#ifndef CONNECTIONMATRIXLOADER_H
#define CONNECTIONMATRIXLOADER_H

//SpikeStream includes
#include "DBInterface.h"

//Qt includes
#include <qstring.h>
#include <qstringlist.h>

//--------------------- Connection Matrix Loader ---------------------------
/*! Loads a connection matrix from a file and creates a neuron group and 
	connection group. The matrix must be square or an error will be thrown.
	The neuron group is created with the first type of neuron and synapse
	in the database and starting at position 0, 0, 0. Will try and make
	the neuron group as close to square as possible.
	FIXME NEED A DIALOG TO SPECIFY THE DETAILS PROPERLY.
	FIXME THIS METHOD HAS NOT BEEN PROPERLY TESTED. */
//--------------------------------------------------------------------------
class ConnectionMatrixLoader {

	public:
		ConnectionMatrixLoader(DBInterface* netDBInter);
		~ConnectionMatrixLoader();
		bool loadConnectionMatrix(QString& fileName);

	private:
		//========================= VARIABLES =============================
		DBInterface* networkDBInterface;

		/*! Id of the neuron group is set here when it has been created.
			Also used to track whether the neuron group has been created.*/
		unsigned int neurGrpID;

		/*! Id of the connection group is set here when it has been created.
			Also used to track whether the connection group has been created.*/
		unsigned int connGrpID;

		/*! The synapse type. Should be the first type in the SynapseTypes 
			database*/
		unsigned int synapseType;

		/*! The neuron type. Should be the first type in the NeuronTypes 
			database*/
		unsigned int neuronType;

		/*! Width of the neuron group.*/
		unsigned int neurGrpWidth;

		/*! Length of the neuron group.*/
		unsigned int neurGrpLength;

		/*! The size of the neuron group that is being created.*/
		unsigned int numNeur;

		/*! Count of the number of connections that have been added.*/
		unsigned int connectionCount;

		/*! Count of the rows. */
		unsigned int rowNum;

		/*! The lowest ID in the neuron group.*/
		unsigned int startNeurID;

		/*! Flag set to true when an error is encountered.*/
		bool errorState;

		/*! The parameters table for the neurons.*/
		QString neurParamTable;


		//======================== METHODS =============================
		void addConnections(QStringList& neuronIDList);
		void createConnectionGroup(const QString& fileName);
		void createNeuronGroup();
		void deleteDatabaseEntries(unsigned int nGrpID, unsigned int cGrpID);
		void showError(const char* errMsg);
		void showError(const QString& errMsg);

};

#endif//CONNECTIONMATRIXLOADER_H


