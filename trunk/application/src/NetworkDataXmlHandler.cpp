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
#include "NetworkDataXmlHandler.h"
#include "Utilities.h"
#include "Debug.h"
#include "SpikeStreamMainWindow.h"

//Other includes
#include <iostream>
using namespace std;


/*! Constructor. */
NetworkDataXmlHandler::NetworkDataXmlHandler() : QXmlDefaultHandler() {
	//Initialise variables
	neurSpikeCount = 0;
	neurSpikeTotal = 0;

	neuronIDHashMap.set_empty_key(EMPTY_NEURON_ID_KEY);
	neuronIDHashMap.set_deleted_key(DELETED_NEURON_ID_KEY);
	ArchiveStatistic::neuronIDHashMap = &this->neuronIDHashMap;
}


/*! Destructor. */
NetworkDataXmlHandler::~NetworkDataXmlHandler(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING NETWORK DATA XML HANDLER"<<endl;
	#endif//MEMORY_DEBUG
}


//---------------------------------------------------------------------------------
//---------------------------- PUBLIC METHODS -------------------------------------
//---------------------------------------------------------------------------------


/*! Adds a new archive statistic holder. Depending on the type this is added to the.*/
	
void NetworkDataXmlHandler::addArchiveStatistics(ArchiveStatisticsHolder* archStatHold){
	/* Work through the vector of archive statistics and put them into
		different containers depending on the type.*/
	for(vector<ArchiveStatistic*>::iterator iter = archStatHold->archStatVector.begin(); iter != archStatHold->archStatVector.end(); ++iter){
		if((*iter)->getType() == ArchiveStatistic::NEURON_GROUP){
			unsigned int tmpNeurGrpID = ((NeuronGrpArchiveStatistic*)*iter)->getNeuronGrpID();
			neuronGrpStatsMap[tmpNeurGrpID].push_back(*iter);//If there is not a vector entry in the map for this neuron group one should be created.
		}
		else{
			otherArchStatsVector.push_back(*iter);
		}
	}
}


/*! Deletes an archive statistic holder using the statistics id to find it. */
void NetworkDataXmlHandler::deleteArchiveStatistics(unsigned int statID){
	//Track what is going on
	int eraseCount = 0;

	//Work through the neuronGrpStatsMap to look for the archive statistic
	for(map<unsigned int, vector<ArchiveStatistic*> >::iterator outerIter = neuronGrpStatsMap.begin(); outerIter != neuronGrpStatsMap.end(); ++outerIter){
		//Work through the vector stored at this position in the map
		for(vector<ArchiveStatistic*>::iterator innerIter = outerIter->second.begin(); innerIter != outerIter->second.end(); ++innerIter){
			if( (*innerIter)->getID() == statID ){
				innerIter = outerIter->second.erase(innerIter);//innerIter now points to the element after the erased element
				++eraseCount;
				if(innerIter == outerIter->second.end())//Don't want to go beyond the end of the vector
					break;
			}
		}
	}
	for(vector<ArchiveStatistic*>::iterator iter = otherArchStatsVector.begin(); iter != otherArchStatsVector.end(); ++iter){
		if( (*iter)->getID() == statID ){
			iter = otherArchStatsVector.erase(iter);//iter now points to the element after the erased element
			++eraseCount;
			if(iter == otherArchStatsVector.end())//Don't want to go beyond the end of the vector
				break;
		}
	}

	//cout<<"NetworkDataXmlHandler: Erased "<<eraseCount<<" ArchiveStatistics."<<endl;
}


/*! Returns true if an error has been encountered during parsing. */
bool NetworkDataXmlHandler::getParseError(){
	return parseError;
}


/*! Returns a string describing the parsing error. */
QString NetworkDataXmlHandler::getParseErrorString(){
	return parseErrorString;
}


/*! Resets statistics gathered about the archive. */
void NetworkDataXmlHandler::resetStatistics(){
	neurSpikeCount  = 0;
	neurSpikeTotal = 0;

	//Work through the neuronGrpStatsMap to look for the archive statistic
	for(map<unsigned int, vector<ArchiveStatistic*> >::iterator outerIter = neuronGrpStatsMap.begin(); outerIter != neuronGrpStatsMap.end(); ++outerIter){
		//Work through the vector stored at this position in the map
		for(vector<ArchiveStatistic*>::iterator innerIter = outerIter->second.begin(); innerIter != outerIter->second.end(); ++innerIter){
			(*innerIter)->resetFiringNeuronCount();
			(*innerIter)->resetFiringNeuronTotal();
		}
	}
	for(vector<ArchiveStatistic*>::iterator iter = otherArchStatsVector.begin(); iter != otherArchStatsVector.end(); ++iter){
		(*iter)->resetFiringNeuronCount();
		(*iter)->resetFiringNeuronTotal();
	}
}


/*! Sets the maps that are updated from the XML file. */
void NetworkDataXmlHandler::setNetworkMonitors(map<unsigned int, NetworkMonitor*> nwMonMap){
	networkMonitorMap = nwMonMap;
}


//---------------------------------------------------------------------------------
//---------------------------- PROTECTED METHODS ----------------------------------
//---------------------------------------------------------------------------------

/*! Called when the parser encounters characters. */
bool NetworkDataXmlHandler::characters(const QString& chars){
	try{
		unsigned int firingNeuronID = 0;
		if(currentElement == "neuron_group"){
			//Get the list of neurons/ spikes
			QStringList tempStringList = QStringList::split(",", chars);
	
			//Add firing neuron IDs to firing neuron maps in this class (for statistics) and elsewhere
			for(unsigned int i=0; i< tempStringList.size(); ++i){
				firingNeuronID = Utilities::getUInt(tempStringList[i].ascii());

				//Store the neuron id for statistical analysis
				neuronIDHashMap[firingNeuronID] = true;

				//Update the neuron group archive statistics
				for(vector<ArchiveStatistic*>::iterator iter = neuronGrpStatsMap[neuronGrpID].begin(); iter != neuronGrpStatsMap[neuronGrpID].end(); ++iter){
					(*iter)->recalculateNeuronGrp();
				}

				//Update the graphical representation of the neurons
				(*networkMonitorMap[neuronGrpID]->bufferMapPointer)[firingNeuronID] = true;//Load neurons into map 2
			}
		}
		else{
			cerr<<"NetworkDataXmlHandler: UNRECOGNIZED ELEMENT"<<endl;
			parseError = true;
			parseErrorString += "Unrecognized element.";
		}
	}
	catch (std::exception& er) {// Catch-all for std exceptions
		parseError = true;
		parseErrorString += er.what();
	}
	return true;
}


/*! Called when the parser is at the end of an element. */
bool NetworkDataXmlHandler::endElement( const QString&, const QString&, const QString& qName){
	if(qName == "neuron_group"){
		//Swap the maps and update the display of the network monitor that has just been changed
		networkMonitorMap[neuronGrpID]->swapMaps();
	}
	else if(qName == "network_pattern"){//End of the pattern for this timestep
		//Call method to analyse the pattern to measure statistics about it.
		processStatistics();

		//Inform any listening classes that the statistics have changed.
		SpikeStreamMainWindow::spikeStreamApplication->lock();//Called by separate thread, so need to lock mutex
		emit spikeCountChanged(neurSpikeCount);
		emit spikeTotalChanged(neurSpikeTotal);
		emit statisticsChanged();
		SpikeStreamMainWindow::spikeStreamApplication->unlock();

		//Empty the hash map containing the neuron ids for this timestep
		neuronIDHashMap.clear();
		
	}
	return true;
}


/*! Called when the parser has an error. */
bool NetworkDataXmlHandler::error ( const QXmlParseException & parseEx ){
        cerr<<"NetworkDataXmlHandler: PARSING ERROR: "<<parseEx.message().toStdString()<<endl;
	parseError = true;
	parseErrorString += parseEx.message();
	return true;
}


/*! Called to get the error string for the error. */
QString NetworkDataXmlHandler::errorString (){
	return QString("NetworkDataXmlHandler: Default error string");
}


/*! Called when the parser has a fatal error. */
bool NetworkDataXmlHandler::fatalError ( const QXmlParseException & parseEx ){
        cerr<<"NetworkDataXmlHandler: PARSING FATAL ERROR: "<<parseEx.message().toStdString()<<endl;
	parseError = true;
	parseErrorString += parseEx.message();
	return true;
}


/*! Called when the parser is at the start of the document. */
bool NetworkDataXmlHandler::startDocument(){
	neurSpikeCount = 0;

	//Set all of the firing neuron counts to zero
	for(vector<ArchiveStatistic*>::iterator iter = otherArchStatsVector.begin(); iter != otherArchStatsVector.end(); ++iter){
		(*iter)->resetFiringNeuronCount();
	}
	for(map<unsigned int, vector<ArchiveStatistic*> >::iterator outerIter = neuronGrpStatsMap.begin(); outerIter != neuronGrpStatsMap.end(); ++outerIter){
		for(vector<ArchiveStatistic*>::iterator innerIter = outerIter->second.begin(); innerIter != outerIter->second.end(); ++innerIter){
			(*innerIter)->resetFiringNeuronCount();
		}
	}

	parseError = false;
	parseErrorString = "";
	neuronIDHashMap.clear();
	return true;
}


/*! Called when the parser encounters the start of an XML element. */
bool NetworkDataXmlHandler::startElement(const QString&, const QString&, const QString& qName, const QXmlAttributes& xmlAttributes){
	currentElement = qName;
	if(qName == "neuron_group"){
		/*Set the neuron group id */
		try{
			bool neuronGrpIDFound = false;
			for(int i=0; i<xmlAttributes.length(); ++i){
				if(xmlAttributes.localName(i) == "id"){
					neuronGrpID = Utilities::getUInt(xmlAttributes.value(i).ascii());
					neuronGrpIDFound = true;
				}
			}
			if(!neuronGrpIDFound){
				cerr<<"NetworkDataXmlHandler: Cannot find neuron group ID"<<endl;
				parseError = true;
				parseErrorString += "Cannot find neuron group ID.";
			}
		}
		catch (std::exception& er) {// Catch-all for std exceptions
			parseError = true;
			parseErrorString += er.what();
		}
	}
	return true;
}


/*! Called when the parser has a warning. */
bool NetworkDataXmlHandler::warning ( const QXmlParseException & exception ){
        cerr<<"NetworkDataXmlHandler: PARSING WARNING: "<<exception.message().toStdString()<<endl;
	return true;
}

//---------------------------------------------------------------------------------
//-----------------             PRIVATE METHODS                --------------------
//---------------------------------------------------------------------------------

/*! Gathers statistical information about the archive. Should be called after each 
	time step. */
void NetworkDataXmlHandler::processStatistics(){
	//Update the ArchiveWidget statistics
	neurSpikeCount += neuronIDHashMap.size();//Should add the number of unique ids found in the archive
	neurSpikeTotal += neurSpikeCount;

	//Work through the statistical classes that are NOT monitoring a complete neuron group+
	for(vector<ArchiveStatistic*>::iterator iter = otherArchStatsVector.begin(); iter != otherArchStatsVector.end(); ++iter){
		(*iter)->recalculate();
	}
}



