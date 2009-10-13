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
#include "NetworkModelXmlHandler.h"
#include "Utilities.h"
#include "Debug.h"

//Other includes
#include <iostream>
using namespace std;


/*! Constructor. */
NetworkModelXmlHandler::NetworkModelXmlHandler(MonitorArea *monArea) : QXmlDefaultHandler() {
	//Store reference to the monitor area
	monitorArea = monArea;
}


/*! Destructor. */
NetworkModelXmlHandler::~NetworkModelXmlHandler(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING NETWORK MODEL XML HANDLER"<<endl;
	#endif//MEMORY_DEBUG
}


//-------------------------------------------------------------------------
//--------------------------  PUBLIC METHODS  -----------------------------
//-------------------------------------------------------------------------

/*! Returns a nicely formatted version of the network model XML. */
QString NetworkModelXmlHandler::getNetworkModelString(){
	return networkModelString;
}


/*! Returns a vector containing details about the neuron groups that have been
	extracted from the XML. */
vector<NeuronGroup*> NetworkModelXmlHandler::getNeuronGrpVector(){
	return neurGrpVect;
}


/*! Returns true if an error has been encountered during parsing. */
bool NetworkModelXmlHandler::getParseError(){
	return parseError;
}


/*! Returns a string describing the parsing error. */
QString NetworkModelXmlHandler::getParseErrorString(){
	return parseErrorString;
}


//-------------------------------------------------------------------------
//------------------------- PROTECTED METHODS -----------------------------
//-------------------------------------------------------------------------

/*! Called when parser encounters characters. */
bool NetworkModelXmlHandler::characters(const QString& chars){
	//Add chars to network model string
	networkModelString += chars;

	//Process the string
	try{
		if(currentElement == "name")
			neuronGrp->name = chars;
		else if(currentElement == "start_neuron_id")
			neuronGrp->startNeuronID = Utilities::getUInt(chars.ascii());
		else if(currentElement == "width")
			neuronGrp->width = Utilities::getUInt(chars.ascii());
		else if(currentElement == "length")
			neuronGrp->length = Utilities::getUInt(chars.ascii());
		else if(currentElement == "location"){
			QStringList tempStringList = QStringList::split(",", chars);
			neuronGrp->xPos = Utilities::getInt(tempStringList[0].ascii());
			neuronGrp->yPos = Utilities::getInt(tempStringList[1].ascii());
			neuronGrp->zPos = Utilities::getInt(tempStringList[2].ascii());
		}
		else if(currentElement == "spacing")
			neuronGrp->spacing = Utilities::getUInt(chars.ascii());
		else if(currentElement == "neuron_type")
			neuronGrp->neuronType = Utilities::getUInt(chars.ascii());
		else{
			cerr<<"NetworkModelXmlHandler: UNRECOGNIZED ELEMENT"<<endl;	
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


/*! Called when the parser encounters the end of an element. */
bool NetworkModelXmlHandler::endElement( const QString&, const QString&, const QString& qName){
	//Add to the network model string
	if(qName == "neural_network"){
		networkModelString += "\n</";
		networkModelString += qName + ">";
	}
	else if(qName == "neuron_group"){
		networkModelString += "\n\t</";
		networkModelString += qName + ">";
	}
	else{
		networkModelString += "</";
		networkModelString += qName + ">";
	}

	//Process the name
	if(qName == "neuron_group"){
		monitorArea->addMonitorWindow(*neuronGrp);//Struct passed by value, so monitor area should get copy
		neurGrpVect.push_back(neuronGrp);
	}
	return true;
}


/*! Called when the parser generates an error. */
bool NetworkModelXmlHandler::error ( const QXmlParseException& parseEx){
	cerr<<"NetworkModelXmlHandler: PARSING ERROR"<<endl;
	parseError = true;
	parseErrorString += parseEx.message();
	return true;
}


/*! Returns a default error string. */
QString NetworkModelXmlHandler::errorString (){
	return QString("NetworkModelXmlHandler: Default error string");

}


/*! Called when the parser generates a fatal error. */
bool NetworkModelXmlHandler::fatalError ( const QXmlParseException& parseEx){
	cerr<<"NetworkModelXmlHandler: PARSING FATAL ERROR"<<endl;
	parseError = true;
	parseErrorString += parseEx.message();
	return true;
}


/*! Called when parser reaches the start of the document. */
bool NetworkModelXmlHandler::startDocument(){
	parseError = false;
	parseErrorString = "";
	return true;
}


/*! Called when parser reaches the start of an element. */
bool NetworkModelXmlHandler::startElement(const QString&, const QString&, const QString& qName, const QXmlAttributes& xmlAttributes){
	//Add to the network model string
	if(qName == "neural_network"){
		networkModelString += "<";
		networkModelString += qName + ">";
	}
	else if(qName == "neuron_group"){
		networkModelString += "\n\t<";
		networkModelString += qName + ">";
	}
	else {
		networkModelString += "\n\t\t<";
		networkModelString += qName + ">";
	}

	//Process name
	if(qName == "neuron_group"){
		neuronGrp = new NeuronGroup;

		/*Set the neuron group id */
		try{
			bool neuronGrpIDFound = false;
			for(int i=0; i<xmlAttributes.length(); ++i)
				if(xmlAttributes.localName(i) == "id"){
					neuronGrp->neuronGrpID = Utilities::getUInt(xmlAttributes.value(i).ascii());
					neuronGrpIDFound = true;
				}
	
			if(!neuronGrpIDFound){
				cerr<<"NetworkDataXmlHandler: Cannot find neuron group ID"<<endl;
				parseError = true;
				parseErrorString += "Cannot find neuron group ID.";
				return true;
			}
		}
		catch (std::exception& er) {// Catch-all for std exceptions
			parseError = true;
			parseErrorString += er.what();
			return true;
		}
	}
	else
		currentElement = qName;
	return true;
}


/*! Called when the parser generates a warning. */
bool NetworkModelXmlHandler::warning ( const QXmlParseException& ){
	cerr<<"NetworkModelXmlHandler: PARSING WARNING"<<endl;
	return true;
}



