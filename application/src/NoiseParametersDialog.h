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

#ifndef NOISEPARAMETERSDIALOG_H
#define NOISEPARAMETERSDIALOG_H

//SpikeStream includes
#include "DBInterface.h"
#include "SimulationManager.h"

//Other includes
#include <qdialog.h>
#include <qpushbutton.h>
#include <q3table.h>


//-------------------------- Noise Parameters Dialog -----------------------
/*! Displays all the noise parameters in a table with combo boxes and check
	boxes to edit them. */
//--------------------------------------------------------------------------

class NoiseParametersDialog : public QDialog {
	Q_OBJECT
	
	public:
		NoiseParametersDialog(QWidget *parent, DBInterface* networkDBInterface, SimulationManager* simMan);
		~NoiseParametersDialog();
		bool loadParameters();

	
	private slots:
		void applyButtonPressed();
		void cancelButtonPressed();
		void defaultsButtonPressed();
		void okButtonPressed();
		void paramValueChanged(int row, int col);
	

	private:
		//============================= VARIABLES ==============================
		/*! Store reference to database interfaces.*/
		DBInterface *networkDBInterface;

		/*! Store reference to simulation manager to inform tasks about changes.*/
		SimulationManager *simulationManager;
		
		//Table and its header for displaying parameters
		Q3Table* paramTable;
		Q3Header* paramTableHeader;

		/*! Applies the parameters without closing the dialog.*/
		QPushButton *applyButton;

		//Store column numbers for data that we want to access
		int neurGrpCol;
		int noiseModeCol;
		int neuronPercentCol;
		int firingModeCol;

		/*! Variable recording whether the parameter table has been changed
			Used to enable or disable the apply button and prevent updating parameters 
			unnecessarily.*/
		bool paramValuesChanged;


		//========================== METHODS ================================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		NoiseParametersDialog (const NoiseParametersDialog&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		NoiseParametersDialog operator = (const NoiseParametersDialog&);

		void storeParameters();

};


#endif //NOISEPARAMETERSDIALOG_H

