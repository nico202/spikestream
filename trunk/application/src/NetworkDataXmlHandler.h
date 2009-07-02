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

#ifndef NETWORKDATAXMLHANDLER_H
#define NETWORKDATAXMLHANDLER_H

//SpikeStream includes
#include "NetworkMonitor.h"
#include "ArchiveStatistic.h"

//Qt includes
#include <qxml.h>
#include <qstring.h>
#include <qwidget.h>

//---------------------- Network Data XML Handler --------------------------
/*! Called during SAX2 parsing to play back an archive. Parses the XML file
	and updates the network monitors. */
//--------------------------------------------------------------------------

class NetworkDataXmlHandler : public QObject, public QXmlDefaultHandler {
	Q_OBJECT

	public:
		NetworkDataXmlHandler();
		~NetworkDataXmlHandler();
		void addArchiveStatistics(ArchiveStatisticsHolder* archStatHold);
		void deleteArchiveStatistics(unsigned int statID);
		bool getParseError();
		QString getParseErrorString();
		void resetStatistics();
		void setNetworkMonitors(map<unsigned int, NetworkMonitor*>);


	signals:
		void spikeCountChanged(unsigned int neurSpikeCount);
		void spikeTotalChanged(unsigned int neurSpikeTotal);
		void statisticsChanged();


	protected:
		//Parsing methods inherited from QXmlDefaultHandler
		bool characters ( const QString & ch );
    	bool endElement( const QString&, const QString&, const QString& );
    	bool startDocument();
    	bool startElement( const QString&, const QString&, const QString& , const QXmlAttributes& );
		
		//Error handling methods inherited from QXmlDefaultHandler
		bool error ( const QXmlParseException & exception );
		QString errorString();
		bool fatalError ( const QXmlParseException & exception );
		bool warning ( const QXmlParseException & exception );


	private:
		//============================ VARIABLES =============================
		/*! Holds the number of spikes in the current time step.*/
		unsigned int neurSpikeCount;

		/*! Holds the total number of spikes since the start of the archive.*/
		unsigned int neurSpikeTotal;

		/*! Name of the current element.*/
		QString currentElement;

		/*! Map of the network monitors that will display the archived data.*/
		map<unsigned int, NetworkMonitor*> networkMonitorMap;

		/*! Records the current neuron group that is being unpacked.*/
		unsigned int neuronGrpID;

		/*! Records whether there has been an error during parsing. */
		bool parseError;

		/*! Error messages generated during parsing. */
		QString parseErrorString;

		/*! Holds the neuron ids firing at each time step for the gathering of 
			statistics.*/
		dense_hash_map<unsigned int, bool, hash<unsigned int> > neuronIDHashMap;

		/*! Holds all of the statistics that are monitoring whole neuron
			groups. The key is the neuron group id*/
		map<unsigned int, vector<ArchiveStatistic*> > neuronGrpStatsMap;

		/*! Holds all of the statistics that are monitoring one or a number
			of neuron ids.*/
		vector<ArchiveStatistic*> otherArchStatsVector;


		//============================ METHODS ===============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		NetworkDataXmlHandler (const NetworkDataXmlHandler&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		NetworkDataXmlHandler operator = (const NetworkDataXmlHandler&);

		void processStatistics();

};


#endif //NETWORKDATAXMLHANDLER_H



