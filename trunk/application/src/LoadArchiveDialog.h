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

#ifndef LOADARCHIVEDIALOG_H
#define LOADARCHIVEDIALOG_H

//SpikeStream includes
#include "DBInterface.h"

//Qt includes
#include <qdialog.h>
#include <q3listbox.h>
#include <qstring.h>

//Other includes
#include <map>


//------------------------ Load Archive Dialog ----------------------------
/*! Used to select an archive for playing back. */
//-------------------------------------------------------------------------

class LoadArchiveDialog : public QDialog {
 	Q_OBJECT

	public:
		LoadArchiveDialog(QWidget *parent, DBInterface*);
		~LoadArchiveDialog();
		QString getArchiveString();


	signals:
		void archiveNameChanged(QString archiveStartTime, QString newName);
		void deleteArchiveStarted();
		void deleteArchiveFinished();


	private slots:
		void deleteAllButtonPressed();
		void deleteButtonPressed();
		void okButtonPressed();
		void renameButtonPressed();


	private:
		//=========================== VARIABLES =============================
		/*! Reference to database handling class.*/
		DBInterface *archiveDBInterface;

		//Qt variables
		Q3ListBox *archiveList;


		//============================ METHODS ==============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		LoadArchiveDialog (const LoadArchiveDialog&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		LoadArchiveDialog operator = (const LoadArchiveDialog&);

		unsigned int getUnixTime(QString &timeString);
		void loadArchiveInformation();

};


#endif //LOADARCHIVEDIALOG_H

