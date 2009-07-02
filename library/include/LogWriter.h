/***************************************************************************
 *   SpikeStream Library                                                   *
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

#ifndef LOGWRITER_H
#define LOGWRITER_H

//Other includes
#include <fstream>
using namespace std;


//---------------------------- Log Writer ----------------------------------
/*! Collection of static methods that control logging for the application.
	Logging can operate at three levels:
	
		0 - disabled
		1 - medium. Logs only some database transactions
		2 - full. Complete record of changes to the database
		
	Logging is disabled by SpikeStreamMainWindow at start up if the log file 
	is too large. The maximum size of log file is set in 
	SpikeStreamApplication.config. This class is not currently used, but is 
	available if needed. */
//---------------------------------------------------------------------------

class LogWriter {

	public:
		static void addLog(char*);
		static void addLog(char*, char*);
		static void addLog(const char*, const char*);
		static void addLog(string);
		static void addLog(string, string);
		static void disableLogging();
		static int getLogLevel();
		static bool loggingEnabled();
		static void setLogLevel(int logLevel);
		static void setLogPath(const char*);
		static void writeDate();


	private:
		/*! The amount of logging used. */
		static int logLevel;

		/*! Location f the log output file. */
		static char logPath [255];

};


#endif//LOGWRITER_H

