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

//SpikeStream includes
#include "LogWriter.h"
#include "Util.h"
using namespace spikestream;

//Other includes
#include <ctime>
#include <iostream>
using namespace std;


/* Declare and initialise static variables. */
int LogWriter::logLevel = 0; //default log level
char LogWriter::logPath[255];


//--------------------------------------------------------------------------
//------------------------- PUBLIC STATIC METHODS --------------------------
//--------------------------------------------------------------------------

/*! Add log to the log file. */
void LogWriter::addLog(char * data){
	if(logLevel != 0){
		ofstream logOut(logPath, ios::app);
		logOut<<data<<endl<<endl;
		logOut.close();
	}
}


/*! Add log to the log file. */
void LogWriter::addLog(char*  data1, char* data2){
	if(logLevel != 0){
		ofstream logOut(logPath, ios::app);
		logOut<<data1<<" "<<data2<<endl<<endl;
		logOut.close();
	}
}


/*! Add log to the log file. */
void LogWriter::addLog(const char*  data1, const char* data2){
	if(logLevel != 0){
		ofstream logOut(logPath, ios::app);
		logOut<<data1<<" "<<data2<<endl<<endl;
		logOut.close();
	}
}


/*! Add log to the log file. */
void LogWriter::addLog(string data){
	if(logLevel != 0){
		ofstream logOut(logPath, ios::app);
		logOut<<data<<endl<<endl;
		logOut.close();
	}
}


/*! Add log to the log file. */
void LogWriter::addLog(string data1, string data2){
	if(logLevel != 0){
		ofstream logOut(logPath, ios::app);
		logOut<<data1<<" "<<data2<<endl<<endl;
		logOut.close();
	}
}


/*! Switche logging off. */
void LogWriter::disableLogging(){
	logLevel = 0;
}


/*! Returns the log level. */
int LogWriter::getLogLevel(){
	return logLevel;
}


/*! Returns true if logging is enabled. */
bool LogWriter::loggingEnabled(){
	if(logLevel == 0)
		return false;
	return true;
}


/*! Sets the log level. */
void LogWriter::setLogLevel(int logLev){
	logLevel = logLev;
}


/*! Sets the location of the log path. */
void LogWriter::setLogPath(const char* lp){
	Util::safeCStringCopy(logPath, lp, 100);
}


/*! Writes the date and time to the log. */
void LogWriter::writeDate(){
	if(logLevel != 0)//Check to see if logging is enabled
		return;

	//Get the time and date
	time_t timeNow = time(NULL);
	struct tm *locTm = localtime(&timeNow);

	//Write the time and date
	ofstream logOut(logPath, ios::app);//Should create a new file if one does not exist already
	if(!logOut){//No point in throwing an exception since this is a non critical function
		cout<<"No log file available. Logging disabled."<<endl;
		logLevel = 0;
		return;
	}
	logOut<<locTm->tm_mday<<"/"<<(locTm->tm_mon +1)<<"/"<<(locTm->tm_year + 1900)<<" "<<locTm->tm_hour<<":"<<locTm->tm_min<<":"<<locTm->tm_sec<<endl;
}

