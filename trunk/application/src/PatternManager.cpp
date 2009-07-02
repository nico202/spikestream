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

//SpikeStream includes
#include "PatternManager.h"
#include "Debug.h"
#include "Utilities.h"
#include "PatternTypes.h"
#include "GlobalVariables.h"

//Qt includes
#include <qstringlist.h>
#include <qfile.h>
//Added by qt3to4:
#include <Q3TextStream>

//Other includes
#include <iostream>
using namespace std;
using namespace mysqlpp;


/*! Constructor. */
PatternManager::PatternManager(DBInterface *pattDBInter){
	//Store reference to database
	patternDBInterface = pattDBInter;
}


/*! Destructor. */
PatternManager::~ PatternManager(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING PATTERN MANAGER"<<endl;
	#endif//MEMORY_DEBUG
}


//--------------------------------------------------------------------------------
//----------------------------- PUBLIC METHODS -----------------------------------
//--------------------------------------------------------------------------------

/*! Loads test spiking patterns into database.
	These are specified as a qstringlist of absolute file locations. */
bool PatternManager::addPattern(QString fileName){
	//Check filename is not too long
	if(fileName.length() > MAX_DATABASE_NAME_LENGTH){
                cerr<<"PatternManager: FILENAME \""<<fileName.toStdString()<<"\" IS TOO LONG TO BE STORED IN DATABASE"<<endl;
		return false;
	}

	//Create file with fileName
	QFile file(fileName);
	
	//Initialise variables for pattern
	unsigned int type = 0, width = 0, height = 0;
	QString description = "";
	int patternGrpID = -1, patternCount = 0;

	//Read in pattern
	if ( file.open( QIODevice::ReadOnly ) ) {
		Q3TextStream stream( &file );
		QString fileLine;

		while ( !stream.atEnd() ) {
			fileLine = stream.readLine(); // line of text excluding '\n'
			#ifdef LOAD_PATTERN_DEBUG
				cout<<"Reading line: \""<<fileLine<<"\""<<endl;
			#endif//LOAD_PATTERN_DEBUG

			//Extract information about pattern
			if(fileLine.upper().startsWith("# TYPE:") || fileLine.upper().startsWith("#TYPE:") ){
				if(fileLine.section(": ", 1, 1) == "static")
					type = STATIC_PATTERN_VALUE;
				else if (fileLine.section(": ", 1, 1) == "temporal")
					type = TEMPORAL_PATTERN_VALUE;
				else{
					cerr<<"PatternManager: PATTERN TYPE NOT RECOGNIZED!"<<endl;
					return false;
				}
			}
			else if( fileLine.upper().startsWith("# WIDTH:") || fileLine.upper().startsWith("#WIDTH:") )
				width = Utilities::getUInt(fileLine.section(": ", 1, 1).ascii());
			else if( fileLine.upper().startsWith("# HEIGHT:") || fileLine.upper().startsWith("#HEIGHT:") )
				height = Utilities::getUInt(fileLine.section(": ", 1, 1).ascii());
			else if(fileLine.upper().startsWith("# DESCRIPTION:") || fileLine.upper().startsWith("#DESCRIPTION:") ){
				description = fileLine.section(": ", 1, 1);
				if(description.length() > MAX_DATABASE_NAME_LENGTH){
                                        cerr<<"PatternManager: DESCRIPTION \""<<description.toStdString()<<"\" IS TOO LONG TO BE STORED IN DATABASE"<<endl;
					return false;
				}
			}
			
			//Store the pattern description and location
			if(patternGrpID == -1 && type > 0 && width > 0 && height > 0 && description != ""){//All information about pattern has been found
				#ifdef LOAD_PATTERN_DEBUG
					cout<<"Storing pattern: "<<description<<" from "<<fileName<<endl;
				#endif//LOAD_PATTERN_DEBUG

				patternGrpID = storePatternDescription(description, type, width, height, fileName);
				if(patternGrpID == -1)//Pattern is already stored, assume that pattern data has been stored too.
					return true;
				else if(patternGrpID == -2)//Error during storing 
					return false;
			}
			
			/* Store pattern data. Only do this if have successfully
				stored pattern description */
			if(fileLine.upper().startsWith("# PATTERN DATA") || fileLine.upper().startsWith("#PATTERN DATA") ){
				if(patternGrpID > -1){
					QString patternLine = "";
					//Read in lines containing the pattern
					for(unsigned int i=0; i<height; ++i){
						fileLine = stream.readLine();
						QStringList numberList = QStringList::split(" ", fileLine);
						fileLine = numberList.join(",");
						fileLine += ",";
						patternLine += fileLine;
					}
					//Trim the last comma
					patternLine.truncate(patternLine.length() - 1);
	
					//Write the pattern to the database
					storePatternData(patternGrpID, patternLine);
					++patternCount;
				}
				else{//Pattern description has not been stored, but we have started reading in pattern data
                                        cerr<<"PatternManager: ERROR IN PATTERN FILE. HAVE NOT FOUND ONE OR MORE OF TYPE, WIDTH, HEIGHT, DESCRIPTION PRIOR TO START OF PATTERN DATA. FILENAME = "<<fileName.toStdString()<<endl;
					return false;
				}
			}
		}
		//Store the number of loaded patterns
		storePatternCount(patternGrpID, patternCount);
		file.close();
	}
	else{
                cerr<<"PatternManager: ERROR OPENING FILE "<<fileName.toStdString()<<endl;
		return false;
	}

	//If we have reached this point without storing anything, something must have gone wrong with pattern loading
	if(patternGrpID == -1){
		cerr<<"PatternManager: FAILED TO STORE PATTERN DESCRIPTION. PROBABLY AN ERROR IN THE PATTERN INPUT FILE"<<endl;
		return false;
	}
	if(patternCount == 0){
		cerr<<"PatternManager: EMPTY PATTERN. PROBABLY AN ERROR IN THE PATTERN INPUT FILE"<<endl;
		return false;
	}


	//If we have reached this point everything should be ok.
	#ifdef LOAD_PATTERN_DEBUG
		cout<<"Pattern loaded successfully from file \""<<fileName<<"\""<<endl;
	#endif//LOAD_PATTERN_DEBUG
	return true;
}


/*! Deletes the specified pattern from the database.
	NOTE Exception handling should be done by the invoking method. */
bool PatternManager::deletePattern(unsigned int patternID){
	//Get query and create variable to record errors
	Query query = patternDBInterface->getQuery();
	bool deleteSuccess = true;

	//Delete description of pattern
	query.reset();
	query<<"DELETE FROM PatternDescriptions WHERE PatternGrpID = "<<patternID;
        SimpleResult queryResult = query.execute();
        if(!queryResult){
		cerr<<"PatternManager: FAILED TO DELETE PATTERN WITH ID"<<patternID<<" FROM PatternDescriptions"<<endl;
		deleteSuccess = false;
	}

	//Delete pattern data
	query.reset();
	query<<"DELETE FROM PatternData WHERE PatternGrpID = "<<patternID;
	queryResult = query.execute();
        if(!queryResult){
		cerr<<"PatternManager: FAILED TO DELETE PATTERN WITH ID"<<patternID<<" FROM PatternData"<<endl;
		deleteSuccess = false;
	}

	//Return status
	return deleteSuccess;
}


//------------------------------------------------------------------------------------------------------
//------------------------------------ PRIVATE METHODS -------------------------------------------------
//------------------------------------------------------------------------------------------------------

/*! Returns true if the pattern has already been stored in the database. */
bool PatternManager::patternAlreadyStored(QString fileName){
        StoreQueryResult result;
	Query query = patternDBInterface->getQuery();
	query.reset();
        query<<"SELECT COUNT(*) FROM PatternDescriptions WHERE File = \""<<fileName.toStdString()<<"\"";
	try{
		result = query.store();
	}
	catch(Exception ex){
		cerr<<"PatternManager: EXCEPTION THROWN CHECKING IF PATTERN WAS STORED: "<<ex.what()<<endl;
		return true;//Stops any further attempt to save pattern
	}

	//Check that query went ok
	if(result.size() < 1){
		cerr<<"PatternManager: UNSUCCESSFUL SEARCH FOR PATTERN DESCRIPTION"<<endl;
		return false;
	}

	//Extract count from result
	Row row(*result.begin());
	unsigned int numberOfRows = Utilities::getUInt((std::string)row["COUNT(*)"]);
	if(numberOfRows > 0)
		return true;
	else{
		return false;
	}
}


/*! Stores a count of the number of patterns in the database to save counting the
	number of rows every time the application starts up. */
void PatternManager::storePatternCount(unsigned int patternGrpID, int patternCount){
	Query query = patternDBInterface->getQuery();
	query.reset();
	query<<"UPDATE PatternDescriptions SET NumberOfPatterns = "<<patternCount<<" WHERE PatternGrpID = "<<patternGrpID;
        SimpleResult queryResult = query.execute();
        if(!queryResult){
		cerr<<"PatternManager: UNSUCCESSFUL MYSQLPP QUERY STORING PATTERN DESCRIPTION"<<endl;
	}
}


/*! Stores the pattern data. */
bool PatternManager::storePatternData(unsigned int patternGrpID, QString patternString){
	//Set up query
	Query patternQuery = patternDBInterface->getQuery();
	patternQuery.reset();

	//Set up string for writing
	string stemp (patternString.ascii(), patternString.length()) ;
	ostringstream strbuf;
        strbuf <<"INSERT INTO PatternData(PatternGrpID, Pattern) VALUES ( "<<patternGrpID<<", "<<mysqlpp::quote<<stemp<<" )";
	try{
		bool queryOk = patternQuery.exec(strbuf.str());
		if(!queryOk){
			cerr<<"PatternManager: UNSUCCESSFUL MYSQLPP QUERY: "<<strbuf.str()<<endl;
			return false;
		}
	}
	catch(Exception ex){
		cerr<<"PatternManager: EXCEPTION THROWN BY UNSUCCESSFUL MYSQLPP QUERY: "<<strbuf.str()<<ex.what()<<endl;
		return false;
	}
	return true;
}


/*! Stores a description of the pattern when the pattern is being loaded. Should be called
	before storing the data. */
int PatternManager::storePatternDescription(QString description, unsigned short patternType, unsigned int width, unsigned int length, QString fileName){
	//First check to see if pattern is already stored
	if(patternAlreadyStored(fileName)){
		#ifdef LOAD_PATTERN_DEBUG
			cout<<"Pattern from file "<<fileName<<" is already stored."<<endl;
		#endif//LOAD_PATTERN_DEBUG
		return -1;//Don't want to store this pattern twice
	}

	Query query = patternDBInterface->getQuery();
	query.reset();
        query<<"INSERT INTO PatternDescriptions(Description, PatternType, Width, Length, File) VALUES ( \""<<description.toStdString()<<"\", "<<patternType<<", "<<width<<", "<<length<<",\""<<fileName.toStdString()<<"\")";
        SimpleResult queryResult = query.execute();
        if(!queryResult){
		cerr<<"PatternManager: UNSUCCESSFUL MYSQLPP QUERY STORING PATTERN DESCRIPTION"<<endl;
		return -2;
	}
	else{
                return queryResult.insert_id();
	}
}



