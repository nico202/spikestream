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
#include "DBInterface.h"
#include "Utilities.h"
#include "GlobalVariables.h"

//Other includes
#include <iostream>
#include <iomanip>
using namespace std;
using namespace mysqlpp;


/*! Empty default constructor - not used. */
DBInterface::DBInterface(){
}


/*! Main constructor. */
DBInterface::DBInterface(const char* host, const char* user, const char* password, const char* database){
	Utilities::safeCStringCopy(dbParam.host, host, dbParam.maxParamLength);
	Utilities::safeCStringCopy(dbParam.user, user, dbParam.maxParamLength);
	Utilities::safeCStringCopy(dbParam.password, password, dbParam.maxParamLength);
	Utilities::safeCStringCopy(dbParam.database, database, dbParam.maxParamLength);
}


/*! Destructor. */
DBInterface::~DBInterface(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING DBINTERFACE. NUMBER OF EXTRA CONNECTIONS: "<<connectionVector.size()<<";"<<endl;
	#endif//MEMORY_DEBUG
	
        //Disconnect and delete the standard connection
	if(connection->connected()){
                connection->disconnect();
	}
	delete connection;
	
        //Disconnect and delete any connections that were created by getNewConnection()
	for(vector<Connection*>::iterator iter = connectionVector.begin(); iter != connectionVector.end(); ++iter){
		if((*iter)->connected()){
                        (*iter)->disconnect();
		}
		delete *iter;
	}
}


//---------------------------------------------------------------------------
//---------------------------- PUBLIC METHODS -------------------------------
//---------------------------------------------------------------------------

/*! Connects to the database using the parameters stored in the constructor. */
bool DBInterface::connectToDatabase(bool useEx){
	//Record whether we are using exceptions or not
	useExceptions = useEx;

	//Connect to database
	try{
  		connection = new Connection(useExceptions);
		connection->connect(dbParam.database, dbParam.host, dbParam.user, dbParam.password);
		if(!connection->connected()){
			cerr<<"DBInterface: MAIN CONNECTION TO DATABASE FAILED: "<<connection->error()<<endl;
			return false;
		}
		else{
			//bool optionRes = connection->set_option(Connection::opt_reconnect, true);
			//if(!optionRes)
			//	cout<<"FAILED TO SET RECONNECTION OPTION"<<endl;
			return true;
		}
	}
	catch (const std::exception er) {
		cerr<<"DBInterface: MAIN CONNECTION TO DATABASE FAILED: "<<connection->error()<<endl;
        return false;
    }
}


/*! Returns the database parameters.
	Mainly used for passing dbparameters to spawned processes. */
const DBParameters DBInterface::getDBParameters(){
	return dbParam;
}


/*! Creates a new connection and returns it. This is generally used for large 
	queries, which need to use ResUse instead of Result.
	NOTE: Exception handling should be done by the calling class.*/
Connection* DBInterface::getNewConnection(){
	// Connect to the database.
	Connection *tempConnection = new Connection(useExceptions);
	tempConnection->connect(dbParam.database, dbParam.host, dbParam.user, dbParam.password);
	if(!tempConnection->connected()){
		cerr<<"DBInterface: NEW CONNECTION TO DATABASE FAILED: "<<tempConnection->error()<<endl;
		return 0;
	}
	else{
		connectionVector.push_back(tempConnection);//Store a reference to the query for deletion later
		return tempConnection;
	}
}


/*! Returns the query from the main connection.*/
Query DBInterface::getQuery(){

	//Check to see if we are still connected to the database before returning the query
	unsigned int reconnectCounter = 1;
	while(!connection->connected()){
		cout<<"DBInterface: MAIN DATABASE CONNECTION FAILED. Attempt "<<reconnectCounter<<" to reconnect to "<<dbParam.database<<" database."<<endl;

		//Try to reconnect to the database
		connection->connect(dbParam.database, dbParam.host, dbParam.user, dbParam.password);

		//Sleep if we have failed less than the maximum number of attempts
		if(!connection->connected() && reconnectCounter <= MAX_NUMBER_RECONNECT_ATTEMPTS)
			sleep(1);

		//Throw an exception if we have failed more than the maximum number
		else if (reconnectCounter > MAX_NUMBER_RECONNECT_ATTEMPTS)
			throw new DatabaseReconnectionException;

		//Increase the count of the number of failed attempts
		++reconnectCounter;
	}

	//Return the query if we have not thrown an exception
	return connection->query();
}

