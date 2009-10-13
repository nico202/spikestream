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

#ifndef SYNAPSEPARAMETERSDIALOG_H
#define SYNAPSEPARAMETERSDIALOG_H

//SpikeStream includes
#include "DBInterface.h"
#include "EditSynapseParametersDialog.h"
#include "ParameterTable.h"
#include "SimulationManager.h"

//Qt includes
#include <qdialog.h>
#include <qstringlist.h>
#include <qpixmap.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <QLabel>


//------------------------ Synapse Parameters Dialog ----------------------
/*! Displays the synapse parameters in a table with an edit button that 
	launches an edit synapse parameters dialog to change the values. The 
	parameters are not changed until the ok or apply buttons are pressed.*/
//-------------------------------------------------------------------------

class SynapseParametersDialog : public QDialog {
	Q_OBJECT
	
	public:
		SynapseParametersDialog(QWidget *parent, DBInterface* networkDBInterface, SimulationManager* simMan);
		~SynapseParametersDialog();
		bool loadSynapseParameters();


	private slots:
		void applyButtonPressed();
		void cancelButtonPressed();
		void defaultsButtonPressed();
		void okButtonPressed();
		void parameterTableClicked(int row, int col, unsigned short typeID);
		void parameterTableValueChanged(int row, int col, unsigned short typeID);


	private:
		//========================== VARIABLES ===============================
		/*! Store reference to database interfaces.*/
		DBInterface *networkDBInterface;

		/*! Store reference to simulation manager to inform it about changes.*/
		SimulationManager *simulationManager;
		
		//Table and its header for displaying parameters
		map<unsigned short, ParameterTable*> paramTableMap;
		map<unsigned short, Q3Header*> paramTableHeaderMap;
		map<unsigned short, QLabel*> tableLabelMap;

		/*! Applies the parameters without closing the dialog.*/
		QPushButton *applyButton;

		//Variables holding the columns containing different information
		int editColumn;
		int connGrpIDColumn;
		int fromGrpColumn;
		int toGrpColumn;
		int paramStartColumn;

		/*! Image displayed in the edit column.*/
		QPixmap *editPixmap;

		/*! List of all the field names of the parameters, used for loading them 
			from the database.*/
		map<unsigned short, QStringList*> fieldNamesMap;

		/*! List of all the value names.*/
		map<unsigned short, QStringList*> valueNamesMap;

		/*! Links descriptions of the parameter with the default value.*/
		map<unsigned short, map<const char*, double, charKeyCompare> > defaultValueMap;

		/*! Links descriptions of the parameter with a column for loading up parameters.*/
		map<unsigned short, map<const char*, int, charKeyCompare> > descriptionColumnMap;

		/*! Records whether a value is a check box type. Only check box values are 
			included in this map.*/
		map<unsigned short, map<const char*, bool, charKeyCompare> > checkBoxMap;

		/*! Stores the link between a description and a parameter name.*/
		map<unsigned short, map<const char*, const char*, charKeyCompare> > descParamMap;

		/*! Stores the link between a parameter name and a description.*/
		map<unsigned short, map<const char*, const char*, charKeyCompare> > paramDescMap;

		/*! Variable recording whether the parameter table has been changed
			Used to enable or disable the apply button and prevent updating parameters
			unnecessarily.*/
		bool parameterValuesChanged;


		//========================== METHODS ================================
		/*! Declare copy constructor private so it cannot be used inadvertently. */
		SynapseParametersDialog (const SynapseParametersDialog&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		SynapseParametersDialog operator = (const SynapseParametersDialog&);

		QString getNeuronGroupName(QString neurGrpID);
		void storeSynapseParameters();

};


#endif //SYNAPSEPARAMETERSDIALOG_H

