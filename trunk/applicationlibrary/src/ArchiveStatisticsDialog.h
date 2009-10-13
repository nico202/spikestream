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

#ifndef ARCHIVESTATISTICSDIALOG_H
#define ARCHIVESTATISTICSDIALOG_H

//SpikeStream includes
#include "NeuronGroup.h"
#include "ArchiveStatistic.h"

//Qt includes
#include <qdialog.h>
#include <qwidget.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <q3textedit.h>
#include <qstring.h>
#include <qlineedit.h>

//Other includes
#include <vector>
using namespace std;


//---------------------- Archive Statistics Dialog -------------------------
/*! Selects an aspect of the current archive to monitor for statistics,
	for example, the number of times a neuron with a particular ID has
	fired. 
	In this widget, neuron IDs should be added in as a comma separated list.
	Or, And and range operations are supported, for example:
		12121 & 12121, 1323-56565, 123213|098098 */
//--------------------------------------------------------------------------
class ArchiveStatisticsDialog : public QDialog {
	Q_OBJECT

	public:
		ArchiveStatisticsDialog(QWidget* parent, vector<NeuronGroup*> neurGrpVect);
		ArchiveStatisticsDialog(QWidget* parent, vector<NeuronGroup*> neurGrpVect, ArchiveStatisticsHolder* archStatHold);
		~ArchiveStatisticsDialog();
		ArchiveStatisticsHolder* getArchiveStatistics();


	private slots:
		void cancelButtonPressed();
		void neurGrpRadioButtonClicked();
		void neurIDRadioButtonClicked();
		void okButtonPressed();

	private:
		//============================ VARIABLES ===============================
		/*! Vector holding details of the neuron groups.*/
		vector<NeuronGroup*> neuronGrpVector;

		/*! Holds the selected statistical information.*/
		ArchiveStatisticsHolder* archStatsHolder;

		/*! Holds errors that arose from checking the string.*/
		bool neuronIDError;

		//Qt widgets
		QComboBox* neuronGrpCombo;
		QRadioButton* neurGrpRadioButt;
		QRadioButton* neurIDRadioButt;
		Q3TextEdit* neurIDText;
		QLineEdit* archStatsNameText;


		//============================ METHODS ===============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		ArchiveStatisticsDialog(const ArchiveStatisticsDialog&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		ArchiveStatisticsDialog operator = (const ArchiveStatisticsDialog&);

		unsigned int checkNeuronID(QString idSTr);
		void initialiseDialog();

};

#endif//ARCHIVESTATISTICSDIALOG_H

