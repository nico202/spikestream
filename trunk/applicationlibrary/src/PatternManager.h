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

#ifndef PATTERNMANAGER_H
#define PATTERNMANAGER_H

//SpikeStream includes
#include "DBInterface.h"

//Qt includes
#include <qstring.h>

 
//--------------------------- Pattern Manager ------------------------------
/*! Controlled by the PatternDialog and adds patterns into the pattern 
	database, so that they can be applied to any part of the neuron 
	simulation. Also deletes patterns from the pattern database. */
//--------------------------------------------------------------------------

class PatternManager {

	public:
		PatternManager(DBInterface*);
		~PatternManager();
		bool addPattern(QString fileName);
		bool deletePattern(unsigned int pattID);


	private:
		//============================ VARIABLES =============================
		/*! Reference to class for handling database.*/
		DBInterface *patternDBInterface;


		//============================ METHODS ===============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		PatternManager (const PatternManager&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		PatternManager operator = (const PatternManager&);

		bool patternAlreadyStored(QString);
		void storePatternCount(unsigned int patternGrpID, int patternCount);
		bool storePatternData(unsigned int, QString);
		int storePatternDescription(QString description, unsigned short patternType, unsigned int width, unsigned int length, QString fileName);

};


#endif //PATTERNMANAGER_H
