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

#ifndef MONITORXMLHANDLER_H
#define MONITORXMLHANDLER_H

//SpikeStream includes
#include "MonitorDataPlotter.h"

//Qt includes
#include <qxml.h>
#include <qstring.h>


//------------------------- Monitor XML Handler ---------------------------
/*! Unpacks the description of data sent by neuron or synapse classes and 
	uses it to create new graphs. */
//-------------------------------------------------------------------------

class MonitorXmlHandler : public QXmlDefaultHandler {

	public:
		MonitorXmlHandler(MonitorDataPlotter*);
		~MonitorXmlHandler();
		bool getParseError();
		QString getParseErrorString();

	protected:
		//Inherited parsing methods
		bool characters ( const QString & ch );
    	bool endElement( const QString&, const QString&, const QString& );
    	bool startDocument();
    	bool startElement( const QString&, const QString&, const QString& , const QXmlAttributes& );

		//Inherited error handling methods
		bool error ( const QXmlParseException & exception );
		QString errorString ();
		bool fatalError ( const QXmlParseException & exception );
		bool warning ( const QXmlParseException & exception );


	private:
		//========================== VARIABLES ================================
		/*! Stores the current element. */
		QString currentElement;

		/*! Reference to the Neuron Monitor that this class is loading data for.*/
		MonitorDataPlotter *monitorDataPlotter;

		/*! NeuronGroup used to hold information about the neuron group
			as it is parsed. */
		NewGraph newGraph;

		/*! Records whether there has been an error during parsing. */
		bool parseError;

		/*! Error messages generated during parsing. */
		QString parseErrorString;


		//=========================== METHODS =================================
		/*! Declare copy constructor private so it cannot be used inadvertently. */
		MonitorXmlHandler (const MonitorXmlHandler&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		MonitorXmlHandler operator = (const MonitorXmlHandler&);

};


#endif //MONITORXMLHANDLER_H


