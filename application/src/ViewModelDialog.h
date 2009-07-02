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

#ifndef VIEWMODELDIALOG_H
#define VIEWMODELDIALOG_H


//Qt includes
#include <qdialog.h>
#include <qwidget.h>
#include <q3textedit.h>


//------------------------  View Model Dialog  -----------------------------
/*! Displays the network model of an archive in XML format.*/
//--------------------------------------------------------------------------
class ViewModelDialog : public QDialog {
	Q_OBJECT

	public:
		ViewModelDialog(QWidget* parent, QString* nwModelStr);
		~ViewModelDialog();
		void reloadText();


	private slots:
		void okButtonClicked();
		void cancelButtonClicked();


	private:
		//============================== VARIABLES =============================
		/*! The string that is to be displayed.*/
		QString* networkModelString;

		/*! The display area.*/
		Q3TextEdit* modelTextEdit;


		//============================ METHODS ===============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		ViewModelDialog(const ViewModelDialog&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		ViewModelDialog operator = (const ViewModelDialog&);

};


#endif//VIEWMODELDIALOG_H


