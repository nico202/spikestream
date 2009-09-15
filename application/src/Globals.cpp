#include "Globals.h"

//Declare static variables
EventRouter* Globals::eventRouter = NULL;
Network* Globals::network = NULL;
QString Globals::spikeStreamRoot = "";
QString Globals::workingDirectory = "";
NetworkDao* Globals::networkDao = NULL;


/*---------------------------------------------------------------------------------*/
/*----------                  PUBLIC METHODS                          -------------*/
/*---------------------------------------------------------------------------------*/

/*! Cleans up all classes stored in Globals.
    Everything stored in Globals is deleted by Globals. */
void Globals::cleanUp(){
    if(network != NULL){
	delete network;
	network = NULL;
    }
    if(networkDao != NULL){
	delete networkDao;
	networkDao = NULL;
    }
    if(eventRouter != NULL){
	delete eventRouter;
	eventRouter = NULL;
    }
}

/*! Returns the current network */
Network* Globals::getNetwork(){
    return network;
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

/*! Sets the event router */
void Globals::setEventRouter(EventRouter* eventRouter){
    //Clean up the old network if it exists
    if(Globals::eventRouter != NULL)
	delete eventRouter;

    Globals::eventRouter = eventRouter;
}


/*! Sets the network */
void Globals::setNetwork(Network* network){
    //Clean up the old network if it exists
    if(Globals::network != NULL)
	delete network;

    //Reset the network display
    networkDisplay->reset();

    //Store reference to the new network.
    Globals::network = network;
}


/*! Sets the network dao. */
void Globals::setNetworkDao(NetworkDao* networkDao){
    //Clean up the old network DAO if it exists
    if(Globals::networkDao != NULL)
	delete networkDao;

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



