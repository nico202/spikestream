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

#ifndef DBINTERFACE_H
#define DBINTERFACE_H

//Other includes
#include <mysql++.h>
#include <vector>
using namespace std;


/*! Structure to hold database parameters.*/
struct DBParameters{
	static const int maxParamLength = 30;
	char host[30];
	char user[30];
	char password[30];
	char database[30];
};


/*! Exception thrown when the database cannot be reconnected. */
class DatabaseReconnectionException : public exception{
  virtual const char* what() const throw(){
    return "Failed to reconnect to the database.";
  }
};


//-------------------------- Database Interface ---------------------------
/*! Interfaces with the data base, creating query objects for use by other 
	classes. */
//-------------------------------------------------------------------------

class DBInterface {

 	public:
		DBInterface();
		DBInterface(const char*, const char*, const char*, const char*);
		~DBInterface();
		bool connectToDatabase(bool useEx);
		const DBParameters getDBParameters();
		mysqlpp::Connection* getNewConnection();
		mysqlpp::Query getQuery();


	private:
		//=========================== VARIABLES ============================
		/*! Main connection to database. Used most of the time by other classes.*/
		mysqlpp::Connection *connection;
		
		/*! Vector containing other connections to database that have been created
			These are generally created for large fast queries using ResUse.*/
		vector<mysqlpp::Connection*> connectionVector;
		
		/*! Parameters for database connection
			Stored to prevent them from disappearing and to pass them on 
			elsewhere when required.*/
		DBParameters dbParam;

		/*! Records whether we are using exceptions or not.*/
		bool useExceptions;


		//=========================== METHODS ===============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		DBInterface (const DBInterface&);

		/*! Declare assignment private so it cannot be used.*/
		DBInterface operator = (const DBInterface&);

};


#endif//DBINTERFACE_H

