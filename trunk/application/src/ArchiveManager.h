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

#ifndef ARCHIVEMANAGER_H
#define ARCHIVEMANAGER_H

//SpikeStream includes
#include "DBInterface.h"
#include "NetworkDataXmlHandler.h"
#include "NetworkMonitor.h"
#include "GlobalVariables.h"

//Qt includes
#include <qthread.h>
#include <qwidget.h>
#include <qxml.h>
#include <qdatetime.h>
#include <qstring.h>


//-------------------------- Archive Manager -------------------------------
/*! Manages the play back of archives that have been recorded.*/
//--------------------------------------------------------------------------

class ArchiveManager : public QThread {
	Q_OBJECT

	public: 
		ArchiveManager(DBInterface *archiveDBInterface, QWidget* archiveWidget);
		~ArchiveManager();
		void addArchiveStatistics(ArchiveStatisticsHolder* newArchStat);
		bool archiveIsLoaded();
		void deleteArchiveStatistics(unsigned int statID);
		bool fastForwardArchive();
		void freezeArchive();
		QString getSimulationStartDateTime();
		static bool isRunning();
		void loadArchive(QString name);
		bool playArchive();
		bool rewindArchive();
		void setFrameRate(double rate);
		void setNetworkMonitors(map<unsigned int, NetworkMonitor*>);
		bool stepArchive();
		bool stopArchive();
		void unfreezeArchive();
		bool unloadArchive();


	signals:
		void archiveError(const QString& message);
		void stopped();


	protected:
		//Method inherited from QThread
		void run();
	

	private:
		//===================== VARIABLES ===============================
		/*! Reference to database used by archive manager. */
		DBInterface *archiveDBInterface;

		/*! Query is held open as archive is playing.*/
		mysqlpp::Query *archiveQuery;

		/*! Results is held open as archive is playing.*/
                mysqlpp::UseQueryResult *archiveResults;

		//XML stuff
		QXmlInputSource xmlInput;
    	QXmlSimpleReader xmlReader;
		NetworkDataXmlHandler *nwDataXmlHandler;
	
		/*! Short version of the reference to the main application to enable 
			updating of events. Used during loading. */
		SpikeStreamApplication *spikeStrApp;
		
		/*! Links neuronGrpIDs to network monitors that are running in this process.*/
		map<unsigned int, NetworkMonitor*> networkMonitorMap;

		/*! Stores the startDateTime for the currently loaded archive. */
		QString archiveStartDateTime;
		
		/*! Set to true to stop the thread running. */
		bool stop;

		/*! Records whether the archive is currently playing. Static so that it can be accessed by other classes.
			There will only ever be one archive running at a time. */
		static bool archiveRunning;

		/*! Records whether the archive is in step mode. */
		bool step;

		/*! Records whether an archive is loaded and therefore can be played.*/
		bool archiveLoaded;

		/*! Records the last time the network monitors were updated.*/
		QTime updateTime;
		
		/*! The interval in between updates to the network monitors.*/
		int updateInterval_msec;

		/*! Variable to hold the update interval in step mode.*/
		int originalUpdateInterval_msec;

		/*! Set to true when there is an error in the archive. Reset when a new
			archive is loaded.*/
		bool archError;
		
		/* Create variables for use in run method here to save 
			redeclaring them */
		QString networkDataString;
		int elapsedTime_msec;
		unsigned int timeStep;


		//========================= METHODS ==============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		ArchiveManager (const ArchiveManager&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		ArchiveManager operator = (const ArchiveManager&);

		void setTimeStep(unsigned int);

};


#endif //ARCHIVEMANAGER_H


