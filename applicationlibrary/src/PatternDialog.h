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

#ifndef PATTERNDIALOG_H
#define PATTERNDIALOG_H

//SpikeStream includes
#include "DBInterface.h"
#include "PatternManager.h"

//Qt includes
#include <q3table.h>
#include <qdialog.h>


//---------------------------- Pattern Dialog ------------------------------
/*! Enables the user to add and delete patterns in the pattern database. */
//--------------------------------------------------------------------------

class PatternDialog : public QDialog{
	Q_OBJECT
	
	public:
    	PatternDialog(QWidget* parent, DBInterface* pattDBInter);
    	~PatternDialog();


	private slots:
		void addPattern();
		void deletePattern();
		void tableHeaderClicked(int colNumber);


	private:
		//============================== VARIABLES ====================================
		DBInterface* patternDBInterface;
		Q3Table* patternTable;

		/*! Column containing the pattern ID.*/
		int patternIDCol;

		/*! Column containing the check boxes for selecting patterns.*/
		int selectionCol;

		/*! Column containing description of pattern.*/
		int descriptionCol;

		/*! Manager that loads and deletes patterns in the database.*/
		PatternManager* patternManager;


		//============================= METHODS =======================================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		PatternDialog (const PatternDialog&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		PatternDialog operator = (const PatternDialog&);

		void loadPatternTable();
		void removePatternFromTable(unsigned int patternID);

};


#endif //PATTERNDIALOG_H
