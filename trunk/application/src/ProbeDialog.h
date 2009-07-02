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

#ifndef PROBEDIALOG_H
#define PROBEDIALOG_H

//SpikeStream includes
#include "DBInterface.h"

//Qt includes
#include <qdialog.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <q3table.h>


//---------------------------  Probe Dialog --------------------------------
/*! Allows the user to select the type of probe. Support for probes is not
	implemented yet. */
	
// FIXME PROBE ARCHITECTURE NOT IMPLEMENTED YET!
// FIXME MAY ALSO ALLOW USER TO CONFIGURE ITS PARAMETERS
//--------------------------------------------------------------------------

class ProbeDialog : public QDialog{
	Q_OBJECT
	
	public:
		ProbeDialog(QWidget *parent, DBInterface* netDBInter);
		~ProbeDialog();


	private slots:
		void addProbe();
		void deleteProbes();
		void probeTableHeaderClicked(int colNumber);


	private:
		//========================== VARIABLES ==============================
		/*! References to database interfaces.*/
		DBInterface* networkDBInterface;

		/*! Stores link between a combo position and a neuron type id.*/
		map<int, unsigned short> typePositionMap;

		//Qt widgets
		QLineEdit* nameText;
		QComboBox* probeTypeCombo;
		Q3Table* probeTable;

		//Location of information in the probeTable
		int selectionCol;
		int probeIDCol;
		int nameCol;
		int typeDescCol;
		int typeCol;


		//=========================== METHODS ===============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		ProbeDialog (const ProbeDialog&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		ProbeDialog operator = (const ProbeDialog&);

		void fillProbeTypeCombo();
		string getDescription(unsigned int pType);
		void loadProbeTable();
		void removeProbeFromTable(unsigned int probeID);

};


#endif//PROBEDIALOG_H

