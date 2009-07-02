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
#include "ConnectionMatrixLoader.h"
#include "Debug.h"
#include "Utilities.h"
#include "ConnectionType.h"
#include "NeuronGroupType.h"

//Qt includes 
#include <qmessagebox.h>
#include <qfile.h>
//Added by qt3to4:
#include <Q3TextStream>

//Other includes
#include <mysql++.h>
#include <cmath>
using namespace std;
using namespace mysqlpp;


//Local debug defines
#define LOAD_CONNECTION_MATRIX_DEBUG


/*! Constructor. */
ConnectionMatrixLoader::ConnectionMatrixLoader(DBInterface* netDBInter){
	//Store references
	networkDBInterface = netDBInter;
}


/*! Destructor. */
ConnectionMatrixLoader::~ConnectionMatrixLoader(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING CONNECTION MATRIX LOADER"<<endl;
	#endif//MEMORY_DEBUG
}


//--------------------------------------------------------------------------------
//---------                     PUBLIC METHODS                           ---------
//--------------------------------------------------------------------------------

/*! Loads a connection matrix from the file into the database. */
bool ConnectionMatrixLoader::loadConnectionMatrix(QString& fileName){
	//Initialise class variables
	neurGrpID = 0;
	connGrpID = 0;
	neuronType = 0;
	synapseType = 0;
	neurGrpWidth = 0;
	neurGrpLength = 0;
	neurParamTable = "";
	numNeur = 0;
	connectionCount = 0;
	rowNum = 0;
	errorState = false;
	
	//Need to create neuron and connection groups once we know size
	bool firstLine = true;

	//Create file with fileName
	QFile file(fileName);

	//Read in pattern
	if ( file.open( QIODevice::ReadOnly ) ) {
		Q3TextStream stream( &file );
		QString fileLine;

		while ( !stream.atEnd() && !errorState) {
			fileLine = stream.readLine(); // line of text excluding '\n'
			#ifdef LOAD_CONNECTION_MATRIX_DEBUG
                                cout<<"Reading line: \""<<fileLine.toStdString()<<"\""<<endl;
			#endif//LOAD_PATTERN_DEBUG

			//Get the line and split it into neuron ids.
			QStringList neuronList = QStringList::split(",", fileLine);

			#ifdef LOAD_CONNECTION_MATRIX_DEBUG
				cout<<"Matrix contains "<<numNeur<<" neurons."<<endl;
			#endif//LOAD_PATTERN_DEBUG

			//Set up neuron and connection groups
			if(firstLine){
				//Store number of neurons
				numNeur = neuronList.size();

				//Create the neuron group
				createNeuronGroup();
				if(neurGrpID == 0 || errorState){//An error has occured
					file.close();
					deleteDatabaseEntries(neurGrpID, connGrpID);
					return false;
				}
	
				//Create connection group to add connections
				createConnectionGroup(fileName);
				if(connGrpID == 0 || errorState){//An error has occured
					file.close();
					deleteDatabaseEntries(neurGrpID, connGrpID);
					return false;
				}

				//Have now set up appropriate neuron and connection groups
				firstLine = false;
			}

			//Add connections from first line that has been read in
			addConnections(neuronList);
			++rowNum;

		}
		//Close file
		file.close();

		//Check that matrix is square
		if(rowNum != numNeur){
			deleteDatabaseEntries(neurGrpID, connGrpID);
			showError("ConnectionMatrixLoader: Matrix not square");
			return false;
		}

		//Check that we have not encountered any errors
		if(errorState){
			deleteDatabaseEntries(neurGrpID, connGrpID);
			return false;
		}

		/* Check that we have added at least one connection.
			If not, get rid of connection group entry because it is not needed.*/
		try{
			Query query = networkDBInterface->getQuery();
			if(connectionCount == 0){
				query.reset();
				query<<"DELETE FROM ConnectionGroups WHERE ConnGrpID = "<<connGrpID;
				query.execute();
			}
			else{//Add entry to parameter table for this connection group.*/
				//Find the appropriate table
				query.reset();
				query<<"SELECT ParameterTableName FROM SynapseTypes WHERE TypeID = "<<synapseType;
                                StoreQueryResult tableNameRes = query.store();
				Row tableNameRow (*tableNameRes.begin());//Should only be 1 row
			
				//Add an entry for this connection group to the appropriate table
				query.reset();
				query<<"INSERT INTO "<<(std::string)tableNameRow["ParameterTableName"]<<" (ConnGrpID) VALUES ("<<connGrpID<<")";
				query.execute();
			}
		}
		catch (const BadQuery& er) {// Handle any query errors
			QString errorString = "ConnectionMatrixLoader: MySQL++ BadQuery thrown creating neuron group: \"";
			errorString += er.what();
			errorString += "\"";
			showError(errorString);
			deleteDatabaseEntries(neurGrpID, connGrpID);
			return false;
		}
		catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
			QString errorString = "ConnectionMatrixLoader: MySQL++ Exception thrown creating neuron group: \"";
			errorString += er.what();
			errorString += "\"";
			showError(errorString);
			deleteDatabaseEntries(neurGrpID, connGrpID);
			return false;
		}
		catch(std::exception& er){// Catch-all for any other exceptions
			QString errorString = "ConnectionMatrixLoader: std::exception thrown creating neuron group\"";
			errorString += er.what();
			errorString += "\"";
			showError(errorString);
			deleteDatabaseEntries(neurGrpID, connGrpID);
			return false;
		}
		
		#ifdef LOAD_CONNECTION_MATRIX_DEBUG
			cout<<"Connection matrix loading complete. "<<connectionCount<<" connections loaded. "<<endl;
		#endif//LOAD_PATTERN_DEBUG
	}
	else{
		showError("ConnectionMatrixLoader: Error opening file.");
		return false;
	}

	//Everything should be ok if we have got to this point.
	return true;
}


//--------------------------------------------------------------------------------
//---------                     PRIVATE METHODS                          ---------
//--------------------------------------------------------------------------------

/*! Adds the connections to the database.	*/
void ConnectionMatrixLoader::addConnections(QStringList& neuronIDList){
	try{
		Query query = networkDBInterface->getQuery();
		double connWeight;

		/* Work through all the connections between the current row number and the
			column numbers.*/
		for(unsigned int colNum=0; colNum<neuronIDList.size(); ++colNum){
			connWeight = Utilities::getDouble(neuronIDList[colNum].ascii());//Weight between 0.0 and 1.0
			connWeight *= 127.0;//Weight between 0.0 and 127.0
			if(connWeight > 0){
				/* Add a connection. This is between the neuron id at the current row 
					and the neuron id at colNum.*/
				query.reset();
				query<<"INSERT INTO Connections (PreSynapticNeuronID, PostSynapticNeuronID, Delay, Weight, ConnGrpID) VALUES ("<<(startNeurID + rowNum)<<", "<<(startNeurID + colNum)<<", "<<0<<", "<<connWeight<<", "<<connGrpID<<")";
				query.execute();
				++connectionCount;
			}
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		QString errorString = "ConnectionMatrixLoader: MySQL++ BadQuery thrown creating neuron group: \"";
		errorString += er.what();
		errorString += "\"";
		showError(errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		QString errorString = "ConnectionMatrixLoader: MySQL++ Exception thrown creating neuron group: \"";
		errorString += er.what();
		errorString += "\"";
		showError(errorString);
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		QString errorString = "ConnectionMatrixLoader: std::exception thrown creating neuron group\"";
		errorString += er.what();
		errorString += "\"";
		showError(errorString);
	}
}


/*! Creates a connection group internal to the specified neuron group.
	Returns the connection group id.*/
void ConnectionMatrixLoader::createConnectionGroup(const QString& fileName){
	/* Create the xml string to hold the parameters of the connection.
		This holds all the information about the connection that is not included elsewhere
		in the connection group table.*/
	ostringstream xmlStrStream;
	xmlStrStream<<"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
	xmlStrStream<<"<connection_parameters>";

	//Add file name as the only parameter
	xmlStrStream<<"<parameter>";
	xmlStrStream<<"<description>"<<"Connection Matrix file name"<<"</description>";
        xmlStrStream<<"<value>"<<fileName.toStdString()<<"</value>";
	xmlStrStream<<"</parameter>";
	xmlStrStream<<"</connection_parameters>";

	try{
		Query query = networkDBInterface->getQuery();

		//Find the first synapse type
		query.reset();
		query<<"SELECT MIN(TypeID) from SynapseTypes";
                StoreQueryResult minTypeIDRes = query.store();
		Row minTypeIDRow(*(minTypeIDRes.begin()));
		synapseType = Utilities::getUInt((std::string)minTypeIDRow["MIN(TypeID)"]);

		//Create connection group
		query.reset();
                query<<"INSERT INTO ConnectionGroups (FromNeuronGrpID, ToNeuronGrpID, ConnType, SynapseType, Parameters) VALUES ("<<neurGrpID<<", "<<neurGrpID<<", "<<ConnectionType::SimpleCortex<<", "<<synapseType<<", "<<mysqlpp::quote<<xmlStrStream.str()<<" )";
                query.exec();
		
		//Get the automatically generated ConnGrpID so that it can be added to the new connections
		query.reset();
		query<<"SELECT MAX(ConnGrpID) from ConnectionGroups";
                StoreQueryResult grpIDResult = query.store();
		Row row(*(grpIDResult.begin()));
		connGrpID = Utilities::getUInt((string)row["MAX(ConnGrpID)"]);
	}
	catch (const BadQuery& er) {// Handle any query errors
		QString errorString = "ConnectionMatrixLoader: MySQL++ BadQuery thrown creating neuron group: \"";
		errorString += er.what();
		errorString += "\"";
		showError(errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		QString errorString = "ConnectionMatrixLoader: MySQL++ Exception thrown creating neuron group: \"";
		errorString += er.what();
		errorString += "\"";
		showError(errorString);
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		QString errorString = "ConnectionMatrixLoader: std::exception thrown creating neuron group\"";
		errorString += er.what();
		errorString += "\"";
		showError(errorString);
	}
}


/*! Creates a neuron group and its associated neurons.
	Returns the neuron id.*/
void ConnectionMatrixLoader::createNeuronGroup(){
	Query query = networkDBInterface->getQuery();
	int neuronCount = 0;
	try{
		Query query = networkDBInterface->getQuery();

		//Work out the width and length of the neuron group - ideally make it square
		neurGrpWidth = (unsigned int)floor(sqrt(numNeur));
		neurGrpLength = numNeur/neurGrpWidth;

		//Double check width and length make sense
		if( (neurGrpWidth * neurGrpLength) != numNeur){
			showError("Neuron width * length does not equal number of neurons.");
			return;
		}

		#ifdef LOAD_CONNECTION_MATRIX_DEBUG
			cout<<"Neuron group width =  "<<neurGrpWidth<<"; neuron group length = "<<neurGrpLength<<endl;
		#endif//LOAD_PATTERN_DEBUG

		//Get all layers with this z coordinate
		query.reset();
		query<<"SELECT NeuronID FROM Neurons WHERE X >="<<0<<" AND X <= "<<neurGrpWidth<<" AND Y >= "<<0<<" AND Y <= "<<neurGrpLength<<" AND Z = "<<0;
                StoreQueryResult zLayerResult = query.store();
		if(zLayerResult.size() > 0){
			showError("ConnectionMatrixLoader: CONFLICT WITH EXISTING LAYERS");
			return;
		}
	
		//Only reach this point if there are no conflicts with existing layers
		//Get the first neuron type in the NeuronTypes database
		query.reset();
		query<<"SELECT MIN(TypeID) from NeuronTypes";
                StoreQueryResult minTypeIDRes = query.store();
		Row minTypeIDRow(*(minTypeIDRes.begin()));
		neuronType = Utilities::getUInt((std::string)minTypeIDRow["MIN(TypeID)"]);

		//Add layer entry to layer database
		query.reset();
		query<<"INSERT INTO NeuronGroups (Name, NeuronGrpType, NeuronType, X, Y, Z, Width, Length, Spacing, TaskID) VALUES (\""<<"Untitled"<<"\", "<<NeuronGroupType::RectangularLayer2D<<", "<<neuronType<<", "<<0<<", "<<0<<", "<<0<<", "<<neurGrpWidth<<", "<<neurGrpLength<<", "<<1<<", -1)";
		query.execute();
		
		//Now need to get the automatically generated NeuronGrpID so that it can be added to the neurons in the new layers
		query.reset();
		query<<"SELECT MAX(NeuronGrpID) from NeuronGroups";
                StoreQueryResult neurGrpIDRes = query.store();
		Row neurGrpIDRow(*(neurGrpIDRes.begin()));
		neurGrpID = Utilities::getUInt((std::string)neurGrpIDRow["MAX(NeuronGrpID)"]);

		/* Add neurons to neuron database. 
			This adds them in a scanning pattern moving horizontally along the x axis 
			before moving to the next line */
		for(unsigned int i=0; i<neurGrpLength; ++i){
			for(unsigned int j=0; j<neurGrpWidth; ++j){
				query.reset();
				query<<"INSERT INTO Neurons (X, Y, Z, NeuronGrpID) VALUES ("<<j<<", "<<i<<", "<<0<<", "<<neurGrpID<<")";
				query.execute();
				++neuronCount;
			}
		}

		//Get the lowest neuron id in this group.
		query.reset();
		query<<"SELECT MIN(NeuronID) from Neurons WHERE NeuronGrpID = "<<neurGrpID;
                StoreQueryResult startIDRes = query.store();
		Row startIDRow(*(startIDRes.begin()));
		startNeurID = Utilities::getUInt((std::string)startIDRow["MIN(NeuronID)"]);

		//Add parameter and noise entries for this neuron group
		//Find the appropriate table
		query.reset();
		query<<"SELECT ParameterTableName FROM NeuronTypes WHERE TypeID = "<<neuronType;
                StoreQueryResult tableNameRes = query.store();
		Row tableNameRow (*tableNameRes.begin());//Should only be 1 row
                neurParamTable = ((std::string)tableNameRow["ParameterTableName"]).data();
		
		//Now add an entry for this neuron group to the appropriate table
		query.reset();
                query<<"INSERT INTO "<<neurParamTable.toStdString()<<" (NeuronGrpID) VALUES ("<<neurGrpID<<")";
		query.execute();
	
		//Add entry for this layer to the noise parameters table
		query.reset();
		query<<"INSERT INTO NoiseParameters (NeuronGrpID) VALUES ("<<neurGrpID<<")";
		query.execute();

	}
	catch (const BadQuery& er) {// Handle any query errors
		QString errorString = "ConnectionMatrixLoader: MySQL++ BadQuery thrown creating neuron group: \"";
		errorString += er.what();
		errorString += "\"";
		showError(errorString);
		deleteDatabaseEntries(neurGrpID, 0);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		QString errorString = "ConnectionMatrixLoader: MySQL++ Exception thrown creating neuron group: \"";
		errorString += er.what();
		errorString += "\"";
		showError(errorString);
		deleteDatabaseEntries(neurGrpID, 0);
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		QString errorString = "ConnectionMatrixLoader: std::exception thrown creating neuron group\"";
		errorString += er.what();
		errorString += "\"";
		showError(errorString);
		deleteDatabaseEntries(neurGrpID, 0);
	}

	#ifdef LOAD_CONNECTION_MATRIX_DEBUG
		cout<<neuronCount<<" neurons added to database."<<endl;
	#endif//LOAD_CONNECTION_MATRIX_DEBUG
}


/*! Cleans up the database when an error occurs.*/
void ConnectionMatrixLoader::deleteDatabaseEntries(unsigned int nGrpID, unsigned int cGrpID){
	try{
		Query query = networkDBInterface->getQuery();

		//Check to see if neuron group has been created.
		if(neurGrpID > 0){
			//Delete neurons
			query.reset();
			query<<"DELETE FROM Neurons WHERE NeuronGrpID = "<<nGrpID;
			query.execute();

			//Delete neuron group
			query.reset();
			query<<"DELETE FROM NeuronGroups WHERE NeuronGrpID = "<<nGrpID;
			query.execute();

			//Delete parameters if they have been added
			if(neurParamTable != ""){
				query.reset();
                                query<<"DELETE FROM "<<neurParamTable.toStdString()<<" WHERE NeuronGrpID = "<<nGrpID;
				query.execute();
			}

			//Delete entry from noise table
			query.reset();
			query<<"DELETE FROM NoiseParameters WHERE NeuronGrpID = "<<nGrpID;
			query.execute();

			//Reset neurGrpID
			neurGrpID = 0;
		}
	
		//Check to see if connection group has been created
		if(connGrpID > 0){
			//Delete connections
			query.reset();
			query<<"DELETE FROM Connections WHERE ConnGrpID = "<<cGrpID;
			query.execute();

			//Delete connection group
			query.reset();
			query<<"DELETE FROM ConnectionGroups WHERE ConnGrpID = "<<cGrpID;
			query.execute();

			//Reset connGrpID
			connGrpID = 0;
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		QString errorString = "ConnectionMatrixLoader: MySQL++ BadQuery thrown deleting database entries: \"";
		errorString += er.what();
		errorString += "\"";
		showError(errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		QString errorString = "ConnectionMatrixLoader: MySQL++ Exception thrown deleting database entries: \"";
		errorString += er.what();
		errorString += "\"";
		showError(errorString);
	}
	catch(std::exception& er){// Catch-all for any other exceptions
		QString errorString = "ConnectionMatrixLoader: std::exception thrown deleting database entries: \"";
		errorString += er.what();
		errorString += "\"";
		showError(errorString);
	}
}


/*! Writes error message to std out and shows message box with error.
	NOTE Not thread safe.*/
void ConnectionMatrixLoader::showError(const char* errMsg){
	cerr<<errMsg<<endl;
	QMessageBox::critical( 0, "Connection Matrix Loader Error", errMsg);
	errorState = true;
}


/*! Writes error message to std out and shows message box with error.
	NOTE Not thread safe.*/
void ConnectionMatrixLoader::showError(const QString& errMsg){
        cerr<<errMsg.toStdString()<<endl;
	QMessageBox::critical( 0, "Connection Matrix Loader Error", errMsg);
	errorState = true;
}

