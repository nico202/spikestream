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

#ifndef NETWORKMODELXMLHANDLER_H
#define NETWORKMODELXMLHANDLER_H

//SpikeStream includes
#include "MonitorArea.h"

//Qt includes
#include <qxml.h>
#include <qstring.h>

//Other includes
#include <vector>


//---------------------- Network Model XML Handler -------------------------
/*! Used during SAX2 parsing of XML file representing the model of a neural
	network. Each neuron group that is read in is used to add a new window
	to the MonitorArea. */
//--------------------------------------------------------------------------

class NetworkModelXmlHandler : public QXmlDefaultHandler {

	public:
		NetworkModelXmlHandler(MonitorArea*);
		~NetworkModelXmlHandler();
		QString getNetworkModelString();
		vector<NeuronGroup*> getNeuronGrpVector();
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
		/*! Stores the current element.*/
		QString currentElement;

		/*! Reference to the monitor area that this class is loading.*/
		MonitorArea *monitorArea;

		/*! NeuronGroup used to hold information about the current neuron group
			as it is parsed. */
		NeuronGroup *neuronGrp;

		/*! Store the neuron groups in the network model to enable statistics
			to be set up.*/
		vector<NeuronGroup*> neurGrpVect;

		/*! Records whether there has been an error during parsing. */
		bool parseError;

		/*! Error messages generated during parsing. */
		QString parseErrorString;

		/*! A nicely formatted version of the network model string that is
			being parsed. This is useful for setting the paramters for the
			statistics.*/
		QString networkModelString;


		//=========================== METHODS =================================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		NetworkModelXmlHandler (const NetworkModelXmlHandler&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		NetworkModelXmlHandler operator = (const NetworkModelXmlHandler&);

};


#endif //NETWORKMODELXMLHANDLER_H


