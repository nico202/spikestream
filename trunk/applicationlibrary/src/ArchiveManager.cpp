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
#include "ArchiveManager.h"
#include "DBInterface.h"
#include "Debug.h"
#include "Utilities.h"
#include "SpikeStreamMainWindow.h"

//Qt includes
#include <qwidget.h>

//Other includes
#include <mysql++.h>
using namespace mysqlpp;


//Declare and initialise static variables
bool ArchiveManager::archiveRunning = false;


/*! Constructor. */
ArchiveManager::ArchiveManager(DBInterface *archDBInter, QWidget* archiveWidget){
	//Store references
	archiveDBInterface = archDBInter;

	//Set up a short version of spike stream application reference
	spikeStrApp = SpikeStreamMainWindow::spikeStreamApplication;
	
	//Initialise variables
	archiveLoaded = false;

	//Set up the xml handler and reader
	nwDataXmlHandler = new NetworkDataXmlHandler();
	xmlReader.setContentHandler(nwDataXmlHandler);
	xmlReader.setErrorHandler(nwDataXmlHandler);
	connect ( nwDataXmlHandler, SIGNAL(spikeCountChanged(unsigned int)), archiveWidget, SLOT(setSpikeCount(unsigned int)));
	connect ( nwDataXmlHandler, SIGNAL(spikeTotalChanged(unsigned int)), archiveWidget, SLOT(setSpikeTotal(unsigned int)));
	connect ( nwDataXmlHandler, SIGNAL(statisticsChanged()), archiveWidget, SLOT(updateStatisticsTable()));

	/* Set up the database. 
		The new database connection should be deleted by the dbInterface*/
	archiveQuery = new Query(archiveDBInterface->getNewConnection()->query());

	//Initialise the variables that will be used in the run method
	updateInterval_msec = 0;
	updateTime = QTime::currentTime();
	networkDataString = "";
	elapsedTime_msec = 0;
	timeStep = 0;

	//Set stop and step to false
	stop = false;
	step = false;
}


/*! Destructor. */
ArchiveManager::~ArchiveManager(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING ARCHIVE MANAGER"<<endl;
	#endif//MEMORY_DEBUG

	delete nwDataXmlHandler;
	delete archiveQuery;
}


//---------------------------------------------------------------------
//-------------------------- PUBLIC METHODS ---------------------------
//---------------------------------------------------------------------

/*! Passes the new statistics on to the network data xml handler. */
void ArchiveManager::addArchiveStatistics(ArchiveStatisticsHolder* newArchStat){
	nwDataXmlHandler->addArchiveStatistics(newArchStat);
}


/*! Returns true if archive is loaded. */
bool ArchiveManager::archiveIsLoaded(){
	return archiveLoaded;
}


/*! Deletes the archive statistics from the network data xml handler
	using the statistic's id. */
void ArchiveManager::deleteArchiveStatistics(unsigned int statID){
	nwDataXmlHandler->deleteArchiveStatistics(statID);
}


/*! Fast forwards archive by csetting the update interval very small. */
bool ArchiveManager::fastForwardArchive(){
	if(archError){
		SpikeStreamMainWindow::spikeStreamApplication->lock();
		emit archiveError(QString("Archive has an error and cannot be fast forwarded."));
		SpikeStreamMainWindow::spikeStreamApplication->unlock();
		return false;
	}

	if(archiveLoaded){//Check to see if an archive is loaded
		updateInterval_msec = 5;
		stop = false;
		if(!archiveRunning)
			start();//Start the thread running if it is not running already
	}
	return true;
}


/*! Used when deleting an archive. This interferes with the UseQueryResult of the
	archiveManager, which has to be deleted and reinitialised after the deletion
	of the archive is complete. */
void ArchiveManager::freezeArchive(){
	if(archiveLoaded){
		//Delete archive results
		delete archiveResults;
		archiveResults = NULL;
	}
}


/*! Used to display the start date time of the currently loaded archive. */
QString ArchiveManager::getSimulationStartDateTime(){
	return archiveStartDateTime;
}


/*! Returns true if the archive is running. */
bool ArchiveManager::isRunning(){
	return archiveRunning;
}


/*! Load an archive ready for playing back.
	Exception handling for this operation is carried out by ArchiveWidget. */
void ArchiveManager::loadArchive(QString startDateTime){
	//Store startDateTime to enable rewind
	archiveStartDateTime = startDateTime;

	//Reset statistics in xml handler	
	nwDataXmlHandler->resetStatistics();

	//Do any steps necessary to clean up previous archive
	if(archiveLoaded){
		unloadArchive();
	}

	//Set up the query and the results that will be used to stream from database
	archiveQuery->reset();
        (*archiveQuery)<<"SELECT TimeStep, FiringNeurons FROM NetworkData WHERE SimulationStartTime = UNIX_TIMESTAMP(\""<<startDateTime.toStdString()<<"\") ORDER BY TimeStep";
        archiveResults = new UseQueryResult(archiveQuery->use());//UseQueryResult is much more efficient for large queries

	//Reset errors
	archError = false;

	//Everything ok up to this point
	archiveLoaded = true;
}


/*! Plays back archive. */
bool ArchiveManager::playArchive(){
	if(archError){
		SpikeStreamMainWindow::spikeStreamApplication->lock();
		emit archiveError(QString("Archive has an error and cannot be played."));
		SpikeStreamMainWindow::spikeStreamApplication->unlock();
		return false;
	}

	if(archiveLoaded){//Check to see if an archive is loaded
		updateInterval_msec = originalUpdateInterval_msec;
		stop = false;
		if(!archiveRunning)
			start();//Start the thread running
	}
	return true;
}


/*! Rewinds archive to the beginning. */
bool ArchiveManager::rewindArchive(){
	if(archError){
		SpikeStreamMainWindow::spikeStreamApplication->lock();
		emit archiveError(QString("Archive has an error and cannot be rewound."));
		SpikeStreamMainWindow::spikeStreamApplication->unlock();
		return false;
	}

	//Stop run method
	stop = true;

	//Reload current archive
	loadArchive(archiveStartDateTime);

	return true;
}


/*! Sets the update interval based on the frame rate. */
void ArchiveManager::setFrameRate(double rate){
	//Set the update interval to the duration of each frame in miliseconds
	originalUpdateInterval_msec = (int) (1000.0 / rate);
	updateInterval_msec = originalUpdateInterval_msec;
}


/*! Sets the network monitors so that archived spike data can be passed to them. */
void ArchiveManager::setNetworkMonitors(map<unsigned int, NetworkMonitor*> nwMonMap){
	networkMonitorMap = nwMonMap;
	nwDataXmlHandler->setNetworkMonitors(nwMonMap);
}


/*! Advances the archive playback by 1 step. Playback is automatically stopped
	after the step. */
bool ArchiveManager::stepArchive(){
	if(archError){
		SpikeStreamMainWindow::spikeStreamApplication->lock();
		emit archiveError(QString("Archive has an error and cannot be stepped."));
		SpikeStreamMainWindow::spikeStreamApplication->unlock();
		return false;
	}

	if(archiveLoaded){//Check to see if an archive is loaded
		stop = false;
		step = true;
		updateInterval_msec = 0;//Set update interval to 0 to get a faster response to the step
		if(!archiveRunning)
			start();//Start the thread running if it is not running already
	}
	return true;
}


/*! Stops archive playback. */
bool ArchiveManager::stopArchive(){
	stop = true;

	#ifdef ARCHIVE_TRANSPORT_DEBUG
		cout<<"ArchiveManager: setting stop to true"<<endl;
	#endif//ARCHIVE_TRANSPORT_DEBUG

	return true;
}


/*! Used when deleting an archive. This interferes with the UseQueryResult of the
	archiveManager, which has to be deleted and reinitialised after the deletion
	of the archive is complete. */
void ArchiveManager::unfreezeArchive(){
	//Set up the query and the results that will be used to stream from database
	archiveQuery->reset();
        (*archiveQuery)<<"SELECT TimeStep, FiringNeurons FROM NetworkData WHERE SimulationStartTime = UNIX_TIMESTAMP(\""<<archiveStartDateTime.toStdString()<<"\") AND TimeStep >= "<<timeStep<<" ORDER BY TimeStep";
        archiveResults = new UseQueryResult(archiveQuery->use());//UseQueryResult is much more efficient for large queries
}


/*! Frees up query object so another UseQueryResult can access it. */
bool ArchiveManager::unloadArchive(){
	if(archiveLoaded){
		delete archiveResults;
		archiveResults = NULL;
		archiveLoaded = false;
	}
	return true;
}


//------------------------------------------------------------------------------
//----------------- PROTECTED METHODS INHERITED FROM QTHREAD -------------------
//------------------------------------------------------------------------------

/*! Thread run method is used when playing back a simulation. */
void ArchiveManager::run(){
	#ifdef ARCHIVE_TRANSPORT_DEBUG
		cout<<"ArchiveManager: Start run"<<endl;
	#endif//ARCHIVE_TRANSPORT_DEBUG
	
	//Record that archive is running
	archiveRunning = true;

	//Declare the row that will be used to get the data from the results
	Row archiveRow;

	while(!stop){
            try{
                //Get XML data for frame
                if(archiveRow = archiveResults->fetch_row()){
                        //Read in time step
                        timeStep = Utilities::getUInt((std::string) archiveRow["TimeStep"]);
                        setTimeStep(timeStep);

                        //Read in xml file
                        networkDataString = ((std::string) archiveRow["FiringNeurons"]).data();
                        xmlInput.setData(networkDataString);

                        /* Check to see if enough time has elapsed and
                                sleep thread if not. */
                        elapsedTime_msec = updateTime.msecsTo(QTime::currentTime());
                        if(elapsedTime_msec < updateInterval_msec){
                                #ifdef ARCHIVE_TRANSPORT_DEBUG
                                        cout<<"ArchiveManager: Going to sleep"<<endl;
                                #endif//ARCHIVE_TRANSPORT_DEBUG

                                //Sleep for remaning time
                                usleep(1000 * (updateInterval_msec - elapsedTime_msec));
                        }

                        //Start xml parser: the XML handler will update the networkMonitors
                        #ifdef ARCHIVE_TRANSPORT_DEBUG
                                cout<<"ArchiveManager: Parse data xml"<<endl;
                        #endif//ARCHIVE_TRANSPORT_DEBUG

                        xmlReader.parse(xmlInput);
                        if(nwDataXmlHandler->getParseError()){
                                cerr<<"ArchiveWidget: ERROR OCCURRED DURING PARSING \""<<nwDataXmlHandler->getParseErrorString().toStdString()<<"\""<<endl;
                                QString archiveErrorString = "Error encountered whilst parsing XML Network Data: \"";
                                archiveErrorString += nwDataXmlHandler->getParseErrorString();
                                archiveErrorString += "\"";
                                stop = true;//Stop the archive playing after the exception
                                archError = true;//Don't want to try playing this archive again
                                SpikeStreamMainWindow::spikeStreamApplication->lock();
                                emit archiveError(archiveErrorString);
                                SpikeStreamMainWindow::spikeStreamApplication->unlock();
                        }

                        //Record time at which this 'frame' was sent
                        updateTime = QTime::currentTime();

                        //In step mode set stop to true
                        if(step){
                                stop = true;
                                step = false;
                                updateInterval_msec = originalUpdateInterval_msec;//Restore the update interval
                        }
                }
                else{//fetch row no longer returns anything
                        stop=true;
                        #ifdef ARCHIVE_TRANSPORT_DEBUG
                                cout<<"ArchiveManager: End of archive. Stopping"<<endl;
                        #endif//ARCHIVE_TRANSPORT_DEBUG
                        SpikeStreamMainWindow::spikeStreamApplication->lock();
                        emit stopped();
                        SpikeStreamMainWindow::spikeStreamApplication->unlock();
                }
            }
            catch (const Exception& er){//Catch any other errors
                    cerr<<"ArchiveWidget: EXCEPTION \""<<er.what()<<"\""<<endl;
                    QString archiveErrorString = "Exception thrown when playing archive: \"";
                    archiveErrorString += er.what();
                    archiveErrorString += "\"";
                    stop = true;//Stop the archive playing after the exception
                    archError = true;
                    SpikeStreamMainWindow::spikeStreamApplication->lock();
                    emit archiveError(archiveErrorString);
                    SpikeStreamMainWindow::spikeStreamApplication->unlock();
            }
            catch (const std::exception& er) {// Catch-all for any std exceptions
                    cerr<<"ArchiveWidget: EXCEPTION \""<<er.what()<<"\""<<endl;
                    QString archiveErrorString = "Exception thrown when playing archive: \"";
                    archiveErrorString += er.what();
                    archiveErrorString += "\"";
                    stop = true;//Stop the archive playing after the exception
                    archError = true;
                    SpikeStreamMainWindow::spikeStreamApplication->lock();
                    emit archiveError(archiveErrorString);
                    SpikeStreamMainWindow::spikeStreamApplication->unlock();
            }
	}
	//Have exited the main while loop
	archiveRunning = false;

	#ifdef ARCHIVE_TRANSPORT_DEBUG
		cout<<"ArchiveManager: Stop run"<<endl;
	#endif//ARCHIVE_TRANSPORT_DEBUG
}


//---------------------------------------------------------------------
//------------------------- PRIVATE METHODS ---------------------------
//---------------------------------------------------------------------

/*! Sets the time step in the network monitors. */
void ArchiveManager::setTimeStep(unsigned int timeStep){
	for(map<unsigned int, NetworkMonitor*>::iterator iter = networkMonitorMap.begin(); iter != networkMonitorMap.end(); ++iter)
		iter->second->setTimeStep(timeStep);
}


