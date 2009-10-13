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

#ifndef LAYERPROPERTIESDIALOG_H
#define LAYERPROPERTIESDIALOG_H

 //SpikeStream includes
#include "LayerManager.h"
#include "DBInterface.h"
#include "NeuronGroup.h"

//Qt includes
#include <qdialog.h>
#include <qcombobox.h>
#include <qlineedit.h>
//Added by qt3to4:
#include <QLabel>


/*! Default value of spacing between neurons. */
#define DEFAULT_NEURON_SPACING "1"


//---------------------- Layer Properties Dialog --------------------------
/*! In non-editing mode this gathers information about the layer to be created.
	In editing mode it displays information about a layer so that it can be 
	changed. This dialog is also used to create layers for SIMNOS components
	that are designed to connect to an input or output layer of sensors. */
//-------------------------------------------------------------------------

class LayerPropertiesDialog : public QDialog{
	Q_OBJECT
	
	public:
		LayerPropertiesDialog(QWidget *parent, const char *name, bool editMode, NeuronGroup&, DBInterface* netDBInter, DBInterface* devDBInter);
		~LayerPropertiesDialog();
		NeuronGroup getNeuronGroup();


	private slots:
		void componentComboChanged(int comboIndex);
		void inputLayerComboChanged(int);
		void neuronGrpTypeChanged(int currentSelection);
		void okButtonPressed();


	private:
		//========================== VARIABLES ============================
		/*! Controls whether data from an existing layer is displayed 
			or layer data is gathered from scratch.*/
		bool editMode;

		//Reference to databases
		DBInterface* networkDBInterface;
		DBInterface* deviceDBInterface;
		
		/*! Stores link between a combo position and a neuron type ID.*/
		map<int, unsigned short> neuronTypePositionMap;
	
		//Pointers to widgets
		QLineEdit *nameText;
		QComboBox *neurGrpTypeCombo;
		QComboBox *neuronTypeCombo;
		QLineEdit *lengthText;
		QLineEdit *widthText;
		QLineEdit *neuronSpacingText;
		QLabel* neuronSpacingLabel;
		QLineEdit *xPosText;
		QLineEdit *yPosText;
		QLineEdit *zPosText;
		QComboBox* componentCombo;
		QComboBox* inputLayerCombo;
		QLabel* inputLayerLabel;
		QLabel* componentLabel;

		/*! Map holding the widths of SIMNOS components.*/
		map<unsigned int, unsigned int> componentWidthMap;

		/*! Map holding the lengths of SIMNOS components.*/
		map<unsigned int, unsigned int> componentLengthMap;

		/*! Tracks when inputLayerCombo does not have any valid input 
			layers (only text).*/
		bool noInputLayers;

		/*! Tracks whether there are any available components.*/
		bool noComponents;

		/*! When neuron type cannot be recognized for an existing layer store 
			index of it here.*/
		int unknownNeuronTypeIndex;


		//========================== METHODS ==============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		LayerPropertiesDialog (const LayerPropertiesDialog&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		LayerPropertiesDialog operator = (const LayerPropertiesDialog&);

		void fillNeurGrpTypeCombo();
		void loadInputLayers();

};


#endif//LAYERPROPERTIESDIALOG_H


