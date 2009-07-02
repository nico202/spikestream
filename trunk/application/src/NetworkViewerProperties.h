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

#ifndef NETWORKVIEWERPROPERTIES_H
#define NETWORKVIEWERPROPERTIES_H

//SpikeStream includes
#include "NetworkViewer.h"
#include "DBInterface.h"
#include "HighlightDialog.h"

//Qt includes
#include <qwidget.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlabel.h>
#include <q3progressbar.h>
#include <q3table.h>
#include <qstring.h>


//-------------------------- Network Viewer Properties----------------------
/*! GUI used to control the NetworkViewer 3D neuron display. */
//--------------------------------------------------------------------------

class NetworkViewerProperties : public QWidget {
	Q_OBJECT
 
 	public:
 		NetworkViewerProperties(QWidget *parent, NetworkViewer*, DBInterface*);
		~NetworkViewerProperties();
		void reloadConnectionDetails();
		void reloadConnections();
		void reloadNeuronGroups();
		void setFirstSingleNeuronNumber(unsigned int singleNeuronNumber);
		void setRenderProgressLabel(const QString labelText);
		void setSecondSingleNeuronNumber(unsigned int singleNeuronNumber);


 	private slots:
		void cancelRenderProgress();
		void connCheckBoxChanged();
		void filterCombosChanged(int);
		void firstNeuronComboChanged(int);
		void highlightButtonPressed();
		void renderDelayChanged(int);
		void renderStateChanged();
		void secondNeuronComboChanged(int);
		void showConnectionDetailsChanged();
		void showConnectionsChanged();
		void sortRows(int);


 	private:
		//======================== VARIABLES =================================
		//Qt variables for layers
		QCheckBox *fullRenderCheckBox;
		Q3ProgressBar *progressBar;
		QComboBox *renderDelayCombo;
		QLabel *renderDelayLabel;
		QLabel *renderProgressLabel;
		QPushButton* renderCancelButton;
		QPushButton* highlightButton;

		//Qt variables for connections
		QRadioButton *allConnRadioButt;
		QCheckBox *connectionDetailsCheckBox;
		Q3Table *connectionDetailsTable;
		QCheckBox *connectionsCheckBox;
		QRadioButton *connsSingleNeurRadioButt;
		QComboBox *neurConnsFilterCombo;
		QComboBox *neurConnsFromToCombo;
		QLabel *neurConnsLabel;
		QComboBox *neurConnsNeurGrpCombo_1;
		QComboBox *neurConnsNeurGrpCombo_2;
		QLabel *neurTotalConnsLabel;
		QLabel *singleNeurNumLabel_1;
		QLabel *singleNeurNumLabel_2;
		QLabel *andLabel;
		QLabel *inLabel_1;
		QLabel *inLabel_2;

		/*! Holds the neuronID when the connections to a single neuron are being displayed.*/
		unsigned int firstSingleNeuronNumber;

		/*! Holds the neuronID when the connections to a single neuron are being displayed.*/
		unsigned int secondSingleNeuronNumber;

		/*! Controls the filtering of the connections.*/
		bool showFromConns;

		/*! Controls the filtering of the connections.*/
		bool showToConns;

		/*! Controls the filtering of the connections.*/
		bool betweenMode;

		/*! Reference to NetworkViewer that this class controls.*/
		NetworkViewer *networkViewer;

		/*! Reference to class handling database.*/
		DBInterface *dbInterface;

		/*! Dialog to control the highlighting of the network*/
		HighlightDialog* highlightDialog;


		//============================ METHODS ========================================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		NetworkViewerProperties (const NetworkViewerProperties&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		NetworkViewerProperties operator = (const NetworkViewerProperties&);

		void loadConnectionDetails();
		void loadLayerNames(QComboBox*);

};


#endif//NETWORKVIEWERPROPERTIES_H




