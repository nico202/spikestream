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
    Cannot initialize archive dao here because constructor is called by a separate thread. */
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
			//Get a map of the neurons firing at this time step with the appropriate highlight color
			QHash<unsigned int, RGBColor*>* newHighlightMap = getNeuronColorMap(timeStep);

			//Pass map to network display, which will delete the old map
			Globals::getNetworkDisplay()->setNeuronColorMap(newHighlightMap);
			Globals::getArchive()->setTimeStep(timeStep);

			//Signal to other classes that time step has changed
			emit archiveTimeStepChanged();

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


/*! Builds a new map of highlighted neurons corresponding to the current firing pattern. */
QHash<unsigned int, RGBColor*>* ArchivePlayerThread::getNeuronColorMap(int timeStep){
    //Check archive dao has been initialized
    if(archiveDao == NULL){
		throw SpikeStreamException("ArchiveDao has not been set.");
    }

    //Get string list of firing neuron ids
    QStringList neuronIDList = archiveDao->getFiringNeuronIDs(archiveID, timeStep);

    //Build new highlight map from list of IDs
    RGBColor* neuronColor = Globals::getNetworkDisplay()->getFiringNeuronColor();
    QHash<unsigned int, RGBColor*>* newHighlightMap = new QHash<unsigned int, RGBColor*>();

    //Return empty map if no neurons are firing at this time step
    if(neuronIDList.isEmpty())
		return newHighlightMap;

    //Fill map with neuron ids
    QStringListIterator iter(neuronIDList);
    while (iter.hasNext()){
		(*newHighlightMap)[Util::getUInt(iter.next())] = neuronColor;
    }

    //Return the pointer to the map that has been created
    return newHighlightMap;
}


/*! Records that there has been an error and sets the thread to stop */
void ArchivePlayerThread::setError(const QString& errMsg){
    error = true;
    stopThread = true;
    errorMessage = errMsg;
}

