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

#ifndef CONNECTIONPROPERTIESDIALOG_H
#define CONNECTIONPROPERTIESDIALOG_H

//SpikeStream includes
#include "DBInterface.h"
#include "ConnectionType.h"
#include "ConnectionManager.h"
#include "ConnectionParameterTable.h"

//Qt includes
#include <qdialog.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <q3table.h>
#include <qstring.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qradiobutton.h>

//Other includes
#include <map>

//Default delay values, stored as strings to save conversion
#define DEFAULT_MIN_DELAY 0
#define DEFAULT_MAX_DELAY 250


//---------------------- Connection Properties Dialog ---------------------
/*! Dialog used to specify the properties for new connections.
	This contains quite a lot of complicated checking for the inter layer 
	connections. At present edit mode is used when obtaining the parameters
	for a SIMNOS connection and has not been generalised from this. */
//-------------------------------------------------------------------------

class ConnectionPropertiesDialog : public QDialog{
	Q_OBJECT
	
	public:
		ConnectionPropertiesDialog(QWidget *, const char*, DBInterface*);
		ConnectionPropertiesDialog(QWidget *, const char*, DBInterface* netDBInter, unsigned int fromNeurGrpID, unsigned int toNeurGrpID, unsigned short connType);
		~ConnectionPropertiesDialog();
		ConnectionHolder getConnectionHolder();
		map<string, double> getConnectionParameters();

	
	private slots:
		void layerRadioStateChanged();
		void okButtonPressed();
		void paramSelectionChanged();
		void paramValueChanged(int, int);
	

	private:
		//=========================== VARIABLES =======================================
		/*! References to database handling class.*/
		DBInterface *dbInterface;

		/*! Validator to check integer input.*/
		QIntValidator *delayValidator;

		/*! Validator to check double input.*/
		QDoubleValidator * paramValidator;

		/*! Stores link between a combo position and a synapse type ID.*/
		map<int, unsigned short> synapseTypePositionMap;

		/*! Records whether the from and to neuron groups and connection
			type has already been determined at the launch of the dialog.*/
		bool editMode;

		/*! Used in edit mode to hold the type of the connection, defined in 
			ConnectionType.h*/
		int connectionType;

		/*! Used in edit mode to hold the from neuron group.*/
		int fromNeurGrpID;

		/*! Used in edit mode to hold the to neuron group.*/
		int toNeurGrpID;

		//GUI Widgets
		QLabel *intraLayerLabel;
		QLabel *fromLayerLabel;
		QLabel *toLayerLabel;
		QComboBox *intraLayerCombo;
		QComboBox *fromLayerCombo;
		QComboBox *toLayerCombo;
		QComboBox *synapseTypeCombo;
		QComboBox *connectionTypeCombo;
		QRadioButton *interRadioButt;
		QRadioButton *intraRadioButt;
		ConnectionParameterTable *connParamTable;
		QLineEdit *minDelayText;
		QLineEdit *maxDelayText;

		/*! Tracks whether an error in the parameters has taken place after the
			ok button has been pressed.*/
		bool parameterError;

		/*! Records whether there was an error loading the dialog. */
		bool loadError;


		//============================ METHODS ========================================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		ConnectionPropertiesDialog(const ConnectionPropertiesDialog&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		ConnectionPropertiesDialog operator = (const ConnectionPropertiesDialog&);

		bool checkConnection_INTER(unsigned int, unsigned int, unsigned short);
		bool checkConnection_INTRA(unsigned short);
		void constructDialog();
		void fillConnectionTypeCombo_INTER();
		void fillConnectionTypeCombo_INTRA();
		double getParameterValue(QString);
		void loadLayerNames();
		void loadParameterTable(unsigned short);
		void setParameterValue(QString, double);

};


#endif//CONNECTIONPROPERTIESDIALOG_H


