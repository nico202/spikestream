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
#include "ConnectionManager.h"
#include "ConnectionType.h"
#include "Utilities.h"
#include "Debug.h"
#include "SpikeStreamMainWindow.h"

//Qt includes
#include <qstringlist.h>
#include <qregexp.h>

//Other includes
#include <mysql++.h>
#include <cmath>
#include <iostream>
using namespace std;
using namespace mysqlpp;


/*! Exception thrown when there is an error creating connections. */
class CreateConnectionException : public exception{
  virtual const char* what() const throw(){
    return "Connection creation error.";
  }
} createConnectionException;


/*! Constructor. */
ConnectionManager::ConnectionManager(DBInterface *netDBInter, DBInterface* devDBInter){
	//Store references to database interfaces
	networkDBInterface = netDBInter;
	deviceDBInterface = devDBInter;

	//Set up a short version of spike stream application reference
	spikeStrApp = SpikeStreamMainWindow::spikeStreamApplication;
	
	//Initialize the random number generator with the seed
	srand(randomSeed);
	
	//Create the progress dialog to provide feedback about connection creation progress
	progressDialog =  new Q3ProgressDialog(0, "Creating Connections", true);
}


/*! Destructor. */
ConnectionManager::~ConnectionManager(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING CONNECTION MANAGER"<<endl;
	#endif//MEMORY_DEBUG

	delete progressDialog;
}


//-------------------------------------------------------------------------------------
//---------------------------- PUBLIC METHODS -----------------------------------------
//-------------------------------------------------------------------------------------

/*! Creates a new connection group of the type specified in the connection holder
	Only one connection group of each type can exist between two neuron groups.
	Only one from-to and one to-from connection can exist between two neurons. */
int ConnectionManager::createConnections(ConnectionHolder connHolder){
	//First check to see if there are any conflicts with existing connection groups
	Query query = networkDBInterface->getQuery();
	query.reset();
	query<<"SELECT ConnGrpID FROM ConnectionGroups WHERE FromNeuronGrpID = "<<connHolder.fromLayerID<<" AND ToNeuronGrpID = "<<connHolder.toLayerID<<" AND ConnType = "<<connHolder.connectionType;
        StoreQueryResult connGrpResult = query.store();
	if(connGrpResult.size() > 0){
		cerr<<"CONFLICT WITH EXISTING CONNECTION GROUP"<<endl;
		return -1;
	}
	
	/* Next add the connectionGroup entry to the database. This may have to be deleted later 
		if no connections are created, but need the connectionGrpID to create the connections.*/

	/* Create the xml string to hold the parameters of the connection.
		This holds all the information about the connection that is not included elsewhere
		in the connection group table.*/
	ostringstream xmlStrStream;
	xmlStrStream<<"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
	xmlStrStream<<"<connection_parameters>";
	for(map<string, double>::iterator iter = connHolder.paramMap.begin(); iter != connHolder.paramMap.end(); ++iter){
		xmlStrStream<<"<parameter>";
		xmlStrStream<<"<description>"<<iter->first<<"</description>";
		xmlStrStream<<"<value>"<<iter->second<<"</value>";
		xmlStrStream<<"</parameter>";
	}
	xmlStrStream<<"<min_delay>"<<connHolder.minDelay<<"</min_delay>";
	xmlStrStream<<"<max_delay>"<<connHolder.maxDelay<<"</max_delay>";
	xmlStrStream<<"</connection_parameters>";

	query.reset();
        query<<"INSERT INTO ConnectionGroups (FromNeuronGrpID, ToNeuronGrpID, ConnType, SynapseType, Parameters) VALUES ("<<connHolder.fromLayerID<<", "<<connHolder.toLayerID<<", "<<connHolder.connectionType<<", "<<connHolder.synapseType<<", "<<mysqlpp::quote<<xmlStrStream.str()<<" )";
        query.exec();
	
	//Now need to get the automatically generated ConnGrpID so that it can be added to the new connections
	query.reset();
	query<<"SELECT MAX(ConnGrpID) from ConnectionGroups";
        StoreQueryResult grpIDResult = query.store();
	Row row(*(grpIDResult.begin()));
	connHolder.connectionGrpID = Utilities::getUInt((string)row["MAX(ConnGrpID)"]);
	
	/*Next create connections. Since there can only be one connection between any two 
		neurons, the connections between layers may be saturated by other types of 
		connection. If at least one pair of neurons are connected, the connection will 
		be created. Otherwise it will be discarded. Connection count is used to check this. */
	int connectionCount = 0;
	
	//Create appropriate connections	
	//Work on assumption that ConnectionPropertiesDialog has done checking on compatibility etc.
	switch(connHolder.connectionType){
		case (ConnectionType::SimpleCortex):
			createSimpleCortexConns(connHolder, connectionCount);
			break;
		case (ConnectionType::UnstructuredExInhibIntra):
			createUnstructuredExInhibConnections(connHolder, connectionCount);
			break;
		case (ConnectionType::OnCentreOffSurround):
			createOnCentreOffSurroundConns(connHolder, connectionCount);
			break;
		case (ConnectionType::OffCentreOnSurround): 
			createOffCentreOnSurroundConns(connHolder, connectionCount);
			break;
		case (ConnectionType::Unstructured):
			createUnstructuredConns(connHolder, connectionCount);
			break;
		case (ConnectionType::UnstructuredExInhibInter):
			createUnstructuredExInhibConnections(connHolder, connectionCount);
			break;
		case (ConnectionType::Topographic):
			createTopographicConns(connHolder, connectionCount);
			break;
		case (ConnectionType::SIMNOSComponent):
			createSIMNOSComponentConns(connHolder, connectionCount);
			break;	
		case (ConnectionType::Virtual):
			//Nothing to be done here. Virtual connections just have an entry in the connection properties table
		break;
		default:
			cerr<<"CONNECTION TYPE NOT RECOGNIZED"<<endl;
			return -1;
	}
	
	//Delete entry from ConnectionGroup in the database if no connections have been created
	if(connectionCount == 0 && connHolder.connectionType != ConnectionType::Virtual){
		//Delete information about this connection group
		query.reset();
		query<<"DELETE FROM ConnectionGroups WHERE ConnGrpID = "<<connHolder.connectionGrpID;
		query.execute();
		return -1;
	}
	else if(connHolder.connectionType != ConnectionType::Virtual && connHolder.connectionType != ConnectionType::TempVirtual){
		/* Add entry for this layer to the appropriate parameters database */
		//Find the appropriate table
		query.reset();
		query<<"SELECT ParameterTableName FROM SynapseTypes WHERE TypeID = "<<connHolder.synapseType;
                StoreQueryResult tableNameRes = query.store();
		Row tableNameRow (*tableNameRes.begin());//Should only be 1 row
	
		//Now add an entry for this connection group to the appropriate table
		query.reset();
		query<<"INSERT INTO "<<(std::string)tableNameRow["ParameterTableName"]<<" (ConnGrpID) VALUES ("<<connHolder.connectionGrpID<<")";
		query.execute();
	}

	//Return new connection group ID
	return connHolder.connectionGrpID;
}


/*! Deletes connections from the ConnectionGroups table and the Connections table. */
void ConnectionManager::deleteConnections(vector<ConnectionHolder> connectionVector){
	Query query = networkDBInterface->getQuery();
	
	vector<ConnectionHolder>::iterator iter;
	for(iter = connectionVector.begin(); iter < connectionVector.end(); ++iter){
		//First get the connection ID from the connection table
		query.reset();
		query<<"SELECT ConnGrpID FROM ConnectionGroups WHERE FromNeuronGrpID="<<iter->fromLayerID<<" AND ToNeuronGrpID = "<<iter->toLayerID<<" AND ConnType = "<<iter->connectionType;
                StoreQueryResult result = query.store();
		Row row(*(result.begin()));//ConnGrpID should be unique
		unsigned int connectionGroupID = Utilities::getUInt((std::string)row["ConnGrpID"]);
		
		//Now use connection ID to delete all connections with this ID
		query.reset();
		query<<"DELETE FROM Connections WHERE ConnGrpID = "<<connectionGroupID;
		query.execute();
		
		//Delete information about this connection from the connection table
		query.reset();
		query<<"DELETE FROM ConnectionGroups WHERE ConnGrpID = "<<connectionGroupID;
		query.execute();

		/*Delete synapse parameters for this connection group
			Simpler just to delete connection group from all parameter tables than to query the
			 id and then delete it from the appropriate table */
		query.reset();
		query<<"SELECT ParameterTableName FROM SynapseTypes";
                StoreQueryResult tableNameRes = query.store();
                for(StoreQueryResult::iterator paramTableIter = tableNameRes.begin(); paramTableIter != tableNameRes.end(); ++paramTableIter){
			Row tableNameRow (*paramTableIter);
			query.reset();
			query<<"DELETE FROM "<<(std::string)tableNameRow["ParameterTableName"]<<" WHERE ConnGrpID= "<<connectionGroupID;
			query.execute();
		}
	}
}


//------------------------------------------------------------------------------------------
//-------------------------------- PRIVATE METHODS -----------------------------------------
//------------------------------------------------------------------------------------------

/*! Method that checks to see whether a connection between these neurons already exists in the database
	At present only a single from, to connection can be made between two neurons.
	This could be easily changed at a later point by making the primary key of the Connections table
	more complex so that it includes the connection group or synapse type, for example.
	In this first version things are being kept simple. */
bool ConnectionManager::connectionExists(mysqlpp::Query &query, unsigned int fromID, unsigned int toID){
	query.reset();
	query<<"SELECT * FROM Connections WHERE PreSynapticNeuronID = "<<fromID<<" AND PostSynapticNeuronID = "<<toID;
        StoreQueryResult result = query.store();
	if(result.size() > 0)//Connection does exist
		return true;
	return false;//No connection found, return false
}


/*! OFF CENTRE ON SURROUND CONNECTIONS
	FIXME NOT IMPLEMENTED AT PRESENT. */
void ConnectionManager::createOffCentreOnSurroundConns(ConnectionHolder &, int &){
}


/*! ON CENTRE OFF SURROUND CONNECTIONS
	Method that creates on centre off surround connections. 
	These are rectangular in shape at present. In the future it would be nice to add a circular option
	Basic strategy for this function is to work through each of the to neurons, select the neurons
	in the from layer that are within the connection area, work through this selection and then
	create the connection. */
void ConnectionManager::createOnCentreOffSurroundConns(ConnectionHolder &connHolder, int &connectionCount){
	//To begin with get all the information about the from neuron group
	Query query = networkDBInterface->getQuery();
	query.reset();
	query<<"SELECT X, Y, Z, Width, Length FROM NeuronGroups WHERE NeuronGrpID = "<<connHolder.fromLayerID;
        StoreQueryResult fromNeuronGrpRes = query.store();
	Row fromGrpRow(*fromNeuronGrpRes.begin());//Assume that there is only one result
	int fromXPos = Utilities::getInt((std::string)fromGrpRow["X"]);
	int fromYPos = Utilities::getInt((std::string)fromGrpRow["Y"]);
	int fromZPos = Utilities::getInt((std::string)fromGrpRow["Z"]);
	int fromGrpWidth_Neur = Utilities::getInt((std::string)fromGrpRow["Width"]);
	int fromGrpLength_Neur = Utilities::getInt((std::string)fromGrpRow["Length"]);
	
	//Next get all information about the to neuron group
	query.reset();
	query<<"SELECT X, Y, Z, Width, Length, Spacing FROM NeuronGroups WHERE NeuronGrpID = "<<connHolder.toLayerID;
        StoreQueryResult toNeuronGrpRes = query.store();
	Row toGrpRow(*toNeuronGrpRes.begin());//Assume that there is only one result - ID should be unique
	int toXPos = Utilities::getInt((std::string)toGrpRow["X"]);
	int toYPos = Utilities::getInt((std::string)toGrpRow["Y"]);
	int toZPos = Utilities::getInt((std::string)toGrpRow["Z"]);
	int toGrpWidth_Neur = Utilities::getInt((std::string)toGrpRow["Width"]);
	int toGrpLength_Neur = Utilities::getInt((std::string)toGrpRow["Length"]);
	int toGrpSpacing_Pts = Utilities::getInt((std::string)toGrpRow["Spacing"]);
	
	//Start up the progress dialog. 
	progressDialog->reset();
	progressDialog->setTotalSteps(toGrpWidth_Neur*toGrpLength_Neur);
	progressDialog->setLabelText("Creating connections, please wait");
	int progressCount = 0;
	
	//Get all the parameters for the connection from the parameter map
	//These are all rounded to two decimal places to make it easier to do the selection from the 
	//from layer when there is no overlap.
	double outerWidth = connHolder.paramMap["Outer width"];
	Utilities::roundTwoDecimalPlaces(outerWidth);
	double outerLength = connHolder.paramMap["Outer length"];
	Utilities::roundTwoDecimalPlaces(outerLength); 
	double innerWidth = connHolder.paramMap["Inner width"];
	Utilities::roundTwoDecimalPlaces(innerWidth);
	double innerLength = connHolder.paramMap["Inner length"];
	Utilities::roundTwoDecimalPlaces(innerLength);
	double overlap = connHolder.paramMap["Overlap"];
	Utilities::roundTwoDecimalPlaces(overlap);
	
	//Layers may need to be rotated at 90 degrees to one another to match up
	//Do this by setting different start, finsh and increments for the to layer.
	//This looks a bit confusing, but it saves having to repeat the same code twice.
	int toStart1, toStart2, toFinish1, toFinish2, toIncrement1, toIncrement2;
	if(connHolder.paramMap["Rotate?"] == 1){//To layer has to be rotated wrt the from layer so start at bottom of Y axis and work backwards
		toStart1 = toYPos + (toGrpLength_Neur-1) * toGrpSpacing_Pts;
		toStart2 = toXPos;
		toFinish1 = toYPos - toGrpSpacing_Pts;
		toFinish2 = toXPos + toGrpWidth_Neur * toGrpSpacing_Pts;
		toIncrement1 = -toGrpSpacing_Pts;//Working backwards along the Y axis
		toIncrement2 = toGrpSpacing_Pts;//And forward along X axis
	}
	else{//Layers can be connected straightforwardly
		toStart1 = toXPos;
		toStart2 = toYPos;
		toFinish1 = toXPos + toGrpWidth_Neur * toGrpSpacing_Pts;//One increment past the width
		toFinish2 = toYPos + toGrpLength_Neur * toGrpSpacing_Pts;//One increment past the length
		toIncrement1 = toGrpSpacing_Pts;
		toIncrement2 = toGrpSpacing_Pts;
	}
	
	//Next create the connections.
	//Assume that all neurons are present in the layer, even if some are 'dead'
	//Each to neuron has to connect with a subselection of the from layer.
	//First set up the variables controlling the from layer selection:
	double fromX = fromXPos;
	double fromY = fromYPos;

	//Need to work through all the neurons in the to layer jumping forward by the spacing each time
	for(int toCounter1 = toStart1; toCounter1 != toFinish1; toCounter1 += toIncrement1){
		for(int toCounter2 = toStart2; toCounter2 != toFinish2; toCounter2 += toIncrement2){
			//Select the neuron at this position in the to layer
			query.reset();
			
			//Need to make changes depending on whether layers are rotated or not.
			if(connHolder.paramMap["Rotate?"] == 0){//In this case toCounter1 is along the X axis and toCounter2 is along the Y axis
				query<<"SELECT NeuronID FROM Neurons WHERE X = "<<toCounter1<<" AND Y = "<<toCounter2<<" AND Z = "<<toZPos;
			}
			else{//toCounter1 is along the Y axis and toCounter2 is along the X axis
				query<<"SELECT NeuronID FROM Neurons WHERE X = "<<toCounter2<<" AND Y = "<<toCounter1<<" AND Z = "<<toZPos;
			}
                        StoreQueryResult toNeuronRes = query.store();
			if(toNeuronRes.size() != 1){
				cerr<<"ConnectionManager: To neuron not found at expected position or too many neurons at single location. Number of results: "<<toNeuronRes.size()<<endl;
				throw createConnectionException;
			}
			Row toNeuronRow(*toNeuronRes.begin());//Have checked that there is only one neuron
			unsigned int toNeuronID = (unsigned int)toNeuronRow["NeuronID"];
			
			//Now work through all of the from neurons
			//Overlap etc. are all rounded to two decimal places. So for each connection select within
			//the layer and then advance it an extra 0.01 to prevent double selection
			
			//First select all the from neurons within the central space to create on centre
			query.reset();
			//Add X stuff to query
			query<<"SELECT NeuronID FROM Neurons WHERE X >= "<<(fromX + outerWidth/2.0 - innerWidth/2.0)<<" AND X <= "<<(fromX + outerWidth/2.0 + innerWidth/2.0);
			//Now add similar Y stuff and Z, which does not change because layers are stacked along z axis
			query<<" AND Y >= "<<(fromY + outerLength/2.0 - innerLength/2.0)<<" AND Y <= "<<(fromY + outerLength/2.0 + innerLength/2.0)<<" AND Z = "<<fromZPos;
                        StoreQueryResult fromLayerSelRes1 = query.store();
			
			//Now work through all the selected neurons and create excitatory connections
                        for(StoreQueryResult::iterator fromIter = fromLayerSelRes1.begin(); fromIter != fromLayerSelRes1.end(); ++fromIter){
				Row fromNeuronRow(*fromIter);
				//Get fromID and check to see if this connection already exists already in the database
				unsigned int fromNeuronID = (unsigned int)fromNeuronRow["NeuronID"];
				if(!connectionExists(query, fromNeuronID, toNeuronID)){
					//Sort out the parameters for the connection
					unsigned short connectionDelay = getDelay(connHolder.minDelay, connHolder.maxDelay);
					short connectionWeight = getWeight(connHolder.paramMap["Excitation weight"], connHolder.paramMap["Weight range"], connHolder.paramMap["Normal weight distribution?"]);
					
					//Add the connection to the database
					query.reset();
					query<<"INSERT INTO Connections (PreSynapticNeuronID, PostSynapticNeuronID, Delay, Weight, ConnGrpID)  VALUES ("<<fromNeuronID<<", "<<toNeuronID<<", "<<connectionDelay<<", "<<connectionWeight<<", "<<connHolder.connectionGrpID<<")";
					query.execute();
					++connectionCount;
				}
			}
			
			//Now select all neurons within the surround to create off surround
			//The version of MySQL that I am using does not support nested queries and so 
			//have to use a more complex query than I would have liked.
			//Initially will select all neurons within the area of the connetion
			query.reset();
			//Select top chunk of connection area
			query<<"SELECT NeuronID FROM Neurons WHERE ( X >= "<<fromX<<" AND X <= "<<(fromX + outerWidth)<<" AND Y >= "<<fromY<<" AND Y < "<<(fromY + outerLength/2.0 - innerLength/2.0)<<" AND Z = "<<fromZPos<<" ) ";
			//Or bottom chunk of connection area
			query<<"OR ( X >= "<<fromX<<" AND X <= "<<(fromX + outerWidth)<<" AND Y > "<<(fromY + outerLength/2.0 + innerLength/2.0)<<" AND Y <= "<<(fromY + outerLength)<<" AND Z = "<<fromZPos<<" ) ";
			//Or left chunk of connection area
			query<<"OR ( X >= "<<fromX<<" AND X < "<<(fromX + outerWidth/2.0 - innerWidth/2.0)<<" AND Y >= "<<fromY<<" AND Y <= "<<(fromY + outerLength)<<" AND Z = "<<fromZPos<<" ) ";
			//Or right chunk of connection area
			query<<"OR ( X > "<<(fromX + outerWidth/2.0 + innerWidth/2.0)<<" AND X <= "<<(fromX + outerWidth)<<" AND Y >= "<<fromY<<" AND Y <= "<<(fromY + outerLength)<<" AND Z = "<<fromZPos<<" ) ";
			
                        StoreQueryResult fromLayerSelRes2 = query.store();
			
			//Now work through all the selected neurons and create inhibitory surround
                        for(StoreQueryResult::iterator fromIter = fromLayerSelRes2.begin(); fromIter != fromLayerSelRes2.end(); ++fromIter){
				Row fromNeuronRow(*fromIter);
				//Get fromNeuronID and check to see if it already exists in the database
				unsigned int fromNeuronID = (unsigned int)fromNeuronRow["NeuronID"];
				if(!connectionExists(query, fromNeuronID, toNeuronID)){
					//Sort out the parameters for the connection
					unsigned short connectionDelay = getDelay(connHolder.minDelay, connHolder.maxDelay);
					short connectionWeight = getWeight(connHolder.paramMap["Inhibition weight"], connHolder.paramMap["Weight range"], connHolder.paramMap["Normal weight distribution?"]);

					//Add the connection to the database 
					query.reset();
					query<<"INSERT INTO Connections (PreSynapticNeuronID, PostSynapticNeuronID, Delay, Weight, ConnGrpID)  VALUES ("<<fromNeuronID<<", "<<toNeuronID<<", "<<connectionDelay<<", "<<connectionWeight<<", "<<connHolder.connectionGrpID<<")";
					query.execute();
					++connectionCount;
				}
			}
			//Increase fromY position
			fromY += (outerLength - overlap + 0.01);
			
			//Adjust the progress dialog
			++progressCount;
			progressDialog->setProgress(progressCount);
			if(progressDialog->wasCancelled())
				break;
		}
		//Increase fromX position and reset Y position
		 fromX += (outerWidth - overlap + 0.01);
		 fromY = fromYPos;
	}
	
	//Need to run a check that all from neurons have been connected to. 
	//Otherwise could be errors with this.
	if(overlap == 0 && connectionCount != (fromGrpWidth_Neur * fromGrpLength_Neur)){
		cerr<<"Possible Connection Error: Overlap is zero and yet not all from layer neurons have been connected to the to layer."<<endl;
		cerr<<"Connection Count = "<<connectionCount<<"; From layer size = "<<(fromGrpWidth_Neur * fromGrpLength_Neur);
	}
}


/*! Creates topographic connections between this layer and part of the device layer
	Each component contains a list of receptor ids and so have to work through these
	and make the appropriate connections to the device input layer. */
void ConnectionManager::createSIMNOSComponentConns(ConnectionHolder& connHolder, int &connectionCount){
	//Get the width and length of the from neuron group
	Query networkQuery = networkDBInterface->getQuery();
	networkQuery.reset();
	networkQuery<<"SELECT Width, Length FROM NeuronGroups WHERE NeuronGrpID = "<<connHolder.fromLayerID;
        StoreQueryResult fromSizeRes = networkQuery.store();
	Row fromSizeRow (*fromSizeRes.begin());//fromLayerID should be unique
	unsigned int fromGrpWidth = Utilities::getUInt((std::string)fromSizeRow["Width"]);
	unsigned int fromGrpLength = Utilities::getUInt((std::string)fromSizeRow["Length"]);

	//Get the width and length of the to neuron group
	networkQuery.reset();
	networkQuery<<"SELECT Width, Length FROM NeuronGroups WHERE NeuronGrpID = "<<connHolder.toLayerID;
        StoreQueryResult toSizeRes = networkQuery.store();
	Row toSizeRow (*toSizeRes.begin());//fromLayerID should be unique
	unsigned int toGrpWidth = Utilities::getUInt((std::string)toSizeRow["Width"]);
	unsigned int toGrpLength = Utilities::getUInt((std::string)toSizeRow["Length"]);

	//In current implementation from and to group widths should be identical
	if(fromGrpWidth != toGrpWidth){
		cerr<<"ConnectionManager: FROM GROUP WIDTH DOES NOT EQUAL TO GROUP WIDTH"<<endl;
		return;
	}

	//Get the list of receptors from the SIMNOSComponents database
	Query deviceQuery = deviceDBInterface->getQuery();
	deviceQuery.reset();
	deviceQuery<<"SELECT ReceptorIDs FROM SIMNOSComponents WHERE ComponentID = "<<connHolder.componentID;
        StoreQueryResult simnosRes = deviceQuery.store();
	Row simnosRow(*simnosRes.begin());//componentID is unique
        QString receptorIDString = ((std::string)simnosRow["ReceptorIDs"]).data();
	QStringList receptorIDStrList = QStringList::split(",", receptorIDString);

	/* Check that the number of receptors does not exceed the length of the new layer.
		There could be less receptors than the length of the new layer since some receptors
		can extend over multiple rows. */
	if(receptorIDStrList.size() > fromGrpLength || receptorIDStrList.size() > toGrpLength){
		cerr<<"ConnectionManager: MISMATCH BETWEN NUMBER OF RECEPTOR IDS AND LAYER LENGTH"<<endl;
		return;
	}

	/* Create a variable to record which row we are connecting to in the new layer
		and assign this to the first neuron id in the layer */
	networkQuery.reset();
	networkQuery<<"SELECT MIN(NeuronID) FROM Neurons WHERE NeuronGrpID = "<<connHolder.fromLayerID;
        StoreQueryResult startFromNeurIDRes = networkQuery.store();
	Row startFromNeurIDRow(*startFromNeurIDRes.begin());
	unsigned int startFromLayerNeurID = Utilities::getUInt((std::string) startFromNeurIDRow["MIN(NeuronID)"]);


	/* Create a variable to record which row we are connecting from in the device layer
		and assign this to the first neuron id in the layer */
	networkQuery.reset();
	networkQuery<<"SELECT MIN(NeuronID) FROM Neurons WHERE NeuronGrpID = "<<connHolder.toLayerID;
        StoreQueryResult startToNeurIDRes = networkQuery.store();
	Row startToNeurIDRow(*startToNeurIDRes.begin());
	unsigned int startToLayerNeurID = Utilities::getUInt((std::string) startToNeurIDRow["MIN(NeuronID)"]);

	/* Work through string list and connect each receptor up to 
		the next part of the new layer 
		NOTE Each receptor is one or more rows in the device input, indicated
		by the StartRow and NumRows values. Currently the colums are ignored. */
	unsigned int devGrpNeurID, newGrpNeurID;
	unsigned int newGrpRowNum = 0;
	for(unsigned int i=0; i<receptorIDStrList.size(); ++i){
		deviceQuery.reset();
                deviceQuery<<"SELECT StartRow, NumRows FROM SIMNOSSpikeReceptors WHERE ReceptorID ="<<receptorIDStrList[i].toStdString();
                StoreQueryResult recYRes = deviceQuery.store();
		Row recYRow (*recYRes.begin());//Receptor ID should be unique
		unsigned int devStartRow = Utilities::getUInt((std::string)recYRow["StartRow"]);
		unsigned int devNumRows = Utilities::getUInt((std::string)recYRow["NumRows"]);

		//Set the deviceLayerNeuronID appropriately
		if(connHolder.deviceIsFrom){
			devGrpNeurID = startFromLayerNeurID + fromGrpWidth * devStartRow;
			newGrpNeurID = startToLayerNeurID + toGrpWidth * newGrpRowNum;
		}
		else{
			devGrpNeurID = startToLayerNeurID + toGrpWidth * devStartRow;
			newGrpNeurID = startFromLayerNeurID + fromGrpWidth * newGrpRowNum;
		}

		//Work through all of the rows in the receptor
		for(unsigned int j=0; j<devNumRows; ++j){
			//Work across row and create connections between the layers
			for(unsigned int i=0; i< fromGrpWidth; ++i){//From and to grp width are the same
				//Sort out weight and delay
				unsigned short connectionDelay = getDelay(connHolder.minDelay, connHolder.maxDelay);
				short connectionWeight = getWeight(connHolder.paramMap["Average weight"], connHolder.paramMap["Weight range"], 0); //Random, not normal, distribution
	
				//Create query and add to database
				networkQuery.reset();
				if(connHolder.deviceIsFrom){//Connection needs to be from device to new layer
					networkQuery<<"INSERT INTO Connections (PreSynapticNeuronID, PostSynapticNeuronID, Delay, Weight, ConnGrpID) VALUES ("<<devGrpNeurID<<", "<<newGrpNeurID<<", "<<connectionDelay<<", "<<connectionWeight<<", "<<connHolder.connectionGrpID<<")";
				}
				else{//Connection needs to be from new layer to device
					networkQuery<<"INSERT INTO Connections (PreSynapticNeuronID, PostSynapticNeuronID, Delay, Weight, ConnGrpID) VALUES ("<<newGrpNeurID<<", "<<devGrpNeurID<<", "<<connectionDelay<<", "<<connectionWeight<<", "<<connHolder.connectionGrpID<<")";
				}
				networkQuery.execute();
				++connectionCount;
				++devGrpNeurID;
				++newGrpNeurID;
			}
		}

		//Increase new group row number so next receptor connection will be on a different row
		++newGrpRowNum;
	}
}


/*! SIMPLE CORTEX CONNECTIONS
	Method that creates cortex style connections within a single layer.
	Combines short range excitation with long range inhibition.
	Normally distributed random selection of neurons within a circle
	Strategy is to select each neuron within the layer and connect appropriateliy to all neurons
	within the circle regardless of whether the central neuron is on the edge or not. */
void ConnectionManager::createSimpleCortexConns(ConnectionHolder &connHolder, int &connectionCount){
        //Get a query object. This will be used whenever StoreQueryResult and not UseQueryResult is used
	Query generalQuery = networkDBInterface->getQuery();
	Connection* neighborTmpConnection = networkDBInterface->getNewConnection();
	Query neighborConnsQuery = neighborTmpConnection->query();
	
	//Count the number of neurons and use this to set up the progress dialog
	generalQuery.reset();
	generalQuery<<"SELECT COUNT(*) FROM Neurons WHERE NeuronGrpID = "<<connHolder.fromLayerID;
        StoreQueryResult neuronCountRes = generalQuery.store();
	Row neuronCountRow(*neuronCountRes.begin());
	unsigned int numberOfNeurons = Utilities::getUInt((std::string)neuronCountRow.at(0));
	progressDialog->reset();
	progressDialog->setTotalSteps(numberOfNeurons);
	progressDialog->setLabelText("Creating connections, please wait");
	int progressCount = 0;
	
	//Obtain the connection parameters.
	//These are all assumed to have been checked by the connection properties dialog
	double excitationRadius = connHolder.paramMap["Excitation radius"];
	double inhibitionRadius = connHolder.paramMap["Inhibition radius"];
	double overlap = connHolder.paramMap["Overlap"];
	double excitationConnDensity = connHolder.paramMap["Excitation connection density"];
	double inhibitionConnDensity = connHolder.paramMap["Inhibition connection density"];

	//Get a list of NeuronIDs in the layer and work through it
	Connection* neuronsTmpConnection = networkDBInterface->getNewConnection();
	Query neuronsQuery = neuronsTmpConnection->query();
	neuronsQuery<<"SELECT NeuronID, X, Y, Z FROM Neurons WHERE NeuronGrpID = "<<connHolder.fromLayerID;
        UseQueryResult layerRes = neuronsQuery.use();//UseQueryResult is much more efficient for large queries
	
	//Work through the neurons
	Row layerRow;
	if(layerRes){
		try{
			while( (layerRow = layerRes.fetch_row()) && !progressDialog->wasCancelled()) {
				int neuronID = Utilities::getInt((std::string)layerRow["NeuronID"]);
				int xPos = Utilities::getInt((std::string)layerRow["X"]);
				int yPos = Utilities::getInt((std::string)layerRow["Y"]);
				int zPos = Utilities::getInt((std::string)layerRow["Z"]);
				
				//Now need to select a percentage of the neurons surrounding this neuron within the excitatory radius
				neighborConnsQuery.reset();
				neighborConnsQuery<<"SELECT NeuronID, X, Y FROM Neurons WHERE X >= "<<(xPos - excitationRadius)<<" AND X <= "<<(xPos + excitationRadius)<<" ";
				neighborConnsQuery<<"AND Y >= "<<(yPos - excitationRadius)<<" AND Y <= "<<(yPos + excitationRadius)<<" ";
				neighborConnsQuery<<"AND Z = "<<zPos<<" AND NeuronGrpID = "<<connHolder.fromLayerID;//This query should give a rectangle around the position
                                UseQueryResult neighborResExcite = neighborConnsQuery.use();
				Row neighborRow;
				if(neighborResExcite){
					try{
						while(neighborRow = neighborResExcite.fetch_row()){
							int tempNeuronID = Utilities::getInt((std::string)neighborRow["NeuronID"]);
							int tempXPos = Utilities::getInt((std::string)neighborRow["X"]);
							int tempYPos = Utilities::getInt((std::string)neighborRow["Y"]);
							double distance = getDistance(xPos, yPos, tempXPos, tempYPos);
							
							//Check to see if point lies within excitatory radius and run probability function to create normal distribution of connnections
							if((distance <= excitationRadius) && evaluateConnectionProbability(excitationRadius, distance, excitationConnDensity)){
								
								//Create connection
								if(!connectionExists(generalQuery, neuronID, tempNeuronID)){//If connection does not exist already
									
									//Sort out the parameters for the connection
									unsigned short connectionDelay = getDelay(connHolder.minDelay, connHolder.maxDelay);
									short connectionWeight = getWeight(connHolder.paramMap["Excitation weight"], connHolder.paramMap["Weight range"], connHolder.paramMap["Normal weight distribution?"]);
									
									//Add the connection to the database
									generalQuery.reset();
									generalQuery<<"INSERT INTO Connections (PreSynapticNeuronID, PostSynapticNeuronID, Delay, Weight, ConnGrpID) VALUES ("<<neuronID<<", "<<tempNeuronID<<", "<<connectionDelay<<", "<<connectionWeight<<", "<<connHolder.connectionGrpID<<")";
									
									generalQuery.execute();
									++connectionCount;
								}
							}
						}
					}
					catch (const Exception& exception){//Catch any other errors
						cerr<<"ConnectionManager: UNRECOGNIZED EXCEPTION: "<<exception.what()<<endl;
					}
				}
				else{
					cerr<<"CANNOT RETRIEVE NEURON DETAILS FOR EXCITATORY CONNECTIONS: "<<neighborConnsQuery.error()<<endl;
					progressDialog->reset();
					return;
				}
				
				//Now need to select a percentage of the neurons surrounding this neuron within the inhibitory radius
				neighborConnsQuery.reset();
				neighborConnsQuery<<"SELECT NeuronID, X, Y FROM Neurons WHERE X >= "<<(xPos - inhibitionRadius)<<" AND X <= "<<(xPos + inhibitionRadius)<<" ";
				neighborConnsQuery<<"AND Y >= "<<(yPos - inhibitionRadius)<<" AND Y <= "<<(yPos + inhibitionRadius)<<" ";
				neighborConnsQuery<<"AND Z = "<<zPos<<" AND NeuronGrpID = "<<connHolder.fromLayerID;//This query should give a rectangle around the position
                                UseQueryResult neighborResInhib = neighborConnsQuery.use();
				if(neighborResInhib){
					Row neighborRow;
					try{
						while(neighborRow = neighborResInhib.fetch_row()){
							int tempNeuronID = Utilities::getInt((std::string)neighborRow["NeuronID"]);
							int tempXPos = Utilities::getInt((std::string)neighborRow["X"]);
							int tempYPos = Utilities::getInt((std::string)neighborRow["Y"]);
							double distance = getDistance(xPos, yPos, tempXPos, tempYPos);
							
							//Check to see if point lies within excitatory radius and run probability function to create normal distribution of connnections
							//TODO MIGHT WANT TO MAKE IT GREATER THAN INHIBITION RADIUS AS WELL
							if((distance >= (excitationRadius - overlap)) && (distance <= inhibitionRadius) && evaluateConnectionProbability(inhibitionRadius, distance, inhibitionConnDensity)){
								
								//Create connection
								if(!connectionExists(generalQuery, neuronID, tempNeuronID)){
									
									//Sort out the parameters for the connection
									unsigned short connectionDelay = getDelay(connHolder.minDelay, connHolder.maxDelay);
									short connectionWeight = getWeight(connHolder.paramMap["Inhibition weight"], connHolder.paramMap["Weight range"], connHolder.paramMap["Normal weight distribution?"]);
									
									//Add the connection to the database
									generalQuery.reset();
									generalQuery<<"INSERT INTO Connections (PreSynapticNeuronID, PostSynapticNeuronID, Delay, Weight, ConnGrpID) VALUES ("<<neuronID<<", "<<tempNeuronID<<", "<<connectionDelay<<", "<<connectionWeight<<", "<<connHolder.connectionGrpID<<")";
									
									generalQuery.execute();
									++connectionCount;
								}
							}
						}
					}
					catch (const Exception& exception){//Catch any other errors
						cerr<<"ConnectionManager: UNRECOGNIZED EXCEPTION: "<<exception.what()<<endl;
					}
				}
				else{
					cerr<<"CANNOT RETRIEVE NEURON DETAIL FOR INHIBITORY CONNECTIONSS: "<<neighborConnsQuery.error()<<endl;
					progressDialog->reset();
					return;
				}
				//Check to see if the progress bar is cancelled
				if(progressDialog->wasCanceled()){
					#ifdef CREATE_CONNECTIONS_DEBUG
						cout<<"CONNECTION CREATION CANCELLED. NUMBER OF CONNECTIONS CREATED = "<<connectionCount<<endl;
					#endif//CREATE_CONNECTIONS_DEBUG	
					return;
				}
				
				//Set the progress in the dialog. This increases with each neuron in the layer
				++progressCount;
				if((progressCount % 10) == 1)
					progressDialog->setProgress(progressCount);
			}
		}
		catch (const Exception& exception){//Catch any other errors
			cerr<<"ConnectionManager: UNRECOGNIZED EXCEPTION: "<<exception.what()<<endl;
		}
	}
	else{
		cerr<<"CANNOT RETRIEVE NEURON GROUP DETAILS: "<<neuronsQuery.error()<<endl;
		progressDialog->reset();
	}
	//Close connections and reset connections dialog
        neighborTmpConnection->disconnect();
        neuronsTmpConnection->disconnect();
	progressDialog->reset();
	#ifdef CREATE_CONNECTIONS_DEBUG
		cout<<"NUMBER OF CONNECTIONS CREATED = "<<connectionCount<<endl;
	#endif//CREATE_CONNECTIONS_DEBUG
}


/*! Creates connections that are topographically mapped between the two layers
	NOTE Only works between layers of identical size. */
void ConnectionManager::createTopographicConns(ConnectionHolder& connHolder, int &connectionCount){
	//Behaviour is different depending on the size relationship between the two layers.
	Query generalQuery = networkDBInterface->getQuery();
	generalQuery<<"SELECT Width, Length FROM NeuronGroups WHERE NeuronGrpID = "<<connHolder.fromLayerID;
        StoreQueryResult fromLayerRes= generalQuery.store();
	Row fromRow(*fromLayerRes.begin());
	int fromWidth = Utilities::getInt((std::string)fromRow["Width"]);
	int fromLength = Utilities::getInt((std::string)fromRow["Length"]);
	
	//Get the dimensions of the to group
	generalQuery.reset();
	generalQuery<<"SELECT Width, Length, X, Y FROM NeuronGroups WHERE NeuronGrpID = "<<connHolder.toLayerID;
        StoreQueryResult toLayerRes= generalQuery.store();
	Row toRow(*toLayerRes.begin());
	int toWidth = Utilities::getInt((std::string)toRow["Width"]);
	int toLength = Utilities::getInt((std::string)toRow["Length"]);
	int toStartX = Utilities::getInt((std::string)toRow["X"]);
	int toStartY = Utilities::getInt((std::string)toRow["Y"]);

	/* If layers are the same size, use a simple method that works through the two layers in parallel.*/
	if(fromWidth == toWidth && fromLength == toLength && connHolder.paramMap["Overlap"] == 0.0){
		//Get two result sets holding the neuron ids of the two layers
		generalQuery.reset();
		generalQuery<<"SELECT NeuronID FROM Neurons WHERE NeuronGrpID = "<<connHolder.fromLayerID<<" ORDER BY NeuronID";
                StoreQueryResult fromLayerResult = generalQuery.store();
		generalQuery.reset();
		generalQuery<<"SELECT NeuronID FROM Neurons WHERE NeuronGrpID = "<<connHolder.toLayerID<<" ORDER BY NeuronID";
                StoreQueryResult toLayerResult = generalQuery.store();
	
		//Double check that they are the same length
		if(fromLayerResult.size() != toLayerResult.size()){
			cerr<<"ConnectionManager: TOPOGRAPHIC CONNECTIONS: FROM LAYER SIZE DOES NOT MATCH TO LAYER SIZE"<<endl;
			return;
		}

		//Set up progress count
		progressDialog->reset();
		progressDialog->setTotalSteps(fromLayerResult.size());
		progressDialog->setLabelText("Creating connections, please wait");
		int progressCount = 0;
	
		//Work through the results in parallel connecting up each of the neurons
                StoreQueryResult::iterator fromIter = fromLayerResult.begin();
                StoreQueryResult::iterator toIter = toLayerResult.begin();
		while((fromIter != fromLayerResult.end())  && !progressDialog->wasCancelled()){
			Row fromRow(*fromIter);
			unsigned int fromNeuronID = Utilities::getUInt((std::string)fromRow["NeuronID"]);
			Row toRow(*toIter);
			unsigned int toNeuronID = Utilities::getUInt((std::string)toRow["NeuronID"]);
	
			//Create connection between from and to neuron
			if(!connectionExists(generalQuery, fromNeuronID, toNeuronID)){//If connection does not exist already
				//Sort out the parameters for the connection
				unsigned short connectionDelay = getDelay(connHolder.minDelay, connHolder.maxDelay);
				short connectionWeight = getWeight(connHolder.paramMap["Average weight"], connHolder.paramMap["Weight range"], 0); //Random, not normal, distribution
	
				//Add the connection to the database
				generalQuery.reset();
				generalQuery<<"INSERT INTO Connections (PreSynapticNeuronID, PostSynapticNeuronID, Delay, Weight, ConnGrpID) VALUES ("<<fromNeuronID<<", "<<toNeuronID<<", "<<connectionDelay<<", "<<connectionWeight<<", "<<connHolder.connectionGrpID<<")";
				
				generalQuery.execute();
				++connectionCount;
			}
	
			//Set the progress in the dialog. This increases with each neuron in the layer
			++progressCount;
			if((progressCount % 100) == 1){
				progressDialog->setProgress(progressCount);
				spikeStrApp->processEvents();
			}
	
			++fromIter;
			++toIter;
		}
		//Clear progress dialog
		progressDialog->reset();
	}
	/* Layers are different sizes or there is overlap specified. */
	else{
		//Set up progress count
		progressDialog->reset();
		progressDialog->setTotalSteps(fromWidth * fromLength);
		progressDialog->setLabelText("Creating connections, please wait");
		spikeStrApp->processEvents();
		int progressCount = 0;

		/* Calculate the from and to connection width and length. */
		int fromConnAreaWidth, fromConnAreaLength, toConnAreaWidth, toConnAreaLength;
		if(connHolder.paramMap["Rotate"] == 0.0){//No rotation of layers
			if(fromWidth > toWidth){
				fromConnAreaWidth = fromWidth / toWidth;
				toConnAreaWidth = 1;
			}
			else{
				fromConnAreaWidth = 1;
				toConnAreaWidth = toWidth / fromWidth;//Will equal 1 if they are the same
			}
			if(fromLength > toLength){
				fromConnAreaLength = fromLength / toLength;
				toConnAreaLength = 1;
			}
			else{
				fromConnAreaLength = 1;
				toConnAreaLength = toLength / fromLength;//Will equal 1 if they are the same
			}
		}
		else{//In rotation mode
			if(fromWidth > toLength){
				fromConnAreaWidth = fromWidth / toLength;
				toConnAreaLength = 1;
			}
			else{
				fromConnAreaWidth = 1;
				toConnAreaLength = toLength / fromWidth;//Will equal 1 if they are the same
			}
			if(fromLength > toWidth){
				fromConnAreaLength = fromLength / toWidth;
				toConnAreaWidth= 1;
			}
			else{
				fromConnAreaLength = 1;
				toConnAreaWidth = toWidth / fromLength;//Will equal 1 if they are the same
			}
		}

		//Convert overlap into an integer
		int overlap = (int)rint(connHolder.paramMap["Overlap"]);

		//Create connections by working through the neurons in the from layer
		//Get the start neuron from ID 
		generalQuery.reset();
		generalQuery<<"SELECT MIN(NeuronID) FROM Neurons WHERE NeuronGrpID = "<<connHolder.fromLayerID;
                StoreQueryResult startNeurIDResult = generalQuery.store();
		Row startNeurIDRow(*startNeurIDResult.begin());//Should only be one row
		unsigned int fromStartID = Utilities::getUInt((std::string)startNeurIDRow["MIN(NeuronID)"]);

		//Create a temporary query whose results can be used rather than stored.
		Connection* toNeuronsConnection = networkDBInterface->getNewConnection();
		Query toNeuronsQuery = toNeuronsConnection->query();

		//Work through the neurons in the from layer
		for(int fromYPos = 0; fromYPos < fromLength; ++fromYPos){
			for(int fromXPos = 0; fromXPos < fromWidth; ++fromXPos){
				//Calculate the from neuron id
				unsigned int fromNeuronID = fromXPos + fromYPos*fromWidth + fromStartID;

				//Find all of the neurons in the to layer that this from neuron should connect to
				int minToX, maxToX, minToY, maxToY;
				if(connHolder.paramMap["Rotate"] == 0.0){
					minToX = (fromXPos / fromConnAreaWidth) * toConnAreaWidth + toStartX - overlap;
					maxToX = minToX + toConnAreaWidth + 2*overlap;
					minToY = (fromYPos / fromConnAreaLength) * toConnAreaLength + toStartY - overlap;
					maxToY = minToY + toConnAreaLength + 2*overlap;
				}
				else{
					minToX = (fromYPos / fromConnAreaLength) * toConnAreaWidth + toStartX - overlap;
					maxToX = minToX + toConnAreaWidth + 2*overlap;
					minToY = (fromXPos / fromConnAreaWidth) * toConnAreaLength + toStartY - overlap;
					maxToY = minToY + toConnAreaLength + 2*overlap;
				}

				//Query for neurons in the to layer that lie within the appropriate area
				toNeuronsQuery.reset();
				toNeuronsQuery<<"SELECT NeuronID FROM Neurons WHERE X >= "<<minToX<<" AND X < "<<maxToX<<" ";
				toNeuronsQuery<<"AND Y >= "<<minToY<<" AND Y < "<<maxToY<<" AND NeuronGrpID = "<<connHolder.toLayerID;//Double check to use neuron group id although it is not strictly necessary
                                UseQueryResult toNeuronsRes= toNeuronsQuery.use();
				if(toNeuronsRes){
					Row toNeuronsRow;
					try{
						while(toNeuronsRow = toNeuronsRes.fetch_row()){
							//Get the to neuron id
							unsigned int toNeuronID = Utilities::getInt((std::string)toNeuronsRow["NeuronID"]);
							
							//Create connection
							if(!connectionExists(generalQuery, fromNeuronID, toNeuronID)){
									
								//Sort out the parameters for the connection
								unsigned short connectionDelay = getDelay(connHolder.minDelay, connHolder.maxDelay);
								short connectionWeight = getWeight(connHolder.paramMap["Average weight"], connHolder.paramMap["Weight range"], 0); //Random, not normal, distribution
		
								//Add the connection to the database
								generalQuery.reset();
								generalQuery<<"INSERT INTO Connections (PreSynapticNeuronID, PostSynapticNeuronID, Delay, Weight, ConnGrpID) VALUES ("<<fromNeuronID<<", "<<toNeuronID<<", "<<connectionDelay<<", "<<connectionWeight<<", "<<connHolder.connectionGrpID<<")";
								
								generalQuery.execute();
								++connectionCount;

								//Set the progress in the dialog. This increases with each neuron in the layer
								++progressCount;
								if((progressCount % 100) == 1){
									progressDialog->setProgress(progressCount);
									spikeStrApp->processEvents();
								}
							}
						}
					}
					catch (const Exception& exception){//Catch any other errors
						cerr<<"ConnectionManager: UNRECOGNIZED EXCEPTION: "<<exception.what()<<endl;
					}
				}
			}
		}
		//Close the temporary connection
                toNeuronsConnection->disconnect();

		//Clear progress dialog
		progressDialog->reset();
	}

	#ifdef CREATE_CONNECTIONS_DEBUG
		cout<<"NUMBER OF CONNECTIONS CREATED = "<<connectionCount<<endl;
	#endif//CREATE_CONNECTIONS_DEBUG
}


/*! Creates an unstructured set of connections selected at random between two layers.
	Works through each of the neurons in the from layer and selects a random subset of 
 	neurons to connect to in the to layer. */
void ConnectionManager::createUnstructuredConns(ConnectionHolder& connHolder, int &connectionCount){
	//Get the number of neurons in to neuron group and start neuronID
	Query generalQuery = networkDBInterface->getQuery();
	generalQuery.reset();
	generalQuery<<"SELECT MIN(NeuronID), MAX(NeuronID) FROM Neurons WHERE NeuronGrpID = "<<connHolder.toLayerID;
        StoreQueryResult toNeuronGrpRes = generalQuery.store();
	Row toGrpRow(*toNeuronGrpRes.begin());//Assume that there is only one result
	unsigned int startNeuronID = Utilities::getUInt((std::string)toGrpRow["MIN(NeuronID)"]);
	unsigned int endNeuronID = Utilities::getUInt((std::string)toGrpRow["MAX(NeuronID)"]);

	/*Work through all the from neurons. 
		For each neuron connect to a random selection of the to neurons */

	//Count the number of neurons and use this to set up the progress dialog
	generalQuery.reset();
	generalQuery<<"SELECT COUNT(*) FROM Neurons WHERE NeuronGrpID = "<<connHolder.fromLayerID;
        StoreQueryResult neuronCountRes = generalQuery.store();
	Row neuronCountRow(*neuronCountRes.begin());
	unsigned int numberOfFromNeurons = Utilities::getUInt((std::string)neuronCountRow["COUNT(*)"]);
	progressDialog->reset();
	
	unsigned int connectionCountEstimate = numberOfFromNeurons * (unsigned int)rint(connHolder.paramMap["Connection density"] * (double)(endNeuronID - startNeuronID));

	progressDialog->setTotalSteps(connectionCountEstimate);
	progressDialog->setLabelText("Creating connections, please wait");
	spikeStrApp->processEvents();

	Connection* neuronsTmpConnection = networkDBInterface->getNewConnection();
	Query neuronsQuery = neuronsTmpConnection->query();
	neuronsQuery<<"SELECT NeuronID FROM Neurons WHERE NeuronGrpID = "<<connHolder.fromLayerID;
        UseQueryResult neuronsRes = neuronsQuery.use();//UseQueryResult is much more efficient for large queries
	
	//Work through the from neurons
	Row neuronsRow;
	if(neuronsRes){
		try{
			while ( (neuronsRow = neuronsRes.fetch_row()) && !progressDialog->wasCancelled()) {
				int fromNeuronID = Utilities::getInt((std::string)neuronsRow["NeuronID"]);
	
				//Get a vector with a randomly selected list of to neuron ids
				vector<unsigned int> ranSelVect;
				fillRandomSelectionVector(ranSelVect, startNeuronID, endNeuronID, connHolder.paramMap["Connection density"]);
				for(vector<unsigned int>::iterator iter = ranSelVect.begin(); iter != ranSelVect.end()&& !progressDialog->wasCancelled(); ++iter){
					unsigned int toNeuronID = *iter;
					//Create connection
					if(!connectionExists(generalQuery, fromNeuronID, toNeuronID)){//If connection does not exist already
						//Sort out the parameters for the connection
						unsigned short connectionDelay = getDelay(connHolder.minDelay, connHolder.maxDelay);
						short connectionWeight = getWeight(connHolder.paramMap["Average weight"], connHolder.paramMap["Weight range"], 0); //Random, not normal, distribution
						
						//Add the connection to the database
						generalQuery.reset();
						generalQuery<<"INSERT INTO Connections (PreSynapticNeuronID, PostSynapticNeuronID, Delay, Weight, ConnGrpID) VALUES ("<<fromNeuronID<<", "<<toNeuronID<<", "<<connectionDelay<<", "<<connectionWeight<<", "<<connHolder.connectionGrpID<<")";
						
						generalQuery.execute();
						++connectionCount;

						//Set the progress in the dialog. This increases with each neuron in the layer
						if((connectionCount % 100) == 1){
							progressDialog->setProgress(connectionCount);
							spikeStrApp->processEvents();
						}
					}
				}
			}
		}
		catch (const Exception& exception){//Catch any other errors
			cerr<<"ConnectionManager: UNRECOGNIZED EXCEPTION: "<<exception.what()<<endl;
		}
	}	
	else{
		#ifdef CREATE_CONNECTIONS_DEBUG
			cerr<<"CANNOT RETRIEVE NEURON DETAILS: "<<neuronsQuery.error()<<endl;
		#endif//CREATE_CONNECTIONS_DEBUG
		progressDialog->reset();
	}
        neuronsTmpConnection->disconnect();
	progressDialog->reset();
	#ifdef CREATE_CONNECTIONS_DEBUG
		cout<<"NUMBER OF CONNECTIONS CREATED = "<<connectionCount<<endl;
	#endif//CREATE_CONNECTIONS_DEBUG
}

/*! Creates unstructured connections in which each neuron either makes excitatory connections
	or inhibitory connections. Similar to the basic unstructured method. */
void ConnectionManager::createUnstructuredExInhibConnections(ConnectionHolder& connHolder, int& connectionCount){
	//Get the number of neurons in to neuron group and start neuronID
	Query generalQuery = networkDBInterface->getQuery();
	generalQuery.reset();
	generalQuery<<"SELECT MIN(NeuronID), MAX(NeuronID) FROM Neurons WHERE NeuronGrpID = "<<connHolder.toLayerID;
        StoreQueryResult toNeuronGrpRes = generalQuery.store();
	Row toGrpRow(*toNeuronGrpRes.begin());//Assume that there is only one result
	unsigned int startNeuronID = Utilities::getUInt((std::string)toGrpRow["MIN(NeuronID)"]);
	unsigned int endNeuronID = Utilities::getUInt((std::string)toGrpRow["MAX(NeuronID)"]);

	/*Work through all the from neurons. 
		For each neuron connect to a random selection of the to neurons */

	//Count the number of neurons and use this to set up the progress dialog
	generalQuery.reset();
	generalQuery<<"SELECT COUNT(*) FROM Neurons WHERE NeuronGrpID = "<<connHolder.fromLayerID;
        StoreQueryResult neuronCountRes = generalQuery.store();
	Row neuronCountRow(*neuronCountRes.begin());
	unsigned int numberOfFromNeurons = Utilities::getUInt((std::string)neuronCountRow["COUNT(*)"]);
	progressDialog->reset();
	
	//Add the excitatory connections to the estimate 
	unsigned int connectionCountEstimate = numberOfFromNeurons * (unsigned int)rint((connHolder.paramMap["Excitatory percentage"] / 100.0) * connHolder.paramMap["Excitation connection prob"] * (double)(endNeuronID - startNeuronID));

	//Add the inhibitory connections to the estimate
	connectionCountEstimate += numberOfFromNeurons * (unsigned int)rint((1.0 - (connHolder.paramMap["Excitatory percentage"] / 100.0) ) * connHolder.paramMap["Inhibition connection prob"] * (double)(endNeuronID - startNeuronID));

	//Set up progress dialog with this estimate
	progressDialog->setTotalSteps(connectionCountEstimate);
	progressDialog->setLabelText("Creating connections, please wait");
	spikeStrApp->processEvents();

	Connection* neuronsTmpConnection = networkDBInterface->getNewConnection();
	Query neuronsQuery = neuronsTmpConnection->query();
	neuronsQuery<<"SELECT NeuronID FROM Neurons WHERE NeuronGrpID = "<<connHolder.fromLayerID;
        UseQueryResult neuronsRes = neuronsQuery.use();//UseQueryResult is much more efficient for large queries
	
	//Work through the from neurons
	Row neuronsRow;
	if(neuronsRes){
		try{
			while ( (neuronsRow = neuronsRes.fetch_row()) && !progressDialog->wasCancelled()) {
				int fromNeuronID = Utilities::getInt((std::string)neuronsRow["NeuronID"]);

				/* Decide whether this is an excitatory or inhibitory neuron.*/
				bool excitatoryNeuron = isExcitatoryNeuron(fromNeuronID, connHolder.paramMap["Excitatory percentage"]);
	
				//Get a vector with a randomly selected list of to neuron ids
				vector<unsigned int> ranSelVect;
				if(excitatoryNeuron)
					fillRandomSelectionVector(ranSelVect, startNeuronID, endNeuronID, connHolder.paramMap["Excitation connection prob"]);
				else
					fillRandomSelectionVector(ranSelVect, startNeuronID, endNeuronID, connHolder.paramMap["Inhibition connection prob"]);

				for(vector<unsigned int>::iterator iter = ranSelVect.begin(); iter != ranSelVect.end()&& !progressDialog->wasCancelled(); ++iter){
					unsigned int toNeuronID = *iter;
					//Create connection
					if(!connectionExists(generalQuery, fromNeuronID, toNeuronID)){//If connection does not exist already
						//Sort out the parameters for the connection
						unsigned short connectionDelay = getDelay(connHolder.minDelay, connHolder.maxDelay);
						
						short connectionWeight;
						if(excitatoryNeuron)
							connectionWeight = getWeight(connHolder.paramMap["Excitation weight"], connHolder.paramMap["Excitation weight range"], 0); //Random, not normal, distribution
						else
							connectionWeight = getWeight(connHolder.paramMap["Inhibition weight"], connHolder.paramMap["Inhibition weight range"], 0); //Random, not normal, distribution

						//Add the connection to the database
						generalQuery.reset();
						generalQuery<<"INSERT INTO Connections (PreSynapticNeuronID, PostSynapticNeuronID, Delay, Weight, ConnGrpID) VALUES ("<<fromNeuronID<<", "<<toNeuronID<<", "<<connectionDelay<<", "<<connectionWeight<<", "<<connHolder.connectionGrpID<<")";
						
						generalQuery.execute();
						++connectionCount;

						//Set the progress in the dialog. This increases with each neuron in the layer
						if((connectionCount % 100) == 1){
							progressDialog->setProgress(connectionCount);
							spikeStrApp->processEvents();
						}
					}
				}
			}
		}
		catch (const Exception& exception){//Catch any other errors
			cerr<<"ConnectionManager: UNRECOGNIZED EXCEPTION: "<<exception.what()<<endl;
		}
	}	
	else{
		#ifdef CREATE_CONNECTIONS_DEBUG
			cerr<<"CANNOT RETRIEVE NEURON DETAILS: "<<neuronsQuery.error()<<endl;
		#endif//CREATE_CONNECTIONS_DEBUG
		progressDialog->reset();
	}
        neuronsTmpConnection->disconnect();
	progressDialog->reset();
	#ifdef CREATE_CONNECTIONS_DEBUG
		cout<<"NUMBER OF CONNECTIONS CREATED = "<<connectionCount<<endl;
	#endif//CREATE_CONNECTIONS_DEBUG
}


/*! For cortex connections want to create a normally distributed connection pattern where neurons 
	that are closest to the connecting neuron are most likely to get connected and neurons closest
	to the radius are least likely to be connected. This method evaluates whether the connection 
	should be created depending on its distance from the neuron. Also has a control 
	(connectionDensity) which varies this probability. Should create a normal distribution of 
	connections surrounding neuron. */
bool ConnectionManager::evaluateConnectionProbability(double radius, double distance, double connectionDensity){
	//Get normally distributed random number
	double normRan = getNormalRandom();
	
	//Limit it to +/- 3
	if(normRan < -3.0)
		normRan =-3.0;
	else if(normRan > 3.0)
		normRan = 3.0;
		
	//Normal random number now varies between -3 and +3 so need to make it fit into the radius range
	normRan *= (radius / 3.0);
	
	//Want a positive number to make comparison easier
	if(normRan < 0.0)
		normRan *= -1.0;
		
	//Minimun distance between neurons is 1, so need to add 1 to normRan or most connections will be lost
	++normRan;
	
	//Divide distance by connection density.
	//If Connection density is greater than 1 this will reduce the distance and increase the probability that normRan > distance
	//If connection density is less than 1 this will increase the distance and reduce the probability that normRan > distance
	distance /= connectionDensity;
	
	//Should now have a number that is between 0 and radius and much more likely to be closer to 0 than to radius
	//In first version will accept connection if normRan is less than the actual distance.
	//The shorter the distance, the more likely this is to occur. Will have to see how this works in practice
	if(normRan >= distance)
		return true;
	return false;
}


/*! Creates a list of to neurons to connect to. 
	A random number is generated and if this is greater than the connection density * ran max,
	then the neuronID is added to the vector */
void ConnectionManager::fillRandomSelectionVector(vector<unsigned int> &ranSelVect, unsigned int startNeuronID, unsigned int endNeuronID, double connectionDensity){
	int threshold = (int)(connectionDensity * (double)RAND_MAX);
	for(unsigned int i=startNeuronID; i<= endNeuronID; ++i){
		if(rand() < threshold)
			ranSelVect.push_back(i);
	}
}


/*! Generates the delay for a connection.
	Returns a positive random number between minDelay and maxDelay. */
unsigned short ConnectionManager::getDelay(unsigned short minDelay, unsigned short maxDelay){
	int delayDifference = maxDelay - minDelay;
	if(delayDifference < 0){
		cerr<<"ConnectionManager: Max delay should be greater than min delay"<<endl;
		throw createConnectionException;
	}
	else if(delayDifference == 0)//A precise figure has been set for the delay
		return minDelay;
	else{//Need to return a random number between minDelay and maxDelay
		int randomNum = rand();
		double randomDelayDoub = (double)minDelay + (double)randomNum *((double)delayDifference / (double)RAND_MAX);
		int randomDelayInt = (int)rint(randomDelayDoub);

		#ifdef DELAY_DEBUG
			cout<<"RANDOM NUMBER IS: "<<randomNum<<" RANDOM DELAY IS: "<<randomDelayInt<<endl;//Check that it is approximately random
		#endif//DELAY_DEBUG

		//Do a final check on the delay
		if(randomDelayInt > 255){
			cerr<<"ConnectionManager: Delay cannot exceed 255!"<<endl;
			throw createConnectionException;
		}
		return randomDelayInt;
	}
}


/*! Returns the distance between point 1 and point 2 using Pythagoras. */
double ConnectionManager::getDistance(int xPos1, int yPos1, int xPos2, int yPos2){
	double distanceSquared = pow((double)xPos1 - (double) xPos2, 2.0) + pow((double)yPos1 - (double)yPos2, 2.0);
	return sqrt(distanceSquared);
}


/*! Returns a normally distributed random number with standar deviation = 1
	Uses Box-Muller method to generate values
	Code adapted from http://www.csit.fsu.edu/~burkardt/cpp_src/random_data/random_data.html. */
double ConnectionManager::getNormalRandom(){
	double PI = 3.141592653589793;
	double rand1, rand2;
  	static int used = 0;
  	double x;
  	static double y = 0.0;
	//  If we've used an even number of values so far, generate two more, return one and save one.
	if (( used % 2 ) == 0 ){
    	for ( ; ; ){
			rand1 = (double)rand()/(double)RAND_MAX;
			if ( rand1 != 0.0 )
				break;
		}
		rand2 = (double)rand()/(double)RAND_MAX;
		x = sqrt ( -2.0 * log ( rand1 ) ) * cos ( 2.0 * PI * rand2 );
		y = sqrt ( -2.0 * log ( rand1 ) ) * sin ( 2.0 * PI * rand2 );
	}
	//  Otherwise, return the second, saved, value.
	else{
		x = y;
	}
	++used;
	return x;
}


/*! Applies noise to the given weight, returning a number between -128 and 127
	First get the random or normally distributed noise within the noise range
	Then convert to a number between -128 and 127. */
short ConnectionManager::getWeight(double weight, double range, double normDist){
	if(normDist == 1){//Produce normally distributed noise around weight within range
		/*Standard deviation is 1 with this implementation
			95.45% of numbers are within two standard deviations
			99% of the numbers are within three standard deviations - i.e. less than 3.0
			So for the moment take three standard deviations either side, round off any numbers that exceed this
			and compress the numbers within the noise range
		*/
		//First sort out random number
		double normRan = getNormalRandom();
		if(normRan < -3.0)
			normRan =-3.0;
		else if(normRan > 3.0)
			normRan = 3.0;
		normRan = normRan * (range / 3.0);//Normally distributed numbers will vary between -3 and +3 so need to make them fit into the range
		weight += normRan;//Add noise to weight

		#ifdef WEIGHT_DEBUG
			cout<<"Normally distributed noise: "<<normRan<<"; Weight: "<<weight;
		#endif //WEIGHT_DEBUG
	}
	else{//Produce random noise around weight within range
		int randomNum = rand();
		double randomNoiseDoub = (double)randomNum *(range / (double)RAND_MAX);
		weight += randomNoiseDoub; //Add noise to weight

		#ifdef WEIGHT_DEBUG
			cout<<"Random noise: "<<randomNoiseDoub<<"; Weight: "<<weight;
		#endif //WEIGHT_DEBUG
	}
	//Check that it is in bounds and correct if not
	if(weight > 1.0)
		weight = 1.0;
	else if (weight < -1.0)
		weight = -1.0;
			
	//Finally convert weight to range -128 - 127
	int finalWeight = (int)rint(weight * 127);

	#ifdef WEIGHT_DEBUG
		cout<<"; Final weight: "<<finalWeight<<"; Range "<<range<<endl;
	#endif //WEIGHT_DEBUG

	return finalWeight;
}


/*! Returns true if the neuron is an excitatory neuron, either because it already has just excitatory connections
	or a random number is used to select its type. */
bool ConnectionManager::isExcitatoryNeuron(unsigned int neuronID, double excitatoryPercentage){
	//Count up the number of excitatory connections that this neuron makes
	Query query = networkDBInterface->getQuery();
	query.reset();
	query<<"SELECT COUNT(*) FROM Connections WHERE PreSynapticNeuronID = "<<neuronID<<" AND Weight > 0";
        StoreQueryResult exciteRes = query.store();
	Row exciteRow(*exciteRes.begin());//Should be only one row
	unsigned int numExcitatoryConns = Utilities::getUInt((std::string)exciteRow["COUNT(*)"]);

	//Count up the number of inhibitory connections that this neuron makes
	query.reset();
	query<<"SELECT COUNT(*) FROM Connections WHERE PreSynapticNeuronID = "<<neuronID<<" AND Weight < 0";
        StoreQueryResult inhibRes = query.store();
	Row inhibRow(*inhibRes.begin());//Should be only one row
	unsigned int numInhibitoryConns = Utilities::getUInt((std::string)inhibRow["COUNT(*)"]);

	if(numExcitatoryConns > 0 && numInhibitoryConns == 0){//Already just has excitatory connections
		return true;
	}
	else if(numExcitatoryConns == 0 && numInhibitoryConns > 0){//Already just has inhibitory connections
		return false;
	}
	else if(numExcitatoryConns > 0 && numInhibitoryConns > 0){//Mixed excitatory and inhibitory should not be processed by this method.
		cerr<<"ConnectionManager: Error creating Unstructured excitatory/ inhibitory connections. Neuron "<<neuronID<<" has both excitatory and inhibitory connections."<<endl;
		throw createConnectionException;
	}

	//If we have reached this point neuron has no connections, so choose its type at random.
	int threshold = (int)rint( (excitatoryPercentage / 100.0) * (double)RAND_MAX);
	if(rand() < threshold)
		return true;
	return false;
}



