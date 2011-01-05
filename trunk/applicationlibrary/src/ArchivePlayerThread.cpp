//SpikeStream includes
#include "ArchivePlayerThread.h"
#include "Globals.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QTime>
#include <QStringList>
#include <QMutexLocker>

/*! Constructor.
	NOTE: Cannot initialize archive dao here because constructor is called by a separate thread. */
ArchivePlayerThread::ArchivePlayerThread(DBInfo archiveDBInfo) {
    this->archiveDBInfo = archiveDBInfo;
    archiveDao = NULL;
    archiveID = 0;
    updateInterval_ms = 1000;
    stepMode = false;
}


/*! Destructor */
ArchivePlayerThread::~ArchivePlayerThread(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Returns the error message associated with an error */
QString ArchivePlayerThread::getErrorMessage(){
    return errorMessage;
}


/*! Returns true if an error has occurred */
bool ArchivePlayerThread::isError(){
    return error;
}


/*! Starts the archive playing */
void ArchivePlayerThread::play(unsigned int startTimeStep, unsigned int archiveID, unsigned int frameRate){
    stepMode = false;
    this->startTimeStep = startTimeStep;
    this->archiveID = archiveID;
    this->setFrameRate(frameRate);
    start();
}


/*! Causes archive to advance by one step */
void ArchivePlayerThread::step(unsigned int startTimeStep, unsigned int archiveID){
    stepMode = true;
    this->startTimeStep = startTimeStep;
    this->archiveID = archiveID;
    start();
}


/*! Sets the number of frames per second.
    This is converted into the update interval */
void ArchivePlayerThread::setFrameRate(unsigned int frameRate){
    QMutexLocker locker(&mutex);
    this->updateInterval_ms = 1000 / frameRate;
}


/*! Causes the player to exit from its run loop and stop */
void ArchivePlayerThread::stop(){
    stopThread = true;
    stepMode = false;
}


/*----------------------------------------------------------*/
/*-----                  PRIVATE SLOTS                 -----*/
/*----------------------------------------------------------*/

/*! Inherited from QThread */
void ArchivePlayerThread::run(){
    stopThread = false;
    clearError();

    //Check archive has been set
    if(archiveID == 0){
		setError("No archive set.");
		return;
    }

    //Connect to the database
    archiveDao = new ArchiveDao(archiveDBInfo);

    //Initialise variables
    unsigned int timeStep = startTimeStep;
    QTime startTime;
    unsigned int elapsedTime_ms;

    //Get the maximum time step so we know when to stop
    unsigned int lastTimeStep = archiveDao->getMaxTimeStep(archiveID);

    if(timeStep > lastTimeStep){
		setError("Play time step is greater than the maximum time step.");
    }

    Globals::setArchivePlaying(true);

    while(!stopThread){
		//Record the current time
		startTime = QTime::currentTime();

		try{
			//Get string list of firing neuron ids
			QList<unsigned> neuronIDList = archiveDao->getFiringNeuronIDs(archiveID, timeStep);

			//Set flag to true so that thread waits for graphics update before moving to next time step
			waitForGraphics = true;

			//Inform other classes that time step has changed
			emit timeStepChanged(timeStep, neuronIDList);

			//Increase time step and quit if we are at the maximum
			++timeStep;
			if(timeStep > lastTimeStep){
				stopThread = true;
			}
			//Only display one time step in step mode
			else if (stepMode){
				stopThread = true;
				stepMode = false;
			}
			//Sleep until the next time step
			else {
				//Lock mutex so that update time interval cannot change during this calculation
				mutex.lock();

				//Sleep if task was completed in less than the prescribed interval
				elapsedTime_ms = startTime.msecsTo(QTime::currentTime());
				if(elapsedTime_ms < updateInterval_ms){
					//Sleep for remaning time
					usleep(1000 * (updateInterval_ms - elapsedTime_ms));
				}

				//Unlock mutex
				mutex.unlock();

				//Wait for graphics to update
				while(!stopThread && waitForGraphics)
					usleep(1000);
			}
		}
		catch(SpikeStreamException &ex){
			setError(ex.getMessage());
		}
    }

    Globals::setArchivePlaying(false);

    //Clear archive dao
    delete archiveDao;
    archiveDao = NULL;
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Clears the error message and state */
void ArchivePlayerThread::clearError(){
    error = false;
    errorMessage = "";
}


/*! Records that there has been an error and sets the thread to stop */
void ArchivePlayerThread::setError(const QString& errMsg){
    error = true;
    stopThread = true;
    errorMessage = errMsg;
}

