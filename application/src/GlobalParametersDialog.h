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

#ifndef GLOBALPARAMETERSDIALOG_H
#define GLOBALPARAMETERSDIALOG_H

//SpikeStream includes
#include "GlobalVariables.h"
#include "DBInterface.h"
#include "SimulationManager.h"

//Qt includes
#include <qdialog.h>
#include <qlineedit.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qvalidator.h>
#include <qlayout.h>
#include <qcheckbox.h>
//Added by qt3to4:
#include <Q3VBoxLayout>


//-------------------------  Global Parameters Dialog ----------------------
/*! Dialog that enables the editing of parameters that apply across the 
	simulation as a whole. */
//--------------------------------------------------------------------------

class GlobalParametersDialog : public QDialog {
	Q_OBJECT
	
	public:
		GlobalParametersDialog(QWidget *parent, DBInterface *netDBInterface, SimulationManager *);
		~GlobalParametersDialog();
		bool loadParameters();

	
	private slots:
		void applyButtonPressed();
		void cancelButtonPressed();
		void loadDefaultsButtonPressed();
		void makeDefaultsButtonPressed();
		void okButtonPressed();

	
	private:
		//====================  VARIABLES  =================================
		/*! Reference to database handling class.*/
		DBInterface *networkDBInterface;

		/*! Reference to simulation manager.*/
		SimulationManager *simulationManager;

		/*! Validator for line edit input.*/
		QDoubleValidator *paramValidator;

		/*! Map holding all the references to the line edits.*/
		map<const char *, QLineEdit*, charKeyCompare> lineEditMap;

		/*! Map holding all the descriptions for the labels.*/
		map<const char *, QString*, charKeyCompare> labelMap;

		/*! Map holding the default values for each of the parameters.*/
		map<const char *, double, charKeyCompare> defaultValueMap;

		/*! Records whether a value is a check box type. Only check box values 
			are included in this map.*/
		map<const char*, bool, charKeyCompare> boolParameterMap;

		/*! Holds references to all the check boxes.*/
		map<const char*, QCheckBox*, charKeyCompare> checkBoxMap;

		/*! Holds all the map keys so they do not vanish.*/
		QStringList mapKeyStrList;


		//=======================  METHODS  ================================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		GlobalParametersDialog (const GlobalParametersDialog&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		GlobalParametersDialog operator = (const GlobalParametersDialog&);

		void addParameter(QString *labelText, QLineEdit *lineEdit, Q3VBoxLayout *vBox);
		void storeParameters();

};


#endif //GLOBALPARAMETERSDIALOG_H

