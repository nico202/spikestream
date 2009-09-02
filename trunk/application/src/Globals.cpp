#include "Globals.h"

//Declare static variables
QString Globals::spikeStreamRoot = "";
QString Globals::workingDirectory = "";
NetworkDao* Globals::networkDao = NULL;


/*---------------------------------------------------------------------------------*/
/*----------                  PUBLIC METHODS                          -------------*/
/*---------------------------------------------------------------------------------*/

/*! Cleans up all classes stored in Globals.
    Everything stored in Globals is deleted by Globals. */
void Globals::cleanUp(){
    delete networkDao;
}


/*! Returns the network dao, which provides a layer of abstraction on top of the
    SpikeStreamNetwork database */
NetworkDao* Globals::getNetworkDao(){
    return networkDao;
}

/*! Returns the root directory, which should correspond to the SPIKESTREAM_ROOT
     environment variable.*/
QString Globals::getSpikeStreamRoot(){
    return spikeStreamRoot;
}

/*! Returns the working directory which is opened when loading projects, import files etc. */
QString Globals::getWorkingDirectory(){
    return workingDirectory;
}


/*---------------------------------------------------------------------------------*/
/*----------                  PRIVATE METHODS                         -------------*/
/*---------------------------------------------------------------------------------*/

/*! Sets the network dao. */
void Globals::setNetworkDao(NetworkDao* networkDao){
    Globals::networkDao = networkDao;
}

/*! Sets the root directory. */
void Globals::setSpikeStreamRoot(QString rootDir){
    spikeStreamRoot = rootDir;
}

/*! Sets the working directory */
void Globals::setWorkingDirectory(QString workDir){
    workingDirectory = workDir;
}



