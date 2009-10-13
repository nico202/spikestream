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

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

//SpikeStream includes
#include "DBInterface.h"
#include "BusyDialog.h"

//Qt includes
#include <qdialog.h>
#include <q3process.h>
#include <qstring.h>
#include <q3table.h>

//Other includes
#include <stack>


//------------------------ Database Manager -------------------------------
/*! Provides an interface that allows the user to select which databases to
	save or load and carries out this saving and loading. */
//-------------------------------------------------------------------------

class DatabaseManager : public QDialog {
	Q_OBJECT
	
	public:
		DatabaseManager(QWidget* parent, QString flNme, bool saveDB, DBInterface* netDBInter, DBInterface* archDBInter, DBInterface* pattDBInter, DBInterface* devDBInter);
		~DatabaseManager();


	private slots:
		void cancelButtonPressed();
		void okButtonPressed();
		void processExited();
		void readFromStderr();
		void readFromStdout();


	private:
		//========================= VARIABLES===============================
		//Database handling classes
		DBInterface* netDBInterface;
		DBInterface* archDBInterface;
		DBInterface* pattDBInterface;
		DBInterface* devDBInterface;

		/*! Are we saving or loading the database.*/
		bool saveMode;

		/*! Set to true when the script is running to list the databases.*/
		bool listingDatabases;

		/*! Set to true when the script is running to save the databases.*/
		bool savingDatabase;

		/*! Set to true when the script is running to load the databases.*/
		bool loadingDatabase;

		//Record which colums have which information
		unsigned int nameCol;
		unsigned int hostCol;
		unsigned int selectionCol;

		//Record which databases are on each row
		int netDBRow;
		int archDBRow;
		int pattDBRow;
		int devDBRow;

		//Qt widgets
		Q3Table* dbTable;

		/*! Name of the file to save or load. Should be with an extension.*/
		QString fileName;

		/*! File name with extension removed. The extension should be added
			back by script.*/
		QString fileNameNoExtension;

		/*! The process that executes the script.*/
		Q3Process* process;

		/*! Holds references to all of the databases that need to be saved.*/
		stack<DBInterface*> dbStack;

		/*! Dialog to inform user that it is busy.*/
		BusyDialog* busyDialog;

		/*! Records whether the script has started outputing information about 
			the files. */
		bool storeList;

		/*! List of databases that can be loaded from the file.*/
		QStringList loadDBList;


		//========================== METHODS ===============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		DatabaseManager (const DatabaseManager&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		DatabaseManager operator = (const DatabaseManager&);

		int addDatabaseToTable(DBInterface* dbInter);
		void fillLoadTable();
		void getAvailableDatabases();
		void loadDatabase(DBInterface* dbInter);
		void saveDatabase(DBInterface* dbInter, bool compress);

};


#endif//DATABASEMANAGER_H

