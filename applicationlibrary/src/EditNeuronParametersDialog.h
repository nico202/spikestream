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

#ifndef EDITNEURONPARAMETERSDIALOG_H
#define EDITNEURONPARAMETERSDIALOG_H

//SpikeStream includes
#include "GlobalVariables.h"

//Qt includes
#include <qdialog.h>
#include <qlayout.h>
#include <qstringlist.h>
#include <qlineedit.h>
#include <qvalidator.h>
//Added by qt3to4:
#include <Q3VBoxLayout>

//Other includes
#include <map>
using namespace std;


//---------------------- Edit Neuron Parameters Dialog --------------------
/*! Displays the neuron parameters for a layer so that they can be 
	edited.*/
//-------------------------------------------------------------------------

class EditNeuronParametersDialog : public QDialog {
	Q_OBJECT

	public:
		EditNeuronParametersDialog(QWidget *parent, map<const char*, double, charKeyCompare> descValueMap, map<const char*, double, charKeyCompare> *defaultValueMap, unsigned int neuronGrpID);
		~EditNeuronParametersDialog();
		map<const char*, QLineEdit*, charKeyCompare>* getDescriptionLineEditMap();

	
	private slots:
		void cancelButtonPressed();
		void loadDefaultsButtonPressed();
		void makeDefaultsButtonPressed();
		void okButtonPressed();


	private:
		//=========================== VARIABLES =============================
		/*! Validator for the parameters.*/
		QDoubleValidator *paramValidator;

		/*! Map holding all of the QLineEdits in the class. Used to access their
			values when ok is pressed.*/
		map<const char*, QLineEdit*, charKeyCompare> descriptionLineEditMap;
		
		/*! Pointer to the map holding the default values in the 
			NeuronParametersDialog.*/
		map<const char*, double, charKeyCompare> *defaultValueMap;

		/*! Store list of field names so they are not deleted.*/
		QStringList fieldNameStubList;


		//=========================== METHODS ===============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		EditNeuronParametersDialog (const EditNeuronParametersDialog&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		EditNeuronParametersDialog operator = (const EditNeuronParametersDialog&);

		void addNeuronParameter(QString labelText, QLineEdit *lineEdit, Q3VBoxLayout *vBox);

};


#endif //EDITNEURONPARAMETERSDIALOG_H

