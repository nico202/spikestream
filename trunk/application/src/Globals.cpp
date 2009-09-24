#include "Globals.h"
#include "SpikeStreamException.h"

//Declare static variables
EventRouter* Globals::eventRouter = new EventRouter();
Network* Globals::network = NULL;
NetworkDisplay* Globals::networkDisplay = new NetworkDisplay();
bool Globals::rendering = false;
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
    if(network == NULL)
	throw SpikeStreamException("No network loaded. You should check that network is loaded using networkLoaded() before calling this method.");
    return network;
}


/*! Returns the network dao, which provides a layer of abstraction on top of the
    SpikeStreamNetwork database */
NetworkDao* Globals::getNetworkDao(){
    return networkDao;
}


/*! Returns the archive dao, which provides a layer of abstraction on top of the
    SpikeStreamArchive database */
NetworkDao* Globals::getArchiveDao(){
    return archiveDao;
}


/*! Returns the network display. */
NetworkDisplay* Globals::getNetworkDisplay(){
    return networkDisplay;
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


/*! Returns true if a network is loaded, false if no network exists. */
bool Globals::networkLoaded(){
    if(network == NULL)
	return false;
    return true;
}


/*! Returns true if OpenGL rendering is in progress */
bool Globals::isRendering() {
    return rendering;
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
void Globals::setNetwork(Network* net){
    //Clean up the old network if it exists
    if(Globals::network != NULL)
	delete Globals::network;

    //Store reference to the new network.
    Globals::network = net;

}


/*! Sets the class controlling the display of the network */
void Globals::setNetworkDisplay(NetworkDisplay* networkDisplay){
    //Clean up the old network display if it exists
    if(Globals::networkDisplay != NULL)
	delete networkDisplay;

    //Store reference to the new network display
    Globals::networkDisplay = networkDisplay;
}


/*! Sets the network dao. */
void Globals::setNetworkDao(NetworkDao* networkDao){
    //Clean up the old network DAO if it exists
    if(Globals::networkDao != NULL)
	delete networkDao;

    Globals::networkDao = networkDao;
}


/*! Sets whether the OpenGL rendering is taking place or is complete */
void Globals::setRendering(bool rendering){
    Globals::rendering = rendering;
}


/*! Sets the root directory. */
void Globals::setSpikeStreamRoot(QString rootDir){
    spikeStreamRoot = rootDir;
}


/*! Sets the working directory */
void Globals::setWorkingDirectory(QString workDir){
    workingDirectory = workDir;
}



