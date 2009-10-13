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
#include "MonitorXmlHandler.h"
#include "Utilities.h"
#include "Debug.h"

//Other includes
#include <iostream>
using namespace std;

/*! Constructor. */
MonitorXmlHandler::MonitorXmlHandler(MonitorDataPlotter* monDatPlot) : QXmlDefaultHandler() {
	//Store reference to the NeuronMonitor
	monitorDataPlotter = monDatPlot;
}


/*! Destructor. */
MonitorXmlHandler::~MonitorXmlHandler(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING MONITOR XML HANDLER"<<endl;
	#endif//MEMORY_DEBUG
}


//-------------------------------------------------------------------------
//--------------------------  PUBLIC METHODS  -----------------------------
//-------------------------------------------------------------------------

/*! Returns true if an error has been encountered during parsing. */
bool MonitorXmlHandler::getParseError(){
	return parseError;
}


/*! Returns a string describing the parsing error. */
QString MonitorXmlHandler::getParseErrorString(){
	return parseErrorString;
}


//-------------------------------------------------------------------------
//------------------------- PROTECTED METHODS -----------------------------
//-------------------------------------------------------------------------

/*! Called when parser encounters characters. */
bool MonitorXmlHandler::characters(const QString& chars){
	try{
		if(currentElement == "description")
			newGraph.description = chars;
		else if(currentElement == "range_high")
			newGraph.rangeHigh = Utilities::getDouble(chars.ascii());
		else if(currentElement == "range_low")
			newGraph.rangeLow = Utilities::getDouble(chars.ascii());
		else{
			parseError = true;
			parseErrorString += "Unrecognized element.";
			cerr<<"MonitorXmlHandler: UNRECOGNIZED ELEMENT"<<endl;
		}
	}
	catch (std::exception& er) {// Catch-all for std exceptions
		parseError = true;
		parseErrorString += er.what();
	}
	return true;
}


/*! Called when the parser encounters the end of an element. */
bool MonitorXmlHandler::endElement( const QString&, const QString&, const QString& qName){
	if(qName == "data"){
		monitorDataPlotter->addGraph(newGraph);//Struct passed by value, so should get copy
	}
	else if(qName == "monitor_info")
		monitorDataPlotter->loadingComplete();
	return true;
}


/*! Called when the parser generates an error. */
bool MonitorXmlHandler::error ( const QXmlParseException& parseEx){
	cerr<<"MonitorXmlHandler: PARSING ERROR"<<endl;
	parseError = true;
	parseErrorString += parseEx.message();
	return true;
}


/*! Returns a default error string. */
QString MonitorXmlHandler::errorString (){
	return QString("MonitorXmlHandler: Default error string");

}


/*! Called when the parser generates a fatal error. */
bool MonitorXmlHandler::fatalError ( const QXmlParseException& parseEx){
	cerr<<"MonitorXmlHandler: PARSING FATAL ERROR"<<endl;
	parseError = true;
	parseErrorString += parseEx.message();
	return true;
}


/*! Called when parser reaches the start of the document. */
bool MonitorXmlHandler::startDocument(){
	parseError = false;
	parseErrorString = "";
	return true;
}


/*! Called when parser reaches the start of an element. */
bool MonitorXmlHandler::startElement(const QString&, const QString&, const QString& qName, const QXmlAttributes& xmlAttributes){
	currentElement = qName;
	return true;
}


/*! Called when the parser generates a warning. */
bool MonitorXmlHandler::warning ( const QXmlParseException& ){
	cerr<<"MonitorXmlHandler: PARSING WARNING"<<endl;
	return true;
}


