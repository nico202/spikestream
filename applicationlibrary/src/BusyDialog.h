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

#ifndef BUSYDIALOG_H
#define BUSYDIALOG_H

//Qt includes
#include <qdialog.h>
#include <qlabel.h>
#include <qlayout.h>


//-------------------------- Busy Dialog -----------------------------------
/*! Displays a busy message for situations in which the length of time of a 
	process cannot be estimated and so a progress bar is not appropriate.*/
//--------------------------------------------------------------------------

class BusyDialog : public QDialog {
	Q_OBJECT

	public:
		BusyDialog(QWidget *parent, const QString &title);
		~BusyDialog();
		void showDialog(const QString&);


	private:
		//======================= VARIABLES ===============================
		/*! Message displayed whilst the process is taking place. */
		QLabel *busyMessage;


		//======================== METHODS ==============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		BusyDialog (const BusyDialog&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		BusyDialog operator = (const BusyDialog&);

};


#endif //BUSYDIALOG_H


